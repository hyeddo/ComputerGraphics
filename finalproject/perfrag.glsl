#version 430

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 proj_matrix;

out vec3 Position;
out vec3 Normal;

void getEyeSpace(out vec3 norm, out vec3 position)
{
	position = vec3(v_matrix*m_matrix*vec4(vPosition, 1.0));
	norm = normalize(mat3(v_matrix*m_matrix)*vNormal);
}

void main()
{
	//getEyeSpace(Normal, Position);

	Normal = normalize(mat3(v_matrix*m_matrix)*vNormal);
	Position = vec3(v_matrix*m_matrix*vec4(vPosition, 1.0));
	gl_Position = proj_matrix*v_matrix*m_matrix*vec4(vPosition,1.0);
}