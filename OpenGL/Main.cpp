#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MyObject.h"

#include <vec.h>
#include <mat.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

MyCube cube;
MyObject object;

GLuint program;
GLuint prog_phong;

GLuint uMat;
GLuint uColor;
mat4 g_Mat = mat4(1.0f);

int winWidth = 500;
int winHeight = 500;

bool bPlay = false;
bool xRotate = true;
bool yRotate = false;
bool zRotate = false;
bool smoothOrFlat = true;
float BoxSize;
float mShiny = 50;		//1~100;
vec4 mSpc = vec4(0.3, 0.3, 0.3, 1);

mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	// Implement your own look-at function
	mat4 V(1.0f);
	vec3 n = at-eye;
	n /= length(n);

	float a = dot(up, n);
	vec3 v = up - a*n;
	v /= length(v);

	vec3 w = cross(n, v);

	mat4 Rw(1.0f);

	Rw[0][0] = w.x;	Rw[0][1] = v.x; Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y; Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z; Rw[2][2] = -n.z;

	mat4 Rc(1.0f);
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			Rc[i][j] = Rw[j][i];

	mat4 Tc = Translate(-eye.x, -eye.y, -eye.z);

	V = Rc*Tc;
		
	return V;
}

mat4 myPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	mat4 P(1.0f);
	
	float rad = fovy * 3.141592 / 180.0;
	
	float sz = 1 / zFar;
	float h = zFar * tan(rad/2);
	
	float sy = 1/h;
	float w = h*aspectRatio;
	float sx = 1/w;

	mat4 S = Scale(sx, sy, sz);
	mat4 M(1.0f);

	float c = -zNear / zFar;
	M[2][2] = 1/(c+1);
	M[2][3] = -c/(c+1);
	M[3][2] = -1;
	M[3][3] = 0;

	P = M*S;

	return P;
}

void myInit()
{
	cube.Init();
	object.Init(vec4(0.1, 0.1, 0.1, 1));
	BoxSize = length(object.MaxBox - object.MinBox);

	program = InitShader("vshader.glsl", "fshader.glsl");
	prog_phong = InitShader("vphong.glsl", "fphong.glsl");
}

void DrawAxis()
{
	glUseProgram(program);
	GLuint uMat = glGetUniformLocation(program, "uMat");
	GLuint uColor = glGetUniformLocation(program, "uColor");

	mat4 x_a = Translate(BoxSize / 3, 0, 0) * Scale(BoxSize / 1.5, BoxSize / 130, BoxSize / 130);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * x_a);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(program);

	mat4 y_a = Translate(0, BoxSize / 3, 0) * Scale(BoxSize / 130, BoxSize / 1.5, BoxSize / 130);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * y_a);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	mat4 z_a = Translate(0, 0, BoxSize / 3) * Scale(BoxSize / 130, BoxSize / 130, BoxSize / 1.5);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * z_a);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(program);
}

float x_Time = 0;
float y_Time = 0;
float z_Time = 0;

