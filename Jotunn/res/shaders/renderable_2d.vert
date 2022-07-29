#version 150

in vec3 position;
in vec4 color;

out vec4 vColor;

uniform mat4 u_view;

void main()
{
   gl_Position = u_view * vec4(position, 1.0);
   vColor = color;
}