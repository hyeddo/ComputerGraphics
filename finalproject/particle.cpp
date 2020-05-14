#include "cg.h"
#include "camera.h"
#include "vboplane.h"
#include "bmpreader.h"
#include <vector>
#include <algorithm>


struct scene {
	int width = 400;
	int height = 400;
	GLuint rendering_program;
	VBOPlane *plane;
	mat4 model, view, proj;

	GLint m_loc, v_loc, proj_loc, color_loc;

	GLuint initVel, startTime, particles; GLuint nParticles;
	float t, angle;
};
scene sc;

void startup();
void render();
void shutdown();
void initBuffers();

float randFloat() {
	return ((float)rand() / RAND_MAX);
}
void startup()
{
	sc.t = 0.0;
	/**** Shaders as variables */
	sc.rendering_program = compile_shaders("particles_vs.glsl", "particles_fs.glsl");
	glUseProgram(sc.rendering_program);

	// modeling
	sc.plane = new VBOPlane(13.0f, 10.0f, 200, 2);

	// matrices
	mat4 model = mat4(1.0); sc.angle = pi<float>() / 2.0; sc.t = 0.0f; 
	mat4 view = LookAt(vec3(3.0f, 1.5f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 projection = Perspective(60.0f, (float)sc.width/sc.height, 0.3f, 100.0f);

	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");

	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(projection));

	// Generate our vertex buffers
	initBuffers();
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Gravity"), 0.0, -0.05, 0.0);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "ParticleLifetime"), 8.0);

	// the particle texture
	glActiveTexture(GL_TEXTURE0);
	BMPReader::loadTex("bluewater.bmp");
	glUniform1i(glGetUniformLocation(sc.rendering_program, "ParticleTex"), 0);
}

void initBuffers()
{
	sc.nParticles = 8000;

	// Generate the buffers 
	glGenBuffers(1, &sc.initVel);   // Initial velocity buffer 
	glGenBuffers(1, &sc.startTime); // Start time buffer 

	// Allocate space for all buffers 
	int size = sc.nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, sc.initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sc.startTime);
	glBufferData(GL_ARRAY_BUFFER, sc.nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities 
	vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat *data = new GLfloat[sc.nParticles * 3];
	for (unsigned int i = 0; i < sc.nParticles; i++) {
		theta = mix(0.0f, pi<float>() / 6.0f, randFloat()); 
		phi = mix(0.0f, (float)two_pi<float>(), randFloat());
		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);
		velocity = mix(5.25f, 10.5f, randFloat());
		v = normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, sc.initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	delete[] data;

	// Fill the start time buffer 
	float *data1 = new GLfloat[sc.nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < sc.nParticles; i++) {
		data1[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, sc.startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sc.nParticles * sizeof(float), data1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data1;

	// Attach these to the torus's vertex array 
	glGenVertexArrays(1, &sc.particles);
	glBindVertexArray(sc.particles);

	glBindBuffer(GL_ARRAY_BUFFER, sc.initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, sc.startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
}

void idle() {
	sc.t += 0.002f;
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	glUseProgram(sc.rendering_program);

	glUniform1f(glGetUniformLocation(sc.rendering_program, "Time"), sc.t);

	// Disable depth test
	glDisable(GL_DEPTH_TEST);
	// Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable Point Size. Enable Point Coord.
	glEnable(GL_PROGRAM_POINT_SIZE); glPointSize(20.0f);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

	glDrawArrays(GL_POINTS, 0, sc.nParticles);

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
	glutCreateWindow("Particle");
	glewInit();

	version();
	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}

