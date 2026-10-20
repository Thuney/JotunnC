#version 150

in vec2 v_textureCoordinate;
in vec4 v_color;

out vec4 fColor;

uniform sampler2D u_texture;

void main()
{
	fColor = v_color * texture(u_texture, v_textureCoordinate);
}
