#ifndef SKYLINES_FLIGHT_H
#define SKYLINES_FLIGHT_H

#include "FlightFix.hpp"
#include <vector>

class Flight {
public:
  std::vector<FlightFix> fixes;
};

#endif /* SKYLINES_FLIGHT_H */
