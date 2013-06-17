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

#ifndef SKYLINES_ANALYSEFLIGHT_H
#define SKYLINES_ANALYSEFLIGHT_H

#include "FlightFix.hpp"
#include "DebugReplayVector.hpp"
#include "DebugReplay.hpp"
#include "Engine/Trace/Trace.hpp"
#include "Contest/ContestManager.hpp"
#include "Time/BrokenDateTime.hpp"

void
Run(DebugReplay &replay, const BrokenDateTime release_time, const BrokenDateTime landing_time,
    Trace &full_trace, Trace &triangle_trace, Trace &sprint_trace);

gcc_pure
ContestStatistics
SolveContest(Contest contest,
             Trace &full_trace, Trace &triangle_trace, Trace &sprint_trace);

void AnalyseFlight(const std::vector<FlightFix> &flight_fixes,
             const BrokenDateTime release_time, const BrokenDateTime landing_time,
             const unsigned full_points = 512,
             const unsigned triangle_points = 1024,
             const unsigned sprint_points = 96);

#endif
