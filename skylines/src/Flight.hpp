#ifndef SKYLINES_FLIGHT_H
#define SKYLINES_FLIGHT_H

#include "FlightFix.hpp"
#include "FlightReader.hpp"
#include "FlightTimes.hpp"
#include "AnalyseFlight.hpp"
#include "Time/BrokenDateTime.hpp"

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

  /* Analyse flight */
  void Analyse(const BrokenDateTime release_time, const BrokenDateTime landing_time,
               const unsigned full = 512,
               const unsigned triangle = 1024,
               const unsigned sprint = 96) {
    AnalyseFlight(fixes, release_time, landing_time, full, triangle, sprint);
  }
};

#endif /* SKYLINES_FLIGHT_H */
