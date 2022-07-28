#version 150

in vec2 position;
in vec4 color;

out vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * vec4(position, 0.0, 1.0);
   vColor = color;
}
