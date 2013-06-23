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

#ifndef SKYLINES_DOUGLASPEUCKERMOD_H
#define SKYLINES_DOUGLASPEUCKERMOD_H

#include <vector>
#include <list>
#include <memory>

class DouglasPeuckerMod {
private:
  int num_levels;
  int zoom_factor;
  double threshold;
  bool force_endpoints;
  double *zoom_level_breaks;

public:
  DouglasPeuckerMod(int _num_levels = 18,
                    int _zoom_factor = 2,
                    double _threshold = 0.00001,
                    bool _force_endpoints = true);

  ~DouglasPeuckerMod();

  std::vector<int> dpEncode(std::vector<std::vector<double>> &points, char *type);

  int getNumLevels() {
    return num_levels;
  }

  int getZoomFactor() {
    return zoom_factor;
  }

private:
  double distance_dp(std::vector<double> &p0,
                     std::vector<double> &p1,
                     std::vector<double> &p2,
                     std::list<size_t> &points);

  double distance_simple(std::vector<double> &p0,
                         std::vector<double> &p1,
                         std::vector<double> &p2,
                         std::list<size_t> &points);

  int computeLevel(double abs_max_dist);

  std::vector<int> classify(size_t n_points, const double dists[], double abs_max_dist);
};

#endif /* SKYLINES_DOUGLASPEUCKERMOD_H */
