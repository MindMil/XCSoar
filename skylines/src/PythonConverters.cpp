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
#include <datetime.h>

#include "PythonConverters.hpp"

#include "Geo/GeoPoint.hpp"
#include "Time/BrokenDateTime.hpp"
#include "Engine/Contest/ContestTrace.hpp"
#include "Engine/Contest/ContestResult.hpp"

PyObject* Python::WriteLonLat(const GeoPoint &location) {
  PyObject *py_location = PyDict_New();

  PyObject *py_lon = PyFloat_FromDouble(location.longitude.Degrees());
  PyObject *py_lat = PyFloat_FromDouble(location.latitude.Degrees());
  PyDict_SetItemString(py_location, "lon", py_lon);
  PyDict_SetItemString(py_location, "lat", py_lat);
  Py_DECREF(py_lon);
  Py_DECREF(py_lat);

  return py_location;
}

PyObject* Python::WriteEvent(const BrokenDateTime &datetime,
                             const GeoPoint &location) {
  PyObject *py_event = PyDict_New();

  if (datetime.IsPlausible()) {
    PyObject *py_datetime = BrokenDateTimeToPy(datetime);
    PyDict_SetItemString(py_event, "time", py_datetime);
    Py_DECREF(py_datetime);
  }

  if (location.IsValid()) {
    PyObject *py_location = WriteLonLat(location);
    PyDict_SetItemString(py_event, "location", py_location);
    Py_DECREF(py_location);
  }

  return py_event;
}

PyObject* Python::WritePoint(const ContestTracePoint &point,
                             const ContestTracePoint *previous) {
  PyObject *py_point = PyDict_New();

  PyObject *py_time = PyInt_FromLong((long)point.GetTime());
  PyObject *py_location = WriteLonLat(point.GetLocation());

  PyDict_SetItemString(py_point, "time", py_time);
  PyDict_SetItemString(py_point, "location", py_location);

  Py_DECREF(py_time);
  Py_DECREF(py_location);

  if (previous != nullptr) {
    double distance = point.DistanceTo(previous->GetLocation());
    PyObject *py_distance = PyFloat_FromDouble(distance);
    PyDict_SetItemString(py_point, "distance", py_distance);
    Py_DECREF(py_distance);

    unsigned duration =
      std::max((int)point.GetTime() - (int)previous->GetTime(), 0);
    PyObject *py_duration = PyFloat_FromDouble(duration);
    PyDict_SetItemString(py_point, "duration", py_duration);
    Py_DECREF(py_duration);

    if (duration > 0) {
      double speed = distance / duration;
      PyObject *py_speed = PyFloat_FromDouble(speed);
      PyDict_SetItemString(py_point, "speed", py_speed);
      Py_DECREF(py_speed);
    }
  }

  return py_point;
}

PyObject* Python::WriteContest(const ContestResult &result,
                               const ContestTraceVector &trace) {
  PyObject *py_contest = PyDict_New();

  PyObject *py_score = PyFloat_FromDouble(result.score);
  PyDict_SetItemString(py_contest, "score", py_score);
  Py_DECREF(py_score);

  PyObject *py_distance = PyFloat_FromDouble(result.distance);
  PyDict_SetItemString(py_contest, "distance", py_distance);
  Py_DECREF(py_distance);

  PyObject *py_duration = PyInt_FromLong((long)result.time);
  PyDict_SetItemString(py_contest, "duration", py_duration);
  Py_DECREF(py_duration);

  PyObject *py_speed = PyFloat_FromDouble(result.GetSpeed());
  PyDict_SetItemString(py_contest, "speed", py_speed);
  Py_DECREF(py_speed);

  PyObject *py_trace = PyList_New(0);

  const ContestTracePoint *previous = NULL;
  for (auto i = trace.begin(), end = trace.end(); i != end; ++i) {
    PyObject *py_point = WritePoint(*i, previous);

    if (PyList_Append(py_trace, py_point))
      return NULL;

    Py_DECREF(py_point);
    previous = &*i;
  }

  PyDict_SetItemString(py_contest, "turnpoints", py_trace);
  Py_DECREF(py_trace);

  return py_contest;
}

