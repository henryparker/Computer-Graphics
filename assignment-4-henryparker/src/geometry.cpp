#include "geometry.h"

#include <utility>
#include <iostream>
using namespace std;

TriangleMesh::TriangleMesh(std::vector<Vec3f> vertices, std::vector<Vec3f> normals,
                           std::vector<int> v_index, std::vector<int> n_index) :
    vertices(std::move(vertices)),
    normals(std::move(normals)),
    v_indices(std::move(v_index)),
    n_indices(std::move(n_index)),
    bvh(nullptr) {}

bool TriangleMesh::intersect(Ray &ray, Interaction &interaction) const {
  if (bvh != nullptr) {
    bvhHit(bvh, interaction, ray);
  } else {
    // If you did not implement BVH
    // directly loop through all triangles in the mesh and test intersection for each triangle.
    for (int i = 0; i < v_indices.size() / 3; i++) {
      Vec3i v_idx(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
      Vec3i n_idx(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
      Interaction temp;
      if (intersectOneTriangle(ray, temp, v_idx, n_idx) && (temp.dist < interaction.dist)) {
        interaction = temp;
      }
    }
  }
  return interaction.type != Interaction::Type::NONE;
}

void TriangleMesh::setMaterial(std::shared_ptr<BSDF> &new_bsdf) {
  bsdf = new_bsdf;
}
void TriangleMesh::buildBVH() {
  // TODO: your implementation
  for (int i = 0; i < v_indices.size(); i += 3) {
    Vec3i pointindex(v_indices[i], v_indices[i + 1], v_indices[i + 2]);
    Vec3i normalindex(n_indices[i], n_indices[i + 1], n_indices[i + 2]);
    Vec3f center = (vertices[pointindex.x()] + vertices[pointindex.y()] + vertices[pointindex.z()]) / 3;
    triangles.push_back(TriangleWithCenter(pointindex, normalindex, center));
  }
  std::vector<TriangleWithCenter> triangles_copy = triangles;
  RecurveBuildBVH(triangles_copy, bvh);
}

void TriangleMesh::RecurveBuildBVH(std::vector<TriangleWithCenter>& Triangles, BVHNode*& node) {
  // newNode
  Vec3f lower(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
  Vec3f upper(MIN_FLOAT, MIN_FLOAT, MIN_FLOAT);
  for (TriangleWithCenter triangle : Triangles) {
    for (int i = 0; i < 3; i++) {
      lower = lower.cwiseMin(vertices[triangle.pointindex[i]]);
      upper = upper.cwiseMax(vertices[triangle.pointindex[i]]);
    }
  }
  node = new BVHNode;
  node->aabb = AABB(lower, upper);
  for (TriangleWithCenter triangle : Triangles) {
    node->trianglesPointIndex.push_back(triangle.pointindex);
    node->trianglesNormalIndex.push_back(triangle.normalindex);
  }

  // if triangles number is small enough(no more than MAX_LEAF_SIZE), do not split, exit build function
  if (Triangles.size() <= MAX_LEAF_SIZE) return;

  // select axis to split
  int sortindex;
  Vec3f(upper - lower).maxCoeff(&sortindex);

  // sort Triangles according to the axis
  std::sort(Triangles.begin(), Triangles.end(), 
    [sortindex](TriangleWithCenter T1, TriangleWithCenter T2) -> bool {return T1.center[sortindex] < T2.center[sortindex];});

  // devide into two parts
  std::vector<TriangleWithCenter> triangles_left(Triangles.begin(), Triangles.begin() + Triangles.size() / 2);
  std::vector<TriangleWithCenter> triangles_right(Triangles.begin() + Triangles.size() / 2, Triangles.end());

  // build left and right
  RecurveBuildBVH(triangles_left, node->left);
  RecurveBuildBVH(triangles_right, node->right);

  // test code
  // cout << "upper\n" << upper << endl;
  // cout << "lower\n" << lower << endl;
  // cout << "diff: " << diff << endl; 
  // cout << "maxindex: " << maxindex << endl;
  // cout << "center: ";
  // for (TriangleWithCenter triangle : Triangles)
  //   cout << triangle.center[sortindex] << ' ';
  // cout << endl;
  // cout << "size: " << Triangles.size() << endl;
}

bool TriangleMesh::intersectOneTriangle(Ray &ray,
                                        Interaction &interaction,
                                        const Vec3i &v_idx,
                                        const Vec3i &n_idx) const {
  Vec3f v0 = vertices[v_idx[0]];
  Vec3f v1 = vertices[v_idx[1]];
  Vec3f v2 = vertices[v_idx[2]];
  Vec3f v0v1 = v1 - v0;
  Vec3f v0v2 = v2 - v0;
  Vec3f pvec = ray.direction.cross(v0v2);
  float det = v0v1.dot(pvec);

  float invDet = 1.0f / det;

  Vec3f tvec = ray.origin - v0;
  float u = tvec.dot(pvec) * invDet;
  if (u < 0 || u > 1) return false;
  Vec3f qvec = tvec.cross(v0v1);
  float v = ray.direction.dot(qvec) * invDet;
  if (v < 0 || u + v > 1) return false;
  float t = v0v2.dot(qvec) * invDet;
  if (t < ray.t_min || t > ray.t_max) return false;

  interaction.dist = t;
  interaction.pos = ray(t);
  interaction.normal = (u * normals[n_idx[1]] + v * normals[n_idx[2]]
      + (1 - u - v) * normals[n_idx[0]]).normalized();
  interaction.material = bsdf;
  interaction.type = Interaction::Type::GEOMETRY;
  return true;
}

void TriangleMesh::bvhHit(BVHNode *p, Interaction &interaction,
                          Ray &ray) const {
  // TODO: traverse through the bvh and do intersection test efficiently.
  
  // empty node
  if (p == nullptr) return;

  // if not hit aaabb, return
  float t_in, t_out;
  if (!p->aabb.intersect(ray, &t_in, &t_out)) return;

  // recursive test
  if (p->trianglesPointIndex.size() <= MAX_LEAF_SIZE) {
    for (int i = 0; i < p->trianglesPointIndex.size(); i++) {
      Interaction temp;
      if (intersectOneTriangle(ray, temp, p->trianglesPointIndex[i], p->trianglesNormalIndex[i]) && temp.dist < interaction.dist) {
        interaction = temp;
      }
    }
  }
  else {
    bvhHit(p->left, interaction, ray);
    bvhHit(p->right, interaction, ray);
  }
}
