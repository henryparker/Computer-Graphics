#ifndef CS171_HW3_INCLUDE_TEXTURE_H_
#define CS171_HW3_INCLUDE_TEXTURE_H_

#include <vector>
#include <string>

#include "core.h"

class Texture {
 public:
  Texture() : width(0), height(0) {}
  Texture(std::string path) {loadTextureFromFile(path);}
  bool isNull() const {return tex_data.empty() || width == 0 || height == 0;}
  enum Type { DIFF, NORM, DISP };
  Type type;
  void loadTextureFromFile(const std::string &path);
  Vec3f getPixel(float u, float v) const;

//  private:
  std::vector<uint8_t> tex_data;
  unsigned int width;
  unsigned int height;
  unsigned int channel;
};

#endif //CS171_HW3_INCLUDE_TEXTURE_H_
