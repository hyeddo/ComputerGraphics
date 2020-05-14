#version 430

//layout (location = 0) in vec3 VertexInitPos;
layout (location = 0) in vec3 VertexInitVel;
layout (location = 1) in float StartTime;

out float Transp;

uniform float Time;
uniform vec3 Gravity; 
uniform float ParticleLifetime;

uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 proj_matrix;

void main()
{
	vec3 pos = vec3(0.0);
	Transp = 0.0;
	
	if (Time > StartTime) {
		float t = Time - StartTime;

		if (t < ParticleLifetime) {
			//pos = VertexInitPos + VertexInitVel * t + Gravity * t * t;
			pos = VertexInitVel * t + Gravity * t * t;
			Transp = 1.0 - t / ParticleLifetime;
		}
	}

	gl_Position = proj_matrix*v_matrix*m_matrix*vec4(pos,1.0);
}