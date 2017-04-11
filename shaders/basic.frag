#version 330

out vec4 color;
in vec2 gsColor;
in vec4 normal;
in vec4 position;
uniform sampler2D colorMap;


void main()
{
  vec3 lightDir = normalize(vec3(0, 0.0, 1));
  float lambertian = max(dot(lightDir, normal.xyz), 0.0);
  float ambient = 0.3; 
  float diffuse = 1. - ambient;
  vec4 baseColor = texture(colorMap, gsColor);
  color = baseColor * (ambient + diffuse * lambertian);
}
