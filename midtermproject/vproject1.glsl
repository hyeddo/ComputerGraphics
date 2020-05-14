in vec4 vPosition;

in vec4 vColor; 
out vec4 vs_color;

uniform mat4 mv_matrix;

void main()
{
	gl_Position = mv_matrix*vPosition;
	vs_color = vColor;
}