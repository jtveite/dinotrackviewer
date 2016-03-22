varying vec3 pos;
void main(void){
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
  pos = g3d_ObjectToWorldMatrix * gl_Vertex;
}
