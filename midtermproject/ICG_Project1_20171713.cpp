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

// 맨 처음 한번 실행
void startup()
{
	//------------------------------------------------------------------------
	sc.rendering_program = compile_shaders("vproject1.glsl", "fproject1.glsl");

	/*
	float ambientLight[] = { 0.5, 0.5, 0.5, 1 };
	float diffuseLight[] = { 0.5, 0.5, 0.5, 1 };
	float specularLight[] = { 1,1,1,1 };
	float lightPosition[] = { 0, 0, 0, 1  }; //조명의 위치를 설정한다.  4번째 인자가 1이니  좌표로 하는것임 ㅇㅇ

	//////////////////////////////////////////주변광 분산광 반사광  설정. ㅇㅇ
	float matAmbient[] = { 1, 1, 1, 1 };
	float matDiff[] = { 1, 1, 1, 1 };
	//////////////////////////////////////////주변광, 분산광 성분 반사하는 정도 결정  1,1,1이니까 다 반사임.
	glShadeModel(GL_SMOOTH);//매끄러운 셰이딩 적용
	glEnable(GL_LIGHTING); // 조명 활성화
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);  ////// LIGHT0에 대한 재질을 설정하는 것....
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);  //주변광 성분 설정
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);  //분산광 성분 설정
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);  //반사광 성분 설정
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);  //광원 위치 설정하기.
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,0.1);    // 감쇠율이다.
	glEnable(GL_COLOR_MATERIAL); //재질에 영향을 받도록 한다.
	glEnable(GL_LIGHT0);  ///LIGHT0을 켠다!
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


//  계속해서 실행됨
void idle()
{
	//sc.theta += 0.0001;
	glutPostRedisplay();
}

void render()
{
	glUseProgram(sc.rendering_program);
	
	//배경색 지정
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
	//눈사람 몸통 3개
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
