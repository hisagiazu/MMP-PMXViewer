#include "viewer.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <SOIL/SOIL.h>
//#include <FreeImage.h>

#include "texthandle.h"
#include "pmx.h"
#include "vmd.h"
#include "shader.h"
#include "pmxvLogger.h"

#include "motioncontroller.h"
#include "bulletphysics.h"
#include "mmdphysics.h"

#include "sound.h"

#include "glfw_func_callbacks.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//#define MODELDUMP true

using namespace std;
using namespace ClosedMMDFormat;


Viewer::Viewer(string modelPath, string motionPath,string musicPath)
{
	int index=modelPath.rfind("/");
	string modelFilePath,modelFolderPath;
	
	if(index==-1)
	{
		modelFolderPath="";
		modelFilePath=modelPath;
	}
	else
	{
		modelFilePath=modelPath.substr(index);
		modelFolderPath=modelPath.substr(0,index);
	}
	
	pmxInfo=&readPMX(modelFolderPath,modelFilePath);
	vmdInfo=&readVMD(motionPath);
	//pmxInfo=&readPMX("data/model/gumiv3/","GUMI_V3.pmx");
	//vmdInfo=&readVMD("data/motion/Watashi no Jikan/私の時間_short_Lat式ミク.vmd");
	
	//Below is a test for VMD file-writing. Write the current vmdInfo object to a file, then read back the output into a new vmdInfo object
	//writeVMD(*vmdInfo,"testVMD.vmd");
	//vmdInfo=&readVMD("testVMD.vmd");
	
	initGLFW();
	
	
	//NOTE: shaderProgram compilation now handled in hackShaderFiles() within initGLFW()
	/*ifstream test("shaders/model.vert");
	if(!test.is_open())
	{
		shaderProgram=compileShaders(DATA_PATH"/shaders/model.vert",DATA_PATH"/shaders/model.frag");
	}
	else
	{
		shaderProgram=compileShaders("shaders/model.vert","shaders/model.frag");
	}
	test.close();*/
	
	
	
	loadTextures();

	
	initBuffers();
	linkShaders(shaderProgram);
	glUseProgram(shaderProgram);
	

	initUniformVarLocations();
	
	MVP_loc = glGetUniformLocation(shaderProgram, "MVP");
    
	//Set OpenGL render settings
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(5.0);
	//glClearDepth(1.0f);
	
	//glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClearColor(1,1,1,1);
	//glDisable(GL_MULTISAMPLE);
	
	//Setup MotionController, Physics
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VertexArrayBuffer]);
	motionController=new VMDMotionController(*pmxInfo,*vmdInfo,shaderProgram);
	
	
	initSound(musicPath);
	
	bulletPhysics = new BulletPhysics(bulletVertPath,bulletFragPath);
	glUseProgram(shaderProgram); //restore GL shader program binding to Viewer's shader program after initializing BulletPhysic's debugDrawer
	mmdPhysics = new MMDPhysics(*pmxInfo,motionController,bulletPhysics);
	
	motionController->updateVertexMorphs();
	motionController->updateBoneAnimation();
	
	//Initialize timer variables
	startTime = glfwGetTime();
	ticks=0;
	
	modelTranslate=glm::vec3(0.0f,-10.0f,0.0f);
}

void Viewer::handleLogic()
{
	bool doPhysics=true;
	if(glfwGetKey('Q')==GLFW_PRESS)
	{
		doPhysics=false;
	}
	
	//if(glfwGetKey('A')==GLFW_RELEASE)
	{
		if (!motionController->advanceTime())
		{
			motionController->updateVertexMorphs();
			motionController->updateBoneAnimation();
		}
		//Debug- hold model in bind pose
		//holdModelInBindPose();
		
		mmdPhysics->updateBones(doPhysics);
		motionController->updateBoneMatrix();
		
		glUseProgram(bulletPhysics->debugDrawer->shaderProgram);
		setCamera(bulletPhysics->debugDrawer->MVPLoc);
		glUseProgram(shaderProgram);
		setCamera(MVP_loc);
	}
}

