#include "cg.h"
#include "camera.h"
#include "vboplane.h"
#include "vbomesh.h"
#include "vbosphere.h"
#include "vbocube.h"
#include "bmpreader.h"
#include <stdlib.h>
#include <mmsystem.h>
#include <vector>
#include <algorithm>
#pragma comment(lib, "winmm.lib")

void startup();
void render();
void shutdown();
extern void version();
extern GLuint compile_shaders(const char*  vsource, const char*  fsource);

struct scene {
	const int width = 800; const int height = 600;

	GLuint rendering_program;
	GLuint vertex_array_object;

	VBOPlane* floor;
	VBOPlane* sky[4];
	VBOCube* box[10];
	VBOMesh* deer;
	VBOMesh* star;
	VBOMesh* bunny;
	VBOMesh* ring;
	VBOSphere* stone;
	VBOMesh* shoes;
	VBOMesh* bear;
	VBOSphere* snowball[90];

	mat4 model;
	mat4 view;
	mat4 proj;
	float angle;//스팟라이트, 
	float angle2;
	float coverAngle = 0.0;//뚜껑 열리는 각도

	float falling[90] = { 0.0 };//눈송이 떨어지게
	float theta;//카메라 돌기

	GLint v_loc, m_loc, proj_loc, color_loc;
	GLuint tex_object[5];

	vector <mat4> mvs;
};
scene sc;

struct model {
	float BASE_HEIGHT = 0.3;
	float LOWER_HEIGHT = 0.3;
	int UPPER_HEIGHT = 1.0;

	// base rotx, lower roty
	float opt[2] = { 0.0, 0.0 };
};
model present;

float xpos[90], ypos[90], zpos[90];

bool isOpening = false;
bool isOpened = false;
int openingCnt = 0;

int presentNum = 0;

void mouse(GLint button, GLint state, GLint x, GLint y)
{
	//움직이는 동안에 누르면 아무 일도 안 생김
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && isOpening == true) {
		;
	}
	//상자가 닫혀 있는 상태에서 누르면 열림
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && isOpened == false) {
		sc.coverAngle = 0.2;
		isOpening = true;
		presentNum++;
		if (presentNum == 4)
			presentNum = 0;
	}
	//상자가 열려 있는 상태에서 누르면 닫힘
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && isOpened == true) {
		sc.coverAngle = -0.2;
		isOpened = false;
		isOpening = true;
	}
}

void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	glEnable(GL_SMOOTH);
	/**** Shaders as variables */
	sc.rendering_program = compile_shaders("tex_lighting_v.glsl", "tex_lighting_f.glsl");
	glUseProgram(sc.rendering_program);

	// modeling
	sc.floor = new VBOPlane(100.0f, 100.0f, 10, 10);
	sc.deer = new VBOMesh("deer.obj", true);
	sc.star = new VBOMesh("star.obj", true);
	sc.bunny = new VBOMesh("bunny.obj", true);
	sc.ring = new VBOMesh("ring.obj", true);
	sc.stone = new VBOSphere();
	sc.shoes = new VBOMesh("shoes.obj", true);
	sc.bear = new VBOMesh("bear.obj", true);

	for (int i = 0; i < 4; i++) {
		sc.sky[i] = new VBOPlane(100.0f, 50.0f, 10, 10);
	}

	for (int i = 0; i < 10; i++) {
		sc.box[i] = new VBOCube();
	}

	for (int i = 0; i < 90; i++) {
		sc.snowball[i] = new VBOSphere();
	}

	// viewing
	sc.angle = 0.957283f;
	sc.model = mat4(1.0);
	sc.view = LookAt(vec3(0.0f, 2.0f, 10.0f), vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	sc.proj = Perspective(70.0f, (float)sc.width / sc.height, 0.3f, 100.0f);

	float cameraZ = sc.height * 0.5 / tan(radians(0.5 * 50.0));
	sc.proj = Perspective(50.0f, (float)sc.width / (float)sc.height, (float)0.001 * cameraZ, (float)10.0 * cameraZ);

	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");
	sc.color_loc = glGetUniformLocation(sc.rendering_program, "color");
	
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(sc.proj));

	// Lighting
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Light.Intensity"), 0.2, 0.2, 0.2);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Spot.Intensity"), 0.3, 0.3, 0.3);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Spot.exponent"), 3.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Spot.cutoff"), 30.0f);

	//눈송이 위치
	for (int i = 0; i < 90; i++) {
		xpos[i] = rand() % 200 * 0.2 - 20;
		ypos[i] = rand() % 100 * 0.2 - 8;
		zpos[i] = rand() % 200 * 0.2 - 20;
	}

	// Load texture file
	glActiveTexture(GL_TEXTURE0);
	sc.tex_object[0] = BMPReader::loadTex("cover.bmp");
	glActiveTexture(GL_TEXTURE1);
	sc.tex_object[1] = BMPReader::loadTex("snow.bmp");
	glActiveTexture(GL_TEXTURE2);
	sc.tex_object[2] = BMPReader::loadTex("yellow.bmp"); 
	glActiveTexture(GL_TEXTURE3);
	sc.tex_object[3] = BMPReader::loadTex("sky.bmp");
	glActiveTexture(GL_TEXTURE4);
	sc.tex_object[4] = BMPReader::loadTex("soccer.bmp");
}

