#include <iostream>
#include <chrono>

#include "integrator.h"

#ifndef TESTBYGITHUB    // PLEASE DO NOT CHANGE
int scene_id = 0;
#else                   // PLEASE DO NOT CHANGE
int scene_id = 0;       // PLEASE DO NOT CHANGE
#endif                  // PLEASE DO NOT CHANGE
void setSceneById(std::shared_ptr<Scene> &scene, int id);

int main(int argc, char *argv[]) {
#ifndef TESTBYGITHUB                    // PLEASE DO NOT CHANGE
  Vec2i img_resolution(400, 400);
#else                                   // PLEASE DO NOT CHANGE
    Vec2i img_resolution(32, 32);     // PLEASE DO NOT CHANGE
#endif                                  // PLEASE DO NOT CHANGE
  // Vec3f cam_pos(0, -3, 0);
  // Vec3f cam_look_at(0, 0, 0);
  Vec3f cam_pos(0, -3, 1.3);
  Vec3f cam_look_at(0, 0, 1);
  // init image.
  std::shared_ptr<ImageRGB> rendered_img
      = std::make_shared<ImageRGB>(img_resolution.x(), img_resolution.y());
  // camera setting
  std::shared_ptr<Camera> camera = std::make_shared<Camera>();
  
  camera->setPosition(cam_pos);
  camera->lookAt(cam_look_at);
  // camera->setFov(120);
  camera->setFov(45);
  camera->setImage(rendered_img);

  // test code
    // trangle test
    // Triangle tran(Vec3f(0,1,0),Vec3f(1,0,0),Vec3f(-1,0,0));
    // Ray ra(Vec3f(0,0,1), Vec3f(0,1,-2));
    // bool test = tran.intersect(ra, Interaction());
    
    // Rectangle test
    // Rectangle rect(Vec3f(0, 0, 0), Vec2f(2, 2), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
    // Ray ra(Vec3f(0,0,1), Vec3f(0,1,-2));
    // bool test = rect.intersect(ra, Interaction());

    // Ellipsoid test
    // Vec4f vec1(1, 2, 3, 4);
    // Vec3f vec2 = vec1.head<3>();
    // std::cout << "vec2: (" << vec2.x() << ',' << vec2.y() << ',' << vec2.z() << ")\n";
    // Ellipsoid elli(Vec3f(0.5, 0.4, 0.4),
    //                       Vec3f(0.3, 0.1, 0),
    //                       Vec3f(-0.1, 0.3, 0),
    //                       Vec3f(0, 0, 0.4));
    // Ray ra(Vec3f(-1,-1,-1), Vec3f(0.5, 0.4, 0.4));
    // bool test = elli.intersect(ra, Interaction());

  // construct scene.
  std::shared_ptr<Scene> scene = std::make_shared<Scene>();
  setSceneById(scene, scene_id);

  std::unique_ptr<PhongLightingIntegrator> integrator
      = std::make_unique<PhongLightingIntegrator>(camera, scene);
  std::cout << "Start Rendering..." << std::endl;
  auto start = std::chrono::steady_clock::now();
  // render scene
  integrator->render();
  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "\nRender Finished in " << time << "ms." << std::endl;
#ifndef TESTBYGITHUB                                    // PLEASE DO NOT CHANGE
  rendered_img->writeImgToFile("../result.png");
#else                                                   // PLEASE DO NOT CHANGE
    rendered_img->writeImgToFile("../submit.png");      // PLEASE DO NOT CHANGE
#endif                                                  // PLEASE DO NOT CHANGE
  std::cout << "Image saved to disk." << std::endl;
  return 0;
}

void setSceneById(std::shared_ptr<Scene> &scene, int id) {
  Vec3f light_pos(0, 0, 1.95f);
  Vec3f light_color(1, 1, 1);
  Vec2f light_dim(0.5, 0.3);
  Vec3f light_n(0, 0, -1);
  Vec3f light_t(1, 0, 0);
  std::shared_ptr<Light>
      square_light = std::make_shared<SquareAreaLight>(light_pos, light_color, light_dim, light_n, light_t);

  std::shared_ptr<Material>
      mat_white =
      std::make_shared<ConstColorMat>(Vec3f(0.7f, 0.7f, 0.7f));
  std::shared_ptr<Material>
      mat_red =
      std::make_shared<ConstColorMat>(Vec3f(0.8f, 0.f, 0.f));
  std::shared_ptr<Material>
      mat_green =
      std::make_shared<ConstColorMat>(Vec3f(0.f, 0.8f, 0.f));
  std::shared_ptr<Material>
      mat_yellow =
      std::make_shared<ConstColorMat>(Vec3f(1, 0.93, 0.6));
  std::shared_ptr<Material>
      mat_blue =
      std::make_shared<ConstColorMat>(Vec3f(0.2, 0.5, 0.9));
  // Texture
  // std::shared_ptr<Material>
  //     tex_grid = 
  //     std::make_shared<TextureMat>("../../textures/grid.png", 16.0f);

  std::shared_ptr<Geometry>
      floor = std::make_shared<Rectangle>(Vec3f(0, 0, 0), Vec2f(2, 2), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
  std::shared_ptr<Geometry>
      ceiling = std::make_shared<Rectangle>(Vec3f(0, 0, 2), Vec2f(2, 2), Vec3f(0, 0, -1), Vec3f(1, 0, 0));
  std::shared_ptr<Geometry>
      wall_left = std::make_shared<Rectangle>(Vec3f(-1, 0, 1), Vec2f(2, 2), Vec3f(1, 0, 0), Vec3f(0, 0, 1));
  std::shared_ptr<Geometry>
      wall_right = std::make_shared<Rectangle>(Vec3f(1, 0, 1), Vec2f(2, 2), Vec3f(-1, 0, 0), Vec3f(0, 0, -1));
  std::shared_ptr<Geometry>
      wall_back = std::make_shared<Rectangle>(Vec3f(0, 1, 1), Vec2f(2, 2), Vec3f(0, -1, 0), Vec3f(1, 0, 0));

  switch (id) {
    case 0 : {
      floor->setMaterial(mat_white);
      ceiling->setMaterial(mat_white);
      wall_back->setMaterial(mat_white);
      wall_left->setMaterial(mat_red);
      wall_right->setMaterial(mat_green);
      scene->addGeometry(floor);
      scene->addGeometry(ceiling);
      scene->addGeometry(wall_left);
      scene->addGeometry(wall_right);
      scene->addGeometry(wall_back);
      addTetrahedronToScene(scene,
                            mat_blue,
                            Vec3f(-1, 0.2, 0.2),
                            Vec3f(-0.5, -0.65, 0.2),
                            Vec3f(0, 0.2, 0.2),
                            Vec3f(-0.5, -0.3774, 1));
      addEllipsoidToScene(scene,
                          mat_yellow,
                          Vec3f(0.5, 0.4, 0.4),
                          Vec3f(0.3, 0.1, 0),
                          Vec3f(-0.1, 0.3, 0),
                          Vec3f(0, 0, 0.4));
      scene->setLight(square_light);
      scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
      break;
    }
    case 1:
      UNIMPLEMENTED
      // You can add your custom scene here.
    default:break;
  }

}
