#version 150

in vec3 vNormal;
in vec4 vColor;

out vec4 fColor;

uniform vec3 u_lightDirection;
uniform vec3 u_lightColor;
uniform float u_ambientStrength;

void main()
{
	vec3 normal = normalize(vNormal);
	float diffuse = max(dot(normal, -normalize(u_lightDirection)), 0.0);
	vec3 lighting = (u_ambientStrength + ((1.0 - u_ambientStrength) * diffuse)) * u_lightColor;

	fColor = vec4(vColor.rgb * lighting, vColor.a);
}
