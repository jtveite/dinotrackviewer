#version 150
layout(points) in;
layout(triangle_strip, max_vertices=128) out;
in vec4 vertexColor[];


out vec4 gsColor;

uniform mat4 mvp;
float rad = 0.0002;

vec4 get_vertex(float theta, float phi){
  return rad * vec4(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi), 1.0);
}

vec4 transform(vec4 i){
  return mvp *  i;
}

void main ()
{ 
  vec4 pos = gl_in[0].gl_Position;
  //pos = vertexColor[0];
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
      gl_Position = pos + get_vertex(theta, phi);
      gl_Position = transform(gl_Position);
      EmitVertex();
      gl_Position = pos + get_vertex(theta, phi+pgap);
      gl_Position = transform(gl_Position);
      EmitVertex();
    }
    EndPrimitive();

  }

}
