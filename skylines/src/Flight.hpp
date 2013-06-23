#ifndef SKYLINES_FLIGHT_H
#define SKYLINES_FLIGHT_H

#include "FlightFix.hpp"
#include "FlightTimes.hpp"
#include "AnalyseFlight.hpp"
#include "GoogleEncode.hpp"

#include "DebugReplayIGC.hpp"
#include "DebugReplayVector.hpp"
#include "FlightPhaseDetector.hpp"

#include <vector>

class ContestStatistics;
class BrokenDateTime;
class DebugReplay;

class Flight {
private:
  std::vector<FlightFix> *fixes;
  bool keep_flight;
  const char *flight_file;

public:
  Flight(const char* _flight_file, bool _keep_flight);
  ~Flight();

  /* Search for flights within the fixes */
  unsigned Times(std::vector<Result> &results) {
    DebugReplay *replay = Replay();
    FlightTimes(*replay, results);
    delete replay;

    return results.size();
  }

  /* Analyse flight */
  void Analyse(const BrokenDateTime takeoff_time,
               const BrokenDateTime release_time,
               const BrokenDateTime landing_time,
               ContestStatistics &olc_plus,
               ContestStatistics &dmst,
               PhaseList &phase_list,
               PhaseTotals &phase_totals,
               const unsigned full = 512,
               const unsigned triangle = 1024,
               const unsigned sprint = 96) {
    DebugReplay *replay = Replay();
    AnalyseFlight(*replay, takeoff_time, release_time, landing_time,
                  olc_plus, dmst,
                  phase_list, phase_totals,
                  full, triangle, sprint);
    delete replay;
  }

  /**
   * Return a DebugReplay, either direct from file or from memory,
   * depending on the keep_flight flag. Don't forget to delete
   * the replay after use.
   */
  DebugReplay *Replay() {
    if (keep_flight) return DebugReplayVector::Create(*fixes);
    else return DebugReplayIGC::Create(flight_file);
  };

  /* Return flight as google encoded string */
  GoogleEncode::EncodedFlight GoogleEncoded(
    const BrokenDateTime start, const BrokenDateTime end,
    const unsigned num_levels, const unsigned zoom_factor,
    const double threshold, const bool force_endpoints);
};

#endif /* SKYLINES_FLIGHT_H */
