#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <list>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"

MyCube cube;
MyPyramid pyramid;
MyTarget target(&cube);

GLuint program;
GLuint uMat;

mat4 CTM;

bool bPlay = false;
bool bChasingTarget= false;
bool bDrawTarget = false;

float ang1 = 0;
float ang2 = 0;
float ang3 = 0;

void myInit()
{
	cube.Init();
	pyramid.Init();	

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
}

float g_time = 0;

void drawRobotArm(float ang1, float ang2, float ang3)
{
	mat4 temp = CTM;
	std::list<mat4> stack;	
	stack.push_back(CTM);

	// BASE
	mat4 M(1.0);

	M = Scale(0.05, 0.05, 0.34);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	M = Translate(0, 0, 0.125) * Scale(0.3, 0.3, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	pyramid.Draw(program);

	M = Translate(0, 0, -0.125) * Scale(0.3, 0.3, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	// Upper Arm
	CTM *= RotateZ(ang1);

	M = Translate(0, 0.2, 0) *  Scale(0.1, 0.5, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	// Lower Arm
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);

	M = Scale(0.05, 0.05, 0.24);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	M = Translate(0, 0.2, 0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	M = Translate(0, 0.2, -0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	
	// Hand
	CTM *= Translate(0, 0.4, 0)	* RotateZ(ang3);

	M = Scale(0.05, 0.05, 0.24);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	M = Translate(0, 0, 0) * Scale(0.4, 0.15, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	CTM = temp;
}

vec4 joint1;
vec4 joint2;
vec4 hand;

void computeAngle()
{
	vec3 targetPos = target.GetPosition(g_time);
	float targetAng = atan2(targetPos.x, targetPos.y) * -180 / 3.141592;

	hand = RotateZ(ang1) * Translate(0, 0.4, 0) * RotateZ(ang2) * Translate(0, 0.4, 0) * RotateZ(ang3) * Translate(-0.2, 0, 0) * vec4(0, 0, 0, 1);
	float dist1 = length(hand - targetPos);

	joint1 = RotateZ(ang1) * Translate(0, 0.4, 0) * RotateZ(ang2) * Translate(0, 0.4, 0) * vec4(0, 0, 0, 1);
	float dist2 = length(joint1 - targetPos);

	joint2 = RotateZ(ang1) * Translate(0, 0.4, 0) * vec4(0, 0, 0, 1);
	float dist3 = length(joint2 - targetPos);

	std::cout << targetAng << std::endl;

	float i = 1;
	if (targetAng < ang1) i = -1.0;
	ang1 += dist1 * i * 5;

	i = 1;
	if (targetAng < ang1 + ang2) i = -1.0;
	ang2 += dist1 * i * 5;

	i = 1;
	if (targetAng < ang1 + ang2 + ang3) i = -1.0;
	ang3 = targetAng - ang1 - ang2;
}


void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GLuint uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, -1,-1,-1,-1);

	
	uMat = glGetUniformLocation(program, "uMat");
	CTM = Translate(0, -0.4, 0);// *RotateY(g_time * 30);
	drawRobotArm(ang1, ang2, ang3);	
	

	glUniform4f(uColor, 1,0,0,1);
	if (bDrawTarget == true)
	{
		target.Draw(program, CTM, g_time);
		//target.Check(program, CTM, hand);
		//target.Check(program, CTM, joint1);
		//target.Check(program, CTM, joint2);
	}
		
	
	glutSwapBuffers();
}

void myIdle()
{
	if(bPlay)
	{
		g_time += 1/60.0f;
		Sleep(1/60.0f*1000);

		if (bChasingTarget == false)
		{
			ang1 = 45 * sin(g_time * 3.141592);
			ang2 = 60 * sin(g_time * 2 * 3.141592);
			ang3 = 30 * sin(g_time * 3.141592);
		}
		else
			computeAngle();

		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char c, int x, int y)
{

	switch(c)
	{
	case '1':
		bChasingTarget = !bChasingTarget;
		break;
	case '2':
		bDrawTarget = !bDrawTarget;
		break;
	case '3':
		target.toggleRandom();
		break;
	case ' ':
		bPlay = !bPlay;
		break;
	default:
		break;
	}
}


int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Simple Robot Arm");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);

	glutMainLoop();

	return 0;
}