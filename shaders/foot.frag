varying out vec4 color;
uniform sampler2D colorMap;

in vec3 normal;

void main()
{
  vec4 baseColor = 0.7 * vec4(1.0, 0.7, 0.7, 1.0);

  vec3 lightDir = normalize(vec3(0,0,1));

  float lambertian = clamp(dot(lightDir, -normal.xyz), 0., 1.);
  float ambient = 0.4;
  float diffuse = 1. - ambient;
  color = baseColor * (ambient + diffuse * lambertian);
  //color = abs(vec4(normal, 1.0));

}
