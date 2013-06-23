#include "Flight.hpp"
#include "FlightReader.hpp"
#include "DouglasPeuckerMod.hpp"
#include "GoogleEncode.hpp"

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

GoogleEncode::EncodedFlight Flight::GoogleEncoded(
  const BrokenDateTime start, const BrokenDateTime end,
  const unsigned num_levels, const unsigned zoom_factor,
  const double threshold, const bool force_endpoints,
  const unsigned max_delta_time) {

  // we need the whole flight, so read it now...
  if (!keep_flight) {
    fixes = new std::vector<FlightFix>;
    FlightReader(flight_file, *fixes);
  }

  DouglasPeuckerMod dp(num_levels, zoom_factor, threshold, force_endpoints, max_delta_time);

  unsigned start_index = 0,
           end_index = 0;

  int64_t start_time = start.ToUnixTimeUTC(),
          end_time = end.ToUnixTimeUTC();

  for (auto fix : *fixes) {
    if (fix.datetime <= start_time)
      start_index++;

    if (fix.datetime <= end_time)
      end_index++;
    else
      break;
  }

  end_index = std::min(end_index, unsigned(fixes->size()));
  start_index = std::min(start_index, end_index);

  std::vector<int> levels = dp.dpEncode(*fixes, start_index, end_index);

  GoogleEncode::EncodedFlight encoded =
    GoogleEncode::encode(fixes->begin() + start_index,
                         fixes->begin() + end_index,
                         levels.begin(),
                         levels.end());

  if (!keep_flight)
    delete fixes;

  return encoded;
}
