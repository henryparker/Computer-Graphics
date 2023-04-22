#ifndef _object_H_
#define _object_H_
#include "defines.h"
#include <shader.h>
#include <vector>

struct Vertex {
  vec3 position;
  vec3 normal;
};

struct DrawMode {
  GLenum primitive_mode;
};

class Object {
public:
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  int verticeNum, faceNum;

  DrawMode draw_mode;

  Object() {};
  ~Object() {};

  void initialize(std::vector<Vertex> vet, std::vector<GLuint> face_indices = std::vector<GLuint>());

  void drawArrays() const;
  void drawArrays(const Shader& shader) const;
  void drawElements() const;
  void drawElements(const Shader& shader) const;
};
#endif