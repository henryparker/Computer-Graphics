#include <bezier.h>
#include <utils.h>
#include <vector>
#include <fstream>

#define DELTA 0.1

vec3 deCasteljau(std::vector<vec3>& control_points, int i, int j, float t, vec3& tangent) {
  if (i == 1)
    return control_points[j];
  else if (i == control_points.size()) {
    vec3 vertex1 = deCasteljau(control_points, i - 1, j, t, tangent);
    vec3 vertex2 = deCasteljau(control_points, i - 1, j + 1, t, tangent);
    tangent = glm::normalize(vertex1 - vertex2);
    return (1 - t) * vertex1 + t * vertex2;
  }
  else
    return (1 - t) * deCasteljau(control_points, i - 1, j, t, tangent) + t * deCasteljau(control_points, i - 1, j + 1, t, tangent);
}

vec4 NURBS_deCasteljau(std::vector<vec4>& control_points, int i, int j, float t, vec3& tangent) {
  if (i == 1)
    return control_points[j];
  else if (i == control_points.size()) {
    vec4 vertex1 = NURBS_deCasteljau(control_points, i - 1, j, t, tangent);
    vec4 vertex2 = NURBS_deCasteljau(control_points, i - 1, j + 1, t, tangent);
    vec3 point1 = vec3(vertex1[0]/vertex1[3], vertex1[1]/vertex1[3], vertex1[2]/vertex1[3]);
    vec3 point2 = vec3(vertex2[0]/vertex2[3], vertex2[1]/vertex2[3], vertex2[2]/vertex2[3]);
    tangent = glm::normalize(point1 - point2);
    return (1 - t) * vertex1 + t * vertex2;
  }
  else
    return (1 - t) * NURBS_deCasteljau(control_points, i - 1, j, t, tangent) + t * NURBS_deCasteljau(control_points, i - 1, j + 1, t, tangent);
}

BezierCurve::BezierCurve(int m) { control_points_.resize(m); }

BezierCurve::BezierCurve(std::vector<vec3>& control_points) {
  control_points_ = control_points;
}

void BezierCurve::setControlPoint(int i, vec3 point) {
  control_points_[i] = point;
}

/**
 * TODO: evaluate the point at t with the given control_points
 */
Vertex BezierCurve::evaluate(std::vector<vec3>& control_points, float t) {
  Vertex vertex;
  vertex.position = deCasteljau(control_points, control_points.size(), 0, t, vertex.normal);
  return vertex;
}

Vertex BezierCurve::evaluate(float t) {
  return evaluate(control_points_, t);
}

/**
 * TODO: generate an Object of the current Bezier curve
 */
Object BezierCurve::generateObject() {
  return Object();
}

BezierSurface::BezierSurface(int m, int n) {
  control_points_m_.resize(m);
  for (auto& sub_vec : control_points_m_) {
    sub_vec.resize(n);
  }
  control_points_n_.resize(n);
  for (auto& sub_vec : control_points_n_) {
    sub_vec.resize(m);
  }
}

/**
 * @param[in] i: index (i < m)
 * @param[in] j: index (j < n)
 * @param[in] point: the control point with index i, j
 */
void BezierSurface::setControlPoint(int i, int j, vec3 point) {
  control_points_m_[i][j] = point;
  control_points_n_[j][i] = point;
}

Vertex BezierSurface::evaluate(std::vector<std::vector<vec3>>& control_points,
                               float u, float v) {
  std::vector<vec3> generated_points_u;
  std::vector<vec3> generated_points_v;
  Vertex vertex_u, vertex_v, result;
  vec3 tmp_tangent;
  int m = control_points.size();
  int n = control_points[0].size();
  BezierCurve curve(0);
  
  // m dimension
  for (int i = 0; i < m; i++ ) {
    std::vector<vec3> curve_points;
    for (int j = 0; j < n; j++ ) {
      curve_points.push_back(control_points[i][j]);
    }
    generated_points_u.push_back(deCasteljau(curve_points, n, 0, u, tmp_tangent));
  }
  vertex_u = curve.evaluate(generated_points_u, v);

  // n dimension
  for (int i = 0; i < n; i++ ) {
    std::vector<vec3> curve_points;
    for (int j = 0; j < m; j++ ) {
      curve_points.push_back(control_points[j][i]);
    }
    generated_points_v.push_back(deCasteljau(curve_points, m, 0, v, tmp_tangent));
  }
  vertex_v = curve.evaluate(generated_points_v, u);

  // return
  result.position = vertex_u.position;
  result.normal = glm::cross(vertex_u.normal, vertex_v.normal);
  return result;
}

