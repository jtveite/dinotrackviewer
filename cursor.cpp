#include "cursor.h"

Cursor::Cursor(float length){
}

void Cursor::Initialize(float length){
  len = length;
  shader = new MyShader("shaders/shape.vert", "shaders/shape.frag");
  shader->checkErrors();

  for (int k=0;k<=360;k+= 30){
    verts.push_back(glm::vec3(0.0f,1.0f,0.0f));
    verts.push_back(glm::vec3(0.1* cos(k),0.0f, 0.1*sin(k)));
    verts.push_back(glm::vec3(0.1* cos(k+30),0.0f, 0.1*sin(k+30)));
  }

//  verts.push_back(glm::vec3(0.0f, len, 0.0f));
//  verts.push_back(glm::vec3(0.3f*len, 0.0f, 0.0f));
//  verts.push_back(glm::vec3(0.0f, 0.3f*len, 0.0f));
//  verts.push_back(glm::vec3(-0.3f*len, 0.0f, 0.0f));
//  verts.push_back(glm::vec3(0.0f, -0.3f*len, 0.0f));
//  verts.push_back(glm::vec3(0.15f*len, 0.15f*len, 0.0f));

  static const GLfloat vertex_data[] = {
      0.0f, 0.0f,-0.01f,
      0.0f, 0.01f, 0.0f,
      0.0f, 0.0f, 0.01f,
  };
  //buffers
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, verts.size()* sizeof(glm::vec3), verts.data(), GL_STATIC_DRAW);
//  glBufferData(GL_ARRAY_BUFFER, 9, vertex_data, GL_STATIC_DRAW);
//  glBufferData(GL_ARRAY_BUFFER, sizeof(rocket_body_verticies), rocket_body_verticies, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Cursor::Draw(glm::mat4 mvp){
    shader->bindShader();
    glm::mat4 m;
    shader->setMatrix4("mvp", mvp);
    glCheckError();
    glEnableVertexAttribArray(0);
    glCheckError();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glCheckError();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glCheckError();
//    glDrawArrays(GL_TRIANGLES, 0, 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, 13);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glCheckError();
//    glDisableVertexAttribArray(0);
    glCheckError();
    shader->unbindShader();

}
