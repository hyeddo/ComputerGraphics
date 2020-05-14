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

void version();
GLuint compile_shaders(const char* vsource, const char* fsource);

void startup();
void render();
void shutdown();


struct scene {
	int width = 700, height = 600;
	GLuint rendering_program;
	GLuint vertex_array_object;

	vec4 vertices[1000];
	vec4 colors[1000];
	unsigned int indices[100];//gldraw
	int num, num2, numIndex;

	mat4 model;
	GLint mv_location;

	float theta;//회전하는 값은 계속 바뀌니까 theta 추가
};

scene sc;

void make_model()
{
	sc.num = 0;

	//눈사람 몸 아래
	for (float theta = 1.6; theta <= 8; theta += 0.1) {
		sc.vertices[sc.num] = vec4(-0.4 + 0.3 * cos(theta), -0.7 + 0.3 * sin(theta), 0.0, 1.0);
		sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0);
		sc.num++;
	}

	//눈사람 몸 중간
	for (float theta = 1.6; theta <= 8; theta += 0.1) {
		sc.vertices[sc.num] = vec4(-0.4 + 0.2 * cos(theta), -0.3 + 0.2 * sin(theta), 0.0, 1.0);
		sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0);
		sc.num++;
	}

	//눈사람 얼굴!
	for (float theta = 1.6; theta <= 8; theta += 0.1) {
		sc.vertices[sc.num] = vec4(-0.4 + 0.15 * cos(theta), -0.02 + 0.15 * sin(theta), 0.0, 1.0);
		sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0);
		sc.num++;
	}

	//별
	sc.vertices[sc.num] = vec4(0.25, 0.5, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.4, 0.51, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.33, 0.38, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.34, 0.57, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.39, 0.42, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.26, 0.41, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	//별 꼬리
	sc.vertices[sc.num] = vec4(0.40, 0.51, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.72, 0.53, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.73, 0.50, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.39, 0.45, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.725, 0.47, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

	sc.vertices[sc.num] = vec4(0.735, 0.44, 0.0, 1.0);
	sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0);
	sc.num++;

}

// 맨 처음 한번 실행
void startup()
{
	//------------------------------------------------------------------------
	sc.rendering_program = compile_shaders("vproject1.glsl", "fproject1.glsl");

	glGenVertexArrays(1, &sc.vertex_array_object);
	glBindVertexArray(sc.vertex_array_object);

	GLuint buffer[2];

	make_model();


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
	glUseProgram(sc.rendering_program);
	glEnableVertexAttribArray(vColor);

	sc.model = mat4(1.0);
	sc.mv_location = glGetUniformLocation(sc.rendering_program, "mv_matrix");
	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));
}

//  계속해서 실행됨
void idle()
{
	//house 화면 중심을 기준으로 회전
	/*
	sc.model = rotate(sc.model, radians(0.01f), vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));
	*/

	//house 화면 중심을 기준으로 하는 원 위를 따라서 움직이는 애니메이션
	//sc.theta += 0.01f;
	//sc.model = translate(mat4(1.0), vec3(0.5 * cos(radians(sc.theta)), 0.5 * sin(radians(sc.theta)), 0.0));
	//sc.model = scale(sc.model, vec3(0.3, 0.3, 0.0));
	//lUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));

	glutPostRedisplay();
}

void render()
{
	glUseProgram(sc.rendering_program);

	const GLfloat color[] = { 0.f, 0.f, 0.f, 1.f };
	glClearBufferfv(GL_COLOR, 0, color);

	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));

	//glDrawArrays(GL_TRIANGLES, 0, sc.num);

	glDrawArrays(GL_POLYGON, 0, sc.num-12);
	glDrawArrays(GL_TRIANGLES, sc.num-12, sc.num-6);
	glDrawArrays(GL_TRIANGLES, sc.num - 6, sc.num-3);
	glDrawArrays(GL_TRIANGLES, sc.num - 3, sc.num);


	glutSwapBuffers();
}


void shutdown()
{
	glDeleteVertexArrays(1, &sc.vertex_array_object);
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char** argv)//window create. 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(700, 600);
	glutCreateWindow("Winter night");
	glewInit();
	version();

	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
