#version 330

out vec4 color;
in vec2 gsColor;
in vec3 normal;
uniform sampler2D pathMap;
uniform float clusterId = -1;
uniform float minCutoff = 0.0;
uniform float maxCutoff = 1.0;
uniform float time = -1;

void main()
{
  if ( gsColor.x < minCutoff || gsColor.x > maxCutoff){
    color = vec4(0.0);
    return;
  }
  color = texture(pathMap, gsColor);

 /* vec4 red = vec4(1,0,0,1);
  vec4 blue = vec4(0,0,1,1);
  color = gsColor.x * red + (1-gsColor.x) * blue;*/
  
  if ( abs(time - (1- gsColor.x)) < 0.01){
    color = vec4(1.0, 1.0, 0.2, 1.0);
  }
  //color.g = time;
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
