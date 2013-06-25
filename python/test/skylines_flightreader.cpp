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
#include "OS/Args.hpp"

#include <stdio.h>

int main(int argc, char **argv) {
  Args args(argc, argv, "FILE");

  std::vector<FlightFix> flight_fixes;

  FlightReader(args.ExpectNext(), flight_fixes);

  for (auto it : flight_fixes) {
    printf("%f : lon %f lat %f alt %f\n", it.time, it.location.longitude,
      it.location.latitude, it.gps_altitude);
  }

  return 0;
}