#define M_PI 3.141592

void idle() {
	sc.angle += 0.003f;
	//if (sc.angle > 2.0 * 3.141592) sc.angle -= 2.0 * 3.141592;

	sc.angle2 += 0.001f;

	//눈송이 아래로
	for (int i = 0; i < 90; i++) {
		sc.falling[i] += 0.002f;
	}
	for (int i = 0; i < 90; i++)
		if ((ypos[i] - sc.falling[i]) < -1.0)
			sc.falling[i] -= 20.0;

	//카메라 애니메이션
	sc.theta += 0.0003f;
	float x = 30.0 * sin(sc.theta); float z = 30.0 * cos(sc.theta);
	sc.view = LookAt(vec3(x, 12.0f, z), vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));

	//뚜껑 열렸다 닫혔다
	if (isOpening) {
		openingCnt++;
		present.opt[0] -= sc.coverAngle;
	}

	if (openingCnt > 450) {
		if (sc.coverAngle >= 0.1)
			isOpened = true;
		sc.coverAngle = 0.0f;
		openingCnt = 0;
		isOpening = false;
	}

	if (isOpened) {
		sc.coverAngle = -0.2;
	}

	present.opt[1] += 0.003f;

	glutPostRedisplay();
}

mat4 m[90];

void makeSnowball(int i, float x, float y, float z)
{
	m[i] = translate(mat4(1.0), vec3(x, y - sc.falling[i], z));
	m[i] = scale(m[i], vec3(0.1, 0.1, 0.1));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m[i]));
	sc.snowball[i]->render();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
	glUseProgram(sc.rendering_program);

	// Lighting
	glUniform4fv(glGetUniformLocation(sc.rendering_program, "Light.Position"), 1, value_ptr(sc.view*vec4(3.0f, 4.0, 3.0f, 1.0)));

	vec4 lightPos = vec4(5.0f * cos(sc.angle), 30.0f, 5.0f * sin(sc.angle), 1.0f);
	//vec4 lightPos = vec4(0.0f,  30.0f, 0.0f, 1.0f);
	vec4 direction = sc.view*vec4(0.0f,-10.0f,0.f,0.f);
	glUniform4fv(glGetUniformLocation(sc.rendering_program, "Spot.Position"), 1, value_ptr(sc.view * lightPos));
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Spot.direction"), direction[0], direction[1], direction[2]);

	/********Object  */
	//box color
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 1.0f, 0.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 1.0f, 1.0f, 0.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 1.0f, 1.0f, 0.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 180.0f);
	glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 2);

	// box_bottom
	mat4 m7;
	m7 = scale(mat4(1.0), vec3(10.0, 0.15, 10.0));
	m7 = translate(m7, vec3(0.0, -11.0, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m7));
	sc.box[0]->render();

	//box_front
	mat4 m8;
	m8 = translate(mat4(1.0), vec3(0.0, 3.0, 4.95));
	m8 = rotate(m8, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	m8 = scale(m8, vec3(10.0, 0.15, 10.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m8));
	sc.box[1]->render();

	//box_left
	mat4 m9;
	m9 = translate(mat4(1.0), vec3(-4.95, 3.0, 0.0));
	m9 = rotate(m9, (float)radians(-90.0), vec3(0.0, 0.0, 1.0));
	m9 = scale(m9, vec3(10.0, 0.15, 10.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m9));
	sc.box[2]->render();

	//box_back
	mat4 m10;
	m10 = translate(mat4(1.0), vec3(0.0, 3.0, -4.95));
	m10 = rotate(m10, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	m10 = scale(m10, vec3(10.0, 0.15, 10.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m10));
	sc.box[3]->render();

	//box_right
	mat4 m11;
	m11 = translate(mat4(1.0), vec3(4.95, 3.0, 0.0));
	m11 = rotate(m11, (float)radians(-90.0), vec3(0.0, 0.0, 1.0));
	m11 = scale(m11, vec3(10.0, 0.15, 10.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m11));
	sc.box[4]->render();

	//box_cover color
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 1.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 1.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 180.0f);
	glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 0);

	//box_cover-hierarchical_base
	sc.model = translate(mat4(1.0), vec3(0.0, 8.0, -5.0));
	sc.model = rotate(sc.model, (float)radians(present.opt[0]), vec3(1.0, 0.0, 0.0));
	sc.mvs.push_back(sc.model);
	sc.model = translate(sc.model, vec3(0.0, present.BASE_HEIGHT / 2.0, 5.0));
	sc.model = scale(sc.model, vec3(10.0, present.BASE_HEIGHT, 10.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	sc.box[5]->render();
	sc.model = sc.mvs.back();
	sc.mvs.pop_back();

	//star color
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.7f, 0.7f, 0.2f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.7f, 0.7f, 0.4f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.3f, 0.3f, 0.3f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 150.0f);

	// star-hierarchical_lower
	sc.model = translate(sc.model, vec3(0.0, present.BASE_HEIGHT+2.0, 0.0));
	sc.mvs.push_back(sc.model);
	sc.model = translate(sc.model, vec3(0.0, present.LOWER_HEIGHT / 2.0 + 2.0, 5.0));
	sc.model = rotate(sc.model, present.opt[1], vec3(0.0, 1.0, 0.0));
	sc.model = scale(sc.model, vec3(0.3, present.LOWER_HEIGHT, 0.40));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	sc.star->render();
	sc.model = sc.mvs.back();
	sc.mvs.pop_back();

	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.7f, 0.7f, 0.7f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 1.0f);

	//ring-hierarchical_upper-1
	if (presentNum == 1) {
	   	sc.model = translate(sc.model, vec3(0.0, present.LOWER_HEIGHT, 0.0));//위치
		sc.mvs.push_back(sc.model);
		sc.model = translate(sc.model, vec3(0.0, -7.0, 4.0));//위치
		sc.model = scale(sc.model, vec3(0.2, 0.2, 0.2));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
		sc.ring->render();
		sc.model = sc.mvs.back();
		sc.mvs.pop_back();

		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.7f, 0.1f, 0.90f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 1.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 1.0f);

		sc.model = translate(sc.model, vec3(0.0, 0.3, 0.0));//위치
		sc.mvs.push_back(sc.model);
		sc.model = translate(sc.model, vec3(0.0, -7.3, 6.3));//위치
		sc.model = scale(sc.model, vec3(0.44, 0.44, 0.44));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
		sc.stone->render();
		sc.model = sc.mvs.back();
		sc.mvs.pop_back();
	}
	
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 1.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 1.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 8.0f);

	//shoes-hierarchical_upper-2
	if (presentNum == 2) {
		sc.model = translate(sc.model, vec3(0.0, present.LOWER_HEIGHT, 0.0));//위치
		sc.mvs.push_back(sc.model);
		sc.model = translate(sc.model, vec3(0.0, -7.0, 5.0));//위치
		sc.model = rotate(sc.model, (float)radians(40.0), vec3(0.0, 0.0, 1.0));
		sc.model = rotate(sc.model, (float)radians(90.0), vec3(1.0, 0.0, 0.0));
		sc.model = scale(sc.model, vec3(0.013, 0.013, 0.013));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
		sc.shoes->render();
		sc.model = sc.mvs.back();
		sc.mvs.pop_back();
	}

	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.3f, 0.3f, 0.3f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 8.0f);
	
	//bear-hierarchical_upper-3
	if (presentNum == 3) {
		sc.model = translate(sc.model, vec3(0.0, present.LOWER_HEIGHT, 0.0));//위치
		sc.mvs.push_back(sc.model);
		sc.model = translate(sc.model, vec3(0.0, -7.0, 5.0));//위치
		sc.model = rotate(sc.model, (float)radians(90.0), vec3(1.0, 0.0, 0.0));
		sc.model = scale(sc.model, vec3(0.15, 0.15, 0.15));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
		sc.bear->render();
		sc.model = sc.mvs.back();
		sc.mvs.pop_back();
	}
	
	//deer color
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.6f, 0.4f, 0.2f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.7f, 0.4f, 0.2f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.1f, 0.1f, 0.1f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 8.0f);

	// deer
	mat4 m2;
	m2 = rotate(mat4(1.0), sc.angle2, vec3(0.0, 1.0, 0.0));
	m2 = translate(m2, vec3(0.0f, 3.0f, -10.0f));
	m2 = rotate(m2, (float)radians(180.0), vec3(0.0, 1.0, 0.0));
	m2 = scale(m2, vec3(0.007, 0.007, 0.007));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m2));
	sc.deer->render();

	//bunny color
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.9f, 0.9f, 0.9f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 180.0f);

	//bunny
	mat4 m4;
	m4 = rotate(mat4(1.0), sc.angle2, vec3(0.0, 1.0, 0.0));
	m4 = translate(m4, vec3(0.0f, 0.0f, 12.0f));
	m4 = rotate(m4, (float)radians(180.0), vec3(0.0, 1.0, 0.0));
	m4 = scale(m4, vec3(25.0, 25.0, 25.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m4));
	sc.bunny->render();

	//floor color
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 180.0f);
	glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 1);  

	// floor
	mat4 m5;
	m5 = translate(mat4(1.0), vec3(0.0, -1.7, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m5));
	sc.floor->render();

	//sky
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 180.0f);
	glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 3);

	mat4 m13;
	m13 = translate(mat4(1.0), vec3(-50.0, 22.0, 0.0));
	m13 = rotate(m13, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	m13 = rotate(m13, (float)radians(-90.0), vec3(0.0, 0.0, 1.0));
	m13 = scale(m13, vec3(1.0, 1.0, 1.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m13));
	sc.sky[0]->render();

	mat4 m14;
	m14 = translate(mat4(1.0), vec3(0.0, 22.0, -50.0));
	m14 = rotate(m14, (float)radians(-90.0), vec3(0.0, 1.0, 0.0));
	m14 = rotate(m14, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	m14 = rotate(m14, (float)radians(-90.0), vec3(0.0, 0.0, 1.0));
	m14 = scale(m14, vec3(1.0, 1.0, 1.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m14));
	sc.sky[1]->render();

	mat4 m15;
	m15 = translate(mat4(1.0), vec3(50.0, 22.0, 0.0));
	m15 = rotate(m15, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	m15 = rotate(m15, (float)radians(90.0), vec3(0.0, 0.0, 1.0));
	m15 = scale(m15, vec3(1.0, 1.0, 1.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m15));
	sc.sky[2]->render();

	mat4 m16;
	m16 = translate(mat4(1.0), vec3(0.0, 22.0, 50.0));
	m16 = rotate(m16, (float)radians(90.0), vec3(0.0, 1.0, 0.0));
	m16 = rotate(m16, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	m16 = rotate(m16, (float)radians(-90.0), vec3(0.0, 0.0, 1.0));
	m16 = scale(m16, vec3(1.0, 1.0, 1.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m16));
	sc.sky[3]->render();

	//snowball color
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 100.0f);
	glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 1);

	//snowball
	for (int i = 0; i < 90; i++) {
		makeSnowball(i, xpos[i], ypos[i], zpos[i]);
	}

	glutSwapBuffers();
}


void shutdown()
{
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(sc.width, sc.height);
	glutCreateWindow("Final Project");
	glewInit();

	PlaySound(TEXT("bgm.wav"), NULL, SND_ASYNC | SND_LOOP);

	version();
	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	//glutSpecialFunc(processSpecialKeys);

	glutMainLoop();
	shutdown();
	return 0;
}