void Viewer::render()
{
	if(glfwGetKey('A')==GLFW_RELEASE)
	{
		drawModel(true); //draw model edges
		drawModel(false); //draw model
	}
	
	if(glfwGetKey('S')==GLFW_PRESS)
	{
		bulletPhysics->SetDebugMode(btIDebugDraw::DBG_DrawWireframe);
		bulletPhysics->DebugDrawWorld();
	}
	else if(glfwGetKey('D')==GLFW_PRESS)
	{
		bulletPhysics->SetDebugMode(btIDebugDraw::DBG_DrawAabb);
		bulletPhysics->DebugDrawWorld();
	}
	else if(glfwGetKey('F')==GLFW_PRESS)
	{
		bulletPhysics->SetDebugMode(btIDebugDraw::DBG_DrawConstraints);
		bulletPhysics->DebugDrawWorld();
	}
	else if(glfwGetKey('G')==GLFW_PRESS)
	{
		bulletPhysics->SetDebugMode(btIDebugDraw::DBG_DrawConstraintLimits);
		bulletPhysics->DebugDrawWorld();
	}
	glUseProgram(shaderProgram); //Restore shader program and buffer's to Viewer's after drawing Bullet debug
	glBindVertexArray(VAOs[Vertices]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VertexArrayBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[VertexIndexBuffer]);
	
	//glFinish();
	//glDrawBuffer(RecordBuffer,Buffers);
	//glReadPixels(0,0,1920,1080,GL_RGB,
}

void Viewer::fpsCount()
{
	static double last_time;
	static int nb_frames[] = {0,0,0,0};
	static const int nbf_fps = sizeof(nb_frames) / sizeof(int);
	static const double seq = 1.0 / double(nbf_fps);
	static int nbf_count = 0;
	static const char* window_name = "PMX Viewer";
	if (!last_time) last_time = glfwGetTime();
	
	double current_time = glfwGetTime();
	for (int i = 0; i < nbf_fps; i++)
	{
		nb_frames[i]++;
	}

	if (current_time - last_time >= seq)
	{
		int nbf_now = nbf_count % nbf_fps;
		char title[128];
		sprintf(title, "%s | %2d fps", window_name, nb_frames[nbf_now]);
		glfwSetWindowTitle(title);
		nb_frames[nbf_now] = 0;
		last_time += seq;
		nbf_count++;
	}
}

void Viewer::run()
{
	while(glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		startTime = glfwGetTime();
		
		fpsCount();
		handleEvents();
		handleLogic();
		render();
		
		glfwSwapBuffers();
	}
}

void Viewer::setCamera(GLuint MVPLoc)
{
	glm::mat4 Projection = glm::perspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
		glm::vec3(cameraPosition.x,cameraPosition.y,-cameraPosition.z), // Camera is at (4,3,3), in World Space
		cameraTarget, // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	//View= glm::rotate(0.0f,0.0f,0.0f,1.0f)* View;
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::translate(modelTranslate.x, modelTranslate.y, modelTranslate.z);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;
	
	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVP[0][0]);
}

