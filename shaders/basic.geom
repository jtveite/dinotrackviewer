#version 150
layout(points) in;
layout(triangle_strip, max_vertices=128) out;


void main ()
{
  vec4 a = gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0);
  vec4 b = gl_in[0].gl_Position + vec4(0., 1., 0., 0.);
  vec4 c = gl_in[0].gl_Position + vec4(0., 0., 1., 0.);
  vec4 d = gl_in[0].gl_Position;

  gl_Position = a;
  EmitVertex();
  gl_Position = b;
  EmitVertex();
  gl_Position = c;
  EmitVertex();
  gl_Position = d;
  EmitVertex();
  gl_Position = a;
  EmitVertex();
  gl_Position = b;
  EmitVertex();
  EndPrimitive();

}
