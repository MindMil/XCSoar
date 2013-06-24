#!/usr/bin/env python

import XCSoarTools
import time
import argparse
from pprint import pprint

# Parse command line parameters
parser = argparse.ArgumentParser(
    description='Please give me a IGC file name...')

parser.add_argument('file_name', type=str)

args = parser.parse_args()

def timing(f):
    def wrap(*args):
        time1 = time.time()
        ret = f(*args)
        time2 = time.time()
        print '%s function took %0.3f ms' % (f.func_name, (time2-time1)*1000.0)
        return ret
    return wrap


@timing
def init_tools(filename, keep):
  return XCSoarTools.Flight(filename, keep=keep)

@timing
def calc_times(tools):
  return tools.Times()

@timing
def calc_analyse(tools, takeoff, release, landing, full, triangle, sprint):
  return tools.Analyse(takeoff, release, landing, full=full, triangle=triangle, sprint=sprint)


print "Init XCSoarTools"
tools = init_tools(args.file_name, False)

print "Calculating times..."
times = calc_times(tools)

print "Found {} flight(s)".format(len(times))

for dtime in times:
  takeoff = dtime['takeoff']
  release = dtime['release']
  landing = dtime['landing']

  print
  print "Takeoff: {}, location {} {}".format(takeoff['time'], takeoff['longitude'], takeoff['latitude'])
  print "Release: {}, location {} {}".format(release['time'], release['longitude'], release['latitude'])
  print "Landing: {}, location {} {}".format(landing['time'], landing['longitude'], landing['latitude'])

  fixes = tools.Path(takeoff['time'], release['time'])
  for fix in fixes:
    print fix

  pprint(calc_analyse(tools, takeoff['time'], release['time'], landing['time'], \
         1024, 1024, 64))

del tools

