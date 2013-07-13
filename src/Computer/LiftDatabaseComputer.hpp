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

#ifndef XCSOAR_LIFT_DATABASE_COMPUTER_HPP
#define XCSOAR_LIFT_DATABASE_COMPUTER_HPP

#include "Math/Angle.hpp"

struct NMEAInfo;
struct MoreData;
struct CirclingInfo;
class LiftDatabase;
class TraceVariableHistory;

/**
 * This computer manages the LiftDatabase and
 * TraceHistory::CirclingAverage.
 *
 * Dependencies: #CirclingComputer.
 */
class LiftDatabaseComputer {
  bool last_circling;
  bool last_turning_left;
  Angle last_heading;

  /**
   * Clear the attributes managed by this computer.
   */
  void Clear(LiftDatabase &lift_database,
             TraceVariableHistory &circling_average_trace);

public:

/**
 * This function converts a heading into an unsigned index for the LiftDatabase.
 *
 * This is calculated with Angles to deal with the 360 degree limit.
 *
 * 357 = 0
 * 4 = 0
 * 5 = 1
 * 14 = 1
 * 15 = 2
 * ...
 * @param heading The heading to convert
 * @return The index for the LiftDatabase array
 */
  static unsigned HeadingToIndex (Angle &heading);

  /**
   * Reset this computer and clear the attributes.
   */
  void Reset(LiftDatabase &lift_database,
             TraceVariableHistory &circling_average_trace);

  void Compute(LiftDatabase &lift_database,
               TraceVariableHistory &circling_average_trace,
               const MoreData &basic, const CirclingInfo &circling_info);
};

#endif
