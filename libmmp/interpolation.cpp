#include "interpolation.h"

#include <iostream>

using namespace std;


float parBezFunc(float t, float p1, float p2) //parameterized Bezier Curve Function
{
	return (3*( (1-t)*(1-t) )*t*p1) + (3*(1-t)*(t*t)*p2) + (t*t*t);
}

float bezier(float X, float x1,float y1, float x2,float y2)
{
	//The below function approximates the Y of a bezier curve for a given X using the bisection method.
	//...This is technically the bisection method, but I wrote the code myself, so it may not be very optimized.
	//It's also well known that there are faster methods of solving bezier curves, such as the Newton method.
	//However, bisection is a simpler algorithm that works well for MMD's curves, who work strictly in the range of [0,1]
	
	x1/=127.0f; x2/=127.0f; y1/=127.0f; y2/=127.0f;
	
	float x0,y0,x3,y3;
	x0=y0=0;
	x3=y3=1;
	
	
	//float t=0.41f;
	
	//3*((1-B8)^2)*B8*$B$4+3*(1-B8)*(B8^2)*$B$5+B8^3
	
	//For example problem, Y=0.0716009
	
	float lowTGuess=0.0f;
	float dividingT=0.5f;
	float highTGuess=1.0f;
	
	float lowXCalc;
	float divideXCalc;
	
	for(int i=0; i<100; ++i)
	{
		lowXCalc=X-parBezFunc(lowTGuess,x1,x2);
		divideXCalc=X-parBezFunc(dividingT,x1,x2);
		
		if(abs(divideXCalc) < 0.0001)
		{
			//cout<<"XCalc close to 0, breaking"<<endl;
			break;
		}
		
		if(glm::dot(lowXCalc,divideXCalc)<0)
		{
			//cout<<"signs different"<<endl;
			highTGuess=dividingT;
			dividingT=(dividingT+lowTGuess)/2.0f;
		}
		else
		{
			//cout<<"signs same"<<endl;
			lowTGuess=dividingT;
			//cout<<"("<<highTGuess<<"+"<<dividingT<<")"<<"/2.0f"<<endl;
			dividingT=(highTGuess+dividingT)/2.0f;
		}
	}
	
	return parBezFunc(dividingT,y1,y2);
	
	//cout<<lowXCalc<<" "<<highXCalc<<" "<<dotProduct<<endl;
	//cout<<glm::dot(5.0,5.0)<<" "<<glm::dot(-5.0,5.0)<<" "<<glm::dot(5.0,-5.0)<<endl;
	
	
	
	//float xT=x0 + (3*t)*(x1) + (3*t*t)*(x2-2*x1) + (t*t*t)*(x3+3*x1-3*x2);
	//float yT=y0 + (3*t)*(y1-y0) + (3*t*t)*(y0+y2-2*y1) + (t*t*t)*(y3-y0+3*y1-3*y2);
	
	return 0.0f;
}

glm::vec4 Vectorize(const glm::quat theQuat)
{
	glm::vec4 ret;

	ret.x = theQuat.x;
	ret.y = theQuat.y;
	ret.z = theQuat.z;
	ret.w = theQuat.w;

	return ret;
}

glm::fquat Lerp(const glm::quat &v0, const glm::quat &v1, float alpha)
{
    glm::vec4 start = Vectorize(v0);
    glm::vec4 end = Vectorize(v1);
    glm::vec4 interp = glm::mix(start, end, alpha);
    interp = glm::normalize(interp);
    return glm::fquat(interp.w, interp.x, interp.y, interp.z);
}

glm::fquat Slerp(glm::quat &v0, glm::quat &v1, float alpha)
{
	float dot = glm::dot(v0, v1);
	
	if(dot<0.0f)
	{
		dot=-dot;
		v0=-v0;
	}
    
	const float DOT_THRESHOLD = 0.9995f;
	if(dot > DOT_THRESHOLD)
		return Lerp(v0, v1, alpha);

	glm::clamp(dot, -1.0f, 1.0f);
	float theta_0 = acosf(dot);
	float theta = theta_0*alpha;
    
	glm::quat v2 = v1 + -v0*dot;
	v2 = glm::normalize(v2);
    
    return v0*cos(theta) + v2*sin(theta);
}

glm::vec4 Lerp(const glm::vec4 &start, const glm::vec4 &end, float alpha)
{
    glm::vec4 interp = glm::mix(start, end, alpha);
    interp = glm::normalize(interp);
    return interp;
}

glm::vec4 Slerp(glm::vec4 &v0, glm::vec4 &v1, float alpha)
{
	float dot = glm::dot(v0, v1);
	
	if(dot<0.0f)
	{
		dot=-dot;
		v0=-v0;
	}
    
	const float DOT_THRESHOLD = 0.9995f;
	if(dot > DOT_THRESHOLD)
		return Lerp(v0, v1, alpha);

	glm::clamp(dot, -1.0f, 1.0f);
	float theta_0 = acosf(dot);
	float theta = theta_0*alpha;
    
	glm::vec4 v2 = v1 + -v0*dot;
	v2 = glm::normalize(v2);
    
    return v0*cos(theta) + v2*sin(theta);
}

float Lerp(const float &start, const float &end, float alpha)
{
    float interp = glm::mix(start, end, alpha);
    interp = glm::normalize(interp);
    return interp;
}

float Slerp(float &v0, float &v1, float alpha)
{
	float dot = glm::dot(v0, v1);
	
	if(dot<0.0f)
	{
		dot=-dot;
		v0=-v0;
	}
    
	const float DOT_THRESHOLD = 0.9995f;
	if(dot > DOT_THRESHOLD)
		return Lerp(v0, v1, alpha);

	//glm::clamp(dot, -1.0f, 1.0f);
	float theta_0 = acosf(dot);
	float theta = theta_0*alpha;
    
	float v2 = v1 + -v0*dot;
	v2 = glm::normalize(v2);
    
    return v0*cos(theta) + v2*sin(theta);
}
