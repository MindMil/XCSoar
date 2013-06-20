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

#ifndef SKYLINES_PYTHONCONVERTERS_H
#define SKYLINES_PYTHONCONVERTERS_H

#include <Python.h>
#include <Time/BrokenDateTime.hpp>

class ContestResult;
class ContestTraceVector;
class ContestTracePoint;
class GeoPoint;

namespace Python {

  /**
   * Convert a XCSoar BrokenDateTime to a Python DateTime object
   */
  inline PyObject* BrokenDateTimeToPy(const BrokenDateTime &datetime) {
    return PyDateTime_FromDateAndTime(
      datetime.year, datetime.month, datetime.day,
      datetime.hour, datetime.minute, datetime.second,
      0);
  };

  /**
   * Convert a Python DateTime object to XCSoar BrokenDateTime
   */
  inline BrokenDateTime PyToBrokenDateTime(PyObject *py_datetime) {
    return BrokenDateTime(
      PyDateTime_GET_YEAR(py_datetime),
      PyDateTime_GET_MONTH(py_datetime),
      PyDateTime_GET_DAY(py_datetime),
      PyDateTime_DATE_GET_HOUR(py_datetime),
      PyDateTime_DATE_GET_MINUTE(py_datetime),
      PyDateTime_DATE_GET_SECOND(py_datetime));
  };

  /**
   * Convert a event (datetime + location) to a python dict
   */
  PyObject* WriteEvent(const BrokenDateTime &datetime,
                              const GeoPoint &location);

  /**
   * Convert a GeoPoint to a python dict {longitude, latitude}
   */
  PyObject* WriteLonLat(const GeoPoint &location);

  /**
   * Convert two points to a python dict with leg statistics
   */
  PyObject* WritePoint(const ContestTracePoint &point,
                              const ContestTracePoint *previous);

  PyObject* WriteContest(const ContestResult &result,
                                const ContestTraceVector &trace);
};

#endif /* SKYLINES_PYTHONCONVERTERS_H */
