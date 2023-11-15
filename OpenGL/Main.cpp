#include <Windows.h>
#include <vgl.h>
#include <InitShader.h>
#include <MyCube.h>

#include <vec.h>
#include <mat.h>

MyCube cube;
GLuint program;

GLuint uMat;
GLuint uColor;
mat4 g_Mat = mat4(1.0f);

float g_aspect = 1.0f;

mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	// Implement your own look-at function
	mat4 V(1.0f);
	vec3 n = at - eye;
	n /= length(n);

	float a = dot(up, n);
	vec3 v = up - a * n;
	v /= length(v);

	vec3 w = cross(n, v);

	mat4 Rw(1.0f);

	Rw[0][0] = w.x;	Rw[0][1] = v.x; Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y; Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z; Rw[2][2] = -n.z;

	//mat4 Rc = transpose(Rw);
	mat4 Rc(1.0);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Rc[i][j] = Rw[j][i];
		}
	}
	mat4 Tc = Translate(-eye.x, -eye.y, -eye.z);

	V = Rc * Tc;

	return V;
}

mat4 myOrtho(float l, float r, float b, float t, float zNear, float zFar)
{
	// Implement your own Ortho function
	mat4 V(1.0f);

	return V;
}

mat4 myPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	// Implement your own Perspective function

	float theta = fovy / 180.0f * 3.141592;
	float ymax = zFar * tan(theta / 2);
	float xmax = aspectRatio * ymax;

	mat4 S = Scale(1 / xmax, 1 / ymax, 1 / zFar);

	//unhinging process matrix
	float c = -(zNear / zFar);
	mat4 M(1.0f);
	M[2][2] = 1 / (c + 1);	M[2][3] = -c / (c + 1);
	M[3][2] = -1;			M[3][3] = 0;

	mat4 P(1.0f);
	P = M * S;
	return P;
}


void myInit()
{
	cube.Init();

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat);
}

void DrawAxis()
{
	glUseProgram(program);
	mat4 x_a = Translate(0.5, 0, 0) * Scale(1, 0.05, 0.05);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * x_a);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(program);

	mat4 y_a = Translate(0, 0.5, 0) * Scale(0.05, 1, 0.05);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * y_a);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	mat4 z_a = Translate(0, 0, 0.5) * Scale(0.05, 0.05, 1);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * z_a);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(program);
}

void DrawGrid()
{
	glUseProgram(program);
	float n = 40;
	float w = 10;
	float h = 10;

	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(0, -0.5, -h + 2 * h / n * i) * Scale(w * 2, 0.02, 0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(-w + 2 * w / n * i, -0.5, 0) * Scale(0.02, 0.02, h * 2);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
}

float g_time = 0;
float angle = 0;
mat4 ModelMat(1.0);
void display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	mat4 ViewMat = myLookAt(vec3(1, 1, 1), vec3(0, 0, 0), vec3(0, 1, 0));
	//focal length(mm = 24mm = full frame): lens camera property(attribute)
	mat4 ProjMat = myPerspective(110, g_aspect, 0.1, 10); //fov, aspectRatio, near, far

	g_Mat = ProjMat * ViewMat;

	DrawAxis();
	DrawGrid();

	//ModelMat = RotateY(angle);
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);

	Sleep(16);					// for vSync
	glutSwapBuffers();
}


void idle()
{

	g_time += 0.016f;
	Sleep(16);

	if ((GetAsyncKeyState('A') & 0x8000) == 0x8000)			// if "A" key is pressed
		ModelMat = RotateY(2) * ModelMat;
	if ((GetAsyncKeyState('D') & 0x8000) == 0x8000)			// if "D" key is pressed
		ModelMat = RotateY(-2) * ModelMat;
	if ((GetAsyncKeyState('W') & 0x8000) == 0x8000)			// if "A" key is pressed
		ModelMat = RotateX(2) * ModelMat;
	if ((GetAsyncKeyState('S') & 0x8000) == 0x8000)			// if "D" key is pressed
		ModelMat = RotateX(-2) * ModelMat;

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	g_aspect = (float)w / h;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);

	glutCreateWindow("OpenGL");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}
