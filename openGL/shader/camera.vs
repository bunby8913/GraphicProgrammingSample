#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 view;
uniform mat4 projection;
void main()
{
texCoord = aTexCoord;
gl_Position = projection * view  * vec4(aPos, 1.0);
};
