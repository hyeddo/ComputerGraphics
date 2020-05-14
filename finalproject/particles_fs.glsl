#version 430
#extension GL_ARB_explicit_attrib_location:enable

in float Transp;
uniform sampler2D ParticleTex;

out vec4 color;

void main(void)
{
	color = texture(ParticleTex, gl_PointCoord);
	color.a *= Transp;
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}