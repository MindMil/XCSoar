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

#ifndef XCSOAR_LIFT_DATABASE_HPP
#define XCSOAR_LIFT_DATABASE_HPP

#include "Math/fixed.hpp"

#include <type_traits>
#include <array>

/**
 * Identifies one lift point in lift database along with
 * associated validity flag and turning direction
 */
struct LiftPoint {
  /**
   * Registered lift in this direction
   */
  fixed lift;

  /**
   * Lift value is valid
   */
  bool is_valid;

  /**
   * Is lift recorded when turning left
   */
  bool turning_left;

  constexpr bool
  operator<(const LiftPoint x) const
  {
    return lift < x.lift;
  }

  constexpr bool
  operator>(const LiftPoint x) const
  {
    return lift > x.lift;
  }

};

class LiftDatabase : public std::array<LiftPoint, 36> {
public:
  void Clear() {
    LiftPoint lp = {}; // sets all values to 0
    fill(lp);
  }
};

static_assert(std::is_trivial<LiftDatabase>::value, "type is not trivial");

#ifdef __clang__
#pragma GCC diagnostic push
/* necessary with clang due to bug in libstdc++ */
#pragma GCC diagnostic ignored "-Wmismatched-tags"
#endif

namespace std {
  template<>
  class tuple_size<LiftDatabase> : public integral_constant<size_t, 36> {
  };
}

#ifdef __clang__
#pragma GCC diagnostic pop
#endif

#endif
