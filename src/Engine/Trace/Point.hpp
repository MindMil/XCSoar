/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
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

#ifndef TRACE_POINT_HPP
#define TRACE_POINT_HPP

#include "Util/TypeTraits.hpp"
#include "Geo/SearchPoint.hpp"
#include "Rough/RoughAltitude.hpp"
#include "Rough/RoughVSpeed.hpp"
#include "Compiler.h"

#include <assert.h>

struct AircraftState;

/**
 * Class for points used in traces (snail trail, OLC scans)
 * Internally, keeps track of predecessors as a kind of a linked-list
 */
class TracePoint : public SearchPoint
{
  /** Time of sample */
  unsigned time;
  /**
   * Thermal drift factor:
   * 256 indicates drift rate equal to wind speed
   * 0 indicates no drift.
   */
  unsigned short drift_factor;

  /**
   * The NavAltitude [m].
   */
  RoughAltitude altitude;

  /**
   * The NettoVario value [m/s].
   */
  RoughVSpeed vario;

  /**
   * The engine noise level
   */
  unsigned engine_noise_level;

public:
  /**
   * Non-initialising constructor.
   */
  TracePoint() = default;

  /**
   * Constructor for a TracePoint which is only used as parameter to
   * TraceTree::find_within_range().  It initializes only the
   * SearchPoint base class.
   */
  TracePoint(const GeoPoint &location):
    SearchPoint(location) {}

  /**
   * Constructor for actual trace points
   *
   * @param state State of aircraft
   * @param tp Projection used internally
   *
   * @return Initialised object
   */
  TracePoint(const AircraftState &state);

  void Clear() {
    time = (unsigned)(0 - 1);
  }

  bool IsDefined() const {
    return time != (unsigned)(0 - 1);
  }

  unsigned GetTime() const {
    return time;
  }

  bool IsOlderThan(const TracePoint &other) const {
    return time < other.time;
  }

  bool IsNewerThan(const TracePoint &other) const {
    return time > other.time;
  }

  unsigned DeltaTime(const TracePoint &previous) const {
    assert(!IsOlderThan(previous));

    return time - previous.time;
  }

  fixed CalculateDrift(fixed now) const {
    const fixed dt = now - fixed(time);
    return dt * drift_factor / 256;
  }

  fixed GetAltitude() const {
    return altitude;
  }

  unsigned GetEngineNoiseLevel() const {
    return engine_noise_level;
  }

  /**
   * Returns the altitude as an integer.  Some calculations may not
   * need the fractional part.
   */
  int GetIntegerAltitude() const {
    return (int)altitude;
  }

  fixed GetVario() const {
    return vario;
  }

  /** 
   * Test match based on time (since time of a sample must be unique)
   * 
   * @param a Point to compare to
   * 
   * @return True if time matches
   */
  bool operator==(TracePoint const &a) const {
    return time == a.time; 
  }
};

static_assert(is_trivial_ndebug<TracePoint>::value, "type is not trivial");

#endif

