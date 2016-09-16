varying out vec4 color;
varying in vec2 vertexColor;
uniform sampler2D pathMap;

void main()
{
  color = texture(pathMap, vertexColor);
}
