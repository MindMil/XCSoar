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
#include "PythonConverters.hpp"
#include "Flight.hpp"
#include "Time/BrokenDateTime.hpp"
#include "FlightPhaseDetector.hpp"
#include "GoogleEncode.hpp"

#include <cstdio>
#include <vector>
#include <cinttypes>
#include <limits>

Pyxcsoar_Flight* xcsoar_Flight_init(Pyxcsoar_Flight *self, PyObject *args, PyObject *kwargs) {
  /* constructor */
  static char *kwlist[] = {"file", "keep", NULL};
  const char *input_file;
  bool keep = false;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|b", kwlist,
                                   &input_file, &keep)) {
    printf("Cannot parse arguments\n");
    return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  self->flight = new Flight(input_file, keep);
  Py_END_ALLOW_THREADS

  return 0;
}

void xcsoar_Flight_dealloc(Pyxcsoar_Flight *self) {
  /* destructor */
  delete self->flight;
}

PyObject* xcsoar_Flight_Path(Pyxcsoar_Flight *self, PyObject *args) {
  PyObject *py_begin = NULL,
           *py_end = NULL;

  if (!PyArg_ParseTuple(args, "|OO", &py_begin, &py_end)) {
    printf("Cannot parse arguments\n");
    return NULL;
  }

  int64_t begin = 0,
          end = std::numeric_limits<int64_t>::max();

  if (py_begin != NULL && PyDateTime_Check(py_begin))
    begin = Python::PyToBrokenDateTime(py_begin).ToUnixTimeUTC();

  if (py_end != NULL && PyDateTime_Check(py_end))
    end = Python::PyToBrokenDateTime(py_end).ToUnixTimeUTC();

  // prepare output
  PyObject *py_fixes = PyList_New(0);

  DebugReplay *replay = self->flight->Replay();
  while (replay->Next()) {
    const MoreData &basic = replay->Basic();
    const int64_t date_time_utc = basic.date_time_utc.ToUnixTimeUTC();

    if (date_time_utc < begin)
      continue;
    else if (date_time_utc > end)
      break;

    if (!basic.time_available || !basic.location_available ||
        !basic.NavAltitudeAvailable())
      continue;

    const FlightFix fix(basic);

    PyObject *py_fix_datetime = PyInt_FromLong(fix.datetime);
    PyObject *py_fix_time = PyFloat_FromDouble(fix.time);
    PyObject *py_fix_longitude = PyFloat_FromDouble(fix.longitude);
    PyObject *py_fix_latitude = PyFloat_FromDouble(fix.latitude);
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

  delete replay;

  return py_fixes;
}

PyObject* xcsoar_Flight_GoogleEncoded(Pyxcsoar_Flight *self, PyObject *args, PyObject *kwargs) {
  PyObject *py_begin = NULL,
           *py_end = NULL,
           *py_force_endpoints = NULL;

  static char *kwlist[] = {"begin", "end", "num_levels", "zoom_factor",
                           "max_delta_time", "threshold",
                           "force_endpoints", NULL};

  /* default values */
  unsigned num_levels = 4,
           zoom_factor = 4,
           max_delta_time = 30;
  double threshold = 0.001;
  bool force_endpoints = true;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOIIIdO", kwlist,
                                   &py_begin, &py_end, &num_levels, &zoom_factor,
                                   &max_delta_time, &threshold, &py_force_endpoints)) {
    printf("Cannot parse arguments\n");
    return NULL;
  }

  if (py_force_endpoints != NULL && !PyObject_IsTrue(py_force_endpoints))
    force_endpoints = false;

  BrokenDateTime begin, end;

  if (py_begin != NULL && PyDateTime_Check(py_begin))
    begin = Python::PyToBrokenDateTime(py_begin);
  else
    begin.FromUnixTimeUTC(0);

  if (py_end != NULL && PyDateTime_Check(py_end))
    end = Python::PyToBrokenDateTime(py_end);
  else
    end.FromUnixTimeUTC(std::numeric_limits<int64_t>::max());

  GoogleEncode::EncodedFlight encoded;

  Py_BEGIN_ALLOW_THREADS
  encoded = self->flight->GoogleEncoded(begin, end, num_levels,
    zoom_factor, threshold, force_endpoints, max_delta_time);
  Py_END_ALLOW_THREADS

  // prepare output
  PyObject *py_points = PyString_FromString(encoded.locations->c_str());
  PyObject *py_levels = PyString_FromString(encoded.levels->c_str());
  PyObject *py_times = PyString_FromString(encoded.times->c_str());
  PyObject *py_gps_alt = PyString_FromString(encoded.gps_alt->c_str());
  PyObject *py_enl = PyString_FromString(encoded.enl->c_str());

  PyObject *py_result = PyDict_New();
  PyDict_SetItemString(py_result, "points", py_points);
  PyDict_SetItemString(py_result, "levels", py_levels);
  PyDict_SetItemString(py_result, "times", py_times);
  PyDict_SetItemString(py_result, "gps_alt", py_gps_alt);
  PyDict_SetItemString(py_result, "enl", py_enl);

  Py_DECREF(py_points);
  Py_DECREF(py_levels);
  Py_DECREF(py_times);
  Py_DECREF(py_gps_alt);
  Py_DECREF(py_enl);

  return py_result;
}

