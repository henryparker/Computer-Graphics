#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "core.h"
#include "ray.h"
#include "interaction.h"
#include "bsdf.h"
#include "accel.h"

#include <vector>
#include <optional>
#include <memory>

struct TriangleWithCenter {
  Vec3i pointindex;
  Vec3i normalindex;
  Vec3f center;

  TriangleWithCenter(Vec3i pi, Vec3i n, Vec3f c) : pointindex(pi), normalindex(n), center(c) {}
};

class TriangleMesh {
 public:
  TriangleMesh() = default;
  TriangleMesh(std::vector<Vec3f> vertices,
               std::vector<Vec3f> normals,
               std::vector<int> v_index,
               std::vector<int> n_index);
  bool intersect(Ray &ray, Interaction &interaction) const;
  void setMaterial(std::shared_ptr<BSDF> &new_bsdf);
  void buildBVH();
  void RecurveBuildBVH(std::vector<TriangleWithCenter>& Triangles, BVHNode*& node);
 private:
  bool intersectOneTriangle(Ray &ray, Interaction &interaction, const Vec3i& v_idx, const Vec3i& n_idx) const;
  void bvhHit(BVHNode *p, Interaction &interaction,
              Ray &ray) const;
  std::shared_ptr<BSDF> bsdf;
  BVHNode* bvh;

  std::vector<Vec3f> vertices;
  std::vector<Vec3f> normals;
  std::vector<int> v_indices;
  std::vector<int> n_indices;

  std::vector<TriangleWithCenter> triangles;

};

#endif // GEOMETRY_H_
