#version 150

in vec3 position;
in vec3 normal;
in vec4 color;

out vec3 vNormal;
out vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model * vec4(position, 1.0);
   vNormal = mat3(model) * normal;
   vColor = color;
}
