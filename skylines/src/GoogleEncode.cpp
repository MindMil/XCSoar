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
#include "FlightFix.hpp"

#include <sstream>
#include <string>
#include <list>
#include <utility>
#include <vector>
#include <memory>

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

GoogleEncode::EncodedFlight
GoogleEncode::encode(std::vector<FlightFix>::iterator fix_start,
                     std::vector<FlightFix>::iterator fix_end,
                     std::vector<int>::iterator levels_start,
                     std::vector<int>::iterator levels_end) {
  std::ostringstream encoded_levels,
                     encoded_points,
                     encoded_time,
                     encoded_gps_alt,
                     encoded_enl;

  int plat = 0,
      plng = 0,
      ptime = 0,
      pgps_alt = 0,
      penl = 0;

  auto level = levels_start;
  for (auto fix = fix_start;
       fix != fix_end, level != levels_end;
       ++fix, ++level) {
    if (*level != -1) {
      encoded_levels << encodeNumber(*level);

      int late5 = floor1e5(fix->latitude);
      int lnge5 = floor1e5(fix->longitude);

      int dlat = late5 - plat;
      int dlng = lnge5 - plng;

      plat = late5;
      plng = lnge5;

      encoded_points << encodeSignedNumber(dlat);
      encoded_points << encodeSignedNumber(dlng);

      int dtime = fix->time - ptime;
      ptime = fix->time;

      int dgps_alt= fix->gps_altitude - pgps_alt;
      pgps_alt= fix->gps_altitude;

      int denl= fix->engine_noise_level - penl;
      penl = fix->engine_noise_level;

      encoded_time << encodeSignedNumber(dtime);
      encoded_gps_alt << encodeSignedNumber(dgps_alt);
      encoded_enl << encodeSignedNumber(denl);
    }
  }

  EncodedFlight flight;
  flight.locations = std::unique_ptr<std::string>(new std::string(encoded_points.str()));
  flight.levels = std::unique_ptr<std::string>(new std::string(encoded_levels.str()));
  flight.times = std::unique_ptr<std::string>(new std::string(encoded_time.str()));
  flight.gps_alt = std::unique_ptr<std::string>(new std::string(encoded_gps_alt.str()));
  flight.enl = std::unique_ptr<std::string>(new std::string(encoded_enl.str()));

  return flight;
}

std::unique_ptr<std::string> GoogleEncode::encodeList(std::vector<int> &points) {
  std::ostringstream encoded_list;

  int val = 0;

  for (auto i : points) {
    int dval = i - val;
    val = i;

    encoded_list << encodeSignedNumber(dval);
  }

  return std::unique_ptr<std::string>(new std::string(encoded_list.str()));
}
