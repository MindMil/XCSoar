/*
Copyright_License {

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
*/

#ifndef SKYLINES_FLIGHT_FIX_H
#define SKYLINES_FLIGHT_FIX_H

#include <cinttypes>

struct FlightFix {
  /* Time as UNIX UTC time stamp */
  int64_t datetime;

  /* Global time (seconds after UTC midnight) */
  double time;

  /* Location of fix, in degrees(!) */
  struct Location {
    double longitude,
           latitude;
  } location;

  /* GPS validity */
  bool gps_valid;

  /* GPS altitude */
  double gps_altitude;
  bool gps_altitude_available;

  /* Pressure altitude */
  double pressure_altitude;
  bool pressure_altitude_available;

  /* Engine Noise Level */
  double engine_noise_level;
  bool engine_noise_level_available;

  /* Track, in degrees(!) */
  double track;
  bool track_available;

  /* Ground speed */
  double ground_speed;
  bool ground_speed_available;

  /* True airspeed */
  double tas;

  /* Indicated airspeed */
  double ias;
  bool airspeed_available;

  /* Number of satellites available */
  int satellites;
  bool satellites_available;
};


#endif /* SKYLINES_FLIGHT_FIX_H */
