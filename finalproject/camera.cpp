#include "cg.h"

mat4 LookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 p, q, r;
	r = vec3(normalize(eye-at));
	p = vec3(normalize(cross(up, r)));
	q = vec3(normalize(cross(r, p)));

	mat4 Result = mat4(1.0);

	Result[0][0] = p.x;
	Result[1][0] = p.y;
	Result[2][0] = p.z;
	Result[0][1] = q.x;
	Result[1][1] = q.y;
	Result[2][1] = q.z;
	Result[0][2] = r.x;
	Result[1][2] = r.y;
	Result[2][2] = r.z;
	Result[3][0] = -dot(p, eye);
	Result[3][1] = -dot(q, eye);
	Result[3][2] = -dot(r, eye);
	return Result;
}

mat4 Ortho(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	mat4 c = mat4(1.0);
	c[0][0] = 2.0 / (right - left);
	c[1][1] = 2.0 / (top - bottom);
	c[2][2] = 2.0 / (zNear - zFar);
	c[3][3] = 1.0;
	c[0][3] = -(right + left) / (right - left);
	c[1][3] = -(top + bottom) / (top - bottom);
	c[2][3] = -(zFar + zNear) / (zFar - zNear);
	return c;
}

mat4 Frustum(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	mat4 c = mat4(1.0);
	c[0][0] = 2.0*zNear / (right - left);
	c[0][2] = (right + left) / (right - left);
	c[1][1] = 2.0*zNear / (top - bottom);
	c[1][2] = (top + bottom) / (top - bottom);
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0*zFar*zNear / (zFar - zNear);
	c[3][2] = -1.0;
	return c;
}

mat4 Perspective(const GLfloat fovy, const GLfloat aspect,
	const GLfloat zNear, const GLfloat zFar)
{
	GLfloat top = tan(radians(fovy) / 2.0) * zNear;
	GLfloat right = top * aspect;

	mat4 c = mat4(1.0);
	c[0][0] = zNear / right;
	c[1][1] = zNear / top;
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0*zFar*zNear / (zFar - zNear);
	c[3][2] = -1.0;
	return c;
}
