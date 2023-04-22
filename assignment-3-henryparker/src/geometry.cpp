#include "geometry.h"

#include <utility>
#include <iostream>
#include "utils.h"

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2)
    : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)) {
  normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
  if (ray.direction.dot(normal) == 0) {
    // std::cout << "ray.direction: (" << ray.direction.x() << ',' << ray.direction.y() << ',' << ray.direction.z() << ")\n";
    // std::cout << "normal: (" << normal.x() << ',' << normal.y() << ',' << normal.z() << ")\n";
    return false;
  }

  Vec3f e1 = v1 - v0;
  Vec3f e2 = v2 - v0;
  Vec3f s = ray.origin - v0;
  Vec3f s1 = ray.direction.cross(e2);
  Vec3f s2 = s.cross(e1);
  float t = s2.dot(e2) / s1.dot(e1);
  float b1 = s1.dot(s) / s1.dot(e1);
  float b2 = s2.dot(ray.direction) / s1.dot(e1);
  
  Vec3f Pos = ray.origin + t * ray.direction;
  float distance = (ray.origin - Pos).norm();
  Vec3f Pos_normal = normal.dot(ray.direction) < 0 ? normal : -normal;
  // std::cout << "t = " << t << " b1 = " << b1 << " b2 = " << b2 << ", distance = " << distance << std::endl;
  // std::cout << "Pos_normal: (" << Pos_normal.x() << ',' << Pos_normal.y() << ',' << Pos_normal.z() << ")\n";

  if (b1 >= 0 && b2 >= 0 && b1 + b2 <= 1.001 && distance < interaction.dist && t >= ray.t_min && t <= ray.t_max) {
    interaction.pos = Pos;
    interaction.dist = distance;
    interaction.normal = Pos_normal;
    interaction.uv = Vec2f(b1, b2);
    // ----------------------------------------------
    interaction.model = material->evaluate(interaction);
    // ----------------------------------------------
    interaction.type = Interaction::Type::GEOMETRY;
    return true;
  }

  return false;
}

Rectangle::Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      size(std::move(dimension)),
      normal(std::move(normal).normalized()),
      tangent(std::move(tangent).normalized()) {}

bool Rectangle::intersect(Ray &ray, Interaction &interaction) const {
  if (ray.direction.dot(normal) == 0) {
    return false;
  }
  Vec3f e1 = tangent.normalized() * size.x() / 2.0f;
  Vec3f e2 = tangent.cross(normal).normalized() * size.y() / 2.0f;
  Vec3f s = ray.origin - position;
  Vec3f s1 = ray.direction.cross(e2);
  Vec3f s2 = s.cross(e1);
  float t = s2.dot(e2) / s1.dot(e1);
  float b1 = s1.dot(s) / s1.dot(e1);
  float b2 = s2.dot(ray.direction) / s1.dot(e1);

  Vec3f Pos = ray.origin + t * ray.direction;
  float distance = (ray.origin - Pos).norm();
  Vec3f Pos_normal = normal.dot(ray.direction) < 0 ? normal : -normal;

  if (b1 >= -1.001 && b2 >= -1.001 && b1 <= 1.001 && b2 <= 1.001 && distance < interaction.dist && t >= ray.t_min && t <= ray.t_max) {
    interaction.pos = Pos;
    interaction.dist = distance;
    interaction.normal = Pos_normal;
    interaction.uv = Vec2f((b1 + 1) / 2, (b2 + 1) / 2);
    // ----------------------------------------------
    if(material!=nullptr){
      interaction.model = material->evaluate(interaction);
    }
    // ----------------------------------------------
    interaction.type = Interaction::Type::GEOMETRY;
    return true;
  }
  return false;
}
Vec2f Rectangle::getSize() const {
  return size;
}
Vec3f Rectangle::getNormal() const {
  return normal;
}
Vec3f Rectangle::getTangent() const {
  return tangent;
}

Ellipsoid::Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c)
    : p(p), a(a), b(b), c(c) {}

