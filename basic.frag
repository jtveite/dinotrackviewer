varying out vec4 color;
in vec4 gsColor;

void main()
{
  color = vec4(1.0, 1.0, 0.0, 1.0);
  color = gsColor;
}
