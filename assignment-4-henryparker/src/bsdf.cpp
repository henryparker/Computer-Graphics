#include "bsdf.h"
#include "utils.h"

#include <utility>

IdealDiffusion::IdealDiffusion(const Vec3f &color) : color(color) {}

Vec3f IdealDiffusion::evaluate(Interaction &interaction) const {
  return color / PI;
}

float IdealDiffusion::pdf(Interaction &interaction) const {
  // TODO: your implementation here
  return interaction.normal.dot(interaction.wi) / PI;
}

void IdealDiffusion::sample(Interaction &interaction, Sampler &sampler) const {
  // TODO: your implementation here
  float theta = sampler.get1D();
  float phi = sampler.get1D();
  theta = acos(1 - theta);
  phi = 2 * PI * phi;

  Vec3f vec(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
  Mat3f rotation;
  rotation = Eigen::Quaternionf().setFromTwoVectors(Vec3f(0.001, 0.001, 1), interaction.normal);
  interaction.wi = rotation * vec;
}
/// return whether the bsdf is perfect transparent or perfect reflection
bool IdealDiffusion::isDelta() const {
  return false;
}

/* Specular BSDF */
IdealSpecular::IdealSpecular(const Vec3f &color) : color(color) {}

Vec3f IdealSpecular::evaluate(Interaction &interaction) const {
  return color * pdf(interaction);
}

float IdealSpecular::pdf(Interaction &interaction) const {
  Vec3f reflect = 2 * interaction.normal.dot(interaction.wo) * interaction.normal - interaction.wo;
  return reflect.dot(interaction.wi) > REFLECTANCE_EPS ? 1 : 0.001;
}
void IdealSpecular::sample(Interaction &interaction, Sampler &sampler) const {
  Vec3f reflect = 2 * interaction.normal.dot(interaction.wo) * interaction.normal - interaction.wo;
  interaction.wi = reflect;
}

bool IdealSpecular::isDelta() const {
    return true;
}