#include "light.h"
#include "define.h"

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) :
    position(pos), color(color) {

}
Vec3f Light::getColor() const {
  return color;
}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension,
                                 const Vec3f &normal,
                                 const Vec3f &tangent) :
    Light(pos, color),
    rectangle(pos, dimension, normal, tangent) {}

std::vector<LightSample> SquareAreaLight::samples() const {
  std::vector<LightSample> samples;
  int light_res = LIGHT_SAMPLE_SQRT;

  Vec3f Light_color = color;
  Vec3f Light_position = position;
  Vec3f vertical = rectangle.getTangent();
  Vec3f horizonal = rectangle.getNormal().cross(vertical);
  Vec2f size = rectangle.getSize();
  Vec3f lower_left = Light_position - size.x() / 2 * vertical - size.y() / 2 * horizonal;

  for (int i = 0; i < light_res; i++ ) {
    for (int j = 0; j < light_res; j++ ) {
      Vec3f pos = lower_left +
                  size.x() / (2*static_cast<float>(light_res)) * vertical +
                  size.x() / static_cast<float>(light_res) * static_cast<float>(i) * vertical +
                  size.y() / (2*static_cast<float>(light_res)) * horizonal +
                  size.y() / static_cast<float>(light_res) * static_cast<float>(j) * horizonal;
      samples.push_back(LightSample{Light_color, pos});
    }
  }
  return samples;
}


bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
  bool intersection = rectangle.intersect(ray,interaction);
  if(intersection) interaction.type=Interaction::LIGHT;
  return intersection;
}