PyObject* xcsoar_Flight_Times(Pyxcsoar_Flight *self) {
  std::vector<Result> results;

  Py_BEGIN_ALLOW_THREADS
  self->flight->Times(results);
  Py_END_ALLOW_THREADS

  PyObject *py_times = PyList_New(0);

  for (auto times : results) {
    PyObject *py_single_flight = PyDict_New();

    PyObject *py_takeoff = Python::WriteEvent(
      times.takeoff_time,
      times.takeoff_location);

    PyObject *py_release = Python::WriteEvent(
      times.release_time,
      times.release_location);

    PyObject *py_landing = Python::WriteEvent(
      times.landing_time,
      times.landing_location);

    PyDict_SetItemString(py_single_flight, "takeoff", py_takeoff);
    PyDict_SetItemString(py_single_flight, "release", py_release);
    PyDict_SetItemString(py_single_flight, "landing", py_landing);

    Py_DECREF(py_takeoff);
    Py_DECREF(py_release);
    Py_DECREF(py_landing);

    if (PyList_Append(py_times, py_single_flight))
      return NULL;

    Py_DECREF(py_single_flight);
  }

  return py_times;
}

PyObject* xcsoar_Flight_Analyse(Pyxcsoar_Flight *self, PyObject *args, PyObject *kwargs) {
  static char *kwlist[] = {"takeoff", "release", "landing", "full", "triangle", "sprint", NULL};
  PyObject *py_takeoff, *py_release, *py_landing;
  unsigned full = 512,
           triangle = 1024,
           sprint = 96;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOO|III", kwlist,
                                   &py_takeoff, &py_release, &py_landing, &full, &triangle, &sprint)) {
    printf("Cannot parse arguments\n");
    return NULL;
  }

  if (!PyDateTime_Check(py_takeoff) || !PyDateTime_Check(py_release) || !PyDateTime_Check(py_landing)) {
    printf("Begin and end are no DateTime objects\n");
    return NULL;
  }

  BrokenDateTime takeoff = Python::PyToBrokenDateTime(py_takeoff);
  BrokenDateTime release = Python::PyToBrokenDateTime(py_release);
  BrokenDateTime landing = Python::PyToBrokenDateTime(py_landing);

  ContestStatistics olc_plus;
  ContestStatistics dmst;

  PhaseList phase_list;
  PhaseTotals phase_totals;

  Py_BEGIN_ALLOW_THREADS
  self->flight->Analyse(takeoff, release, landing,
    olc_plus, dmst,
    phase_list, phase_totals,
    full, triangle, sprint);
  Py_END_ALLOW_THREADS

  PyObject *py_result = PyDict_New();

  /* write contests */
  PyObject *py_contests = PyDict_New();

  /* write olc_plus statistics */
  PyObject *py_olc_plus = PyDict_New();

  PyObject *py_classic = Python::WriteContest(olc_plus.result[0], olc_plus.solution[0]);
  PyObject *py_triangle = Python::WriteContest(olc_plus.result[1], olc_plus.solution[1]);
  PyObject *py_plus = Python::WriteContest(olc_plus.result[2], olc_plus.solution[2]);

  PyDict_SetItemString(py_olc_plus, "classic", py_classic);
  PyDict_SetItemString(py_olc_plus, "triangle", py_triangle);
  PyDict_SetItemString(py_olc_plus, "plus", py_plus);

  Py_DECREF(py_classic);
  Py_DECREF(py_triangle);
  Py_DECREF(py_plus);

  PyDict_SetItemString(py_contests, "olc_plus", py_olc_plus);
  Py_DECREF(py_olc_plus);

  /* write dmst statistics */
  PyObject *py_dmst = PyDict_New();

  PyObject *py_quadrilateral = Python::WriteContest(dmst.result[0], dmst.solution[0]);
  PyDict_SetItemString(py_dmst, "quadrilateral", py_quadrilateral);
  Py_DECREF(py_quadrilateral);

  PyDict_SetItemString(py_contests, "dmst", py_dmst);
  Py_DECREF(py_dmst);

  PyDict_SetItemString(py_result, "contests", py_contests);
  Py_DECREF(py_contests);

  /* write fligh phases */
  PyObject *py_phases = PyList_New(0);

  for (Phase phase : phase_list) {
    PyObject *py_phase = Python::WritePhase(phase);
    if (PyList_Append(py_phases, py_phase))
      return NULL;

    Py_DECREF(py_phase);
  }

  PyDict_SetItemString(py_result, "phases", py_phases);
  Py_DECREF(py_phases);

  /* write performance stats */
  PyObject *py_performance = Python::WritePerformanceStats(phase_totals);
  PyDict_SetItemString(py_result, "performance", py_performance);
  Py_DECREF(py_performance);

  return py_result;
}

