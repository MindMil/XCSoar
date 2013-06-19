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

#ifndef SKYLINES_FLIGHT_FIX_H
#define SKYLINES_FLIGHT_FIX_H

#include "NMEA/MoreData.hpp"
#include <cinttypes>

struct FlightFix {
  /* Time as UNIX UTC time stamp */
  int64_t datetime;

  /* Global time (seconds after UTC midnight) */
  double time;

  /* Location of fix, in degrees(!) */
  double longitude,
         latitude;

  /* GPS validity */
  bool gps_valid;

  /* GPS altitude */
  double gps_altitude;
  bool gps_altitude_available;

  /* Pressure altitude */
  double pressure_altitude;
  bool pressure_altitude_available;

  /* Engine Noise Level */
  double engine_noise_level;
  bool engine_noise_level_available;

  /* Track, in degrees(!) */
  double track;
  bool track_available;

  /* Ground speed */
  double ground_speed;
  bool ground_speed_available;

  /* True airspeed */
  double tas;

  /* Indicated airspeed */
  double ias;
  bool airspeed_available;

  /* Number of satellites available */
  int satellites;
  bool satellites_available;

  FlightFix(int64_t _datetime, double _time, double _longitude, double _latitude,
            bool _gps_valid, double _gps_altitude, bool _gps_altitude_available,
            double _pressure_altitude, bool _pressure_altitude_available,
            double _engine_noise_level, bool _engine_noise_level_available,
            double _track, bool _track_available,
            double _ground_speed, bool _ground_speed_available,
            double _tas, double _ias, double _airspeed_available,
            int _satellites, bool _satellites_available)
    : datetime(_datetime), time(_time), longitude(_longitude), latitude(_latitude),
      gps_valid(_gps_valid), gps_altitude(_gps_altitude),
      gps_altitude_available(_gps_altitude_available),
      pressure_altitude(_pressure_altitude),
      pressure_altitude_available(_pressure_altitude_available),
      engine_noise_level(_engine_noise_level),
      engine_noise_level_available(_engine_noise_level_available),
      track(_track),
      track_available(_track_available),
      ground_speed(_ground_speed),
      ground_speed_available(_ground_speed_available),
      tas(_tas),
      ias(_ias),
      airspeed_available(_airspeed_available),
      satellites(_satellites),
      satellites_available(_satellites_available) {};

  FlightFix(const MoreData &basic)
    : datetime(basic.date_time_utc.ToUnixTimeUTC()),
      time(basic.time),
      longitude(basic.location.longitude.Degrees()),
      latitude(basic.location.latitude.Degrees()),
      gps_valid(basic.location_available),
      gps_altitude(basic.gps_altitude_available ? basic.gps_altitude : 0),
      gps_altitude_available(basic.gps_altitude_available),
      pressure_altitude(basic.pressure_altitude_available ? basic.pressure_altitude : 0),
      pressure_altitude_available(basic.pressure_altitude_available),
      engine_noise_level(basic.engine_noise_level_available ? basic.engine_noise_level : 0),
      engine_noise_level_available(basic.engine_noise_level_available),
      track(basic.track_available ? basic.track.Degrees() : 0),
      track_available(basic.track_available),
      ground_speed(basic.ground_speed_available ? basic.ground_speed : 0),
      ground_speed_available(basic.ground_speed_available),
      tas(basic.airspeed_available ? basic.true_airspeed : 0),
      ias(basic.airspeed_available ? basic.indicated_airspeed : 0),
      airspeed_available(basic.airspeed_available),
      satellites(basic.gps.satellites_used_available ? basic.gps.satellites_used : 0),
      satellites_available(basic.gps.satellites_used_available) {};

};


#endif /* SKYLINES_FLIGHT_FIX_H */
