#version 330 core

uniform float newposition;
uniform sampler2D objectTexture;

in Vertex
{
  vec2 texCoord;
  vec4 colour;
} IN;

out vec4 fragCol;

void main(void)
{
  fragCol = texture(objectTexture, IN.texCoord);
}