void Viewer::drawModel(bool drawEdges)
{
	//Bind VAO and related Buffers
	glBindVertexArray(VAOs[Vertices]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VertexArrayBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[VertexIndexBuffer]);
	
	if(drawEdges)
	{
		glDisable(GL_BLEND);
		glCullFace(GL_FRONT);
        glUniform1i(uniformVars[uIsEdge], 1);
        
        glDisable(GL_DEPTH_TEST);
	}
	else
	{        
        glEnable(GL_BLEND);
		glCullFace(GL_BACK);
		glUniform1i(uniformVars[uIsEdge], 0);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
	}
	
	
	glm::vec3 halfVector=glm::normalize(cameraPosition-cameraTarget);
	halfVector.z=-halfVector.z;
	
	glm::vec3 lightDirection=glm::normalize(glm::vec3(0.3,1.0,2.0));
	
	int faceCount=0;
	for(int m=0; m<pmxInfo->material_continuing_datasets; ++m) //pmxInfo->material_continuing_datasets
	{
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,textures[pmxInfo->materials[m]->textureIndex]);
		glUniform1iARB(uniformVars[uTextureSampler], 0);
		
		if((int)pmxInfo->materials[m]->sphereMode>0)
		{
			glActiveTexture(GL_TEXTURE1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,textures[pmxInfo->materials[m]->sphereIndex]);
			glUniform1iARB(uniformVars[uSphereSampler], 1);
		}
		
		if((int)pmxInfo->materials[m]->shareToon==0)
		{
			glActiveTexture(GL_TEXTURE2);
			glEnable(GL_TEXTURE_2D);
			
			glBindTexture(GL_TEXTURE_2D,textures[pmxInfo->materials[m]->toonTextureIndex]);
			glUniform1iARB(uniformVars[uToonSampler], 2);
		}
		else if((int)pmxInfo->materials[m]->shareToon==1)
		{
			glActiveTexture(GL_TEXTURE2);
			glEnable(GL_TEXTURE_2D);
			
			glBindTexture(GL_TEXTURE_2D,textures[textures.size()-11+pmxInfo->materials[m]->shareToonTexture]);
			glUniform1iARB(uniformVars[uToonSampler], 2);
		}
		
		
		glUniform3fv(uniformVars[uAmbient],1,(GLfloat*)&pmxInfo->materials[m]->ambient);
		glUniform4fv(uniformVars[uDiffuse],1,(GLfloat*)&pmxInfo->materials[m]->diffuse);
		glUniform3fv(uniformVars[uSpecular],1,(GLfloat*)&pmxInfo->materials[m]->specular);
		
		glUniform1f(uniformVars[uShininess],glm::normalize(pmxInfo->materials[m]->shininess));
		glUniform3f(uniformVars[uHalfVector],halfVector.x,halfVector.y,halfVector.z);
		glUniform3f(uniformVars[uLightDirection],lightDirection.x,lightDirection.y,lightDirection.z);
		
		glUniform4fv(uniformVars[uEdgeColor],1,(GLfloat*)&pmxInfo->materials[m]->edgeColor);
		glUniform1f(uniformVars[uEdgeSize],glm::normalize(pmxInfo->materials[m]->edgeSize));
		
		glUniform1f(uniformVars[uSphereMode],pmxInfo->materials[m]->sphereMode);
        
		glDrawElements(GL_TRIANGLES, (pmxInfo->materials[m]->hasFaceNum), GL_UNSIGNED_INT, BUFFER_OFFSET(sizeof(GLuint)*faceCount));
		faceCount+=pmxInfo->materials[m]->hasFaceNum;
	}
}


