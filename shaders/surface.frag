varying out vec4 color;
varying in vec2 gsColor;
uniform sampler2D colorMap;

in vec3 normal;

void main()
{
  vec4 baseColor = texture(colorMap, gsColor);

  vec3 lightDir = normalize(vec3(0,0,1));

  float lambertian = abs(dot(lightDir, normal.xyz));
  float ambient = 0.4;
  float diffuse = 1. - ambient;
  color = baseColor * (ambient + diffuse * lambertian);
  //color = abs(vec4(normal, 1.0));
}
