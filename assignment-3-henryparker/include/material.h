#ifndef CS171_HW3_INCLUDE_MATERIAL_H_
#define CS171_HW3_INCLUDE_MATERIAL_H_

#include "interaction.h"
#include "texture.h"
#include <iostream>
#include <string>

class Material {
 public:
  Material() = default;
  virtual ~Material() = default;
  [[nodiscard]] virtual InteractionPhongLightingModel evaluate(Interaction &interaction) const = 0;
};

class ConstColorMat : public Material {
 public:
  ConstColorMat();
  explicit ConstColorMat(const Vec3f &color, float sh = 16.f);
  ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh = 16.f);
  [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
 private:
  Vec3f diffuse_color;
  Vec3f specular_color;
  Vec3f ambient_color;
  float shininess;
};

class TextureMat : public Material {
public:
  TextureMat() : texture(nullptr), shininess(16.0f) {}
  TextureMat(std::string path, float sh = 16.0f) : texture(path), shininess(sh) {}
  [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;

protected:
  Texture texture;
  float shininess;
};

#endif //CS171_HW3_INCLUDE_MATERIAL_H_