//#define MODELDUMP
void Viewer::initBuffers()
{
	#ifdef MODELDUMP
	ofstream modeldump("modeldump.txt");
	modeldump << "indices:" << endl;
	#endif
	
	//Note: vertex indices are loaded statically, since they do not change.
	//The actual vertex data is loaded dynamically each frame, so its memory is managed by the MotionController.
	GLuint *vertexIndices= (GLuint*) calloc(pmxInfo->face_continuing_datasets,sizeof(GLuint)*3);
	for(int i=0; i<pmxInfo->faces.size(); ++i) //faces.size()
	{
		int j=i*3;
		vertexIndices[j]=pmxInfo->faces[i]->points[0];
		vertexIndices[j+1]=pmxInfo->faces[i]->points[1];
		vertexIndices[j+2]=pmxInfo->faces[i]->points[2];
		
		#ifdef MODELDUMP
		modeldump << vertexIndices[j] << " " << vertexIndices[j+1] << " " << vertexIndices[j+2] << endl;
		#endif
	}
	
	
	#ifdef MODELDUMP
	modeldump << "vertices:" << endl;
	#endif
	
	#ifdef MODELDUMP
	modeldump.close();
	#endif
	
	//Generate all Viewer Buffers
	glGenBuffers(NumBuffers,Buffers);
	
	//init Element Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[VertexIndexBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pmxInfo->face_continuing_datasets*sizeof(GLuint)*3, vertexIndices, GL_STATIC_DRAW);
	
	free(vertexIndices);
	
	
	//Init Vertex Array Buffer Object
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Vertices]);
	
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VertexArrayBuffer]);
	
	//Intialize Vertex Attribute Pointers
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(0)); //4=number of components updated per vertex
	glBindAttribLocation(shaderProgram, vPosition, "vPosition"); //Explicit vertex attribute index specification for older OpenGL version support. (Newer method is layout qualifier in vertex shader)
	glEnableVertexAttribArray(vPosition);

	glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4)));
	glBindAttribLocation(shaderProgram, vUV, "vUV");
	glEnableVertexAttribArray(vUV);

	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4)+sizeof(glm::vec2)));
	glBindAttribLocation(shaderProgram, vNormal, "vNormal");
	glEnableVertexAttribArray(vNormal);

	glVertexAttribPointer(vBoneIndices, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4)+sizeof(glm::vec2)+sizeof(glm::vec3)+sizeof(GLfloat)));
	glBindAttribLocation(shaderProgram, vBoneIndices, "vBoneIndices");
	glEnableVertexAttribArray(vBoneIndices);

	glVertexAttribPointer(vBoneWeights, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4)+sizeof(glm::vec2)+sizeof(glm::vec3)+sizeof(GLfloat)*5));
	glBindAttribLocation(shaderProgram, vBoneWeights, "vBoneWeights");
	glEnableVertexAttribArray(vBoneWeights);

	glVertexAttribPointer(vWeightFormula, 1, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4)+sizeof(glm::vec2)+sizeof(glm::vec3)));
	glBindAttribLocation(shaderProgram, vWeightFormula, "vWeightFormula");
	glEnableVertexAttribArray(vWeightFormula);
}

