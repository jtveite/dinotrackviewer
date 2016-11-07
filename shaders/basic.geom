#version 150
layout(points) in;
layout(triangle_strip, max_vertices=85) out;
in vec2 vertexColor[];


out vec2 gsColor;
out vec4 normal;
out vec4 position;

uniform mat4 mvp;

uniform float rad;
//float rad = 0.002;

vec4 get_vertex(float theta, float phi){
  return  vec4(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi), 1.0);
}

vec4 transform(vec4 i){
  return mvp * i;
}

void main ()
{ 
  vec4 pos = gl_in[0].gl_Position;
  float layers_gap = 30;
  float around_gap = 60;
  float tgap = radians(around_gap);
  float pgap = radians(layers_gap);
  float pi = 3.1415926;
  gsColor = vertexColor[0];
  for (float up = 0; up <= pi; up += pgap){
    float phi = up;
    for (float around = 0; around <= 2 * pi; around += tgap){
      float theta = around;
      vec4 offsetDir = get_vertex(theta, phi);
      gl_Position = transform(pos + rad * offsetDir);
      position = gl_Position;
      normal = offsetDir;
      EmitVertex();
      offsetDir = get_vertex(theta, phi + pgap);
      gl_Position = transform(pos + rad * offsetDir);
      position = gl_Position;
      normal = offsetDir;
      EmitVertex();
    }
    EndPrimitive();

  }

}
