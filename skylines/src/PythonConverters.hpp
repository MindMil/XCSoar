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

#include <Python.h>
#include <Time/BrokenDateTime.hpp>

namespace Python {

  /**
   * Convert a XCSoar BrokenDateTime to a Python DateTime object
   */
  static PyObject* BrokenDateTimeToPy(const BrokenDateTime &datetime) {
    return PyDateTime_FromDateAndTime(
      datetime.year, datetime.month, datetime.day,
      datetime.hour, datetime.minute, datetime.second,
      0);
  };

  /**
   * Convert a Python DateTime object to XCSoar BrokenDateTime
   */
  static BrokenDateTime PyToBrokenDateTime(PyObject *py_datetime) {
    return BrokenDateTime(
      PyDateTime_GET_YEAR(py_datetime),
      PyDateTime_GET_MONTH(py_datetime),
      PyDateTime_GET_DAY(py_datetime),
      PyDateTime_DATE_GET_HOUR(py_datetime),
      PyDateTime_DATE_GET_MINUTE(py_datetime),
      PyDateTime_DATE_GET_SECOND(py_datetime));
  };
};