void Viewer::loadTextures()
{
	//Thought about replacing SOIL with FreeImage, but for now sticking with SOIL. Don't fix what isn't broken.
	//Warning: Commented-out FreeImage code below is buggy (DOES NOT WORK)
	
	//FreeImage_Initialise();	
	
	for(int i=0; i<pmxInfo->texture_continuing_datasets; ++i)
	{
		cout<<"Loading "<<pmxInfo->texturePaths[i]<<"...";
		if(pmxInfo->texturePaths[i].substr(pmxInfo->texturePaths[i].size()-3)=="png" || pmxInfo->texturePaths[i].substr(pmxInfo->texturePaths[i].size()-3)=="spa")
		{
			GLuint texture;
			int width, height;
			unsigned char* image;
			string loc=pmxInfo->texturePaths[i];
			
			ifstream test(loc);
			if(!test.is_open())
			{
				cerr<<"Texture file could not be found: "<<loc<<endl;
				//exit(EXIT_FAILURE);
			}
			test.close();
			
			glActiveTexture( GL_TEXTURE0 );
			glGenTextures( 1, &texture );
			glBindTexture( GL_TEXTURE_2D, texture );
				image = SOIL_load_image( loc.c_str(), &width, &height, 0, SOIL_LOAD_RGBA );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image );
			SOIL_free_image_data( image );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			
			if(texture==0)
			{
				cerr<<"Texture failed to load: "<<pmxInfo->texturePaths[i]<<endl;
				printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
				exit(EXIT_FAILURE);
			}
			
			cout<<"done"<<endl;
			
			textures.push_back(texture);
		}
		else if(pmxInfo->texturePaths[i].substr(pmxInfo->texturePaths[i].size()-3)=="tga")
		{
			//cerr<<"WARNING: TGA files only mildly tested"<<endl;
			
			/*static GLuint texture = 0;
	
			FIBITMAP *bitmap = FreeImage_Load( FreeImage_GetFileType(pmxInfo->texturePaths[i].c_str(), 0),pmxInfo->texturePaths[i].c_str() );
			FIBITMAP *pImage = FreeImage_ConvertTo24Bits(bitmap);
			
			int nWidth = FreeImage_GetWidth(pImage);
			int nHeight = FreeImage_GetHeight(pImage);
			
			glActiveTexture( GL_TEXTURE0 );
			glGenTextures(1, &texture);

			glBindTexture(GL_TEXTURE_2D, texture);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight,
			0, GL_BGR, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
			
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glGet
			if(texture==0)
			{
				cerr<<"Texture failed to load: "<<pmxInfo->texturePaths[i]<<endl;
				exit(EXIT_FAILURE);
			}

			FreeImage_Unload(bitmap);
			
			textures.push_back(texture);
			
			cout<<"done"<<endl;*/
			
			GLuint texture;
			int width, height, channels;
			unsigned char* image;
			string loc=pmxInfo->texturePaths[i];
			
			ifstream test(loc);
			if(!test.is_open())
			{
				cerr<<"Texture file could not be found: "<<loc<<endl;
				//exit(EXIT_FAILURE);
			}
			test.close();
			
			glActiveTexture( GL_TEXTURE0 );
			glGenTextures( 1, &texture );
			glBindTexture( GL_TEXTURE_2D, texture );
				image = SOIL_load_image( loc.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA );
			
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image );
			SOIL_free_image_data( image );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			
			if(texture==0)
			{
				cerr<<"Texture failed to load: "<<pmxInfo->texturePaths[i]<<endl;
				printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
				exit(EXIT_FAILURE);
			}
			
			cout<<"done"<<endl;
			
			textures.push_back(texture);
		}
		else
		{
			GLuint texture;
			int width, height;
			unsigned char* image;
			string loc=pmxInfo->texturePaths[i];
			
			ifstream test(loc);
			if(!test.is_open())
			{
				cerr<<"Texture file could not be found: "<<loc<<endl;
				//exit(EXIT_FAILURE);
			}
			test.close();
			
			glActiveTexture(GL_TEXTURE0);
			glGenTextures( 1, &texture );
			glBindTexture( GL_TEXTURE_2D, texture );
			image = SOIL_load_image( loc.c_str(), &width, &height, 0, SOIL_LOAD_RGB );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
			SOIL_free_image_data( image );
			
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
 
			if(texture == 0)
			{
				cerr<<"Texture failed to load: "<<pmxInfo->texturePaths[i]<<endl;
				printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
				exit(EXIT_FAILURE);
			}
			
			textures.push_back(texture);
			
			cout<<"done"<<endl;
		}
	}
	
	for(int i=1; i<=10; ++i)
	{
		//cout<<"Loading toon"<<i<<".bmp...";
		
		GLuint texture;
		int width, height, channels;
		unsigned char* image;
		stringstream loc;
		if(i!=10) loc<<"data/share/toon0"<<i<<".bmp";
		else loc<<"data/share/toon10.bmp";
		
		ifstream test(loc.str());
		if(!test.is_open())
		{
			//cerr<<"Texture file could not be found: "<<loc.str()<<endl;
			//exit(EXIT_FAILURE);
			
			loc.str(std::string()); //clear ss
			if(i!=10) loc<<DATA_PATH<<"/textures/toon0"<<i<<".bmp";
			else loc<<DATA_PATH<<"/textures/toon10.bmp";
			
			ifstream test2(loc.str());
			if(!test2.is_open())
			{
				cerr<<"Texture file could not be found: "<<loc.str()<<endl;
			}
			
		}
		test.close();
		
		glActiveTexture( GL_TEXTURE0 );
		glGenTextures( 1, &texture );
		glBindTexture( GL_TEXTURE_2D, texture );
			image = SOIL_load_image( loc.str().c_str(), &width, &height, &channels, SOIL_LOAD_RGBA );
		
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image );
		SOIL_free_image_data( image );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		
		if(texture==0)
		{
			cerr<<"Toon Texture failed to load: "<<i<<endl;
			printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
			exit(EXIT_FAILURE);
		}
		
		//cout<<"done"<<endl;
		
		textures.push_back(texture);
	}
	
	
	//FreeImage_DeInitialise();
}

