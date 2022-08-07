#version 450 core

in vec3 position;
in vec4 color;

out vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model * vec4(position, 1.0);
   vColor = color;
}