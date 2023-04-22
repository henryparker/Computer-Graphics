#include <mesh.h>
#include <utils.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

Mesh::Mesh(const std::string &path) { loadDataFromFile(getPath(path)); }

/**
 * TODO: implement load mesh data from file here
 * you need to open file with [path]
 *
 * File format
 *
 *
 * For each line starting with 'v' contains 3 floats, which
 * represents the position of a vertex
 *
 * For each line starting with 'n' contains 3 floats, which
 * represents the normal of a vertex
 *
 * For each line starting with 'f' contains 6 integers
 * [v0, n0, v1, n1, v2, n2], which represents the triangle face
 * v0, n0 means the vertex index and normal index of the first vertex
 * v1, n1 means the vertex index and normal index of the second vertex
 * v2, n2 means the vertex index and normal index of the third vertex
 */
void Mesh::loadDataFromFile(const std::string &path) {
    /**
     * path stands for the where the object is storaged
     * */
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  std::ifstream infile(path, std::ifstream::in);
  if (!infile.is_open()) {
    std::cout << "ERROE::MESH::LOAD::FILE_OPEN_FAIL" << std::endl;
  }

  std::vector<float> vertexpoint;
  std::vector<float> normalpoint;
  char flag;
  face_num = 0;
  vertex_num = 0;
  while (infile >> flag) {
    float cord;
    float norm;
    GLuint index;
    std::string line;
    if (flag == '#') std::getline(infile, line);
    if (flag == 'v') {
      vertex_num++;
      for (int i = 0; i < 3; i++ ) {
        infile >> cord;
        vertexpoint.push_back(cord);
      }
    }
    if (flag == 'n')
      for (int i = 0; i < 3; i++ ) {
        infile >> norm;
        normalpoint.push_back(norm);
      }
    if (flag == 'f')
      {
        face_num++;
        for (int i = 0; i < 3; i++ ) {
          infile >> index;
          infile >> index;
          // std::cout << "index" << index << std::endl;
          indices.push_back(index);
        }
      }
  }
  for (int i = 0; i < vertex_num; i++ ) {
    Vertex ver;
    ver.position = vec3(vertexpoint[3*i], vertexpoint[3*i+1], vertexpoint[3*i+2]);
    ver.normal = vec3(normalpoint[3*i], normalpoint[3*i+1], normalpoint[3*i+2]);
    // std::cout << "vertex " << vertexpoint[3*i] << ' ' << vertexpoint[3*i+1] << ' ' << vertexpoint[3*i+2] << std::endl;
    // std::cout << "normal " << normalpoint[3*i] << ' ' << normalpoint[3*i+1] << ' ' << normalpoint[3*i+2] << std::endl;
    vertices.push_back(ver);
  }
}

/**
 * TODO: implement your draw code here
 */
void Mesh::draw() const {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  UNIMPLEMENTED;
}