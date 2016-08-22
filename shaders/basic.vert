#version 330

layout(location=0) in vec3 vertexPos;
layout(location=1) in vec2 color;

out vec2 vertexColor;
uniform mat4 mvp;
void main(void){
//  pos =g3d_WorldToCameraMatrix *  g3d_ObjectToWorldMatrix * vertexPos;
  gl_Position =  vec4(vertexPos, 1.0);
  vertexColor = color;
}
