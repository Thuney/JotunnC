#version 150

in vec2 position;
in vec2 texCoord;
in vec4 color;

out vec2 v_textureCoordinate;
out vec4 v_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   v_textureCoordinate = texCoord;
   v_color = color;

   gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
}
