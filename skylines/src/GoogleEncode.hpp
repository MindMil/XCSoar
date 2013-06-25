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

#ifndef SKYLINES_GOOGLEENCODE_H
#define SKYLINES_GOOGLEENCODE_H

#include <string>
#include <cmath>
#include <list>
#include <utility>
#include <vector>
#include <memory>

struct FlightFix;

class GoogleEncode {
public:
  struct EncodedFlight {
    std::unique_ptr<std::string> locations;
    std::unique_ptr<std::string> levels;
    std::unique_ptr<std::string> times;
    std::unique_ptr<std::string> gps_alt;
    std::unique_ptr<std::string> enl;
  };

private:
  static inline int floor1e5(double coordinate) {
    return floor(coordinate * 1e5);
  }

  static std::string encodeSignedNumber(int num);

  static std::string encodeNumber(int num);
    
public:
  static EncodedFlight encode(std::vector<FlightFix>::iterator fix_start,
                              std::vector<FlightFix>::iterator fix_end,
                              std::vector<int>::iterator levels_start,
                              std::vector<int>::iterator levels_end);

  /* encode a list of (lat, lon) pairs */
  static std::unique_ptr<std::string>
    encodeLonLat(std::vector<std::pair<double, double>> &points);

  static std::unique_ptr<std::string> encodeList(std::vector<int> &points);
};

#endif /* SKYLINES_GOOGLEENCODE_H */
