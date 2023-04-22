#include "camera.h"
#include <iostream>

Camera::Camera()
    : position(0, -1, 0), fov(45), focal_len(1) {
  lookAt({0, 0, 0}, {0, 0, 1});
}
// return the lower left corner of each pixel
Ray Camera::generateRay(float dx, float dy) {
  // You need to generate ray according to screen coordinate (dx, dy)
  Eigen::Vector3f screen_center = position + focal_len * forward;

  // std::cout << "screen_center: (" << screen_center.x() << ',' << screen_center.y() << ',' << screen_center.z() << ")\n";
  Eigen::Vector3f vertical = focal_len * tanf(fov * PI / 360) * 2 * up;
  Eigen::Vector3f horizontal = focal_len * tanf(fov * PI / 360) * image->getAspectRatio() * 2 * right;

  Eigen::Vector3f lower_left_corner = screen_center - vertical / 2 - horizontal / 2;
  // std::cout << "lower_left_corner: (" << lower_left_corner.x() << ',' << lower_left_corner.y() << ',' << lower_left_corner.z() << ")\n";
  Eigen::Vector3f dx_dy_pos = lower_left_corner + dx*horizontal / image->getResolution().x() + dy*vertical / image->getResolution().y();

  // std::cout << "dx = " << dx << ", dy = " << dy << "position: (" << dx_dy_pos.x() << ',' << dx_dy_pos.y() << ',' << dx_dy_pos.z() << ")\n";

  return Ray(position, dx_dy_pos - position);
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
  forward = (look_at - position).normalized();
  right = forward.cross(ref_up).normalized();
  up = right.cross(forward);
  // std::cout << "forward: (" << forward.x() << ',' << forward.y() << ',' << forward.z() << ")\n";
  // std::cout << "right: (" << right.x() << ',' << right.y() << ',' << right.z() << ")\n";
  // std::cout << "up: (" << up.x() << ',' << up.y() << ',' << up.z() << ")\n";
}
void Camera::setPosition(const Vec3f &pos) {
  position = pos;
}
Vec3f Camera::getPosition() const {
  return position;
}
void Camera::setFov(float new_fov) {
  fov = new_fov;
}
float Camera::getFov() const {
  return fov;
}

void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
  image = img;
}
std::shared_ptr<ImageRGB> &Camera::getImage() {
  return image;
}

std::vector<Vec2f> Camera::sampleInPx(float dx, float dy, int samplesqrt) {
  std::vector<Vec2f> samples;
  float delta_x = 1.0f/(samplesqrt + 1);
  float delta_y = 1.0f/(samplesqrt + 1);
  for (int i = 1; i <= samplesqrt; i++ ) {
    for (int j = 1; j <= samplesqrt; j++ ) {
      samples.push_back(Vec2f(dx + i * delta_x, dy + j * delta_y));
    }
  }
  return samples;
}