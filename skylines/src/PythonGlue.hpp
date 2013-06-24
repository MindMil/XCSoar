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

#ifndef SKYLINES_PYTHONGLUE_H
#define SKYLINES_PYTHONGLUE_H

#include <Python.h>
#include "Flight.hpp"

struct PyXCSoarTools {
  PyObject_HEAD Flight *flight;
};

static PyXCSoarTools* XCSoarTools_init(PyXCSoarTools *self, PyObject *args, PyObject *kwargs);
static void XCSoarTools_dealloc(PyXCSoarTools *self);

static PyObject* XCSoarTools_Path(PyXCSoarTools *self, PyObject *args);
static PyObject* XCSoarTools_GoogleEncoded(PyXCSoarTools *self, PyObject *args);
static PyObject* XCSoarTools_Times(PyXCSoarTools *self);
static PyObject* XCSoarTools_Analyse(PyXCSoarTools *self, PyObject *args, PyObject *kwargs);
static PyObject* XCSoarTools_EncodeList(PyXCSoarTools *self, PyObject *args);
 
static PyMethodDef XCSoarTools_methods[] = {
  {"EncodeList", (PyCFunction)XCSoarTools_EncodeList, METH_VARARGS, "Encode a list of int."},
  {NULL, NULL, 0, NULL}
};

static PyMethodDef XCSoarTools_Flight_methods[] = {
  {"Path", (PyCFunction)XCSoarTools_Path, METH_VARARGS, "Get flight as list."},
  {"GoogleEncoded", (PyCFunction)XCSoarTools_GoogleEncoded, METH_VARARGS, "Get flight as google encoded string."},
  {"Times", (PyCFunction)XCSoarTools_Times, METH_VARARGS, "Get takeoff/release/landing times from flight."},
  {"Analyse", (PyCFunction)XCSoarTools_Analyse, METH_VARARGS | METH_KEYWORDS, "Analyse flight."},
  {NULL, NULL, 0, NULL}
};

static PyMemberDef XCSoarTools_members[] = {
  {NULL}  /* Sentinel */
};

PyTypeObject XCSoarToolsType = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                     /* obj_size */
  "XCSoarTools",         /* char *tp_name; */
  sizeof(PyXCSoarTools), /* int tp_basicsize; */
  0,                     /* int tp_itemsize; not used much */
  (destructor)XCSoarTools_dealloc, /* destructor tp_dealloc; */
  0,                     /* printfunc  tp_print; */
  0,                     /* getattrfunc  tp_getattr; __getattr__ */
  0,                     /* setattrfunc  tp_setattr; __setattr__ */
  0,                     /* cmpfunc  tp_compare; __cmp__ */
  0,                     /* reprfunc  tp_repr; __repr__ */
  0,                     /* PyNumberMethods *tp_as_number; */
  0,                     /* PySequenceMethods *tp_as_sequence; */
  0,                     /* PyMappingMethods *tp_as_mapping; */
  0,                     /* hashfunc tp_hash; __hash__ */
  0,                     /* ternaryfunc tp_call; __call__ */
  0,                     /* reprfunc tp_str; __str__ */
  0,                     /* tp_getattro */
  0,                     /* tp_setattro */
  0,                     /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
  "XCSoarTools object",  /* tp_doc */
  0,		         /* tp_traverse */
  0,		         /* tp_clear */
  0,		         /* tp_richcompare */
  0,		         /* tp_weaklistoffset */
  0,		         /* tp_iter */
  0,		         /* tp_iternext */
  XCSoarTools_Flight_methods,   /* tp_methods */
  XCSoarTools_members,   /* tp_members */
  0,                     /* tp_getset */
  0,                     /* tp_base */
  0,                     /* tp_dict */
  0,                     /* tp_descr_get */
  0,                     /* tp_descr_set */
  0,                     /* tp_dictoffset */
  (initproc)XCSoarTools_init, /* tp_init */
  0,                     /* tp_alloc */
  PyType_GenericNew,     /* tp_new */
/* this could be extended even further...
   * http://starship.python.net/crew/arcege/extwriting/pyext.html
   */
};

PyMODINIT_FUNC initXCSoarTools();

#endif /* SKYLINES_PYTHONGLUE_H */
