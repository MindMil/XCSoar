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

#include "DebugReplayVector.hpp"
#include "FlightFix.hpp"
#include "Time/BrokenDateTime.hpp"

bool
DebugReplayVector::Next()
{
  last_basic = computed_basic;

  if (position != fixes.size()) {
    CopyFromFix(fixes[position]);
    Compute();
    ++position;
    return true;
  }

  if (computed_basic.time_available)
    flying_computer.Finish(calculated.flight, computed_basic.time);

  return false;
}

void
DebugReplayVector::CopyFromFix(const FlightFix &fix)
{
  NMEAInfo &basic = raw_basic;

  basic.clock = basic.time = fix.time;
  basic.time_available.Update(basic.clock);

  basic.date_time_utc = BrokenDateTime::FromUnixTimeUTC(fix.datetime);
  basic.alive.Update(basic.clock);

  basic.location.longitude = Angle::Degrees(fix.location.longitude);
  basic.location.latitude = Angle::Degrees(fix.location.latitude);

  if (fix.gps_valid) {
    basic.location_available.Update(basic.clock);
  } else
    basic.location_available.Clear();

  if (fix.gps_altitude_available) {
    basic.gps_altitude = fixed(fix.gps_altitude);

    if (fix.gps_valid)
      basic.gps_altitude_available.Update(basic.clock);
    else
      basic.gps_altitude_available.Clear();
  } else
    basic.gps_altitude_available.Clear();

  if (fix.pressure_altitude_available) {
    basic.pressure_altitude = basic.baro_altitude = fixed(fix.pressure_altitude);
    basic.pressure_altitude_available.Update(basic.clock);
    basic.baro_altitude_available.Update(basic.clock);
  }

  if (fix.engine_noise_level_available) {
    basic.engine_noise_level = fix.engine_noise_level;
    basic.engine_noise_level_available.Update(basic.clock);
  }

  if (fix.track_available) {
    basic.track = Angle::Degrees(fixed(fix.track));
    basic.track_available.Update(basic.clock);
  }

  if (fix.ground_speed_available) {
    basic.ground_speed = fix.ground_speed;
    basic.ground_speed_available.Update(basic.clock);
  }

  if (fix.airspeed_available && fix.ias >= 0) {
    if (fix.tas >= 0)
      basic.ProvideBothAirspeeds(fix.ias, fix.tas);
    else
      basic.ProvideIndicatedAirspeedWithAltitude(fix.ias, basic.pressure_altitude);
  } else if (fix.airspeed_available && fix.tas >= 0)
    basic.ProvideTrueAirspeed(fix.tas);

  if (fix.satellites_available) {
    basic.gps.satellites_used = fix.satellites;
    basic.gps.satellites_used_available.Update(basic.clock);
  }
}
