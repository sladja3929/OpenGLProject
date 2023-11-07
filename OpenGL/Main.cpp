#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <stack>
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

vec3 joint2;
vec3 joint1;
vec3 endPoint;

float findTheta(vec3 t, vec3 e, vec3 j)
{
	vec3 jt = t - j;
	vec3 je = e - j;

	float theta = acos(dot(je, jt) / (length(je) * length(jt)));

	//std::cout << theta * 180 / 3.141592 << std::endl;

	if (cross(je, jt).z > 0) return theta * 180 / 3.141592;
	else return theta * -180 / 3.141592;
}

void computeAngle()
{
	vec3 targetPos = target.GetPosition(g_time);
	float targetAng = atan2(targetPos.x, targetPos.y) * -180 / 3.141592;

	vec4 _endPoint = RotateZ(ang1) * Translate(0, 0.4, 0) * RotateZ(ang2) * Translate(0, 0.4, 0) * RotateZ(ang3) * Translate(-0.2, 0, 0) * vec4(0, 0, 0, 1);
	endPoint.x = _endPoint.x;	endPoint.y = _endPoint.y; endPoint.z = 0;
	float dist1 = length(endPoint - targetPos);

	std::stack<vec3> stack;

	vec4 _joint2 = RotateZ(ang1) * Translate(0, 0.4, 0) * RotateZ(ang2) * Translate(0, 0.4, 0) * vec4(0, 0, 0, 1);
	joint2.x = _joint2.x;	joint2.y = _joint2.y; joint2.z = 0;
	stack.push(joint2);
	float dist2 = length(joint2 - targetPos);

	vec4 _joint1 = RotateZ(ang1) * Translate(0, 0.4, 0) * vec4(0, 0, 0, 1);
	joint1.x = _joint1.x;	joint1.y = _joint1.y; joint1.z = 0;
	stack.push(joint1);
	float dist3 = length(joint1 - targetPos);

	stack.push(vec3(0));

	/*float i = 1;
	if (targetAng < ang1) i = -1.0;
	ang1 += dist1 * i * 5;

	i = 1;
	if (targetAng < ang1 + ang2) i = -1.0;
	ang2 += dist1 * i * 5;

	i = 1;
	if (targetAng < ang1 + ang2 + ang3) i = -1.0;
	ang3 = targetAng - ang1 - ang2;*/

	//std::cout << findTheta(targetPos, endPoint, joint1) << std::endl;

	float theta3 = findTheta(targetPos, endPoint, joint2);
	float theta2 = findTheta(targetPos, endPoint, joint1);
	float theta1 = findTheta(targetPos, endPoint, vec3(0));

	if (theta3 != 0) ang3 += theta3 / 5;
	if (theta3 != 0) ang2 += theta2 / 5;
	if (theta3 != 0) ang1 += theta1 / 5;
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
		//target.Check(program, CTM, endPoint);
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