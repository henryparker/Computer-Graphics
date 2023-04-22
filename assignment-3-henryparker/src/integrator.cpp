#include "integrator.h"
#include <omp.h>
#include "define.h"
#include <iostream>
#include <utility>

PhongLightingIntegrator::PhongLightingIntegrator(std::shared_ptr<Camera> cam,
                                                 std::shared_ptr<Scene> scene)
    : camera(std::move(cam)), scene(std::move(scene)) {
}

void PhongLightingIntegrator::render() const {
  Vec2i resolution = camera->getImage()->getResolution();
  int cnt = 0;
#pragma omp parallel for schedule(guided, 2), shared(cnt)
  for (int dx = 0; dx < resolution.x(); dx++) { // resolution.x()
#pragma omp atomic
    ++cnt;
    printf("\r%.02f%%", cnt * 100.0 / resolution.x());
    for (int dy = 0; dy < resolution.y(); dy++) {  // resolution.y()
      Vec3f L(0, 0, 0);
      auto partion_list = camera->sampleInPx(dx, dy, RAY_SAMPLE_SQRT);

      // std::cout << "dx = " << dx << "dy = " << dy << std::endl;
      // std::cout << "size of partion_list = " << partion_list.size() << std::endl;
      // for (Vec2f partion : partion_list)
      // {
      //   std::cout << "partion = (" << partion.x() << ',' << partion.y() << ")\n";
      // }

      for (Vec2f partion : partion_list) {
        // std::cout << "enter partion" << std::endl;
        Ray ray = camera->generateRay(partion.x(), partion.y());
        // std::cout << "ray origin = (" << ray.origin.x() << ',' << ray.origin.y() << ',' << ray.origin.z() << ")\n";
        // std::cout << "ray direction = (" << ray.direction.x() << ',' << ray.direction.y() << ',' << ray.direction.z() << ")\n";
        Interaction interact;
        
        // std::cout << "scene->intersect = " << scene->intersect(ray, interact) << std::endl;
        // if (!scene->intersect(ray, interact))
        //   std::cout << "Does not intersect" << std::endl;
        if (scene->intersect(ray, interact)) {
          L += radiance(ray, interact);
        }
      }
      // std::cout << "radiance = (" << L.x() << ',' << L.y() << ',' << L.z() << ")\n";
      camera->getImage()->setPixel(dx, dy, L / static_cast<float>(partion_list.size()));
    }
  }
}

Vec3f PhongLightingIntegrator::radiance(Ray &ray, Interaction &interaction) const {
  Vec3f radiance(0, 0, 0);
  if (interaction.type == Interaction::Type::LIGHT)
  {
    return scene->getLight()->getColor();
  }

  Vec3f amb_light = scene->getAmbient().cwiseProduct(interaction.model.ambient);
  Array3f diff = Array3f::Zero();
  Array3f spec = Array3f::Zero();

  auto light_samples = scene->getLight()->samples();
  for (auto pointlight : light_samples)
  {
    float diff_var = 0;
    float spec_var = 0;
    Vec3f light_dir = (pointlight.position - interaction.pos).normalized();
    Ray surface_to_light(interaction.pos, light_dir);
    if (!scene->isShadowed(surface_to_light))
    {
      diff_var = std::max(interaction.normal.dot(light_dir), 0.0f);
      Vec3f reflect = (2 * interaction.normal.dot(light_dir) * interaction.normal - light_dir).normalized();
      spec_var = powf(std::max(reflect.dot(-ray.direction), 0.0f), interaction.model.shininess);

      diff += diff_var * pointlight.color.array() * interaction.model.diffusion.array() / light_samples.size();
      spec += spec_var * pointlight.color.array() * interaction.model.specular.array() / light_samples.size();
    }
  }
  

  return diff.matrix() + spec.matrix() + amb_light;
}
