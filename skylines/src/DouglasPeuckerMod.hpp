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

struct FlightFix;

class DouglasPeuckerMod {
private:
  const unsigned num_levels;
  const unsigned zoom_factor;
  const double threshold;
  const bool force_endpoints;
  double *zoom_level_breaks;

public:
  DouglasPeuckerMod(const unsigned _num_levels = 18,
                    const unsigned _zoom_factor = 2,
                    const double _threshold = 0.00001,
                    const bool _force_endpoints = true);

  ~DouglasPeuckerMod();

  std::vector<int> dpEncode(std::vector<FlightFix> &fixes,
                            const unsigned start, const unsigned end);

  unsigned getNumLevels() {
    return num_levels;
  }

  unsigned getZoomFactor() {
    return zoom_factor;
  }

private:
  double distance_dp(FlightFix &p0,
                     FlightFix &p1,
                     FlightFix &p2);

  double distance_simple(FlightFix &p0,
                         FlightFix &p1,
                         FlightFix &p2);

  unsigned computeLevel(const double abs_max_dist);

  std::vector<int> classify(size_t n_points, const double dists[], double abs_max_dist);
};

#endif /* SKYLINES_DOUGLASPEUCKERMOD_H */
