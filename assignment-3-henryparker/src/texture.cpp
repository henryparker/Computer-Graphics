#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "texture.h"
#include <iostream>

// void Texture::loadTextureFromFile(const std::string &path) {
//   int w, h, ch;
//   stbi_uc *img = stbi_load(path.c_str(), &w, &h, &ch, 0);
//   tex_data.assign(img, img + w * h * ch);
//   stbi_image_free(img);
// }

void Texture::loadTextureFromFile(const std::string &path) {
  int w, h, ch;
  stbi_uc *img = stbi_load(path.c_str(), &w, &h, &ch, 0);
  if (img == nullptr)
    throw("image unload\n");
  tex_data.assign(img, img + w * h * ch);
  this->width = w;
  this->height = h;
  this->channel = ch;
  std::cout << width << height << channel << std::endl;
  // unsigned int x = static_cast<unsigned int>(0.0561049 * width);
  // unsigned int y = static_cast<unsigned int>(0.251094 * height);
  // if (x == width) x--;
  // if (y == height) y--;
  // std::cout << "x = " << x << " y = " << y << std::endl;
  // Vec3f px_color(tex_data[width * channel * y + channel * x],
  //                         tex_data[width * channel * y + channel * x + 1],
  //                         tex_data[width * channel * y + channel * x + 2] );
  // std::cout << "ray px_color = (" << px_color.x() << ',' << px_color.y() << ',' << px_color.z() << ")\n";
  stbi_image_free(img);
}

Vec3f Texture::getPixel(float u, float v) const {
  // std::cout << "u = " << u << " v = " << v << std::endl;
  // std::cout << "width = " << width << " height = " << height << " channel = " << channel << std::endl;
  unsigned int x = static_cast<unsigned int>(u * width);
  unsigned int y = static_cast<unsigned int>(v * height);
  if (x == width) x--;
  if (y == height) y--;
  // std::cout << "x = " << x << " y = " << y << std::endl;
  Vec3f px_color(static_cast<float>(tex_data[width * channel * y + channel * x] / 255.0f),
                 static_cast<float>(tex_data[width * channel * y + channel * x + 1] / 255.0f),
                 static_cast<float>(tex_data[width * channel * y + channel * x + 2]) / 255.0f);
  // std::cout << "px_color = (" << px_color.x() << ',' << px_color.y() << ',' << px_color.z() << ")\n";
  return px_color;
}