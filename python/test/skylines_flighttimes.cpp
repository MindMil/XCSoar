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

#include "FlightReader.hpp"
#include "FlightFix.hpp"
#include "FlightTimes.hpp"
#include "OS/Args.hpp"

#include <stdio.h>

int main(int argc, char **argv) {
  Args args(argc, argv, "FILE");
  const char *file_name = args.ExpectNext();

  std::vector<FlightFix> flight_fixes;

  printf("Reading file...\n");
  FlightReader(file_name, flight_fixes);

  std::vector<Result> results;

  printf("Replay from vector\n");
  FlightTimes(flight_fixes, results);

  for (auto times : results) {
    if (!times.takeoff_time.IsPlausible()) continue;

    printf("\nNext flight:\n");
    printf("Takeoff: lon %f lat %f at %d-%d-%d %d:%d:%d\n",
      times.takeoff_location.longitude.Degrees(),
      times.takeoff_location.latitude.Degrees(),
      times.takeoff_time.year, times.takeoff_time.month, times.takeoff_time.day,
      times.takeoff_time.hour, times.takeoff_time.minute, times.takeoff_time.second);

    printf("Release: lon %f lat %f at %d-%d-%d %d:%d:%d\n",
      times.release_location.longitude.Degrees(),
      times.release_location.latitude.Degrees(),
      times.release_time.year, times.release_time.month, times.release_time.day,
      times.release_time.hour, times.release_time.minute, times.release_time.second);

    printf("Landing: lon %f lat %f at %d-%d-%d %d:%d:%d\n",
      times.landing_location.longitude.Degrees(),
      times.landing_location.latitude.Degrees(),
      times.landing_time.year, times.landing_time.month, times.landing_time.day,
      times.landing_time.hour, times.landing_time.minute, times.landing_time.second);
  }

  return 0;
}
