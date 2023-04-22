#include <object.h>
#include <shader.h>
#include <utils.h>
#include <fstream>
#include <vector>


/**
 * TODO: initialize VAO, VBO, VEO and set the related varialbes
 */
void Object::initialize(std::vector<Vertex> vet, std::vector<GLuint> face_indices) {
  vertices.assign(vet.begin(), vet.end());
  indices.assign(face_indices.begin(), face_indices.end());

  verticeNum = vertices.size();
  faceNum = indices.size() / 3;
  // std::cout << "faceNum = " << faceNum << std::endl;
  // std::cout << vertices[1].position[0] << std::endl;
  // std::cout << vertices[1].position[1] << std::endl;
  // std::cout << vertices[1].position[2] << std::endl;
  // for (int i = 0; i < vertices.size(); i++) {
  //   std::cout << i << std::endl;
  // }
  // for (int i = 0; i < indices.size(); i+=3 ) {
  //   std::cout << indices[i] << ' ' << indices[i + 1] << ' ' << indices[i + 2] << std::endl;
  // }
  if (faceNum == 0) {
    glGenVertexArrays(1, &VAO);  
    glGenBuffers(1, &VBO); 
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(vec3)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
  }
  else {
    glGenVertexArrays(1, &VAO);  
    glGenBuffers(1, &VBO); 
    glGenBuffers(1, &EBO);
    // bind VAO first
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_STATIC_DRAW); 
    // attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(vec3)));
        glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
  }

}

/**
 * TODO: draw object with VAO and VBO
 * You can choose to implement either one or both of the following functions.
 */

/* Implement this one if you do not use a shader */
void Object::drawArrays() const {
  
}

/* Implement this one if you do use a shader */
void Object::drawArrays(const Shader& shader) const {
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, verticeNum);
  glBindVertexArray(0);
}

/**
 * TODO: draw object with VAO, VBO, and VEO
 * You can choose to implement either one or both of the following functions.
 */

/* Implement this one if you do not use a shader */
void Object::drawElements() const {
  
}

/* Implement this one if you do use a shader */
void Object::drawElements(const Shader& shader) const {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}