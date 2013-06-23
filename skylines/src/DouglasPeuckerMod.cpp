/* Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2013 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}

  Original code of Douglas-Peucker algorithm by Robert Coup <robert.coup@koordinates.com>
*/

#include "DouglasPeuckerMod.hpp"

#include <stack>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>

DouglasPeuckerMod::DouglasPeuckerMod(const unsigned _num_levels,
                                     const unsigned _zoom_factor,
                                     const double _threshold,
                                     const bool _force_endpoints)
  : num_levels(_num_levels), zoom_factor(_zoom_factor),
    threshold(_threshold), force_endpoints(_force_endpoints) {
  zoom_level_breaks = new double[num_levels];

  for (unsigned i = 0; i < num_levels; i++) {
    zoom_level_breaks[i] = threshold * pow(zoom_factor, num_levels - i - 1);
  }
}

DouglasPeuckerMod::~DouglasPeuckerMod() {
  delete[] zoom_level_breaks;
}

std::vector<int> DouglasPeuckerMod::dpEncode(std::vector<std::vector<double>> &points, char *type) {
  unsigned i,
           max_loc = 0;
  std::stack<std::pair<unsigned, unsigned>> stack;

  double *dists = new double[points.size()];
  std::fill(&dists[0], &dists[points.size()], 0.0);

  double temp,
         max_dist,
         abs_max_dist_squared = 0.0,
         abs_max_dist,
         threshold_squared = pow(threshold, 2);

  /**
   * use normal douglas peucker distance (perpendicular to segment)
   * or use simple distance calculation from adjacent points
   */
  std::list<size_t> points_dp, points_simple;

  for (i = 0; i < sizeof(type) / sizeof(type[0]); i++) {
    if (type[i] == 'd')
      points_simple.push_back(i);
    else if (type[i] == 'p')
      points_dp.push_back(i);
    else
      break;
  }

  for (i; i < points[0].size(); i++)
    points_dp.push_back(i);

  // simplify using Douglas-Peucker
  if (points.size() > 2) {
    stack.push(std::pair<unsigned, unsigned>(0, (points.size() - 1)));

    while (stack.size() > 0) {
      std::pair<unsigned, unsigned> current = stack.top();
      stack.pop();
      max_dist = 0;

      for (unsigned i = current.first + 1; i < current.second; i++) {
        temp = std::max(distance_dp(points[i], points[current.first],
                                    points[current.second], points_dp),
                        distance_simple(points[i], points[current.first],
                                        points[current.second], points_simple));

        if (temp > max_dist) {
          max_dist = temp;
          max_loc = i;
        }
      }

      if (max_dist > abs_max_dist_squared) {
        abs_max_dist_squared = max_dist;
      }

      if (max_dist > threshold_squared) {
        dists[max_loc] = sqrt(max_dist);
        stack.push(std::pair<unsigned, unsigned>(current.first, max_loc));
        stack.push(std::pair<unsigned, unsigned>(max_loc, current.second));
      }
    }
  }

  abs_max_dist = sqrt(abs_max_dist_squared);

  std::vector<int> r = classify(points.size(), dists, abs_max_dist);

  delete[] dists;
  return r;
}

/**
 * distance(p0, p1, p2) computes the distance between the point p0 and the
 * segment [p1,p2]. This could probably be replaced with something that is a
 * bit more numerically stable.
 */
double DouglasPeuckerMod::distance_dp(std::vector<double> &p0,
                                      std::vector<double> &p1,
                                      std::vector<double> &p2,
                                      std::list<size_t> &points) {
  double u,
         out = 0.0,
         u_nom = 0.0,
         u_denom = 0.0;

  if (p1 == p2) {
    for (auto i : points) {
      out += pow(p2[i] - p0[i], 2);
    }
  } else {
    for (auto i : points) {
      u_nom += (p0[i] - p1[i]) * (p2[i] - p1[i]);
    }

    for (auto i : points) {
      u_denom += pow(p2[i] - p1[i], 2);
    }

    u = u_nom / u_denom;

    if (u <= 0) {
      for (auto i : points) {
        out += pow(p0[i] - p1[i], 2);
      }
    } else if (u >= 1) {
      for (auto i : points) {
        out += pow(p0[i] - p2[i], 2);
      }
    } else if (0 < u && u < 1) {
      for (auto i : points) {
        out += pow(p0[i] - p1[i] - u * (p2[i] - p1[i]), 2);
      }
    }
  }

  return out;
}

double DouglasPeuckerMod::distance_simple(std::vector<double> &p0,
                                          std::vector<double> &p1,
                                          std::vector<double> &p2,
                                          std::list<size_t> &points) {
  double out = 0.0;

  for (auto i : points) {
    out += sqrt(abs(p1[i] - p0[i])) + sqrt(abs(p2[i] - p0[i]));
  }

  out = pow(out, 2)/4;

  return pow(out, 2);
}

std::vector<int> DouglasPeuckerMod::classify(size_t n_points,
                                             const double dists[],
                                             double abs_max_dist) {
  std::vector<int> r;

  if (force_endpoints) {
    r.push_back(numLevels - 1);
  } else {
    r.push_back(numLevels - computeLevel(abs_max_dist) - 1);
  }

  if (n_points > 1) {
    for (size_t i = 1; i < n_points - 1; i++) {
      if (dists[i] != 0.0)
        r.push_back(num_levels - computeLevel(dists[i]) - 1);
      else
        r.push_back(-1);
    }

    if (force_endpoints) {
      r.push_back(numLevels - 1);
    } else {
      r.push_back(numLevels - computeLevel(abs_max_dist) - 1);
    }
  }

  return r;
}

/**
 * This computes the appropriate zoom level of a point in terms of it's
 * distance from the relevant segment in the DP algorithm. Could be done in
 * terms of a logarithm, but this approach makes it a bit easier to ensure
 * that the level is not too large.
 */
unsigned DouglasPeuckerMod::computeLevel(const double abs_max_dist) {
  unsigned lev = 0;

  if (abs_max_dist > threshold) {
    while (abs_max_dist < zoom_level_breaks[lev]) {
      lev++;
    }
  }

  return lev;
}
