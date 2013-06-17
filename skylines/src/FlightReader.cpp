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
*/

#include "FlightReader.hpp"
#include "FlightFix.hpp"
#include "DebugReplay.hpp"
#include "DebugReplayIGC.hpp"
#include "Util/Macros.hpp"

void
Run(DebugReplay &replay, std::vector<FlightFix> &flight_fixes)
{
  while (replay.Next()) {
    const MoreData &basic = replay.Basic();

    FlightFix fix;

    fix.datetime = basic.date_time_utc.ToUnixTimeUTC();
    fix.time = basic.time;

    fix.location.longitude = basic.location.longitude.Degrees();
    fix.location.latitude = basic.location.latitude.Degrees();
    fix.gps_valid = basic.location_available;

    fix.gps_altitude = basic.gps_altitude_available ? basic.gps_altitude : 0;
    fix.gps_altitude_available = basic.gps_altitude_available;

    fix.pressure_altitude = basic.pressure_altitude_available ? basic.pressure_altitude : 0;
    fix.pressure_altitude_available = basic.pressure_altitude_available;

    fix.engine_noise_level = basic.engine_noise_level_available ? basic.engine_noise_level : 0;
    fix.engine_noise_level_available = basic.engine_noise_level_available;

    fix.track = basic.track_available ? basic.track.Degrees() : 0;
    fix.track_available = basic.track_available;

    fix.ground_speed = basic.ground_speed_available ? basic.ground_speed : 0;
    fix.ground_speed_available = basic.ground_speed_available;

    fix.ias = basic.airspeed_available ? basic.indicated_airspeed : 0;
    fix.tas = basic.airspeed_available ? basic.true_airspeed : 0;
    fix.airspeed_available = basic.airspeed_available;

    fix.satellites = basic.gps.satellites_used_available ? basic.gps.satellites_used : 0;
    fix.satellites_available = basic.gps.satellites_used_available;

    flight_fixes.push_back(fix);
  }
}

bool FlightReader(const char *input_file, std::vector<FlightFix> &flight_fixes)
{
  DebugReplay *replay = DebugReplayIGC::Create(input_file);

  if (replay) {
    Run(*replay, flight_fixes);
    delete replay;
    return true;
  }

  return false;
}
