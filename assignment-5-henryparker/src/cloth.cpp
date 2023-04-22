#include "cloth.h"
#include "object.h"
#include "transform.h"
#include <iostream>


using namespace std;

///////////////////
/// constructor ///
///////////////////

RectCloth::RectCloth(Float cloth_weight,
                     const UVec2& mass_dim,
                     Float dx_local,
                     Float stiffness, Float damping_ratio) :
    Mesh(std::vector<MeshVertex>(mass_dim.x * mass_dim.y),
         std::vector<UVec3>((mass_dim.y - 1) * (mass_dim.x - 1) * 2),
         GL_STREAM_DRAW, GL_STATIC_DRAW,
         true),
    // private
    initializeFlag(1),
    mass_dim(mass_dim),
    mass_weight(cloth_weight / Float(mass_dim.x * mass_dim.y)),
    dx_local(dx_local),
    stiffness(stiffness),
    damping_ratio(damping_ratio),
    is_fixed_masses(mass_dim.x * mass_dim.y),
    local_or_world_positions(mass_dim.x * mass_dim.y),
    world_velocities(mass_dim.x * mass_dim.y),
    world_accelerations(mass_dim.x * mass_dim.y) {

  // initialize local positions
  const auto local_width = Float(mass_dim.x) * dx_local;
  const auto local_height = Float(mass_dim.y) * dx_local;

#pragma omp parallel for collapse(2)
  for (int ih = 0; ih < mass_dim.y; ++ih)
    for (int iw = 0; iw < mass_dim.x; ++iw)
      local_or_world_positions[Get1DIndex(iw, ih)] = Vec3(Float(iw) * dx_local - local_width * Float(0.5),
                                                          Float(ih) * dx_local - local_height * Float(0.5),
                                                          0);

  // initialize mesh vertices
  UpdateMeshVertices();

  // initialize mesh indices
#pragma omp parallel for collapse(2)
  for (int ih = 0; ih < mass_dim.y - 1; ++ih)
    for (int iw = 0; iw < mass_dim.x - 1; ++iw) {
      size_t i_indices = (size_t(ih) * size_t(mass_dim.x - 1) + size_t(iw)) << 1;

      auto i  = Get1DIndex(iw, ih);
      auto r  = Get1DIndex(iw + 1, ih);
      auto u  = Get1DIndex(iw, ih + 1);
      auto ru = Get1DIndex(iw + 1, ih + 1);

      indices[i_indices    ] = UVec3(i, r, u);
      indices[i_indices + 1] = UVec3(r, ru, u);
    }
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(UVec3) * indices.size(), indices.data(), GL_STATIC_DRAW);
  glBindVertexArray(0);
}



//////////////////
/// interfaces ///
//////////////////

bool RectCloth::SetMassFixedOrNot(int iw, int ih, bool fixed_or_not) {
  iw = iw < 0 ? int(mass_dim.x) + iw : iw;
  ih = ih < 0 ? int(mass_dim.y) + ih : ih;

  size_t idx;
  if (!Get1DIndex(iw, ih, idx))
    return false;

  is_fixed_masses[idx] = fixed_or_not;
  return true;
}

/*override*/ void RectCloth::FixedUpdate() {
  // initialize
  if (initializeFlag) {
    initializeFlag = 0;
    InitializeOnlyOnce();
  }  
  // real time
  float dt = Time::delta_time;
  delta_time = dt / float(simulation_steps_per_fixed_update_time);

  // period wind
  if (static_cast<int>(Time::elapsed_time) % 10 < 5) {
    sample.setSeed(dt * 1000);
    local_wind = Vec3(0, 0, sample.get1D() * (float)10);
  }
  else local_wind = Vec3(0, 0, 0);
  // simulate
  Simulate(simulation_steps_per_fixed_update_time);

  // update mesh vertices
  UpdateMeshVertices();
}



/////////////////////////
/// force computation ///
/////////////////////////

Vec3 RectCloth::ComputeHookeForce(int iw_this, int ih_this,
                                  int iw_that, int ih_that,
                                  Float dx_world) const {

  /*! TODO: implement this: compute the force according to Hooke's law
   *                        applied to mass(iw_this, ih_this)
   *                                by mass(iw_that, ih_that)
   *                        `dx_world` is "the zero-force distance" in world coordinate
   *
   *        note: for invalid `iw` or `ih`, you may simply return { 0, 0, 0 }
   */
  size_t idx_this, idx_that;
  if (!Get1DIndex(iw_this, ih_this, idx_this) || !Get1DIndex(iw_that, ih_that, idx_that))
    return { 0, 0, 0 };
  Vec3 diffVec = local_or_world_positions[idx_this] - local_or_world_positions[idx_that];
  float len = glm::length(diffVec);
  return damping_ratio * stiffness * (dx_world - len) * diffVec / len;
}

