#version 330

layout(location=0) in vec3 vertexPos;
layout(location=1) in vec2 color;

out vec2 vertexColor;
out vec3 vertPos;

uniform mat4 mvp;

void main(void){
  gl_Position = vec4(vertexPos, 1.0);
  vertPos = vertexPos;
  vertexColor = color;
}

