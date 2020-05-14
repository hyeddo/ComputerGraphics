#pragma once

#include "cg-2.h"

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
	int num;

	mat4 model, model1;
	GLint mv_location;

	float theta;//È¸ÀüÇÏ´Â °ªÀº °è¼Ó ¹Ù²î´Ï±î theta Ãß°¡
	float move=0.8f;
	float size=1.f;
};

scene sc;

void make_model()
{
	sc.num = 0;
	//¶¥
	sc.vertices[sc.num] = vec4(-1.0, -1.0, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(-1.0, -0.85, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(1.0, -0.85, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(1.0, -1.0, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0); sc.num++;
	//´«»ç¶÷ ¸ö ¾Æ·¡
	for (float theta = 1.6; theta <= 7.9; theta += 0.1) {
		sc.vertices[sc.num] = vec4(-0.4 + 0.3 * cos(theta), -0.7 + 0.3 * sin(theta), 0.0, 1.0);
		sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0);
		sc.num++;
	}
	//´«»ç¶÷ ¸ö Áß°£
	for (float theta = 1.6; theta <= 7.9; theta += 0.1) {
		sc.vertices[sc.num] = vec4(-0.4 + 0.2 * cos(theta), -0.3 + 0.2 * sin(theta), 0.0, 1.0);
		sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0);
		sc.num++;
	}
	//´«»ç¶÷ ¾ó±¼!
	for (float theta = 1.6; theta <= 7.9; theta += 0.1) {
		sc.vertices[sc.num] = vec4(-0.4 + 0.15 * cos(theta), -0.02 + 0.15 * sin(theta), 0.0, 1.0);
		sc.colors[sc.num] = vec4(1.0, 1.0, 1.0, 1.0);
		sc.num++;
	}
	//´«»ç¶÷ ´«
	for (float theta = 1.6; theta <= 7.9; theta += 0.1) {
		sc.vertices[sc.num] = vec4(-0.48 + 0.02 * cos(theta), 0.02 + 0.02 * sin(theta), 0.0, 1.0);
		sc.colors[sc.num] = vec4(0.0, 0.0, 0.0, 1.0);
		sc.num++;
	}
	for (float theta = 1.6; theta <= 7.9; theta += 0.1) {
		sc.vertices[sc.num] = vec4(-0.32 + 0.02 * cos(theta), 0.02 + 0.02 * sin(theta), 0.0, 1.0);
		sc.colors[sc.num] = vec4(0.0, 0.0, 0.0, 1.0);
		sc.num++;
	}
	//´«»ç¶÷ ÄÚ
	sc.vertices[sc.num] = vec4(-0.4, -0.01, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 0.5, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(-0.4, -0.06, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 0.5, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(-0.20, -0.04, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 0.5, 0.0, 1.0); sc.num++;
	//´«»ç¶÷ ÆÈ
	sc.vertices[sc.num] = vec4(-0.2, -0.3, 0.0, 1.0); sc.colors[sc.num] = vec4(0.8, 0.4, 0.2, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(-0.2, -0.33, 0.0, 1.0); sc.colors[sc.num] = vec4(0.8, 0.4, 0.2, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.2, -0.22, 0.0, 1.0); sc.colors[sc.num] = vec4(0.8, 0.4, 0.2, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.2, -0.25, 0.0, 1.0); sc.colors[sc.num] = vec4(0.8, 0.4, 0.2, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(-0.6, -0.3, 0.0, 1.0); sc.colors[sc.num] = vec4(0.8, 0.4, 0.2, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(-0.6, -0.33, 0.0, 1.0); sc.colors[sc.num] = vec4(0.8, 0.4, 0.2, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(-0.98, -0.22, 0.0, 1.0); sc.colors[sc.num] = vec4(0.8, 0.4, 0.2, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(-0.98, -0.25, 0.0, 1.0); sc.colors[sc.num] = vec4(0.8, 0.4, 0.2, 1.0); sc.num++;
	//º°
	sc.vertices[sc.num] = vec4(0.25, 0.7, 0.0, 1.0 ); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4( 0.4, 0.71, 0.0, 1.0 ); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4( 0.33, 0.58, 0.0, 1.0 ); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4( 0.34, 0.77, 0.0, 1.0 ); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.39, 0.62, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.26, 0.61, 0.0, 1.0 ); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	// º° ²¿¸®
	sc.vertices[sc.num] = vec4(0.40, 0.71, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.72, 0.73, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.73, 0.70, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.39, 0.65, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.725, 0.67, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
	sc.vertices[sc.num] = vec4(0.735, 0.64, 0.0, 1.0); sc.colors[sc.num] = vec4(1.0, 1.0, 0.0, 1.0); sc.num++;
}

// ¸Ç Ã³À½ ÇÑ¹ø ½ÇÇà
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

	sc.mv_location = glGetUniformLocation(sc.rendering_program, "mv_matrix");
	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));
}

//  °è¼ÓÇØ¼­ ½ÇÇàµÊ
void idle()
{
	sc.size += 0.0001f;
	if (sc.size > 1.35f) sc.size =1.0f;

	sc.move -= 0.0007f;
	if (sc.move < -1.65f) sc.move = 0.8f;

	glutPostRedisplay();
}

void render()
{
	glUseProgram(sc.rendering_program);

	const GLfloat color[] = { 0.f, 0.f, 0.f, 1.f };
	glClearBufferfv(GL_COLOR, 0, color);

	sc.model1 = scale(mat4(1.0), vec3(sc.size, sc.size, 0.0));//´«»ç¶÷ scaling
	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model1));
	glDrawArrays(GL_QUADS, 0, 4);//¶¥
	glDrawArrays(GL_POLYGON, 4, 192);//´«»ç¶÷ ¸ö+¸Ó¸®
	glDrawArrays(GL_TRIANGLE_FAN, sc.num - 151, 64);//´«»ç¶÷ ´«
	glDrawArrays(GL_TRIANGLE_FAN, sc.num-87, 64);//´«»ç¶÷ ´«
	glDrawArrays(GL_TRIANGLES, sc.num - 23, 3);//´«»ç¶÷ ÄÚ
	glDrawArrays(GL_QUADS, sc.num - 20, 4);//´«»ç¶÷ ÆÈ
	glDrawArrays(GL_QUADS, sc.num - 16, 4);//´«»ç¶÷ ÆÈ
	sc.model = translate(mat4(1.0), vec3(sc.move, 0.2 * sc.move, 0.0));//º° translating
	glUniformMatrix4fv(sc.mv_location, 1, GL_FALSE, value_ptr(sc.model));
	glDrawArrays(GL_TRIANGLES, sc.num-12, 6);//º°
	glDrawArrays(GL_TRIANGLES, sc.num - 6, 3);//º°²¿¸®1
	glDrawArrays(GL_TRIANGLES, sc.num - 3, 3);//º°²¿¸®2

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