void display()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	float aspect = winWidth / (float)winHeight;

	mat4 ModelMat = RotateX(x_Time * 50) * RotateY(y_Time * 50) * RotateZ(z_Time * 50);
	mat4 ViewMat = myLookAt(vec3(0, 0, BoxSize * 1.3), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 ProjMat = myPerspective(45, aspect, 0.01, 100.0f);

	g_Mat = ProjMat * ViewMat * ModelMat;

	DrawAxis();

	ModelMat *= Translate(-(object.MaxBox + object.MinBox) / 2);
	glUseProgram(prog_phong);

	// 1. Define Light Properties
	// 
	vec4 lPos = vec4(0, BoxSize, BoxSize * 1.3, 1);
	vec4 lAmb = vec4(0.5, 0.5, 0.5, 1);
	vec4 lDif = vec4(1, 1, 1, 1);
	vec4 lSpc = lDif;

	// 2. Define Material Properties
	//
	vec4 mAmb = vec4(0.3, 0.3, 0.3, 1);
	vec4 mDif = vec4(0.6, 0.5, 0.6, 1);

	// I = lAmb*mAmb + lDif*mDif*(N¡¤L) + lSpc*mSpc*(V¡¤R)^n; 
	vec4 amb = lAmb * mAmb;
	vec4 dif = lDif * mDif;
	vec4 spc = lSpc * mSpc;

	// 3. Send Uniform Variables to the shader
	//
	GLuint uModelMat = glGetUniformLocation(prog_phong, "uModelMat");
	GLuint uViewMat = glGetUniformLocation(prog_phong, "uViewMat");
	GLuint uProjMat = glGetUniformLocation(prog_phong, "uProjMat");
	GLuint uLPos = glGetUniformLocation(prog_phong, "uLPos");
	GLuint uAmb = glGetUniformLocation(prog_phong, "uAmb");
	GLuint uDif = glGetUniformLocation(prog_phong, "uDif");
	GLuint uSpc = glGetUniformLocation(prog_phong, "uSpc");
	GLuint uShininess = glGetUniformLocation(prog_phong, "uShininess");

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat);
	glUniformMatrix4fv(uViewMat, 1, true, ViewMat);
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat);
	glUniform4f(uLPos, lPos[0], lPos[1], lPos[2], lPos[3]);
	glUniform4f(uAmb, amb[0], amb[1], amb[2], amb[3]);
	glUniform4f(uDif, dif[0], dif[1], dif[2], dif[3]);
	glUniform4f(uSpc, spc[0], spc[1], spc[2], spc[3]);
	glUniform1f(uShininess, mShiny);

	object.Draw(prog_phong, smoothOrFlat);
	
	glutSwapBuffers();
}


void idle()
{
	if (bPlay)
	{
		if(xRotate) x_Time += 0.016;
		else if(yRotate) y_Time += 0.016;
		else if(zRotate) z_Time += 0.016;
	}

	if ((GetAsyncKeyState('3') & 0x8000) == 0x8000 && mSpc.x <= 1)
	{
		mSpc += vec4(0.01, 0.01, 0.01, 0);
		printf("Increasing Specular!\n");
	}

	if ((GetAsyncKeyState('4') & 0x8000) == 0x8000 && mSpc.x >= 0.03)
	{
		mSpc -= vec4(0.01, 0.01, 0.01, 0);
		mSpc -= vec4(0.01, 0.01, 0.01, 0);
		printf("Decreasing Specular!\n");
	}

	if ((GetAsyncKeyState('5') & 0x8000) == 0x8000 && mShiny <= 100)
	{
		mShiny++;
		printf("Increasing Shininess!\n");
	}

	if ((GetAsyncKeyState('6') & 0x8000) == 0x8000 && mShiny >= 7)
	{
		mShiny--;
		printf("Decreasing Shininess!\n");
	} 

	Sleep(16);					// for vSync
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0,0,w,h);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		xRotate = true;
		yRotate = false;
		zRotate = false;
	}

	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		xRotate = false;
		yRotate = true;
		zRotate = false;
	}

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		xRotate = false;
		yRotate = false;
		zRotate = true;
	}
}

void keyboard(unsigned char c, int x, int y)
{
	switch (c)
	{
	case ' ': bPlay = !bPlay; break;
	case '1': smoothOrFlat = true; printf("Using Vertex Normal!\n"); break;
	case '2': smoothOrFlat = false; printf("Using Surface Normal!\n"); break;
	case 'q': glutLeaveMainLoop(); break;
	default: break;
	}
}

int main(int argc, char ** argv)
{
	printf("SIMPLE OBJ MODEL VIEWER\n");
	printf("Programming Assignment #4 for Computer Graphics.  Department of Software, Sejong University\n");
	printf("\n----------------------------------------------------------------\n");
	printf("Spacebar: starting/stoping rotation\n");
	printf("\nLeft Mouse Button: rotating around x-axis\n");
	printf("Middle Mouse Button: rotating around y-axis\n");
	printf("Right Mouse Button: rotating around z-axis\n");
	printf("\n`1' key: Using Vertex Normal for shading\n");
	printf("`2' key: Using Surface Normal for shading\n");
	printf("`3' key: Increasing Specular effect (ks)\n");
	printf("`4' key: Decreasing Specular effect (ks)\n");
	printf("`5' key: Increasing Shininess (n)\n");
	printf("`6' key: Decreasing Shininess (n)\n");
	printf("\n`Q' Key: Exit the program.\n");
	printf("----------------------------------------------------------------\n");

	object.ReadObjFIle();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(winWidth,winHeight);

	glutCreateWindow("OpenGL");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}
