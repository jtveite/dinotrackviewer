#version 330

out vec4 color;
in vec2 vertexUV;
uniform sampler2D slideTex;

void main()
{
  color = texture(slideTex, vertexUV);
}
