#include "cg.h"
#include "camera.h"
#include "vboplane.h"

void startup();
void render();
void shutdown();

struct scene {
	GLuint rendering_program;
	VBOPlane *plane;
	VBOTeapot *teapot;
	VBOTorus *torus;

	mat4 model, view, proj;
	float angle;

	GLint m_loc, v_loc, proj_loc;
};
scene sc;

const int WIDTH = 800; const int HEIGHT = 600;

void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	/**** Shaders as variables */
	sc.rendering_program = compile_shaders("spotv.glsl", "spotf.glsl");
	glUseProgram(sc.rendering_program);

	// modeling
	sc.plane = new VBOPlane(50.0f, 50.0f, 1, 1);
	sc.torus = new VBOTorus(1.75f*0.75, 0.75f*0.75, 50, 50);
	sc.teapot = new VBOTeapot(14, mat4(1.0f));
	
	// viewing
	sc.angle = 0.0;
	sc.model = mat4(1.0); 
	sc.view = LookAt(vec3(5.0f,5.0f,7.5f), vec3(0.0f,0.75f,0.0f), vec3(0.0f,1.0f,0.0f));
	sc.proj = Perspective(60.0f, (float)WIDTH/HEIGHT, 0.3f, 100.0f);
	
	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");

	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(sc.proj));

	// Lighting
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Spot.Intensity"),0.9,0.9,0.9);
	glUniform1f(glGetUniformLocation(sc.rendering_program,"Spot.exponent"),3.0f);
	glUniform1f(glGetUniformLocation(sc.rendering_program,"Spot.cutoff"),30.0f);
}

#define M_PI 3.141592

void idle(){ 
	sc.angle += 0.01f;
	if (sc.angle > 2.0*M_PI) sc.angle -= 2.0*M_PI;
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.6f, 0.6f, 0.6f, 1.0f); 
	
	glUseProgram(sc.rendering_program);

	// Lighting
	vec4 lightPos = vec4(15.0f * cos(sc.angle), 15.0f, 15.0f * sin(sc.angle), 1.0f);
	vec4 direction = sc.view*vec4(-lightPos); 
	glUniform4fv(glGetUniformLocation(sc.rendering_program,"Spot.Position"),1, value_ptr(sc.view*lightPos));
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Spot.direction"), direction[0], direction[1], direction[2]);

	/******** Mesh  */
	// Teapot
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Ka"), 0.27f, 0.15f, 0.09f);
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Kd"), 0.9f, 0.5f, 0.3f);
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Ks"), 0.95f, 0.95f, 0.95f);
	glUniform1f(glGetUniformLocation(sc.rendering_program,"Material.Shininess"), 100.0f);

	// Matrix
	sc.model = translate(mat4(1.0f), vec3(0.0, 0.0, -2.0)); 
	sc.model = rotate(sc.model, (float)radians(45.0), vec3(0.0, 1.0, 0.0));
	sc.model = rotate(sc.model, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
    // Draw
	sc.teapot->render();

	// Torus
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Ka"), 0.4f, 0.15f, 0.09f);
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Kd"), 0.9f, 0.5f, 0.3f);
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Ks"), 0.95f, 0.95f, 0.95f);
	glUniform1f(glGetUniformLocation(sc.rendering_program,"Material.Shininess"), 100.0f);

	sc.model = translate(mat4(1.0f), vec3(0.0, 0.0, -2.0));
	sc.model = rotate(sc.model, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	sc.torus->render();

	// plane
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Ka"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Kd"), 0.7f, 0.7f, 0.7f);
	glUniform3f(glGetUniformLocation(sc.rendering_program,"Material.Ks"),  0.9f, 0.9f, 0.9f);
	glUniform1f(glGetUniformLocation(sc.rendering_program,"Material.Shininess"), 180.0f);

	sc.model = mat4(1.0f);
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	sc.plane->render();

	glutSwapBuffers();
}

void shutdown()
{
	glDeleteProgram(sc.rendering_program);
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow( "Spot Light" );
	glewInit();
    
	version();
	startup();
	glutDisplayFunc(render); 
    glutIdleFunc(idle);

    glutMainLoop();
	shutdown();
    return 0;
}
