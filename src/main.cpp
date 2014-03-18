//g++ *.cpp src/*.cpp src/*.cc -o pmx -I/usr/include/bullet -lglfw -lGLU -lGLEW -lGL -lSDL2 -lSDL2_mixer -ggdb -lSOIL -lBulletDynamics -lBulletCollision -lLinearMath -lBulletSoftBody -std=c++11
#include <iostream>
#include <fstream>
#include <sstream>

#include "pmxvLogger.h"
#include "viewer.h"

#include <libmmp.h>
/*#include "texthandle.h"
#include "pmx.h"
#include "vmd.h"
#include "pmxvLogger.h"

#include "viewer.h"*/

using namespace std;

void printHelpInfo()
{
	cout<<"This is PMXViewer, a demonstration using the MikuMikuPenguin library."<<endl
	<<"Usage: pmx [options] model_file.pmx motion_file.vmd"<<endl
	<<"Options: "<<endl
	<<"  -s\tPlay sound file to go with VMD motion"<<endl
	<<"\tOnly formats compatible with your build of SDL2_mixer are accepted."<<endl
	<<"\tIf libmmp was compiled without SDL2_mixer support, no music will play."<<endl<<endl
	<<"  --help\tdisplay this help and exit"<<endl
	<<"  --version\toutput version information and exit"<<endl<<endl
	<<"FOR JAPANESE DOCUMENTATION, SEE:"<<endl
	<<"<file://"<<DATA_PATH<<"/doc/README_JP.html>"<<endl<<endl
	<<"Report bugs to <ibenrunnin@gmail.com> (English or Japanese is ok)."<<endl;
}

void printVersionInfo()
{
	cout<<"PMXViewer v0.1"<<endl
	<<"Copyright (C) 2014 Ben Clapp (sn0w75)"<<endl
     <<"This is free software; see the source for copying conditions.  There is NO"<<endl
     <<"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."<<endl
     <<"Written by Ben Clapp (sn0w75), <ibenrunnin@gmail.com>."<<endl<<endl;
}

int main(int argc, char** argv)
{
	string model_file="";
	string motion_file="";
	string music_file="";
	
	int count=0;
	if(argc > 1)
	{
	  for (count = 1; count < argc; count++)
		{
			if(string(argv[count]) == "--help")
			{
				printHelpInfo();
				return 0;
			}
			else if(string(argv[count]) == "--version")
			{
				printVersionInfo();
				return 0;
			}
			else if(string(argv[count]) == "-s")
			{
				if(count+1<argc) music_file=argv[count+1];
			}
			else
			{
				string str=string(argv[count]);
				if(str.substr(str.size()-4)==".pmx")
				{
					model_file=str;
				}
				else if(str.substr(str.size()-4)==".vmd")
				{
					motion_file=str;
				}
			}
		}
	}
	else
	{
		printHelpInfo();
		return 0;
	}
	
	if(model_file=="")
	{
		cout<<"Did not specify a model file!"<<endl<<endl;
		printHelpInfo();
		return 0;
	}
	else if(motion_file=="")
	{
		cout<<"Did not specify a motion file!"<<endl<<endl;
		printHelpInfo();
		return 0;
	}
	cout<<"sizeinfo: "<<sizeof(VertexData)<<" "<<sizeof(GLfloat)<<" "<<sizeof(glm::vec2)<<endl;
	
	Viewer viewer(model_file,motion_file,music_file);
	
	viewer.run();
	
	delete pmxvLogger::get();	
	return 0;
}
