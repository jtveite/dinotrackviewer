#version 330

layout(location=0) in vec4 vertexPos;
layout(location=1) in vec4 color;

out vec4 vertexColor;
uniform mat4 mvp;
void main(void){
//  pos =g3d_WorldToCameraMatrix *  g3d_ObjectToWorldMatrix * vertexPos;
  gl_Position =  vertexPos;
  vertexColor = color;
}