string hackShaderFile(string filename)
{
	//Variables for temporary file name paths
	char *tmpFilePathChar;
	tmpFilePathChar=tmpnam(tmpFilePathChar); //Get path to a temporary file location
	
	string tmpFilePath=tmpFilePathChar;
	ofstream tmpShaderFile(tmpFilePath);
	
	ifstream shaderFile(filename);
	
	string line;
	getline(shaderFile,line); //discard first line
	tmpShaderFile<<"#version 130"<<endl;
	while(shaderFile.good())
	{
		getline(shaderFile,line);
		tmpShaderFile<<line<<endl;
	}
	
	//cout<<tmpFilePath<<endl;
	
	return tmpFilePath;
}

void Viewer::hackShaderFiles()
{
	//If using OpenGL 3.0, produce temporary shader files that use #version 130 (to prevent issues with OpenGL 3.0/GLSL 1.30 users
	//Otherwise, use #version 150 to avoid issues in Mac OSX (which does not support #version 130)
	
	if(GLVersionMajor==3 && (GLVersionMinor==0 || GLVersionMinor==1))
	{
		cout<<"Going to hack shader files for OpenGL "<<GLVersionMajor<<"."<<GLVersionMinor<<" (assuming GLSL version 130 supported)"<<endl;
		
		string vertPath,fragPath; //vertex/fragment shader file paths
		ifstream test("shaders/model.vert");
		if(!test.is_open())
		{
			vertPath=DATA_PATH"/shaders/model.vert";
			fragPath=DATA_PATH"/shaders/model.frag";
		}
		else
		{
			vertPath="shaders/model.vert";
			fragPath="shaders/model.frag";
		}
		vertPath=hackShaderFile(vertPath);
		fragPath=hackShaderFile(fragPath);
		
		shaderProgram=compileShaders(vertPath,fragPath);
		
		remove(vertPath.c_str());
		remove(fragPath.c_str());
		
		ifstream test2("shaders/bulletDebug.vert");
		if(!test2.is_open())
		{
			vertPath=DATA_PATH"/shaders/bulletDebug.vert";
			fragPath=DATA_PATH"/shaders/bulletDebug.frag";
		}
		else
		{
			vertPath="shaders/bulletDebug.vert";
			fragPath="shaders/bulletDebug.frag";
		}
		vertPath=hackShaderFile(vertPath);
		fragPath=hackShaderFile(fragPath);
		
		bulletVertPath=vertPath;
		bulletFragPath=fragPath;
		
	}
	else
	{
		//Load included shader files as-is
		ifstream test("shaders/model.vert");
		if(!test.is_open())
		{
			shaderProgram=compileShaders(DATA_PATH"/shaders/model.vert",DATA_PATH"/shaders/model.frag");
		}
		else
		{
			shaderProgram=compileShaders("shaders/model.vert","shaders/model.frag");
		}
		test.close();
		
		ifstream test2("shaders/bulletDebug.vert");
		if(!test2.is_open())
		{
			bulletVertPath=DATA_PATH"/shaders/bulletDebug.vert";
			bulletFragPath=DATA_PATH"/shaders/bulletDebug.frag";
		}
		else
		{
			bulletVertPath="shaders/bulletDebug.vert";
			bulletFragPath="shaders/bulletDebug.frag";
		}
	}
}

