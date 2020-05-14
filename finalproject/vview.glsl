#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 proj_matrix;

void main()
{
	gl_Position = proj_matrix*v_matrix*m_matrix*vec4(vPosition, 1.0);
}