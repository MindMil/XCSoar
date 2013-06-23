#ifndef SKYLINES_GOOGLEENCODE_H
#define SKYLINES_GOOGLEENCODE_H

#include <vector>
#include <list>
#include <string>
#include <cmath>
#include <memory>

class GoogleEncode {

private:
    inline int floor1e5(double coordinate) { return (int)floor(coordinate * 1e5); }
    std::string encodeSignedNumber(int num);
    std::string encodeNumber(int num);
    
public:
    std::auto_ptr<std::pair<std::string, std::string> > encode(std::vector<std::pair<double,double> >& points, std::vector<int>& levels);
    std::string encodeList(std::list<int>& points);
};

#endif /* SKYLINES_GOOGLEENCODE_H */