void Viewer::initGLFW()
{	
	if (!glfwInit()) exit(EXIT_FAILURE);
	
	//glfwOpenWindow variables, feel free to modify if glfwOpenWindow() fails
	static const int SCREEN_WIDTH=1920,SCREEN_HEIGHT=1080;
	static const int redBits=0,greenBits=0,blueBits=0,alphaBits=0,depthBits=32,stencilBits=0;
	
	
	//First, try to start in OpenGL 3.2+ mode
	GLVersionHintMajor=3,GLVersionHintMinor=2;
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 2); //2x antialiasing
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, GLVersionHintMajor); //OpenGL version
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, GLVersionHintMinor);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Don't want old OpenGL
	
	cout<<"Attempting to open GLFW window (OpenGL "<<GLVersionHintMajor<<"."<<GLVersionHintMinor<<")...";
	//Open a window and create its OpenGL context
	if( !glfwOpenWindow( SCREEN_WIDTH, SCREEN_HEIGHT, redBits,greenBits,blueBits,alphaBits, depthBits,stencilBits, GLFW_WINDOW ) )
	{
		cout<<"Failed to open GLFW window"<<endl;
		
		GLVersionHintMajor=3,GLVersionHintMinor=0;
		
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, GLVersionHintMajor);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, GLVersionHintMinor);
		glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwOpenWindowHint(GLFW_OPENGL_PROFILE, 0);
		
		cout<<"Attempting to open GLFW window (OpenGL "<<GLVersionHintMajor<<"."<<GLVersionHintMinor<<")...";
		if( !glfwOpenWindow( SCREEN_WIDTH, SCREEN_HEIGHT, redBits,greenBits,blueBits,alphaBits, depthBits,stencilBits, GLFW_WINDOW ) )
		{
			cout<<"FATAL ERROR: Failed to open GLFW window"<<endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
	}
	cout<<"done"<<endl;
	
	glfwGetGLVersion(&GLVersionMajor, &GLVersionMinor, &GLVersionRevision);
	cout<<"(GLFW) OpenGL version recieved: "<<GLVersionMajor<<"."<<GLVersionMinor<<" (Revision "<<GLVersionRevision<<")"<<endl;
	cout<<"OpenGL version info: "<<glGetString(GL_VERSION)<<endl;
	cout<<"GLSL version info: "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<endl<<endl;

	// Initialize GLEW
	glewExperimental=true; //Needed in core profile
	if(glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(EXIT_FAILURE);
	}
	glfwSetWindowTitle("PMX Viewer");
	//Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	
	glfwSetMousePosCallback(mouseMotion);
	glfwSetMouseWheelCallback(mouseWheel);
	
	
	hackShaderFiles();
	
	//Ensure vsync
	glfwSwapInterval(1);
}

void Viewer::initUniformVarLocations()
{
	uniformVars[uAmbient]=glGetUniformLocation(shaderProgram, "ambient");
	uniformVars[uDiffuse]=glGetUniformLocation(shaderProgram, "diffuse");
	uniformVars[uSpecular]=glGetUniformLocation(shaderProgram, "specular");
	
	uniformVars[uShininess]=glGetUniformLocation(shaderProgram, "shininess");
	
	uniformVars[uIsEdge]=glGetUniformLocation(shaderProgram, "isEdge");
	uniformVars[uEdgeColor]=glGetUniformLocation(shaderProgram, "edgeColor");
	uniformVars[uEdgeSize]=glGetUniformLocation(shaderProgram, "edgeSize");
	
	uniformVars[uHalfVector]=glGetUniformLocation(shaderProgram, "halfVector");
	
	uniformVars[uLightDirection]=glGetUniformLocation(shaderProgram, "lightDirection");
	
	uniformVars[uSphereMode]=glGetUniformLocation(shaderProgram, "fSphereMode");
	
	uniformVars[uTextureSampler]=glGetUniformLocationARB(shaderProgram,"textureSampler");
	uniformVars[uSphereSampler]=glGetUniformLocationARB(shaderProgram,"sphereSampler");
	uniformVars[uToonSampler]=glGetUniformLocationARB(shaderProgram,"toonSampler");
}

Viewer::~Viewer()
{
	glfwTerminate();
}

void Viewer::handleEvents()
{
	glfwPollEvents();
	if(glfwGetKey(GLFW_KEY_UP)==GLFW_PRESS)
	{
		modelTranslate.y-=0.1;
	}
	else if(glfwGetKey(GLFW_KEY_DOWN)==GLFW_PRESS)
	{
		modelTranslate.y+=0.1;
	}
}

void Viewer::holdModelInBindPose()
{
	for(unsigned i = 0; i<pmxInfo->bone_continuing_datasets; i++)
	{
		PMXBone *b=pmxInfo->bones[i];
		
		if(b->parentBoneIndex!=-1)
		{
			PMXBone *parent = pmxInfo->bones[b->parentBoneIndex];
			b->Local = glm::translate( b->position - parent->position );
		}
		else
		{
			b->Local = glm::translate( b->position );
		}
	
		motionController->skinMatrix[i] = b->calculateGlobalMatrix()*motionController->invBindPose[i];
	}
	motionController->updateBoneMatrix();
}


