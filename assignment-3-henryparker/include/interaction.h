#ifndef CS171_HW3_INCLUDE_INTERACTION_H_
#define CS171_HW3_INCLUDE_INTERACTION_H_

#include "core.h"

struct InteractionPhongLightingModel {
  Vec3f diffusion;
  Vec3f specular;
  Vec3f ambient;
  float shininess{};
};

struct Interaction {
  enum Type 
  { 
    LIGHT,      /* Interaction happens on a light source */
    GEOMETRY,   /* Interaction happens on a geometry */
    NONE        /* No interaction */
  };
  Vec3f pos{};                            /* Position of intersection point */
  float dist{RAY_DEFAULT_MAX};            /* Distance (in units of t) to intersection point */
  Vec3f normal{};                         /* Normal of intersection point (if existed) */
  Vec2f uv{};                             /* UV coordinate of intersection point (if existed) */
  InteractionPhongLightingModel model;    /* Phong lighting model at the intersected point (if existed) */
  Type type{Type::NONE};                  /* Type of interacting object */
};

#endif //CS171_HW3_INCLUDE_INTERACTION_H_
