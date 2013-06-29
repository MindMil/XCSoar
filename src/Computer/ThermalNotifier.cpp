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

/*
 * ThermalNotifier.cpp
 *
 *  Created on: Jun 29, 2013
 *      Author: Mindaugas Milasauskas
 */

#include "ThermalNotifier.hpp"
#include "NMEA/LiftDatabase.hpp"
#include "NMEA/MoreData.hpp"
#include "NMEA/CirclingInfo.hpp"
#include "Audio/Sound.hpp"
#include "LogFile.hpp"

static constexpr fixed TURN_STOP_SWITCH(6); // additional time threshold to declare that turning has stopped. It is needed to avoid full circle breaks during straightening segments.
static constexpr fixed OFF_CENTER_THRESHOLD(1.0);// estimated off-center threshold to play centering sound
static constexpr fixed MAX_BOTTOM_MARGIN(0.5);// bottom margin of max value calculation which is used for off-center coeficient estimate
static constexpr fixed NOTIFICATION_LEAD_TIME(1.5); // lead time in seconds before straightening notification

static bool MadeFullCircle (  const MoreData &basic,
                              const CirclingInfo &circling,
                              const Angle direction) {

  static bool full_circle = false; // indicates if full circle was done in one direction
  static bool full_circle_left = false; // indicates that full circle was done in left direction (used to reset full_circle flag)
  static bool traverse_reached = false; // set to true if direction has exceeded traverse to avoid erroneous full circle events on sudden direction changes
  static Angle last_direction = Angle::Zero();

  static fixed turn_stop_time = fixed(0);
  Angle diff;

  if (circling.turning)
    turn_stop_time = basic.time;

  if (((basic.time - turn_stop_time) < TURN_STOP_SWITCH))
  {
    if (circling.TurningLeft() != full_circle_left) {
      // changed direction
      full_circle = false;
      traverse_reached = false;
      full_circle_left = circling.TurningLeft();
      last_direction = direction;
    }

    if (!full_circle) {
      // check if we reached full circle with algorithm that is better protected against erroneous direction changes
      if (full_circle_left) {
        // circling left
        diff = (last_direction.AsBearing() - direction.AsBearing()).AsBearing();
        if (!traverse_reached) {
          // traverse not reached yet
          if (diff.Degrees() > fixed(180) && diff.Degrees() < fixed(315)) {
            traverse_reached = true;
            last_direction -= Angle::Degrees(180); // shift reference angle counter-clockwise
          }
        } else {
          // traverse was already reached
          if (diff.Degrees() > fixed(180) && diff.Degrees() < fixed(315)) {
            full_circle = true;
          }
        }
      } else {
        // circling right
        diff = (direction.AsBearing() - last_direction.AsBearing()).AsBearing();
        if (!traverse_reached) {
          // traverse not reached yet
          if (diff.Degrees() > fixed(180) && diff.Degrees() < fixed(315)) {
            traverse_reached = true;
            last_direction += Angle::Degrees(180);  // shift reference angle clockwise
          }
        } else {
          // traverse was already reached
          if (diff.Degrees() > fixed(180) && diff.Degrees() < fixed(315)) {
            full_circle = true;
          }
        }
      }
    }
  }
  else
  {
    full_circle = false;
    last_direction = direction;
  }

  return full_circle;
}


void ThermalNotifier::ProcessNotification (LiftDatabase &lift_database,
                              const MoreData &basic,
                              const CirclingInfo &circling) {

  bool made_full_circle; // indicates if full circle was done in one direction
  static bool half_circle_passed = false; // flag that half a circle passed since last notification
  static bool positive_lead_angle = false; // flag that positive lead angle was detected
  static Angle last_notification_direction;  // direction at which notification was played
  const Angle direction = basic.attitude.heading;

  made_full_circle = MadeFullCircle( basic, circling, direction);

  if (circling.circling && made_full_circle)
  {
    Angle angle = -direction;
    Angle thermal_direction;
    constexpr unsigned num_points = (std::tuple_size<LiftDatabase>());
    constexpr Angle delta = Angle::FullCircle() / num_points;
    fixed average = fixed(0);
    fixed x = fixed(0), y = fixed(0); // location of thermal center relative to glider direction coordinate system

    fixed max_lift = std::max(MAX_BOTTOM_MARGIN, *std::max_element(lift_database.begin(),
                                        lift_database.end()));

    for (unsigned i = 0; i < num_points; i++, angle += delta) {
      auto sincos = angle.SinCos();
      auto lift = std::max(fixed(0),lift_database[i]);
      average += lift;
      if (circling.TurningLeft()) {
        x -= sincos.first * lift;
        y -= sincos.second * lift;
      } else {
        x += sincos.first * lift;
        y -= sincos.second * lift;
      }
    }
    average /= num_points;
    thermal_direction = Angle::FromXY(x, y);// calculates the heading to thermal center
    fixed off_center = sqrt(sqr(x) + sqr(y)) / max_lift; // relative off-center evaluation to determine if sound notification is needed (> 1)

    // add lead time to determine straightening moment
    fixed lead_thermal_direction = thermal_direction.Degrees() - fabs(circling.turn_rate_smoothed) * NOTIFICATION_LEAD_TIME;

    if (half_circle_passed && positive_lead_angle) {
      if (lead_thermal_direction < fixed(0)) {
       if (off_center > OFF_CENTER_THRESHOLD ) {
         PlayResource(_T("IDR_WAV_CLEAR"));
       }
       half_circle_passed = false;
       last_notification_direction = direction;
       positive_lead_angle = false;

     }
    } else {
      Angle diff; // difference between last notification and current heading
      // Ensure that half circle has passed since last notification
      if (circling.TurningLeft()) {
        // circling left
        diff = (last_notification_direction.AsBearing() - direction.AsBearing()).AsBearing();
        if (diff.Degrees() > fixed(180) && diff.Degrees() < fixed(315))
          half_circle_passed = true;

      } else {
        diff = (direction.AsBearing() - last_notification_direction.AsBearing()).AsBearing();
        if (diff.Degrees() > fixed(180) && diff.Degrees() < fixed(315))
          half_circle_passed = true;
      }
      // Ensure that we hade positive lead angle before next notificaton
        if (half_circle_passed  &&  lead_thermal_direction > fixed(0))
          positive_lead_angle = true;
    }
  } else {
    half_circle_passed = true;
    positive_lead_angle = false;

    last_notification_direction = direction; // save the direction on non-turning to avoid premature notification on next thermal
  }
}
