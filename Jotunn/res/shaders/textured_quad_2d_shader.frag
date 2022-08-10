#version 150

out vec4 fColor;

in vec2 v_textureCoordinate;
in float v_textureIndex;
in float v_tilingFactor;

uniform sampler2D u_textures[16];

void main()
{

	vec4 texColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	switch(int(v_textureIndex))
	{
		case  0: texColor *= texture(u_textures[0], v_textureCoordinate * v_tilingFactor); break;
		case  1: texColor *= texture(u_textures[1], v_textureCoordinate * v_tilingFactor); break;
		case  2: texColor *= texture(u_textures[2], v_textureCoordinate * v_tilingFactor); break;
		case  3: texColor *= texture(u_textures[3], v_textureCoordinate * v_tilingFactor); break;
		case  4: texColor *= texture(u_textures[4], v_textureCoordinate * v_tilingFactor); break;
		case  5: texColor *= texture(u_textures[5], v_textureCoordinate * v_tilingFactor); break;
		case  6: texColor *= texture(u_textures[6], v_textureCoordinate * v_tilingFactor); break;
		case  7: texColor *= texture(u_textures[7], v_textureCoordinate * v_tilingFactor); break;
		case  8: texColor *= texture(u_textures[8], v_textureCoordinate * v_tilingFactor); break;
		case  9: texColor *= texture(u_textures[9], v_textureCoordinate * v_tilingFactor); break;
		case 10: texColor *= texture(u_textures[10], v_textureCoordinate * v_tilingFactor); break;
		case 11: texColor *= texture(u_textures[11], v_textureCoordinate * v_tilingFactor); break;
		case 12: texColor *= texture(u_textures[12], v_textureCoordinate * v_tilingFactor); break;
		case 13: texColor *= texture(u_textures[13], v_textureCoordinate * v_tilingFactor); break;
		case 14: texColor *= texture(u_textures[14], v_textureCoordinate * v_tilingFactor); break;
		case 15: texColor *= texture(u_textures[15], v_textureCoordinate * v_tilingFactor); break;
	}

	if (texColor.a == 0.0)
		discard;

	fColor = texColor;
}