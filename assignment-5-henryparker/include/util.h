#pragma once

#include <random>
#include "common.h"

class Sampler {
 public:
  Sampler() = default;
  float get1D() { return dis(engine); }
  void setSeed(int i) { engine.seed(i); }
 private:
  std::default_random_engine engine;
  std::uniform_real_distribution<float> dis;
};

inline Vec3 ClosestPtPointTriangle(Vec3 p, Vec3 a, Vec3 b, Vec3 c) {
  // Check if P in vertex region outside A
  Vec3 ab = b - a;
  Vec3 ac = c - a;
  Vec3 ap = p - a;
  float d1 = glm::dot(ab, ap);
  float d2 = glm::dot(ac, ap);
  if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)

  // Check if P in vertex region outside B
  Vec3 bp = p - b;
  float d3 = glm::dot(ab, bp);
  float d4 = glm::dot(ac, bp);
  if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)

  // Check if P in edge region of AB, if so return projection of P onto AB
  float vc = d1 * d4 - d3 * d2;
  if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
    float v = d1 / (d1 - d3);
    return a + v * ab; // barycentric coordinates (1-v,v,0)
  }

  // Check if P in vertex region outside C
  Vec3 cp = p - c;
  float d5 = glm::dot(ab, cp);
  float d6 = glm::dot(ac, cp);
  if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)

  // Check if P in edge region of AC, if so return projection of P onto AC
  float vb = d5 * d2 - d1 * d6;
  if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
    float w = d2 / (d2 - d6);
    return a + w * ac; // barycentric coordinates (1-w,0,w)
  }

  // Check if P in edge region of BC, if so return projection of P onto BC
  float va = d3 * d6 - d5 * d4;
  if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
    float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    return b + w * (c - b); // barycentric coordinates (0,1-w,w)
  }

  // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
  float denom = 1.0f / (va + vb + vc);
  float v = vb * denom;
  float w = vc * denom;
  return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}