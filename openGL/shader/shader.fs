#version 330 core
out vec4 fragColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
  vec2 reverseTexCoord = vec2(-texCoord.s, texCoord.t);
fragColor = mix(texture(texture1, texCoord), texture(texture2, reverseTexCoord), 0.3);
};
