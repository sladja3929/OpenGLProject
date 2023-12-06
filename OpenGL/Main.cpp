#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MySphere.h"
#include "Targa.h"

#include <vec.h>
#include <mat.h>

MyCube cube;
MySphere sphere;

GLuint program;
GLuint phong_prog;
GLuint cube_prog;

mat4 g_Mat = mat4(1.0f);

float g_Time = 0;

float g_aspect = 1;

mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	mat4 V = mat4(1.0f);
	
	up = normalize(up);
	vec3 n = normalize(at - eye);
	float a = dot(up, n);
	vec3 v = normalize(up-a*n);
	vec3 w = cross(n, v);

	V[0] = vec4(w, dot(-w, eye));
	V[1] = vec4(v, dot(-v, eye));
	V[2] = vec4(-n, dot(n, eye));
	
	return V;
}

mat4 myOrtho(float l, float r, float b, float t, float zNear, float zFar)
{
	vec3 center = vec3((l+r)/2, (b+t)/2, -(zNear)/2);
	mat4 T = Translate(-center);
	mat4 S = Scale(2/(r-l), 2/(t-b), -1/(-zNear+zFar));
	mat4 V = S*T;

	return V;
}

mat4 myPerspective(float angle, float aspect, float zNear, float zFar)
{
	float rad = angle*3.141592/180.0f;
	mat4 V(1.0f);
	float h = 2*zFar*tan(rad/2);
	float w = aspect*h;
	mat4 S = Scale(2/w, 2/h, 1/zFar);

	float c = -zNear/zFar;
	
	mat4 Mpt(1.0f);
	Mpt[2] = vec4(0, 0, 1/(c+1), -c/(c+1));
	Mpt[3] = vec4(0, 0, -1, 0);
	

	V = Mpt*S;

	return V;

}


void myInit()
{
	cube.Init();
	sphere.Init(40,40);

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	
	phong_prog = InitShader("vPhong.glsl", "fPhong.glsl");
	glUseProgram(phong_prog);

	cube_prog = InitShader("vcube.glsl", "fcube.glsl");

	
}

void DrawAxis()
{
	glUseProgram(program);
	GLuint uMat = glGetUniformLocation(program, "uMat");
	GLuint uColor = glGetUniformLocation(program, "uColor");

	mat4 x_a= Translate(0.5,0,0)*Scale(1,0.05,0.05);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*x_a);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(program);

	mat4 y_a= Translate(0,0.5,0)*Scale(0.05,1,0.05);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*y_a);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	mat4 z_a= Translate(0,0,0.5)*Scale(0.05,0.05,1);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*z_a);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(program);
}

#include "Targa.h"

void myInitTexuture()
{
	GLuint cubeMap;
	glGenTextures(1, &cubeMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	STGA img;

	img.loadTGA("church_posx.tga");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RG, img.width, img.height, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	img.loadTGA("church_negx.tga");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RG, img.width, img.height, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	img.loadTGA("church_negy.tga");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RG, img.width, img.height, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	img.loadTGA("church_posY.tga");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RG, img.width, img.height, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	img.loadTGA("church_posz.tga");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RG, img.width, img.height, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	img.loadTGA("church_negx.tga");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RG, img.width, img.height, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAP_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


}

void display()
{
	glClearColor(0,0,0,1);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	vec3 ePos = vec3(4*cos(g_Time*1/3.141592),1,4*sin(g_Time*1/3.141592));
	
	mat4 ModelMat = RotateY(g_Time*30.0f);
	mat4 ViewMat = myLookAt(ePos, vec3(0,0,0), vec3(0,1,0));
	mat4 ProjMat = myPerspective(45, g_aspect, 0.01, 100.0f);

	g_Mat = ProjMat*ViewMat*ModelMat;
	
	DrawAxis();


	glUseProgram(cube_prog);
	GLuint uMat = glGetUniformLocation(cube_prog, "uMat");
	GLuint uCubeTex = glGetUniformLocation(cube_prog, "uCubeTex");
	glUniformMatrix4fv(uMat, 1, GL_TRUE, ProjMat*ViewMat*Scale(1,1,1));
	glUniform1i(uCubeTex, 0);

	cube.Draw(cube_prog);




	glUseProgram(phong_prog);
	
	// 1. Define Light Properties
	// 
	vec4 lPos = vec4(2, 2, 0, 1);			 
	vec4 lAmb = vec4(0.5, 0.5, 0.5, 1);		
	vec4 lDif = vec4(1, 1, 1, 1);
	vec4 lSpc = lDif; 

	// 2. Define Material Properties
	//
	vec4 mAmb = vec4(0.3, 0.3, 0.3, 1);		
	vec4 mDif = vec4(1.0, 1.0, 1.0, 1);		
	vec4 mSpc = vec4(0.3, 0.3, 0.3, 1); 
	float mShiny = 50;										//1~100;

	// I = lAmb*mAmb + lDif*mDif*(N¡¤L) + lSpc*mSpc*(V¡¤R)^n; 
	vec4 amb = lAmb*mAmb;					
	vec4 dif = lDif*mDif;					
	vec4 spc = lSpc*mSpc; 

	// 3. Send Uniform Variables to the shader
	//
	GLuint uModelMat = glGetUniformLocation(phong_prog, "uModelMat");
	GLuint uViewMat = glGetUniformLocation(phong_prog, "uViewMat");
	GLuint uProjMat = glGetUniformLocation(phong_prog, "uProjMat"); 
	GLuint uLPos = glGetUniformLocation(phong_prog, "uLPos"); 
	GLuint uAmb = glGetUniformLocation(phong_prog, "uAmb"); 
	GLuint uDif = glGetUniformLocation(phong_prog, "uDif"); 
	GLuint uSpc = glGetUniformLocation(phong_prog, "uSpc"); 
	GLuint uShininess = glGetUniformLocation(phong_prog, "uShininess"); 


	glUniformMatrix4fv(uModelMat, 1, true, ModelMat); 
	glUniformMatrix4fv(uViewMat, 1, true, ViewMat); 
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat); 
	glUniform4f(uLPos, lPos[0], lPos[1], lPos[2], lPos[3]); 
	glUniform4f(uAmb, amb[0], amb[1], amb[2], amb[3]); 
	glUniform4f(uDif, dif[0], dif[1], dif[2], dif[3]); 
	glUniform4f(uSpc, spc[0], spc[1], spc[2], spc[3]); 
	glUniform1f(uShininess, mShiny); 

//	sphere.Draw(phong_prog);

	glutSwapBuffers();
}


void idle()
{
	g_Time += 0.016;
	Sleep(16);					// for vSync
	glutPostRedisplay();
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	
	g_aspect = w/float(h);
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(512,512);

	glutCreateWindow("Shader Test");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n",	glGetString(GL_VERSION),
									glGetString(GL_SHADING_LANGUAGE_VERSION));

	
	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}
