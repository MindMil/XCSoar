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

#include "ThermalNotifier.hpp"
#include "NMEA/LiftDatabase.hpp"
#include "NMEA/MoreData.hpp"
#include "NMEA/CirclingInfo.hpp"
#include "Audio/Sound.hpp"
#include "Settings.hpp"
#include "Protection.hpp"
#include "LogFile.hpp"

/**
 * Additional time threshold to declare that turning has stopped.
 * It is needed to avoid full circle breaks during straightening segments.
 */
static constexpr fixed TURN_STOP_SWITCH(6);
/**
 * Estimated off-center threshold to play centering sound
 */
static constexpr fixed OFF_CENTER_THRESHOLD(1.0);
/**
 * Bottom margin of max value calculation which is used for off-center
 * coefficient estimate
 */
static constexpr fixed MAX_BOTTOM_MARGIN(0.5);//


/**
 * Evaluates and returns true if glider made full circle
 * in either direction. If circle direction was changed then full circle
 * flag is reset until full circle is made in new direction again.
 */
static bool MadeFullCircle (  const MoreData &basic,
                              const CirclingInfo &circling,
                              const Angle direction) {

  static bool full_circle = false;

  // Indicates that full circle was done in left direction
  // (used to reset full_circle flag)
  static bool full_circle_left = false;

  // Set to true if direction has exceeded traverse to avoid
  // erroneous full circle events on sudden direction changes
  static bool traverse_reached = false;
  static Angle last_direction = Angle::Zero();

  static fixed turn_stop_time = fixed(0);
  Angle diff;

  if (circling.turning)
    turn_stop_time = basic.time;

  if ((basic.time - turn_stop_time) < TURN_STOP_SWITCH)
  {
    if (circling.TurningLeft() != full_circle_left) {
      // changed direction
      full_circle = false;
      traverse_reached = false;
      full_circle_left = circling.TurningLeft();
      last_direction = direction;
    }

    if (!full_circle) {
      // Check if we reached full circle with algorithm that is better protected
      // against erroneous direction changes
      if (full_circle_left) {
        // circling left
        diff = (last_direction.AsBearing() - direction.AsBearing()).AsBearing();
        if (!traverse_reached) {
          // traverse not reached yet
          if (diff > Angle::HalfCircle() && diff < Angle::Degrees(315)) {
            traverse_reached = true;
            // shift reference angle counter-clockwise
            last_direction -= Angle::HalfCircle();
          }
        } else {
          // traverse was already reached
          if (diff > Angle::HalfCircle() && diff < Angle::Degrees(315)) {
            full_circle = true;
          }
        }
      } else {
        // circling right
        diff = (direction.AsBearing() - last_direction.AsBearing()).AsBearing();
        if (!traverse_reached) {
          // traverse not reached yet
          if (diff > Angle::HalfCircle() && diff < Angle::Degrees(315)) {
            traverse_reached = true;
            // shift reference angle clockwise
            last_direction += Angle::HalfCircle();
          }
        } else {
          // traverse was already reached
          if (diff > Angle::HalfCircle() && diff < Angle::Degrees(315)) {
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

/**
 * Processes and triggers thermal notification sound when roll-out
 * straightening is advised when circling in the thermal from
 * circling information and lift database.
 * This function is enabled/disabled by the user in configuration
 * settings.
 * It's lead time for playing a sound in advance of reaching thermal
 * center direction is also configurable by the user.
 */
void ProcessThermalNotifier (LiftDatabase &lift_database,
                              const MoreData &basic,
                              const CirclingInfo &circling,
                              const ComputerSettings &settings) {


  static bool half_circle_passed = false;
  static bool positive_lead_angle = false;
  static Angle last_notification_direction;
  const Angle direction = basic.attitude.heading;


  if (!settings.circling.thermal_notifier_sound_enabled)
    return;

  if (circling.circling && MadeFullCircle( basic, circling, direction))
  {
    Angle angle = -direction;
    Angle thermal_direction;
    constexpr unsigned num_points = (std::tuple_size<LiftDatabase>());
    constexpr Angle delta = Angle::FullCircle() / num_points;
    // Location of thermal center relative to glider direction coordinate system
    fixed x = fixed(0), y = fixed(0);

    fixed max_lift = std::max(MAX_BOTTOM_MARGIN,
                              (*std::max_element(lift_database.begin(),
                                                lift_database.end())).lift);

    for (unsigned i = 0; i < num_points; i++, angle += delta) {
      auto sincos = angle.SinCos();
      auto lift = std::max(fixed(0),lift_database[i].lift);
      if (circling.TurningLeft()) {
        x -= sincos.first * lift;
        y -= sincos.second * lift;
      } else {
        x += sincos.first * lift;
        y -= sincos.second * lift;
      }
    }
    // Calculate the heading to thermal center
    thermal_direction = Angle::FromXY(x, y);
    // Relative off-center evaluation to determine if sound
    // notification is needed (> 1)
    fixed off_center = sqrt(sqr(x) + sqr(y)) / max_lift;

    // add lead time to determine straightening moment
    fixed lead_thermal_direction = (thermal_direction
        - circling.turn_rate_heading_smoothed.Absolute()).Degrees()
        * settings.circling.thermal_notifier_lead_time;

    if (half_circle_passed && positive_lead_angle) {
      if (lead_thermal_direction < fixed(0)) {
       if (off_center > OFF_CENTER_THRESHOLD ) {
         TriggerThermalNotification ();
       }
       half_circle_passed = false;
       last_notification_direction = direction;
       positive_lead_angle = false;

     }
    } else {
      Angle diff;
      if (circling.TurningLeft()) {
        // circling left
        diff = (last_notification_direction.AsBearing()
            - direction.AsBearing()).AsBearing();
        if (diff > Angle::HalfCircle() && diff < Angle::Degrees(315))
          half_circle_passed = true;

      } else {
        diff = (direction.AsBearing()
            - last_notification_direction.AsBearing()).AsBearing();
        if (diff > Angle::HalfCircle() && diff < Angle::Degrees(315))
          half_circle_passed = true;
      }
      // Ensure that we hade positive lead angle before next notificaton
        if (half_circle_passed  &&  lead_thermal_direction > fixed(0))
          positive_lead_angle = true;
    }
  } else {
    half_circle_passed = true;
    positive_lead_angle = false;
    // Save the direction on non-turning to avoid premature notification
    // on next thermal
    last_notification_direction = direction;
  }
}
