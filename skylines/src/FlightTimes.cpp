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

#include "FlightTimes.hpp"
#include "DebugReplay.hpp"


void
Update(const MoreData &basic, const FlyingState &state,
       Result &result)
{
  if (!basic.time_available || !basic.date_time_utc.IsDatePlausible())
    return;

  if (state.flying && !result.takeoff_time.IsPlausible()) {
    result.takeoff_time = basic.GetDateTimeAt(state.takeoff_time);
    result.takeoff_location = state.takeoff_location;
  }

  if (!state.flying && result.takeoff_time.IsPlausible() &&
      !result.landing_time.IsPlausible()) {
    result.landing_time = basic.GetDateTimeAt(state.landing_time);
    result.landing_location = state.landing_location;
  }

  if (!negative(state.release_time) && !result.release_time.IsPlausible()) {
    result.release_time = basic.GetDateTimeAt(state.release_time);
    result.release_location = state.release_location;
  }
}

void
Update(const MoreData &basic, const DerivedInfo &calculated,
       Result &result)
{
  Update(basic, calculated.flight, result);
}

void
Finish(const MoreData &basic, const DerivedInfo &calculated,
       Result &result)
{
  if (!basic.time_available || !basic.date_time_utc.IsDatePlausible())
    return;

  if (result.takeoff_time.IsPlausible() && !result.landing_time.IsPlausible()) {
    result.landing_time = basic.date_time_utc;

    if (basic.location_available)
      result.landing_location = basic.location;
  }
}

bool
Run(DebugReplay &replay, Result &result)
{
  bool released = false;

  GeoPoint last_location = GeoPoint::Invalid();
  constexpr Angle max_longitude_change = Angle::Degrees(30);
  constexpr Angle max_latitude_change = Angle::Degrees(1);

  while (replay.Next()) {
    const MoreData &basic = replay.Basic();

    Update(basic, replay.Calculated(), result);

    if (!basic.time_available || !basic.location_available ||
        !basic.NavAltitudeAvailable())
      continue;

    if (last_location.IsValid() &&
        ((last_location.latitude - basic.location.latitude).Absolute() > max_latitude_change ||
         (last_location.longitude - basic.location.longitude).Absolute() > max_longitude_change))
      /* there was an implausible warp, which is usually triggered by
         an invalid point declared "valid" by a bugged logger; if that
         happens, we stop the analysis, because the IGC file is
         obviously broken */
      break;

    last_location = basic.location;

    if (!released && !negative(replay.Calculated().flight.release_time)) {
      released = true;
    }

    if (released && !replay.Calculated().flight.flying)
      /* the aircraft has landed, stop here */
      /* TODO: at some point, we might want to emit the analysis of
         all flights in this IGC file */
      break;
  }

  Update(replay.Basic(), replay.Calculated(), result);
  Finish(replay.Basic(), replay.Calculated(), result);

  // landing detected or eof?
  if (replay.Tell() != replay.Size())
    return false;
  else
    return true;
}

void FlightTimes(DebugReplay &replay, std::vector<Result> &results)
{
  bool eof = false;
  while (!eof) {
    Result result;
    eof = Run(replay, result);

    if (result.takeoff_time.IsPlausible()
        && result.release_time.IsPlausible()
        && result.landing_time.IsPlausible()) {

      if (result.release_time.ToUnixTimeUTC() < result.takeoff_time.ToUnixTimeUTC())
        result.release_time = result.takeoff_time;

      results.push_back(result);
    }
  }
}
