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


print "Init XCSoarTools"
tools = init_tools(args.file_name, True)

print "Calculating times..."
times = calc_times(tools)

print "Found {} flight(s)".format(len(times))

for time in times:
  print
  print "Takeoff: {}, location {} {}".format(time['takeoff_time'], time['takeoff_lon'], time['takeoff_lat'])
  print "Release: {}, location {} {}".format(time['release_time'], time['release_lon'], time['release_lat'])
  print "Landing: {}, location {} {}".format(time['landing_time'], time['landing_lon'], time['landing_lat'])
  flight_path = tools.Path()
  print flight_path[0]
  print flight_path[len(flight_path) - 1]
  pprint(tools.Analyse(time['release_time'], time['landing_time'], full=1024, triangle=1024, sprint=64))
