#include "cursor.h"

Cursor::Cursor(float length){
}

void Cursor::Initialize(float length){
  len = length;
  shader = new MyShader("shaders/shape.vert", "shaders/shape.frag");
  shader->checkErrors();

//  glGenVertexArrays(1, &vao);
//  glBindVertexArray(vao);

//  verts.push_back(glm::vec3(1.0f, 0.0f,-0.1f));
//  verts.push_back(glm::vec3(0.0f, len, 0.0f));
//  verts.push_back(glm::vec3(0.0f, 0.0f, 0.1f));

  verts.push_back(glm::vec3(0.0f, len, 0.0f));
  verts.push_back(glm::vec3(0.3f*len, 0.0f, 0.0f));
  verts.push_back(glm::vec3(0.0f, 0.3f*len, 0.0f));
  verts.push_back(glm::vec3(-0.3f*len, 0.0f, 0.0f));
  verts.push_back(glm::vec3(0.0f, -0.3f*len, 0.0f));
  verts.push_back(glm::vec3(0.15f*len, 0.15f*len, 0.0f));

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
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glCheckError();
//    glDisableVertexAttribArray(0);
    glCheckError();
    shader->unbindShader();

//  glCheckError();
//  shader->bindShader();
//  glCheckError();
//  shader->setMatrix4("mvp", mvp);
//  shader->setFloat("rad", len);
//  glCheckError();

//  glBindBuffer(GL_ARRAY_BUFFER, vbo);
//  Vertex v = Vertex(pos);
//  std::vector<Vertex> *array;
//  array->push_back(v);
//  int numElements = array->size();
//  int bufferSize = array->size() * sizeof(Vertex);
//  Vertex* bufferData = array->data();
//  glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);

//  glEnableVertexAttribArray(0);
//  glEnableVertexAttribArray(1);
//  glCheckError();
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
//  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(glm::vec3));

//  shader->setFloat("numClusters", 0);
//  glDrawArrays(GL_POINTS, 0, numElements);
//  glBindBuffer(GL_ARRAY_BUFFER, 0);
//  glCheckError();

//  glCheckError();
//  shader->unbindShader();
//  glCheckError();

}