Vec3 RectCloth::ComputeSpringForce(int iw, int ih) const {

  const Vec3 scale = object->transform->scale;
  /*! TODO: implement this: compute the total spring force applied to mass(iw, ih)
   *                        by some other neighboring masses
   *
   *        note: you MUST consider structural, shear, and bending springs
   *              you MUST consider scaling of "the zero-force distance"
   *              you may find ComputeHookeForce() helpful
   *              for invalid `iw` or `ih`, you may simply return { 0, 0, 0 }
   *              for "fixed masses", you may also simply return { 0, 0, 0 }
   */
  size_t idx_this;
  if (!Get1DIndex(iw, ih, idx_this))
    return { 0, 0, 0 };
  if (is_fixed_masses[idx_this]) {
    return { 0, 0, 0 };
  }
  Vec3 force = { 0, 0, 0 };
  for (int i = 0; i < 12; i++) {
    int iw_that = iw + springIndexOffset[i][0];
    int ih_that = ih + springIndexOffset[i][1];
    size_t idx_that;
    if (!Get1DIndex(iw_that, ih_that, idx_that))
      continue;
    float norm = dx_local * springIndexNorm[i];

    force += ComputeHookeForce(iw, ih, iw_that, ih_that, norm);
      
  }
  return force;
}



///////////////////////////
/// simulation pipeline ///
///////////////////////////

void RectCloth::LocalToWorldPositions() {

  // const Mat4 model_matrix = object->transform->ModelMat();

  /*! TODO: implement this: transform mass positions from local coordinate to world coordinate
   *
   *        note: you may find `class Transform` in `transform.h` helpful
   */
  for (auto pos : local_or_world_positions) {
    Vec4 pos4 = modelMatrix * Vec4(pos, 1);
    pos = Vec3(pos4) / pos4.w;
  }
}

void RectCloth::ComputeAccelerations() {

  /*! TODO: implement this: compute accelerations for each mass
   *
   *        note: you may find ComputeSpringForce() helpful
   *              you may store the results into `world_accelerations`
   */
  // const Mat4 model_matrix_inverse = glm::inverse(object->transform->ModelMat());
  const Vec3 local_gravity = -Vec3(modelMatrixInverse * Vec4(gravity, 1));

  float averageMass = mass_weight / (mass_dim.x * mass_dim.y);
  for (auto ih = 0; ih < mass_dim.y; ih++) {
    for (auto iw = 0; iw < mass_dim.x; iw++ ) {
      size_t idx;
      if (Get1DIndex(iw, ih, idx))
        world_accelerations[idx] = ComputeSpringForce(iw, ih)  / averageMass + local_gravity /*+ Vec3(modelMatrixInverse * Vec4(local_wind, 1)) */;
      
    }
  }
}

void RectCloth::ComputeVelocities() {

  /*! TODO: implement this: update velocities for each mass
   *
   *        note: you may store the results into `world_velocities`
   *              you may use `this->fixed_delta_time` instead of `Time::fixed_delta_time`, why?
   */
  for (auto ih = 0; ih < mass_dim.y; ih++)
    for (auto iw = 0; iw < mass_dim.x; iw++) {
      size_t idx;
      if (Get1DIndex(iw, ih, idx)) {
        world_velocities[idx] *= (float)0.99995;
        // world_velocities[idx] *= (float)0.99945;
        world_velocities[idx] += world_accelerations[idx] * this->delta_time;
      }
    }
}

void RectCloth::ComputePositions() {

  /*! TODO: implement this: update positions for each mass
   *
   *        note: you may store the results into `local_or_world_positions`
   *              you may use `this->fixed_delta_time` instead of `Time::fixed_delta_time`, why?
   */
  for (auto ih = 0; ih < mass_dim.y; ih++)
    for (auto iw = 0; iw < mass_dim.x; iw++) {
      size_t idx;
      if (!Get1DIndex(iw, ih, idx))
        continue;
      if (is_fixed_masses[idx])
        continue;
      // if (!IntersectSphereFix(iw, ih, Vec3(0, 2, -1), 1.0f))
        local_or_world_positions[idx] += world_velocities[idx] * this->delta_time;
        
    }
}

