#version 150

uniform sampler2D textureSampler;
uniform sampler2D sphereSampler;
uniform sampler2D toonSampler;

uniform vec3 ambient;
uniform vec4 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform vec3 lightDirection; //direction towards the light
uniform vec3 halfVector; //surface orientation for shiniest spots- the vector stretching from from the camera position to the target

uniform bool isEdge;
uniform vec4 edgeColor;
uniform float edgeSize;

uniform float fSphereMode; //sphereMode stored as a float


in vec2 UV;
in vec3 normal;

out vec4 color;

vec4 getSphereColor()
{
	int sphereMode=int(fSphereMode);
	
	vec4 sphereColor=vec4(0,0,0,0);
	
	if(sphereMode>0) //if sphere mapping is used
	{
		vec2 sphereCoord = 0.5 * (1.0 + vec2(1.0, -1.0) * normalize(normal).xy);
		sphereColor=texture(sphereSampler, sphereCoord);
	}
	
	return sphereColor;
}

void main()
{	
	//color = vec4(0.0, 1.0, 0.0, 1.0);
	//return;
	
	if(isEdge)
	{
		color=edgeColor;
		return;
	}
	
	int sphereMode=int(fSphereMode);
	
	vec3 textureColor=vec3(1.0,1.0,1.0);
	
	textureColor*=texture(textureSampler, UV).rgb;
	
	vec4 sphereColor=getSphereColor();
	
	//finalColor*=sphereColor;
	
	if((UV[0]!=0 && UV[1]!=0))
	{
		float intensity = dot(normalize(normal),lightDirection);
		vec3 lightColor=normalize(vec3(1.0,1.0,0.95));
		
		vec3 scatteredLight;
		
		if(intensity > 0.99) scatteredLight=ambient+ (diffuse.rgb*lightColor*.8);
		else if(intensity > 0.3) scatteredLight=ambient+ (diffuse.rgb*lightColor * 0.75);
		else scatteredLight = ambient + (diffuse.rgb *lightColor * 0.5);
		
		float specularWeight;
		if(intensity==0.0) //if surface is facing away from light
		{
			specularWeight=0.0;
		}
		else
		{ 
			specularWeight = pow(max(0.0,dot(halfVector,normal)),shininess); //sharpen the highlight
		}
		
		intensity = dot(normalize(normal),halfVector);
		vec3 reflectedLight=vec3(0.0);
		if(intensity>.999)reflectedLight=(specular*specularWeight);
		
		
		float dotNL = max(0.0, dot(normalize(lightDirection), normalize(normal)));
        vec2 toonCoord = vec2(0.0, 0.5 * (1.0 - dotNL));
        vec3 toon = texture(toonSampler, toonCoord).rgb;
		
		vec3 colorRGB=min((textureColor*scatteredLight) + reflectedLight,vec3(1.0));
		color=vec4(colorRGB,texture(textureSampler, UV).a);
		
		/*if(sphereMode==1)
		{
			//color=vec4(color.x*sphereColor.x,color.y*sphereColor.y,color.z*sphereColor.z,color.w);
			vec3 colorTest=cross(vec3(sphereColor),vec3(color));
			//vec3 colorTest=vec3(color)*vec3(sphereColor);
			color+=vec4(colorTest,color.w);
		}
		else if(sphereMode==2)
		{
			color+=sphereColor;
		}*/
		
		/*if(sphereMode>0)
		{
			color=vec4(1.0,0.0,0,1);
		}*/
		
	}
	else
	{
		if(normal.x==1)
		{
			color = vec4(0.0, 1.0, 0.0, 1.0);
		}
		else if(normal.y==1)
		{
			color = vec4(0.0, 0.4, 1.0, 1.0);
		}
		else
		{
			color = vec4(1.0, 0.5, 0.5, 0.5);
		}
		//vec2 pos={0.1,0.1};
		
		//color = texture(textureSampler, UV).rgb;
		//color = texture(textureSampler, UV).rgb;
		//color = texture(textureSampler2, UV).rgb;
	}
}