bool Ellipsoid::intersect(Ray &ray, Interaction &interaction) const {
  Vec3f a_normalize = a.normalized();
  Vec3f b_normalize = b.normalized();
  Vec3f c_normalize = c.normalized();

  Matrix4f T{
    {1, 0, 0, p.x()},
    {0, 1, 0, p.y()},
    {0, 0, 1, p.z()},
    {0, 0, 0, 1}
  };
  Matrix4f R{
    {a_normalize.x(), b_normalize.x(), c_normalize.x(), 0},
    {a_normalize.y(), b_normalize.y(), c_normalize.y(), 0},
    {a_normalize.z(), b_normalize.z(), c_normalize.z(), 0},
    {0, 0, 0, 1}
  };
  Matrix4f S{
    {a.norm(), 0, 0, 0},
    {0, b.norm(), 0, 0},
    {0, 0, c.norm(), 0},
    {0, 0, 0, 1}
  };
  Matrix4f M = T*R*S;
  Matrix4f M_inverse = M.inverse();

  Vec3f ray_origin_trans = (M_inverse * Vec4f(ray.origin.x(), ray.origin.y(), ray.origin.z(), 1)).head<3>();
  Vec3f ray_direction_trans = (M_inverse * Vec4f(ray.direction.x(), ray.direction.y(), ray.direction.z(), 0)).head<3>();
  
  Vec3f center(0, 0, 0);
  float radius = 1;

  Vec3f oc = ray_origin_trans - center;
  auto a = ray_direction_trans.dot(ray_direction_trans);
  auto half_b = oc.dot(ray_direction_trans);
  auto c = oc.dot(oc) - radius*radius;

  auto discriminant = half_b*half_b - a*c;
  if (discriminant < 0) 
    return false;
  auto sqrtd = sqrt(discriminant);
  auto root = (-half_b - sqrtd) / a;

  Vec3f sphere_point = ray_origin_trans + root * ray_direction_trans;
  Vec3f sphere_normal = (sphere_point - center) / radius;
  Vec4f sphere_normal4f(sphere_normal.x(), sphere_normal.y(), sphere_normal.z(), 1);

  // float theta = acos(sphere_point.z()) / (2*PI);
  // float phi = atan2(sphere_point.y() , sphere_point.x()) / (2*PI);
  // if (theta < 0 || theta > 1 || phi < 0 || phi > 1)
  // std::cout << "theta = " << theta << " phi = " << phi << std::endl;
  // Vec3f shere_xy_plane = sphere_point - sphere_point * Vec3f(0, 0, 1);


  // std::cout << "sphere_point: (" << sphere_point.x() << ',' << sphere_point.y() << ',' << sphere_point.z() << ")\n";
  // std::cout << "sphere_normal: (" << sphere_normal.x() << ',' << sphere_normal.y() << ',' << sphere_normal.z() << ")\n";

  Vec3f ellip_normal = (M_inverse.eval().transpose() * sphere_normal4f).head<3>();
  Vec3f ellip_point = ray.origin + root * ray.direction;
  // std::cout << "ellip_normal: (" << ellip_normal.x() << ',' << ellip_normal.y() << ',' << ellip_normal.z() << ")\n";

  if ((ray.origin - ellip_point).norm() < interaction.dist && root >= ray.t_min && root <= ray.t_max) {
    interaction.pos = ellip_point;
    interaction.dist = (ray.origin - ellip_point).norm();
    interaction.normal = ellip_normal.normalized();
    // ----------------------------------------------
    interaction.uv = Vec2f(0, 0);
    interaction.model = material->evaluate(interaction);
    // ----------------------------------------------
    interaction.type = Interaction::Type::GEOMETRY;
    // std::cout << "interaction.pos: (" << interaction.pos.x() << ',' << interaction.pos.y() << ',' << interaction.pos.z() << ")\n";
    // std::cout << "interaction.normal: (" << interaction.normal.x() << ',' << interaction.normal.y() << ',' << interaction.normal.z() << ")\n";
    return true;
  }

  return false;
}
