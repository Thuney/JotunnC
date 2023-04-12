#version 150

out vec4 fColor;
  
in vec2 v_textureCoordinate;

uniform sampler2D u_textures[16];

void main()
{ 
    fColor = texture(u_textures[0], v_textureCoordinate);
}