void RectCloth::WorldToLocalPositions() {

  /*! TODO: implement this: transform mass positions from world coordinate to local coordinate
   *
   *        note: you may find `class Transform` in `transform.h` helpful
   */
  for (auto pos : local_or_world_positions) {
    Vec4 pos4 = modelMatrixInverse * Vec4(pos, 1);
    pos = Vec3(pos4) / pos4.w;
  }
}

void RectCloth::Simulate(unsigned num_steps) {
  for (unsigned i = 0; i < num_steps; ++i) {
    LocalToWorldPositions();
    ComputeAccelerations();
    ComputeVelocities();
    ComputePositions();
    WorldToLocalPositions();
  }
}

/////////////////
/// rendering ///
/////////////////

void RectCloth::UpdateMeshVertices() {
  // set vertex positions
  for (size_t i = 0; i < local_or_world_positions.size(); ++i)
    vertices[i].position = local_or_world_positions[i];

  // reset vertex normals
  auto compute_normal = [&](auto v1, auto v2, auto v3) {
    return glm::normalize(glm::cross(vertices[v2].position - vertices[v1].position, vertices[v3].position - vertices[v1].position));
  };

#pragma omp parallel for collapse(2)
  for (int ih = 0; ih < mass_dim.y; ++ih)
    for (int iw = 0; iw < mass_dim.x; ++iw) {
      constexpr Float w_small = Float(0.125);
      constexpr Float w_large = Float(0.25);

      auto i  = Get1DIndex(iw, ih);
      auto l  = Get1DIndex(iw - 1, ih);
      auto r  = Get1DIndex(iw + 1, ih);
      auto u  = Get1DIndex(iw, ih + 1);
      auto d  = Get1DIndex(iw, ih - 1);
      auto lu = Get1DIndex(iw - 1, ih + 1);
      auto rd = Get1DIndex(iw + 1, ih - 1);
      auto& normal = vertices[i].normal;

      normal = { 0, 0, 0 };

      if (iw > 0 && ih < mass_dim.y - 1) {
        normal += compute_normal(l, i, lu) * w_small;
        normal += compute_normal(i, u, lu) * w_small;
      }
      if (iw < mass_dim.x - 1 && ih < mass_dim.y - 1) {
        normal += compute_normal(i, r, u) * w_large;
      }
      if (iw > 0 && ih > 0) {
        normal += compute_normal(l, d, i) * w_large;
      }
      if (iw < mass_dim.x - 1 && ih > 0) {
        normal += compute_normal(d, rd, i) * w_small;
        normal += compute_normal(rd, r, i) * w_small;
      }

      normal = glm::normalize(normal);
    }

  // vbo
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertices.size(), vertices.data(), buffer_data_usage_vbo);
  glBindVertexArray(0);
}


//////////////////////////
/// collision handling ///
//////////////////////////

bool RectCloth::IntersectSphereFix(size_t iw, size_t ih, Vec3 sphere_center, Float sphere_radius) {
  Vec3 local_sphere_center = -Vec3(modelMatrixInverse * Vec4(sphere_center, 1));
  size_t thisidx;
  size_t idx1;
  size_t idx2;
  Get1DIndex(iw, ih, thisidx);
  for (int i = 0; i < 12; i+=2) {
    if (!Get1DIndex(iw + nearMesh[i].x, ih + nearMesh[i].y, idx1) || !Get1DIndex(iw + nearMesh[i+1].x, ih + nearMesh[i+1].y, idx2))
      continue;
    float distance = glm::distance(local_sphere_center, ClosestPtPointTriangle(local_sphere_center, local_or_world_positions[idx1], local_or_world_positions[idx2], local_or_world_positions[thisidx]));
    if (distance < sphere_radius) {
      return true;
    }
  }
  return false;
  // return glm::length(local_or_world_positions[idx] - local_sphere_center) < sphere_radius;
}


//////////////////////////
/// supporting methods ///
//////////////////////////

size_t RectCloth::Get1DIndex(int iw, int ih) const {
  return size_t(ih) * size_t(mass_dim.x) + size_t(iw);
}

bool RectCloth::Get1DIndex(int iw, int ih, size_t &idx) const {
  if (iw < 0 || ih < 0 || iw >= mass_dim.x || ih >= mass_dim.y)
    return false;
  idx = size_t(ih) * size_t(mass_dim.x) + size_t(iw);
  return true;
}

void RectCloth::InitializeOnlyOnce() {
  modelMatrix = object->transform->ModelMat();
  modelMatrixInverse = glm::inverse(modelMatrix);
}