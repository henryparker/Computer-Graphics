#include "material.h"
#include <iostream>
#include <utility>

ConstColorMat::ConstColorMat() : diffuse_color({1, 1, 1}),
                                 specular_color({1, 1, 1}),
                                 ambient_color(1, 1, 1),
                                 shininess(16.f) {}

ConstColorMat::ConstColorMat(const Vec3f &color, float sh)
    : diffuse_color(color), specular_color(color), ambient_color(color), shininess(sh) {}

ConstColorMat::ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh)
    : diffuse_color(std::move(diff)),
      specular_color(std::move(spec)),
      ambient_color(std::move(ambient)),
      shininess(sh) {}

InteractionPhongLightingModel ConstColorMat::evaluate(Interaction &interaction) const {
  InteractionPhongLightingModel m;
  m.ambient = ambient_color;
  m.specular = specular_color;
  m.diffusion = diffuse_color;
  m.shininess = shininess;
  return m;
}

InteractionPhongLightingModel TextureMat::evaluate(Interaction &interaction) const {
  InteractionPhongLightingModel m;
  m.ambient = texture.getPixel(interaction.uv.x(), interaction.uv.y());
  // std::cout << interaction.uv.x() << ' ' << interaction.uv.y() << std::endl;
  // std::cout << "texture->width" << texture->width << std::endl;
  m.diffusion = texture.getPixel(interaction.uv.x(), interaction.uv.y());
  m.specular = texture.getPixel(interaction.uv.x(), interaction.uv.y());
  m.shininess = shininess;
  return m;
}
