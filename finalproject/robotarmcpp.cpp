#include "cg.h"
#include "camera.h"
#include "vboplane.h"
#include "vbocube.h"
#include <vector>
#include <algorithm>

void startup();
void render();
void shutdown();

struct scene {
	int width = 800; 
	int height = 600;
	GLuint rendering_program;
	VBOPlane *plane;
	VBOCube *cube;

	mat4 model, view, proj;
	float angle;

	GLint m_loc, v_loc, proj_loc, color_loc;

	vector <mat4> mvs;
};
scene sc;

struct model {
	int BASE_HEIGHT = 2.5;
	int LOWER_HEIGHT = 1.0;
	int UPPER_HEIGHT = 1.0;

	// base tx, base tz, base roty, lower arm rotx, lower arm rotz, upper arm rotx, upper arm rotz
	float opt[7] = { 0.0, 0.0, 30.0, 20.0, 40.0, 20.0, 40.0 };
};
model robot;

void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	/**** Shaders as variables */
	sc.rendering_program = compile_shaders("robotarmv.glsl", "robotarmf.glsl");
	glUseProgram(sc.rendering_program);

	// modeling
	sc.plane = new VBOPlane(50.0f, 50.0f, 1, 1);
	sc.cube = new VBOCube();

	// viewing
	sc.angle = 0.0;
	sc.model = mat4(1.0);
	sc.view = LookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	sc.proj = Perspective(60.0f, (float)sc.width/sc.height, 0.3f, 100.0f);

	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");
	sc.color_loc = glGetUniformLocation(sc.rendering_program, "color");

	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(sc.proj));

}

#define M_PI 3.141592

void idle() {
	//  0         1         2           3              4                  5             6
	// base tx, base tz, base roty, lower arm rotx, lower arm rotz, upper arm rotx, upper arm rotz
	robot.opt[0] += 0.0001;
	robot.opt[1] += 0.0001;

	robot.opt[6] += .5;
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
	glUseProgram(sc.rendering_program);

	// floor
	sc.model = mat4(1.0f);
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	glUniform3f(sc.color_loc, 0.8, 0.8, 0.8);
	sc.plane->render();

	/************************************************/
	// base hierarchical transformation
	sc.model = translate(mat4(1.0f), vec3(robot.opt[0], 0.0, robot.opt[1]));
	sc.model = rotate(sc.model, (float)radians(robot.opt[2]), vec3(0.0, 1.0, 0.0));
	sc.mvs.push_back(sc.model);
	// base instance transformation
	sc.model = translate(sc.model, vec3(0.0, robot.BASE_HEIGHT/2.0, 0.0));
	sc.model = scale(sc.model, vec3(1.0, robot.BASE_HEIGHT, 1.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	glUniform3f(sc.color_loc, 0.3, 0.2, 0.9);
	sc.cube->render();
	sc.model = sc.mvs.back(); sc.mvs.pop_back();

	/***********************************************/
	// Lower Arm hierarchical transformation
	sc.model= translate(sc.model, vec3(0.0, robot.BASE_HEIGHT , 0.0));
	sc.model = rotate(sc.model, (float)radians(robot.opt[4]), vec3(0.0, 0.0, 1.0));
	sc.model = rotate(sc.model, (float)radians(robot.opt[3]), vec3(1.0, 0.0, 0.0));
	sc.mvs.push_back(sc.model);
	// Lower Arm instance transformation
	sc.model = translate(sc.model, vec3(0.0, robot.LOWER_HEIGHT / 2.0, 0.0));
	sc.model = scale(sc.model, vec3(0.2, robot.LOWER_HEIGHT, 0.2));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	glUniform3f(sc.color_loc, 0.9, 0.2, 0.2);
	sc.cube->render();
	sc.model = sc.mvs.back(); sc.mvs.pop_back();

	/***********************************************/
	// Upper Arm hierarchical transformation
	sc.model = translate(sc.model, vec3(0.0, robot.LOWER_HEIGHT, 0.0));
	sc.model = rotate(sc.model, (float)radians(robot.opt[6]), vec3(0.0, 0.0, 1.0));
	sc.model = rotate(sc.model, (float)radians(robot.opt[5]), vec3(1.0, 0.0, 0.0));
	sc.mvs.push_back(sc.model);
	// UPPER Arm instance transformation
	sc.model = translate(sc.model, vec3(0.0, robot.UPPER_HEIGHT / 2.0, 0.0));
	sc.model = scale(sc.model, vec3(0.2, robot.UPPER_HEIGHT, 0.2));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	glUniform3f(sc.color_loc, 0.1, 0.9, 0.3);
	sc.cube->render();
	sc.model = sc.mvs.back(); sc.mvs.pop_back();

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
	glutCreateWindow("Robot Arm");
	glewInit();

	version();
	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
