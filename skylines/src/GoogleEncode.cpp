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

#include "GoogleEncode.hpp"

#include <sstream>
#include <string>

std::string GoogleEncode::encodeSignedNumber(int num) {
  int sgn_num = num << 1;

  if (num < 0) {
    sgn_num = ~sgn_num;
  }

  return encodeNumber(sgn_num);
}

std::string GoogleEncode::encodeNumber(int num) {
  std::ostringstream encoded_string;

  while (num >= 0x20) {
    int next_value = (0x20 | (num & 0x1f)) + 63;
    encoded_string << char(next_value);
    num >>= 5;
  }

  num += 63;
  encoded_string << char(num);

  return encoded_string.str();
}

std::auto_ptr<std::pair<std::string, std::string>>
GoogleEncode::encode(std::vector<std::pair<double, double>> &points, std::vector<int> &levels) {
  std::ostringstream encoded_levels,
                     encoded_points;

  int plat = 0,
      plng = 0;

  size_t n_points = points.size();

  for (size_t i = 0; i < n_points; i++) {
    if (levels[i] != -1) {
      encoded_levels << encodeNumber(levels[i]);

      std::pair<double, double> point = points[i];

      int late5 = floor1e5(point.second);
      int lnge5 = floor1e5(point.first);

      int dlat = late5 - plat;
      int dlng = lnge5 - plng;

      plat = late5;
      plng = lnge5;

      encoded_points << encodeSignedNumber(dlat);
      encoded_points << encodeSignedNumber(dlng);
    }
  }
    
  std::auto_ptr<std::pair<std::string, std::string>> r(new std::pair<std::string, std::string>);
  r->first = encoded_points.str();
  r->second = encoded_levels.str();

  return r;
}

std::string GoogleEncode::encodeList(std::list<int> &points) {
  std::ostringstream encoded_list;

  int val = 0;

  for (auto i : points) {
    int dval = i - val;
    val = i;

    encoded_list << encodeSignedNumber(dval);
  }

  return encoded_list.str();
}
