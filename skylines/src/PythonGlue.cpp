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
#include <structmember.h> /* required for PyMemberDef */
#include <datetime.h>

#include "PythonGlue.hpp"
#include "Flight.hpp"
#include "FlightTimes.hpp"

#include <cstdio>
#include <vector>

PyXCSoarTools* XCSoarTools_init(PyXCSoarTools *self, PyObject *args) {
  /* constructor */
  self->flight = new Flight();
  return 0;
}

void XCSoarTools_dealloc(PyXCSoarTools *self) {
  /* destructor */
  delete self->flight;
}

PyObject* XCSoarTools_LoadIGC(PyXCSoarTools *self, PyObject *args) {
  const char *input_file;

  if (!PyArg_ParseTuple(args, "s", &input_file)) {
    printf("Item is not a string\n");
    return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  if (!self->flight->LoadIGC(input_file)) {
    printf("Can't read flight\n");
    return NULL;
  }
  Py_END_ALLOW_THREADS

  Py_RETURN_NONE;
}

PyObject* XCSoarTools_Path(PyXCSoarTools *self) {
  // prepare output
  PyObject *py_fixes = PyList_New(0);

  for (auto fix : self->flight->fixes) {
    PyObject *py_fix_datetime = PyInt_FromLong(fix.datetime);
    PyObject *py_fix_time = PyFloat_FromDouble(fix.time);
    PyObject *py_fix_longitude = PyFloat_FromDouble(fix.location.longitude);
    PyObject *py_fix_latitude = PyFloat_FromDouble(fix.location.latitude);
    PyObject *py_fix_gps_altitude = PyFloat_FromDouble(fix.gps_altitude);
    PyObject *py_fix_pressure_altitude = PyFloat_FromDouble(fix.pressure_altitude);
    PyObject *py_fix_engine_noise_level = PyFloat_FromDouble(fix.engine_noise_level);
    PyObject *py_fix_track = PyFloat_FromDouble(fix.track);
    PyObject *py_fix_ground_speed = PyFloat_FromDouble(fix.ground_speed);
    PyObject *py_fix_tas = PyFloat_FromDouble(fix.tas);
    PyObject *py_fix_ias = PyFloat_FromDouble(fix.ias);
    PyObject *py_fix_satellites = PyInt_FromLong(fix.satellites);

    PyObject *py_fix = PyTuple_Pack(12,
      py_fix_datetime,
      py_fix_time,
      py_fix_longitude,
      py_fix_latitude,
      py_fix_gps_altitude,
      py_fix_pressure_altitude,
      py_fix_engine_noise_level,
      py_fix_track,
      py_fix_ground_speed,
      py_fix_tas,
      py_fix_ias,
      py_fix_satellites);

    if (PyList_Append(py_fixes, py_fix))
      return NULL;

    Py_DECREF(py_fix);

    Py_DECREF(py_fix_time);
    Py_DECREF(py_fix_datetime);
    Py_DECREF(py_fix_longitude);
    Py_DECREF(py_fix_latitude);
    Py_DECREF(py_fix_gps_altitude);
    Py_DECREF(py_fix_pressure_altitude);
    Py_DECREF(py_fix_engine_noise_level);
    Py_DECREF(py_fix_track);
    Py_DECREF(py_fix_ground_speed);
    Py_DECREF(py_fix_tas);
    Py_DECREF(py_fix_ias);
    Py_DECREF(py_fix_satellites);
  }

  return py_fixes;
}

PyObject* XCSoarTools_Times(PyXCSoarTools *self) {
  std::vector<Result> results;

  FlightTimes(self->flight->fixes, results); 

  PyObject *py_times = PyList_New(0);

  for (auto times : results) {
    PyObject *py_single_flight = PyDict_New();

    PyObject *py_takeoff_longitude =
      PyFloat_FromDouble(times.takeoff_location.longitude.Degrees());
    PyObject *py_takeoff_latitude =
      PyFloat_FromDouble(times.takeoff_location.latitude.Degrees());
    PyObject *py_takeoff_datetime = PyDateTime_FromDateAndTime(
        times.takeoff_time.year,
        times.takeoff_time.month,
        times.takeoff_time.day,
        times.takeoff_time.hour,
        times.takeoff_time.minute,
        times.takeoff_time.second,
        0);

    PyObject *py_release_longitude =
      PyFloat_FromDouble(times.release_location.longitude.Degrees());
    PyObject *py_release_latitude =
      PyFloat_FromDouble(times.release_location.latitude.Degrees());
    PyObject *py_release_datetime = PyDateTime_FromDateAndTime(
        times.release_time.year,
        times.release_time.month,
        times.release_time.day,
        times.release_time.hour,
        times.release_time.minute,
        times.release_time.second,
        0);

    PyObject *py_landing_longitude =
      PyFloat_FromDouble(times.landing_location.longitude.Degrees());
    PyObject *py_landing_latitude =
      PyFloat_FromDouble(times.landing_location.latitude.Degrees());
    PyObject *py_landing_datetime = PyDateTime_FromDateAndTime(
        times.landing_time.year,
        times.landing_time.month,
        times.landing_time.day,
        times.landing_time.hour,
        times.landing_time.minute,
        times.landing_time.second,
        0);

    PyDict_SetItemString(py_single_flight, "takeoff_lon", py_takeoff_longitude);
    PyDict_SetItemString(py_single_flight, "takeoff_lat", py_takeoff_latitude);
    PyDict_SetItemString(py_single_flight, "takeoff_time", py_takeoff_datetime);
    PyDict_SetItemString(py_single_flight, "release_lon", py_release_longitude);
    PyDict_SetItemString(py_single_flight, "release_lat", py_release_latitude);
    PyDict_SetItemString(py_single_flight, "release_time", py_release_datetime);
    PyDict_SetItemString(py_single_flight, "landing_lon", py_landing_longitude);
    PyDict_SetItemString(py_single_flight, "landing_lat", py_landing_latitude);
    PyDict_SetItemString(py_single_flight, "landing_time", py_landing_datetime);

    Py_DECREF(py_takeoff_longitude);
    Py_DECREF(py_takeoff_latitude);
    Py_DECREF(py_takeoff_datetime);
    Py_DECREF(py_release_longitude);
    Py_DECREF(py_release_latitude);
    Py_DECREF(py_release_datetime);
    Py_DECREF(py_landing_longitude);
    Py_DECREF(py_landing_latitude);
    Py_DECREF(py_landing_datetime);

    if (PyList_Append(py_times, py_single_flight))
      return NULL;

    Py_DECREF(py_single_flight);
  }

  return py_times;
}

PyMODINIT_FUNC initXCSoarTools() {
  PyObject* m;

  if (PyType_Ready(&XCSoarToolsType) < 0)
      return;

  m = Py_InitModule3("XCSoarTools", XCSoarTools_methods, "XCSoar Tools");

  if (m == NULL)
    return;

  PyDateTime_IMPORT;

  Py_INCREF(&XCSoarToolsType);
  PyModule_AddObject(m, "XCSoarTools", (PyObject *)&XCSoarToolsType);
}
