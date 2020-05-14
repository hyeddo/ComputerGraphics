#include "vbosphere.h"
#include "cg.h"

#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define RAD (M_PI/180.0f)

VBOSphere::VBOSphere()
{
	std::vector<GLfloat> ballVerts;
	float radius = 1.0f;
	for (int i = 0; i <= 40; i++)
	{
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / 40);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / 40);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		for (int j = 0; j <= 40; j++)
		{
			double lng = 2 * M_PI * (double)(j - 1) / 40;
			double x = cos(lng);
			double y = sin(lng);

			ballVerts.push_back(radius*x * zr0); //X
			ballVerts.push_back(radius*y * zr0); //Y
			ballVerts.push_back(radius*z0);      //Z

			ballVerts.push_back(radius*x * zr1); //X
			ballVerts.push_back(radius*y * zr1); //Y
			ballVerts.push_back(radius*z1);      //Z
		}
	}

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 3200 * 3 * sizeof(float), &ballVerts[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

}

VBOSphere::~VBOSphere()
{
}

void VBOSphere::render()
{
	/*
	Draw a ball
	*/
	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3200);
	//glDrawElements(GL_TRIANGLE_STRIP, 3198, GL_UNSIGNED_BYTE, ((GLubyte*)NULL + (0)));
	/*glBindBuffer(GL_VERTEX_ARRAY, ballVbo);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 7 * 4, 0);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 7 * 4, (void*)(3 * 4));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3200);

	glBindBuffer(GL_ARRAY_BUFFER, 0); */
}
