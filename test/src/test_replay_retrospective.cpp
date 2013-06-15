#include "test_debug.hpp"
#include "harness_aircraft.hpp"
#include "Replay/IgcReplay.hpp"
#include "Engine/Navigation/Aircraft.hpp"
#include "NMEA/FlyingState.hpp"
#include "OS/ConvertPathName.hpp"
#include "OS/FileUtil.hpp"
#include "IO/FileLineReader.hpp"
#include "NMEA/Info.hpp"
#include "Engine/Waypoint/Waypoints.hpp"
#include "Engine/Contest/Solvers/Retrospective.hpp"
#include "Waypoint/WaypointReader.hpp"
#include "Waypoint/WaypointReaderBase.hpp"
#include "Operation/Operation.hpp"

#include <fstream>

static bool
test_replay_retrospective()
{
  Directory::Create(_T("output/results"));
  std::ofstream f("output/results/res-sample.txt");

  Waypoints waypoints;
  WaypointReader w(waypoint_file.c_str(), 0);
  if (!ok1(!w.Error())) {
    printf("# waypoint file %s\n", waypoint_file.c_str());
    skip(2, 0, "opening waypoint file failed");
    return false;
  }

  NullOperationEnvironment operation;
  if(!ok1(w.Parse(waypoints, operation))) {
    skip(1, 0, "parsing waypoint file failed");
    return false;
  }

  waypoints.Optimise();

  ok1(!waypoints.IsEmpty());

  Retrospective retro(waypoints);

  FileLineReaderA *reader = new FileLineReaderA(replay_file.c_str());
  if (reader->error()) {
    delete reader;
    return false;
  }

  waypoints.Optimise();

  IgcReplay sim(reader);

  NMEAInfo basic;
  basic.Reset();

  while (sim.Update(basic)) {
    n_samples++;

    if (retro.UpdateSample(basic.location)) {
      std::ofstream g("output/results/res-retro.txt");
      retro.Print(g);
    }

    f << (double)basic.time << " " 
      <<  (double)basic.location.longitude.Degrees() << " "
      <<  (double)basic.location.latitude.Degrees() << "\n";
    f.flush();
  };

  return true;
}


int main(int argc, char** argv) 
{
  output_skip = 60;

  replay_file = "test/data/9crx3101.igc";
  waypoint_file = "test/data/benalla9.xcw";

  if (!ParseArgs(argc,argv)) {
    return 0;
  }

  plan_tests(4);

  ok(test_replay_retrospective(),"replay retrospective",0);

  return exit_status();
}

