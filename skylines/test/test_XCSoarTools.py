#!/usr/bin/env python

from XCSoarTools import XCSoarTools
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
  return XCSoarTools(filename, keep=keep)

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
  print
  print "Takeoff: {}, location {} {}".format(dtime['takeoff_time'], dtime['takeoff_lon'], dtime['takeoff_lat'])
  print "Release: {}, location {} {}".format(dtime['release_time'], dtime['release_lon'], dtime['release_lat'])
  print "Landing: {}, location {} {}".format(dtime['landing_time'], dtime['landing_lon'], dtime['landing_lat'])

  fixes = tools.Path(dtime['takeoff_time'], dtime['release_time'])
  for fix in fixes:
    print fix

  pprint(calc_analyse(tools, dtime['takeoff_time'], dtime['release_time'], dtime['landing_time'], \
         1024, 1024, 64))

del tools

