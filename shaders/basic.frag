varying out vec4 color;
in vec4 gsColor;
in vec4 normal;
//uniform sampler2D colorMap;


void main()
{
  vec3 lightDir = normalize(vec3(0, 0.5, 1));
  float lambertian = max(dot(lightDir, normal.xyz), 0.0);
  float ambient = 0.3; 
  float diffuse = 1. - ambient;
  color = gsColor * (ambient + diffuse * lambertian);
}
