#include "integrator.h"
#include "utils.h"
#include <omp.h>
#include <iostream>

#include <utility>

using namespace std;
Integrator::Integrator(std::shared_ptr<Camera> cam,
                       std::shared_ptr<Scene> scene, int spp, int max_depth)
    : camera(std::move(cam)), scene(std::move(scene)), spp(spp), max_depth(max_depth) {
}

void Integrator::render() const {
  Vec2i resolution = camera->getImage()->getResolution();
  int cnt = 0;
  Sampler sampler;
#pragma omp parallel for schedule(dynamic), shared(cnt), private(sampler)
  for (int dx = 0; dx < resolution.x(); dx++) {
#pragma omp atomic
    ++cnt;
    printf("\r%.02f%%", cnt * 100.0 / resolution.x());
    sampler.setSeed(omp_get_thread_num());
    for (int dy = 0; dy < resolution.y(); dy++) {
      // cout << "pixel: " << dx << ' ' << dy << endl;
      Vec3f L(0, 0, 0);
      // TODO: generate #spp rays for each pixel and use Monte Carlo integration to compute radiance.
      auto partion_list = camera->sampleInPx(dx, dy, sqrt(spp));
      for (Vec2f partion : partion_list) {
        Ray ray = camera->generateRay(dx, dy);
        L += radiance(ray, sampler);
      }
      camera->getImage()->setPixel(dx, dy, L / static_cast<float>(partion_list.size()));
    }
  }
}

Vec3f Integrator::radiance(Ray &ray, Sampler &sampler) const {
  Vec3f L(0, 0, 0);
  Vec3f beta(1, 1, 1);
  bool isDelta = false;
  
  // Interaction inter;
  // scene->intersect(ray, inter);
  // return inter.normal.cwiseAbs();

  Ray tmpRay = ray;
  
  for (int i = 0; i < max_depth; ++i) {
    /// Compute radiance (direct + indirect)
    Interaction inter;
    if (scene->intersect(tmpRay, inter)) {

      if (inter.type == Interaction::Type::GEOMETRY) {
        inter.wo = -tmpRay.direction;
        inter.normal.normalized();
        L += (beta.array() * directLighting(inter, sampler).array()).matrix();
        
        inter.material->sample(inter, sampler);
        tmpRay = Ray(inter.pos, inter.wi);
        float pdf = inter.material->pdf(inter);

        beta = (inter.material->evaluate(inter).array() * beta.array()).matrix() * inter.normal.dot(inter.wi) / pdf;

        isDelta = inter.material->isDelta();
      }
      else if (i == 0 && inter.type == Interaction::Type::LIGHT) {
        return scene->getLight()->getColor();
      }
      else if (isDelta && inter.type == Interaction::Type::LIGHT) {
        L += (beta.array() * scene->getLight()->getColor().array()).matrix();
        break;
      }
      else break;
    }
  }
  return L;
}

Vec3f Integrator::directLighting(Interaction &interaction, Sampler &sampler) const {
  Vec3f L(0, 0, 0);
  // Compute direct lighting.
  std::vector<Vec3f> lightSample;
  Interaction tmpinter;
  
  for (int i = 0; i < LIGHT_SAMPLE_NUM; i++)
    lightSample.push_back(
      scene->getLight()->sample(tmpinter, scene->getLight()->pdf(tmpinter), sampler));

  for (int i = 0; i < LIGHT_SAMPLE_NUM; i++) {
    Ray ray(interaction.pos, lightSample[i] - interaction.pos);
    Interaction inter;
    if (!scene->isShadowed(ray)) {
      // cout << "without shadow!" << endl;
      Vec3f f_r = interaction.material->evaluate(interaction);
      // cout << "f_r:" << '(' << f_r.x() << ", " << f_r.y() << ", " << f_r.z() << ')' << endl;
      Vec3f L_i = scene->getLight()->emission(lightSample[i], interaction.pos - lightSample[i]);
      // cout << "interaction.normal.dot(interaction.wi):" << interaction.normal << endl;
      L = L + (f_r.array() * L_i.array()).matrix() * interaction.normal.dot((lightSample[i] - interaction.pos).normalized()) /
      (scene->getLight()->pdf(inter) * (interaction.pos - lightSample[i]).norm() * (interaction.pos - lightSample[i]).norm() * LIGHT_SAMPLE_NUM);
    }
  }

  return L;
}