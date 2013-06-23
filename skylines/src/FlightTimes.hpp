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

#ifndef SKYLINES_FLIGHTTIMES_H
#define SKYLINES_FLIGHTTIMES_H

#include "Time/BrokenDateTime.hpp"
#include "Geo/GeoPoint.hpp"
#include <vector>

struct Result {
  BrokenDateTime takeoff_time, release_time, landing_time;
  GeoPoint takeoff_location, release_location, landing_location;

  Result() {
    takeoff_time.Clear();
    landing_time.Clear();
    release_time.Clear();

    takeoff_location.SetInvalid();
    landing_location.SetInvalid();
    release_location.SetInvalid();
  }
};

struct MoreData;
struct FlyingState;
struct DerivedInfo;
class DebugReplay;
struct FlightFix;

void Update(const MoreData &basic, const FlyingState &state, Result &result);

void Update(const MoreData &basic, const DerivedInfo &calculated, Result &result);

void Finish(const MoreData &basic, const DerivedInfo &calculated, Result &result);

bool Run(DebugReplay &replay, Result &result);

void FlightTimes(DebugReplay &replay, std::vector<Result> &results);

#endif