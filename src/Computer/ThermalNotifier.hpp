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

#ifndef XCSOAR_THERMALNOTIFIER_HPP
#define XCSOAR_THERMALNOTIFIER_HPP
#include "NMEA/LiftDatabase.hpp"
#include "NMEA/MoreData.hpp"
#include "NMEA/CirclingInfo.hpp"


/**
 *  Class which plays sound when circle straightening is needed during thermaling
 */
class ThermalNotifier {

  public:
  /**
   * Process the current data to determine when to play thermal straightening notification sound.
   *
   * @param lift_database Database of lift points around the full circle.
   * @param basic Information set of current flight parameters
   * @param circling Circling information parameters set
   */
  static void ProcessNotification (LiftDatabase &lift_database,
                              const MoreData &basic,
                              const CirclingInfo &circling);
};


#endif /* XCSOAR_THERMALNOTIFIER_HPP */
