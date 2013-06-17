#ifndef SKYLINES_FLIGHT_H
#define SKYLINES_FLIGHT_H

#include "FlightFix.hpp"
#include "FlightReader.hpp"
#include "FlightTimes.hpp"
#include <vector>

class Flight {
public:
  std::vector<FlightFix> fixes;

public:
  /* Load a IGC file into the fixes vector */
  bool LoadIGC(const char *input_file) {
    return FlightReader(input_file, fixes);
  }

  /* Search for flights within the fixes */
  unsigned Times(std::vector<Result> &results) {
    FlightTimes(fixes, results);
    return results.size();
  }
};

#endif /* SKYLINES_FLIGHT_H */
