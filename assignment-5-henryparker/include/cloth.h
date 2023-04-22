#pragma once

#include "mesh.h"
#include "time_system.h"
#include <chrono>
#include "util.h"

const int springIndexOffset[12][2] = {
  {-2, 0}, {-1, 0}, {1, 0}, {2, 0},
  {0, -2}, {0, -1}, {0, 1}, {0, 2},
  {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
};

const float springIndexNorm[12] = {
  2, 1, 1, 2,
  2, 1, 1, 2,
  1.4142135624, 1.4142135624, 1.4142135624, 1.4142135624
};

const UVec2 nearMesh[12] = {
  {-1, 0}, {0, -1}, {0, -1}, {1, -1},
  {1, -1}, {1, 0}, {1, 0}, {0, 1},
  {0, 1}, {-1, 1}, {-1, 1}, {-1, 0}
};

class RectCloth : public Mesh {
 public:

  /// constructor

  RectCloth(Float cloth_weight,
            const UVec2& mass_dim,
            Float dx_local,
            Float stiffness, Float damping_ratio);

  RectCloth(const RectCloth&) = default;
  RectCloth(RectCloth&&) = default;
  RectCloth& operator=(const RectCloth&) = default;
  RectCloth& operator=(RectCloth&&) = default;
  virtual ~RectCloth() override = default;



  /// interfaces

  bool SetMassFixedOrNot(int iw, int ih, bool fixed_or_not);

  virtual void FixedUpdate() override;

  Sampler sample;

 private:
  static constexpr unsigned simulation_steps_per_fixed_update_time = 100;
  static constexpr Float fixed_delta_time = Time::fixed_delta_time / Float(simulation_steps_per_fixed_update_time);

  UVec2 mass_dim;
  Vec3 local_wind;
  Mat4 modelMatrix;
  Mat4 modelMatrixInverse;

  int initializeFlag;

  Float mass_weight;

  Float dx_local;

  Float stiffness;
  Float damping_ratio;
  Float delta_time;

  std::vector<bool> is_fixed_masses;
  std::vector<Vec3> local_or_world_positions;
  std::vector<Vec3> world_velocities;
  std::vector<Vec3> world_accelerations;



  /// force computation

  [[nodiscard]] Vec3 ComputeHookeForce(int iw_this, int ih_this,
                                       int iw_that, int ih_that,
                                       Float dx_world) const;

  [[nodiscard]] Vec3 ComputeSpringForce(int iw, int ih) const;



  /// simulation pipeline

  void LocalToWorldPositions();

  void ComputeAccelerations();

  void ComputeVelocities();

  void ComputePositions();

  void WorldToLocalPositions();

  void Simulate(unsigned num_steps);



  /// rendering

  void UpdateMeshVertices();

  // intersect
  // void IntersectSphereFix();
  bool IntersectSphereFix(size_t iw, size_t ih, Vec3 sphere_center, Float sphere_radius);

  /// supporting methods

  [[nodiscard]] size_t Get1DIndex(int iw, int ih) const;
  bool Get1DIndex(int iw, int ih, size_t& idx) const;
  void InitializeOnlyOnce();
};
