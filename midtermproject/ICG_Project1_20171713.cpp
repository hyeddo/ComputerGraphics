#pragma once

#include "cg-2.h"
#include "time.h"

#include <cmath>
#include <iostream>

#pragma comment (lib, "lib/freeglut.lib")
#pragma comment (lib, "lib/glew32.lib")

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "GL/freeglut_std.h"
#include "GL/glut.h"

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;
using namespace glm;

void startup();
void render();
void shutdown();

struct scene {
	int width = 700, height = 600;
	GLuint rendering_program;
	GLuint vertex_array_object;

	vec4 vertices[100];
	vec4 colors[100];
	int color;
	unsigned int indices[100];
	int num, numIndex;

	mat4 model;
	GLint mv_location;
	float theta;
};

scene sc;

void Draw_circle(float x, float y, float r) {
	glUniform4f(sc.color, 1.0, 1.0, 1.0, 1.0);

	sc.colors[1] = { (1.0f, 1.0f, 1.0f, 1.0f) };
	glBegin(GL_TRIANGLE_FAN);
	for (float theta = 0; theta < 6.4; theta += 0.1) {
		glVertex3f(x+r*cos(theta), y+r*sin(theta), 0.f);
	}
	glEnd();
}

void make_model()
{
	sc.num = 0;

	sc.vertices[sc.num] = vec4(0.5, 0.2, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);  sc.num++;
	sc.vertices[sc.num] = vec4(0.0, 0.5, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);  sc.num++;
	sc.vertices[sc.num] = vec4(-0.5, 0.2, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);  sc.num++;

	sc.vertices[sc.num] = vec4(0.5, 0.2, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);  sc.num++;
	sc.vertices[sc.num] = vec4(-0.5, 0.2, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);  sc.num++;
	sc.vertices[sc.num] = vec4(-0.5, -0.5, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 0.0, 0.0, 1.0);  sc.num++;

	sc.vertices[sc.num] = vec4(0.5, 0.2, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);  sc.num++;
	sc.vertices[sc.num] = vec4(-0.5, -0.5, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 0.0, 0.0, 1.0);  sc.num++;
	sc.vertices[sc.num] = vec4(0.5, -0.5, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 0.0, 0.0, 1.0);  sc.num++;
}

// �� ó�� �ѹ� ����
void startup()
{
	//------------------------------------------------------------------------
	sc.rendering_program = compile_shaders("vproject1.glsl", "fproject1.glsl");

	/*
	float ambientLight[] = { 0.5, 0.5, 0.5, 1 };
	float diffuseLight[] = { 0.5, 0.5, 0.5, 1 };
	float specularLight[] = { 1,1,1,1 };
	float lightPosition[] = { 0, 0, 0, 1  }; //������ ��ġ�� �����Ѵ�.  4��° ���ڰ� 1�̴�  ��ǥ�� �ϴ°��� ����

	//////////////////////////////////////////�ֺ��� �л걤 �ݻ籤  ����. ����
	float matAmbient[] = { 1, 1, 1, 1 };
	float matDiff[] = { 1, 1, 1, 1 };
	//////////////////////////////////////////�ֺ���, �л걤 ���� �ݻ��ϴ� ���� ����  1,1,1�̴ϱ� �� �ݻ���.
	glShadeModel(GL_SMOOTH);//�Ų����� ���̵� ����
	glEnable(GL_LIGHTING); // ���� Ȱ��ȭ
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);  ////// LIGHT0�� ���� ������ �����ϴ� ��....
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);  //�ֺ��� ���� ����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);  //�л걤 ���� ����
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);  //�ݻ籤 ���� ����
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);  //���� ��ġ �����ϱ�.
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,0.1);    // �������̴�.
	glEnable(GL_COLOR_MATERIAL); //������ ������ �޵��� �Ѵ�.
	glEnable(GL_LIGHT0);  ///LIGHT0�� �Ҵ�!
	*/

	glGenVertexArrays(1, &sc.vertex_array_object);
	glBindVertexArray(sc.vertex_array_object);

	make_model();

	GLuint buffer[2];
	glGenBuffers(2, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sc.vertices), sc.vertices, GL_STATIC_DRAW);
	GLuint vPosition = glGetAttribLocation(sc.rendering_program, "vPosition");
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vPosition);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sc.colors), sc.colors, GL_STATIC_DRAW);
	GLuint vColor = glGetAttribLocation(sc.rendering_program, "vColor");
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vColor);

	glUseProgram(sc.rendering_program);
	//sc.model = rotate(mat4(1.0), radians(0.0f), vec3(1.0, 1.0, 1.0));
	sc.mv_location = glGetUniformLocation(sc.rendering_program, "mv_matrix");
	sc.color = glGetUniformLocation(sc.rendering_program, "vColor");
	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));
	glUniform4f(sc.color, 1.0, 1.0, 1.0, 1.0);

	sc.theta = 0.0;
}


//  ����ؼ� �����
void idle()
{
	//sc.theta += 0.0001;
	glutPostRedisplay();
}

void render()
{
	glUseProgram(sc.rendering_program);
	
	//���� ����
	const GLfloat color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);
	glShadeModel(GL_SMOOTH);

	/*sc.model = rotate(sc.model, sc.theta, vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));
	glDrawArrays(GL_TRIANGLES, 0, sc.num);
	//	glDrawElements(GL_TRIANGLES, sc.numIndex, GL_UNSIGNED_INT, sc.indices);  
	*/
	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));

	glUniform4f(sc.color, 1.0, 1.0, 1.0, 1.0);
	//����� ���� 3��
	Draw_circle(-0.4, -0.7, 0.3);
	Draw_circle(-0.4, -0.275, 0.2);
	Draw_circle(-0.4, 0.03, 0.15);

	glutSwapBuffers();
}


void shutdown()
{
	glDeleteVertexArrays(1, &sc.vertex_array_object);
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(700, 600);
	glutCreateWindow("20171713 Homework");
	glewInit();
	version();

	startup();

	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
