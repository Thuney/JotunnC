#version 450 core

in vec3 position;
in vec2 texCoord;
in float texIndex;
in float tilingFactor;

out vec2 v_textureCoordinate;
out float v_textureIndex;
out float v_tilingFactor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   v_textureCoordinate = texCoord;
   v_textureIndex = texIndex;
   v_tilingFactor = tilingFactor;
   
   gl_Position = projection * view * model * vec4(position, 1.0);
}