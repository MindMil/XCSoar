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

#ifndef XCSOAR_CATMULL_ROM_INTERPOLATOR_HPP
#define XCSOAR_CATMULL_ROM_INTERPOLATOR_HPP

#include "Math/fixed.hpp"
#include "Geo/GeoPoint.hpp"
#include "Geo/GeoVector.hpp"
#include "Util/Clamp.hpp"

#include <algorithm>
#include <assert.h>

/**
 * A Catmull-Rom splines interpolator
 * @see http://www.cs.cmu.edu/~462/projects/assn2/assn2/catmullRom.pdf
 */
class CatmullRomInterpolator
{
public:
  struct Record {
    GeoPoint location;
    fixed gps_altitude;
    fixed baro_altitude;
    fixed time;
  };

private:
  const fixed time;

  unsigned num;
  Record p[4];

public:
  CatmullRomInterpolator(fixed _time):
    time(_time)
  {
    Reset();
  }

  void
  Reset()
  {
    num = 0;
  }

  void
  Update(fixed t, GeoPoint location, fixed alt, fixed palt)
  {
    if (num && (t <= p[3].time))
      return;

    if (num < 4)
      num++;

    std::copy(p + 1, p + 4, p);

    p[3].location = location;
    p[3].gps_altitude = alt;
    p[3].baro_altitude = palt;
    p[3].time = t;
  }

  bool
  Ready() const
  {
    return (num == 4);
  }

  GeoVector 
  GetVector(fixed _time) const
  {
    assert(Ready());

    if (!positive(p[2].time-p[1].time))
      return GeoVector(fixed(0), Angle::Zero());

    const Record r0 = Interpolate(_time - fixed(0.05));
    const Record r1 = Interpolate(_time + fixed(0.05));

    fixed speed = p[1].location.Distance(p[2].location) / (p[2].time - p[1].time);
    Angle bearing = r0.location.Bearing(r1.location);

    return GeoVector(speed, bearing);
  }

  gcc_pure
  Record
  Interpolate(fixed _time) const
  {
    assert(Ready());

    const fixed u = GetTimeFraction(_time, false);

    /*
      ps = ( c0   c1    c2  c3)
           [  0    1     0   0] 1
           [ -t    0     t   0] u
           [ 2t  t-3  3-2t  -t] u^2
           [ -t  2-t   t-2   t] u^3
    */

    const fixed u2 = sqr(u);
    const fixed u3 = u2 * u;
    const fixed c[4]= {-time * u3 + 2 * time * u2 - time * u,
                       (fixed(2) - time) * u3 + (time - fixed(3)) * u2 + fixed(1),
                       (time - fixed(2)) * u3 + (fixed(3) - 2 * time) * u2 + time * u,
                        time * u3 - time * u2};

    Record r;
    r.location.latitude =
        p[0].location.latitude * c[0] + p[1].location.latitude * c[1] +
        p[2].location.latitude * c[2] + p[3].location.latitude * c[3];

    r.location.longitude =
        p[0].location.longitude * c[0] + p[1].location.longitude * c[1] +
        p[2].location.longitude * c[2] + p[3].location.longitude * c[3];

    r.gps_altitude =
        p[0].gps_altitude * c[0] + p[1].gps_altitude * c[1] +
        p[2].gps_altitude * c[2] + p[3].gps_altitude * c[3];

    r.baro_altitude =
        p[0].baro_altitude * c[0] + p[1].baro_altitude * c[1] +
        p[2].baro_altitude * c[2] + p[3].baro_altitude * c[3];

    r.time = _time;

    return r;
  }

  fixed
  GetMinTime() const
  {
    assert(Ready());

    return p[0].time;
  }

  fixed
  GetMaxTime() const
  {
    assert(Ready());

    return std::max({fixed(0), p[0].time, p[1].time, p[2].time, p[3].time});
  }

  bool
  NeedData(fixed t_simulation) const
  {
    return !Ready() || (p[2].time <= t_simulation + fixed(0.1));
  }

private:
  fixed
  GetTimeFraction(const fixed time, bool limit_range = true) const
  {
    assert(Ready());
    assert(p[2].time > p[1].time);

    const fixed fraction = (time - p[1].time) / (p[2].time - p[1].time);

    if (limit_range)
      return Clamp(fraction, fixed(0), fixed(1));
    else
      return fraction;
  }
};

#endif
