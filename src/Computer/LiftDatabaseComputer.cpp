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

#include "LiftDatabaseComputer.hpp"
#include "Engine/Navigation/TraceHistory.hpp"
#include "NMEA/LiftDatabase.hpp"
#include "NMEA/MoreData.hpp"
#include "NMEA/CirclingInfo.hpp"
#include "Util/Clamp.hpp"

void
LiftDatabaseComputer::Clear(LiftDatabase &lift_database,
                            TraceVariableHistory &circling_average_trace)
{
  lift_database.Clear();

  circling_average_trace.clear();
}

void
LiftDatabaseComputer::Reset(LiftDatabase &lift_database,
                            TraceVariableHistory &circling_average_trace)
{
  last_circling = false;
  last_heading = Angle::Zero();

  Clear(lift_database, circling_average_trace);
}


unsigned
LiftDatabaseComputer::HeadingToIndex (Angle &heading)
{
  static constexpr Angle afive = Angle::Degrees(5);

  unsigned index = (unsigned)
      ((heading + afive).AsBearing().Degrees() / 10);

  return Clamp(index, 0u, 35u);
}

void
LiftDatabaseComputer::Compute(LiftDatabase &lift_database,
                              TraceVariableHistory &circling_average_trace,
                              const MoreData &basic,
                              const CirclingInfo &circling_info)
{
  // If we just started circling
  // -> reset the database because this is a new thermal
  if (!circling_info.circling && last_circling)
    Clear(lift_database, circling_average_trace);

  // Determine the direction in which we are circling
  bool left = circling_info.TurningLeft();

  // Check if the direction of turning has changed since last cycle.
  // Clear database if it did since the previous lift values are no longer
  // valid if glider changed circling direction as it will be now flying
  // through different regions of thermal.
  if (left != last_turning_left)
    lift_database.Clear();

  // Depending on the direction set the step size sign for the
  // following loop
  Angle heading_step = left ? Angle::Degrees(-10) : Angle::Degrees(10);

  const Angle heading = basic.attitude.heading;

  // Start at the last heading and add heading_step until the current heading
  // is reached. For each heading save the current lift value into the
  // LiftDatabase. Last and current heading are included since they are
  // a part of the ten degree interval most of the time.
  //
  // This is done with Angles to deal with the 360 degrees limit.
  // e.g. last heading 348 degrees, current heading 21 degrees
  //
  // The loop condition stops until the current heading is reached.
  // Depending on the circling direction the current heading will be
  // smaller or bigger then the last one, because of that negative() is
  // tested against the left variable.
  for (Angle h = last_heading;
       left == negative((heading - h).AsDelta().Degrees());
       h += heading_step) {
    unsigned index = HeadingToIndex(h);
    lift_database[index].lift = basic.brutto_vario;
    lift_database[index].is_valid = true;
    lift_database[index].turning_left = left;
  }

  // detect zero crossing
  if ((heading < Angle::QuarterCircle() &&
       last_heading > Angle::Degrees(270)) ||
      (last_heading < Angle::QuarterCircle() &&
       heading > Angle::Degrees(270))) {

    fixed h_av = fixed(0);
    int count = 0;
    for (auto i : lift_database){
      if (i.is_valid) {
        h_av += i.lift;
        count ++;
      }
    }

    h_av /= count;
    circling_average_trace.push(h_av);
  }

  last_circling = circling_info.circling;
  last_turning_left = left;
  last_heading = basic.attitude.heading;
}
