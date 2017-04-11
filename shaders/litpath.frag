#version 330

out vec4 color;
in vec2 gsColor;
in vec4 normal;
uniform sampler2D pathMap;
uniform float clusterId = -1;
uniform float minCutoff = 0.0;
uniform float maxCutoff = 1.0;

void main()
{
  if ( gsColor.x < minCutoff || gsColor.x > maxCutoff){
    color = vec4(0.0);
    return;
  }
  color = texture(pathMap, gsColor);
  /*
  if(clusterId > -.5){
    color = texture(pathMap, vec2(clusterId));
  }*/

  vec3 lightDir = normalize(-vec3(0, 0.0, 1));
  float lambertian = max(dot(lightDir, normal.xyz), 0.0);
  float ambient = 0.3;
  float diffuse = 1. - ambient;

  color = color * (ambient + diffuse * lambertian);
  
}