PyObject* xcsoar_EncodePoints(PyObject *self, PyObject *args) {
  PyObject *py_list;

  if (!PyArg_ParseTuple(args, "O", &py_list)) {
    printf("Cannot parse argument\n");
    return NULL;
  }

  if (!PySequence_Check(py_list)) {
    printf("Not a list");
    return NULL;
  }

  Py_ssize_t num_items = PySequence_Fast_GET_SIZE(py_list);

  std::vector<std::pair<double, double>> items;

  for (Py_ssize_t i = 0; i < num_items; i++) {
    PyObject *py_item = PySequence_Fast_GET_ITEM(py_list, i);
    if (PySequence_Fast_GET_SIZE(py_item) != 2) {
      PyErr_SetString(PyExc_TypeError, "expected (n == 2)-tuple with numbers ((x0, y0), (x1, y1), ...)");
      return NULL;
    }

    PyObject *py_lat = PySequence_Fast_GET_ITEM(py_item, 0);
    PyObject *py_lon = PySequence_Fast_GET_ITEM(py_item, 1);

    if (!PyFloat_Check(py_lat) || !PyFloat_Check(py_lon)) {
      PyErr_SetString(PyExc_TypeError, "expected floats");
      return NULL;
    }

    items.push_back(std::pair<double, double>(
      PyFloat_AsDouble(py_lat), PyFloat_AsDouble(py_lon)));

    Py_DECREF(py_lat);
    Py_DECREF(py_lon);
    Py_DECREF(py_item);
  }

  Py_DECREF(py_list);

  std::unique_ptr<std::string> encoded;

  Py_BEGIN_ALLOW_THREADS
  encoded = GoogleEncode::encodeLonLat(items);
  Py_END_ALLOW_THREADS

  // prepare output
  PyObject *py_result = PyString_FromString(encoded->c_str());

  return py_result;
}


PyObject* xcsoar_EncodeList(PyObject *self, PyObject *args) {
  PyObject *py_list;

  if (!PyArg_ParseTuple(args, "O", &py_list)) {
    printf("Cannot parse argument\n");
    return NULL;
  }

  if (!PySequence_Check(py_list)) {
    printf("Not a list");
    return NULL;
  }

  Py_ssize_t num_items = PySequence_Fast_GET_SIZE(py_list);

  std::vector<int> items;

  for (Py_ssize_t i = 0; i < num_items; i++) {
    PyObject *py_item = PySequence_Fast_GET_ITEM(py_list, i);
    if (!PyNumber_Check(py_item)) {
      PyErr_SetString(PyExc_TypeError, "expected list with numbers");
      return NULL;
    }

    items.push_back(PyInt_AsLong(py_item));
    Py_DECREF(py_item);
  }

  Py_DECREF(py_list);

  std::unique_ptr<std::string> encoded;

  Py_BEGIN_ALLOW_THREADS
  encoded = GoogleEncode::encodeList(items);
  Py_END_ALLOW_THREADS

  // prepare output
  PyObject *py_result = PyString_FromString(encoded->c_str());

  return py_result;
}

PyMODINIT_FUNC
__attribute__ ((visibility("default")))
initxcsoar() {
  PyObject* m;

  if (PyType_Ready(&xcsoar_Flight_Type) < 0)
      return;

  m = Py_InitModule3("xcsoar", xcsoar_methods, "XCSoar Tools");

  if (m == NULL)
    return;

  PyDateTime_IMPORT;

  Py_INCREF(&xcsoar_Flight_Type);
  PyModule_AddObject(m, "Flight", (PyObject *)&xcsoar_Flight_Type);
}
