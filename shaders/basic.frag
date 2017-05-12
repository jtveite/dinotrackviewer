#version 330

out vec4 color;
in vec2 gsColor;
in float gsCluster;
in vec4 normal;
in vec4 position;
uniform sampler2D colorMap;
uniform float numClusters = 0;

void main()
{
  vec3 lightDir = normalize(vec3(0, 0.0, 1));
  float lambertian = max(dot(lightDir, normal.xyz), 0.0);
  float ambient = 0.3; 
  float diffuse = 1. - ambient;
  vec4 baseColor;// = texture(colorMap, gsColor);

  if (numClusters > 0){
    
    baseColor = texture(colorMap, vec2(gsCluster / numClusters, 0.5));
    if (gsCluster < -0.5){
      baseColor = vec4(0.3, 0.3, 0.4, 1.0);
    }
  }
  else{
    baseColor = texture(colorMap, gsColor);// vec4(1, 0,0,1);
  }
  color = baseColor * (ambient + diffuse * lambertian);
}
