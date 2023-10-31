#include <vgl.h>
#include <InitShader.h>
#include <vec.h>
#include "MyPlain.h"

MyPlain Plain;

bool bPlay = false;
bool bWave = false;

float mytime = 0;

void myInit()
{
	Plain.init();
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	Plain.draw(mytime, bWave);

	glutSwapBuffers();
}
void myIdle()
{
	if (bPlay != true) return;

	Sleep(16);
	mytime += 0.016;

	glutPostRedisplay();
}
void myKeyboard(unsigned char c, int x, int y)
{
	printf("Division: %d, Num.of Triangles: %d, Num. of Vertices: %d\n", Plain.division, Plain.numTriangle, Plain.numVertex);

	if (c == ' ')
	{
		bPlay = !bPlay;
	}
		
	else if (c == '1' && Plain.division > 2)
	{
		Plain.division--;
		Plain.calculatePos();
		Plain.copyDataToGpu();
		glutPostRedisplay();
	}
		
	else if (c == '2')
	{
		Plain.division++;
		Plain.calculatePos();
		Plain.copyDataToGpu();
		glutPostRedisplay();
	}
		
	else if (c == 'w')
	{
		bWave = !bWave;
		glutPostRedisplay();
	}

	else if (c == 'q')
	{
		glutLeaveMainLoop();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Wave Plain");

	glewExperimental = true;
	glewInit();

	myInit();

	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);
	glutKeyboardFunc(myKeyboard);

	glutMainLoop();

	return 0;
}