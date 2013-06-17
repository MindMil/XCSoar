#ifndef SKYLINES_FLIGHT_H
#define SKYLINES_FLIGHT_H

#include "FlightFix.hpp"
#include "FlightReader.hpp"
#include <vector>

class Flight {
public:
  std::vector<FlightFix> fixes;

public:
  /* Load a IGC file into the fixes vector */
  bool LoadIGC(const char *input_file) {
    return FlightReader(input_file, fixes);
  }
};

#endif /* SKYLINES_FLIGHT_H */
