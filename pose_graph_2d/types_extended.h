// Ceres Solver - A fast non-linear least squares minimizer
// Copyright 2016 Google Inc. All rights reserved.
// http://ceres-solver.org/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of Google Inc. nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: vitus@google.com (Michael Vitus)
//
// Defines the types used in the 2D pose graph SLAM formulation. Each vertex of
// the graph has a unique integer ID with a position and orientation. There are
// delta transformation constraints between two vertices.

#ifndef CERES_EXAMPLES_POSE_GRAPH_2D_TYPES_EXTENDED_H_
#define CERES_EXAMPLES_POSE_GRAPH_2D_TYPES_EXTENDED_H_

#include <fstream>

#include "Eigen/Core"
#include "normalize_angle.h"

namespace ceres {
namespace examples {

struct PoseBase{
  virtual ~PoseBase(){};

  virtual void operator>>(std::istream& input){};
  // The name of the data type in the g2o file format.
  static std::string name() {
    return "VERTEX";
  }
};

// The state for each vertex in the pose graph.
struct Pose2d : public PoseBase {
  double x;
  double y;
  double yaw_radians;

  void operator>>(std::istream& input) {
    input >> x >> y >> yaw_radians;
    // Normalize the angle between -pi to pi.
    yaw_radians = NormalizeAngle(yaw_radians);

  }

  // The name of the data type in the g2o file format.
  static std::string name() {
    return "VERTEX_SE2";
  }
};

// The state for each lanmark vertex in the pose graph.
// we do not have orientation info for landmarks
struct Pose2dXY :public PoseBase {
  double x;
  double y;

  // The name of the data type in the g2o file format.
  static std::string name() {
    return "VERTEX_XY";
  }

  void operator>>(std::istream& input) {
    input >> x >> y;
  }

};

struct ConstraintBase{
  virtual ~ConstraintBase(){};
  // The name of the data type in the g2o file format.

  virtual void operator>>(std::istream& input){};

  static std::string name() {
    return "EDGE";
  }
};

// The constraint between two vertices in the pose graph. The constraint is the
// transformation from vertex id_begin to vertex id_end.
struct Constraint2d : public ConstraintBase {
  int id_begin;
  int id_end;

  double x;
  double y;
  double yaw_radians;

  // The inverse of the covariance matrix for the measurement. The order of the
  // entries are x, y, and yaw.
  Eigen::Matrix3d information;

  void operator>>(std::istream& input) {
    input >> id_begin >> id_end >> x >>
        y >> yaw_radians >>
        information(0, 0) >> information(0, 1) >>
        information(0, 2) >> information(1, 1) >>
        information(1, 2) >> information(2, 2);

    // Set the lower triangular part of the information matrix.
    information(1, 0) = information(0, 1);
    information(2, 0) = information(0, 2);
    information(2, 1) = information(1, 2);

    // Normalize the angle between -pi to pi.
    yaw_radians = NormalizeAngle(yaw_radians);
  }

  // The name of the data type in the g2o file format.
  static std::string name() {
    return "EDGE_SE2";
  }
};

// The constraint between a vertex and a landmark in the pose graph. The constraint is the
// transformation from vertex id_begin to vertex id_end.
struct Constraint2dXY : public ConstraintBase {
  int id_begin;
  int id_end;

  double x;
  double y;

  // The inverse of the covariance matrix for the measurement. The order of the
  // entries are x, y.
  Eigen::Matrix2d information;

  void operator>>(std::istream& input) {
    input >> id_begin >> id_end >> x >>
        y >> information(0, 0) >> information(0, 1) >>
        information(1, 1);

    // Set the lower triangular part of the information matrix.
    information(1, 0) = information(0, 1);

  }

  // The name of the data type in the g2o file format.
  static std::string name() {
    return "EDGE_SE2_XY";
  }
};


}  // namespace examples
}  // namespace ceres

#endif  // CERES_EXAMPLES_POSE_GRAPH_2D_TYPES_EXTENDED_H_