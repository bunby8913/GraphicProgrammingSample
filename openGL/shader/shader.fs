#version 330 core
out vec4 fragColor;
in vec3 ourPosition;
void main()
{
fragColor = vec4(ourPosition, 1.0);
};
