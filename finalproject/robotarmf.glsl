#version 330
#extension GL_ARB_explicit_attrib_location:enable

uniform vec3 color;

void main(void)
{
	gl_FragColor = vec4(color, 1.0);
}