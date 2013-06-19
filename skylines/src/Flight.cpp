#include "Flight.hpp"
#include "FlightReader.hpp"
#include <vector>

Flight::Flight(const char* _flight_file, bool _keep_flight)
  : fixes(nullptr), keep_flight(_keep_flight), flight_file(_flight_file) {
  if (keep_flight) {
    fixes = new std::vector<FlightFix>;
    FlightReader(flight_file, *fixes);
  }
}

Flight::~Flight() {
  if (keep_flight)
    delete fixes;
}

