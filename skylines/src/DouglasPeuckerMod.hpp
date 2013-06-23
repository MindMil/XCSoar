#ifndef SKYLINES_DOUGLASPEUCKERMOD_H
#define SKYLINES_DOUGLASPEUCKERMOD_H

#include <vector>
#include <list>
#include <memory>

class DouglasPeuckerMod {

private:
    int numLevels;
    int zoomFactor;
    double threshold;
    bool forceEndpoints;
    double *zoomLevelBreaks;

    double distance_dp(std::vector<double>& p0, std::vector<double>& p1, std::vector<double>& p2, std::list<size_t>& points);
    double distance_simple(std::vector<double>& p0, std::vector<double>& p1, std::vector<double>& p2, std::list<size_t>& points);
    int computeLevel(double absMaxDist);
    std::vector<int> classify(size_t n_points, const double dists[], double absMaxDist);

public:
    DouglasPeuckerMod(int numLevels=18, int zoomFactor=2, double threshold=0.00001, bool forceEndpoints=true);
    ~DouglasPeuckerMod();

    std::vector<int> dpEncode(std::vector<std::vector<double> >& points, char *type);
    int getNumLevels() { return numLevels; }
    int getZoomFactor() { return zoomFactor; }
};

#endif /* SKYLINES_DOUGLASPEUCKERMOD_H */
