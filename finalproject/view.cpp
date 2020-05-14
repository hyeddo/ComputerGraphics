#include "cg.h"
#include "camera.h"
#include "vbocube.h"
#include "vboplane.h"
#include "vbomesh.h"
#include "vboteapot.h"
#include "vbotorus.h"
#include "vbosphere.h"
#include "vbotrianglebar.h"

void startup();
void render();
void shutdown();

struct scene {
	int width = 800, height = 600;
	GLuint rendering_program;
	GLuint vertex_array_object;

	vec4 vertices[100];
	vec4 colors[100];
	unsigned int indices[100];
	int num, numIndex;

	// 모델링 변수
	VBOCube *cube;
	VBOPlane *plane;
	VBOSphere *sphere;
	VBOTeapot *teapot;
	VBOMesh *mesh;
	VBOTorus *torus;
	VBOTriangleBar *tb;

	// 뷰잉 변수
	mat4 model, view, proj;
	GLint m_loc, v_loc, proj_loc, color_loc;
	float theta;
};

scene sc;


// 맨 처음 한번 실행
void startup()
{
	//------------------------------------------------------------------------
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);

	sc.rendering_program = compile_shaders("vview.glsl", "fview.glsl");
	glUseProgram(sc.rendering_program);
	
	glGenVertexArrays(1, &sc.vertex_array_object);
	glBindVertexArray(sc.vertex_array_object);

	sc.cube = new VBOCube();
	sc.plane = new VBOPlane(100.0f, 100.0f, 10, 10);
	sc.mesh = new VBOMesh("pig_triangulated.obj", true);
	sc.torus = new VBOTorus(0.7f, 0.3f, 50, 50);
	sc.sphere = new VBOSphere();
	sc.teapot = new VBOTeapot(14, mat4(1.0));
	sc.tb = new VBOTriangleBar();

	sc.view = LookAt(vec3(0.0f, 2.0f, 8.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	float cameraZ = sc.height * 0.5 / tan(radians(0.5*50.0));
	sc.proj = Perspective(50.0f, (float)sc.width/(float)sc.height, (float)0.001*cameraZ, (float)10.0*cameraZ);
//	sc.proj = Perspective(50.0f, 1.33f, 0.5f, 100.0f);
//	sc.proj= Ortho(-5.0, 5.0,-2.0, 8.0, 0.3, 100.0);

	//ortho
	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");
	sc.color_loc = glGetUniformLocation(sc.rendering_program, "color");

	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0))); 
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(sc.proj));	

	sc.theta = 0.0;
}

//  계속해서 실행됨
void idle()
{
	/*sc.theta += 0.001;
	float x = 5.0*sin(sc.theta); float z = 5.0*cos(sc.theta);
	sc.view = LookAt(vec3(x, 1.0f, z), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));*/

	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	glUseProgram(sc.rendering_program);

	// floor
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniform4f(sc.color_loc, 0.3, 0.3, 0.3, 1.0);
	sc.plane->render();
     
	// Cube
	mat4 model_base;
	model_base = translate(mat4(1.0), vec3(0.0, 1.0, 0.0));
	model_base = scale(model_base, vec3(1.0, 2.0, 1.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniform4f(sc.color_loc, 0.3, 0.2, 0.9, 1.0);
	sc.cube->render();

	//torus
	mat4 m0; 
	m0 = translate(mat4(1.0), vec3(3.0, 0.0, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m0));
	glUniform4f(sc.color_loc, 0.8, 0.2, 0.9, 1.0);
	sc.torus->render();

	//sphere
	mat4 m1;
	m1 = translate(mat4(1.0), vec3(-3.0, 1.0, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m1));
	glUniform4f(sc.color_loc, 0.8, 0.4, 0.7, 1.0);
	sc.sphere->render();

	//teapot
	mat4 m2;
	m2 = translate(mat4(1.0), vec3(2.0, 0.0, -4.0));
	m2 = rotate(m2, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m2));
	glUniform4f(sc.color_loc, 0.2, 0.7, 0.7, 1.0);
	sc.teapot->render();

	// obj 
	mat4 m3;
	m3 = translate(mat4(1.0), vec3(-2.0, 1.0, -4.0));
	m3 = scale(m3, vec3(2.0, 2.0, 2.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m3));
	glUniform4f(sc.color_loc, 1.0, 0.7, 0.7, 1.0);
	sc.mesh->render();

	// triangular bar
	mat4 m4;
	m4 = translate(mat4(1.0), vec3(-2.0, 1.0, 4.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(m4));
	glUniform4f(sc.color_loc, 1.0, 0.7, 0.7, 1.0);
	sc.tb->render();

	glutSwapBuffers();
}


void shutdown()
{
	glDeleteVertexArrays(1, &sc.vertex_array_object);
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); 
	// MAC
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);

	glutInitWindowSize(sc.width, sc.height);
	glutCreateWindow("3D Viewing");
	glewInit();
	version();

	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
