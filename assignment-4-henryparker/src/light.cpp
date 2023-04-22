#include "light.h"
#include "utils.h"

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) :
    position(pos), radiance(color) {}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &size) :
    Light(pos, color), size(size) {
  Vec3f v1, v2, v3, v4;
  v1 = pos + Vec3f(size.x() / 2, 0.f, -size.y() / 2);
  v2 = pos + Vec3f(-size.x() / 2, 0.f, -size.y() / 2);
  v3 = pos + Vec3f(-size.x() / 2, 0.f, size.y() / 2);
  v4 = pos + Vec3f(size.x() / 2, 0.f, size.y() / 2);
  light_mesh =TriangleMesh({v1, v2, v3, v4}, {Vec3f(0, -1, 0)}, {0, 1, 2, 0, 2, 3}, {0,0,0,0,0,0});
}

Vec3f SquareAreaLight::emission(const Vec3f &pos, const Vec3f &dir) const {
  return dir.normalized().dot(Vec3f(0, -1, 0)) > 0 ? dir.normalized().dot(Vec3f(0, -1, 0)) * radiance : Vec3f(0, 0, 0);
}

float SquareAreaLight::pdf(const Interaction &interaction, Vec3f pos) {
  return 1.0 / (size.x() * size.y());
}

Vec3f SquareAreaLight::sample(Interaction &interaction, float pdf, Sampler &sampler) const {
  Vec2f uv = sampler.get2D();
  Vec3f lowerLeftCorner = position + Vec3f(-size.x() / 2, 0.f, -size.y() / 2);
  return lowerLeftCorner + Vec3f(uv.x() * size.x(), 0.f, uv.y() * size.y());
}

bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
  if (light_mesh.intersect(ray, interaction)) {
    interaction.type = Interaction::Type::LIGHT;
    return true;
  }
  return false;
}

