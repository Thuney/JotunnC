#version 150

in vec3 position;
in vec2 texCoord;

out vec2 v_textureCoordinate;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    v_textureCoordinate = texCoord;
}