Vertex BezierSurface::NURBS_evaluate(std::vector<std::vector<vec4>>& control_points, float u, float v) {
  
  Vertex vertex_u, vertex_v, result;
  vec3 tmp_tangent, u_tangent, v_tangent;
  int m = control_points.size();
  int n = control_points[0].size();
  BezierCurve curve(0);

  // m dimension
  std::vector<vec4> generated_points_u;
  for (int i = 0; i < m; i++ ) {
    std::vector<vec4> curve_points;
    for (int j = 0; j < n; j++ ) {
      curve_points.push_back(vec4(control_points[i][j][0]*control_points[i][j][3],
                                  control_points[i][j][1]*control_points[i][j][3],
                                  control_points[i][j][2]*control_points[i][j][3],
                                  control_points[i][j][3]));
    }
    generated_points_u.push_back(NURBS_deCasteljau(curve_points, n, 0, u, tmp_tangent));
  }

  vec4 u_position = NURBS_deCasteljau(generated_points_u, m, 0, v, u_tangent);

  // n dimension
  std::vector<vec4> generated_points_v;
  for (int i = 0; i < n; i++ ) {
    std::vector<vec4> curve_points;
    for (int j = 0; j < m; j++ ) {
      curve_points.push_back(vec4(control_points[i][j][0]*control_points[i][j][3],
                                  control_points[i][j][1]*control_points[i][j][3],
                                  control_points[i][j][2]*control_points[i][j][3],
                                  control_points[i][j][3]));
    }
    generated_points_v.push_back(NURBS_deCasteljau(curve_points, m, 0, v, tmp_tangent));
  }

  vec4 v_position = NURBS_deCasteljau(generated_points_v, n, 0, u, v_tangent);

  // return
  result.position = vec3(u_position[0]/u_position[3], u_position[1]/u_position[3], u_position[2]/u_position[3]);
  result.normal = glm::cross(u_tangent, v_tangent);
  return result;
}

/**
 * TODO: generate an Object of the current Bezier surface
 */
Object BezierSurface::generateObject() {

  return Object();
}

/**
 * TODO: read in bzs file to generate a vector of Bezier surface
 * for the first line we have b p m n
 * b means b bezier surfaces, p means p control points.
 *
 */
std::vector<BezierSurface> read(const std::string &path) {
  // data store
  std::ifstream infile(path, std::ifstream::in);
  if (!infile.is_open()) {
    std::cout << "ERROE::MESH::LOAD::FILE_OPEN_FAIL" << std::endl;
  }
  int b, p, m, n;
  int tmp_int;
  float tmpf_x, tmpf_y, tmpf_z;
  infile >> b >> p >> m >> n;
  std::vector<std::vector<int>> control_index;
  std::vector<vec3> control_points;

  for (int i = 0; i < b; i++ ) {
    std::vector<int> single_index;
    for (int j = 0; j < m * n; j++ ) {
      infile >> tmp_int;
      single_index.push_back(tmp_int);
    }
    control_index.push_back(single_index);
  }

  for (int i = 0; i < p; i++ ) {
    infile >> tmpf_x >> tmpf_y >> tmpf_z;
    // std::cout << tmpf_x << ' ' << tmpf_y << ' ' << tmpf_z << std::endl;
    control_points.push_back(vec3(tmpf_x, tmpf_y, tmpf_z));
  }

  // store check
  // std::cout << "control_index" << std::endl;
  // for (int i = 0; i < control_index.size(); i++ ) {
  //   for (int j = 0; j < m*n; j++ ) {
  //     std::cout << control_index[i][j];
  //     std::cout << ' ';
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << "control_points" << std::endl;
  // for (int i = 0; i < control_points.size(); i++ ) {
  //   for (int j = 0; j < 3; j++ ) {
  //     std::cout << control_points[i][j];
  //     std::cout << ' ';
  //   }
  //   std::cout << std::endl;
  // }

  // generating bezier surface
  std::vector<BezierSurface> surfaces;
  for (int i = 0; i < b; i++ ) {
    std::vector<std::vector<vec3>> processing_control_arary;
    for (int j = 0; j < m; j++ ) {
      std::vector<vec3> processing_control_line;
      for (int k = 0; k < n; k++ ) {
        processing_control_line.push_back(control_points[control_index[i][j*m+k]]);
      }
      processing_control_arary.push_back(processing_control_line);
    }

    BezierSurface single_surface(0,0);
    std::vector<Vertex> surface_points;
    std::vector<GLuint> indices;
    for (float u = 0; u <= 1.01; u += DELTA)
        for (float v = 0; v <= 1.01; v += DELTA)
            surface_points.push_back(single_surface.evaluate(processing_control_arary, u, v));
    
    int row = 1 / (float)DELTA + 1;
    int column = 1 / (float)DELTA + 1;
    // std::cout << "size of surface points is " << surface_points.size() << std::endl;
    
    for (int j = 0; j < row - 1; j++ ) {
        for (int k = 0; k < column - 1; k++ ) {
            indices.push_back(row*j+k);
            indices.push_back(row*j+k+1);
            indices.push_back(row*(j+1)+k);

            indices.push_back(row*j+k+1);
            indices.push_back(row*(j+1)+k);
            indices.push_back(row*(j+1)+k+1);
        }
    }
    // std::cout << "the surface index is " << i << std::endl;
    // std::cout << "indices size is " << indices.size() << std::endl;
    // if (i == 0) {
    //   for (int j = 0; j < indices.size(); j+=3) {
    //     std::cout << indices[j];
    //     std::cout << ' ';
    //     std::cout << indices[j+1];
    //     std::cout << ' ';
    //     std::cout << indices[j+2];
    //     std::cout << ' ';
    //     std::cout << std::endl;
    //   }
    // }
    single_surface.actual_points.assign(surface_points.begin(), surface_points.end());
    single_surface.indices.assign(indices.begin(), indices.end());
    surfaces.push_back(single_surface);
  }

  return surfaces;
}