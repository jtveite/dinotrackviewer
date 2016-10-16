#version 330

out vec4 color;
in vec2 vertexColor;
uniform sampler2D pathMap;

void main()
{
  color = texture(pathMap, vertexColor);
}
