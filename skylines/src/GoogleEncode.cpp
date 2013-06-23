#include "GoogleEncode.hpp"

#include <stack>
#include <sstream>

#include <stdio.h>

using namespace std;

string SkyLinesPolyEncoder::encodeSignedNumber(int num) {
    int sgn_num = num << 1;
    if (num < 0) {
        sgn_num = ~(sgn_num);
    }
    return (encodeNumber(sgn_num));
}

string SkyLinesPolyEncoder::encodeNumber(int num) {
    ostringstream encodeString;

    while (num >= 0x20) {
        int nextValue = (0x20 | (num & 0x1f)) + 63;
        encodeString << ((char) (nextValue));
        num >>= 5;
    }

    num += 63;
    encodeString << ((char) (num));

    return encodeString.str();
}


auto_ptr<pair<string, string> > SkyLinesPolyEncoder::encode(vector<pair<double,double> >& points, vector<int>& levels) {
    ostringstream encodedLevels;
    ostringstream encodedPoints;

    int plat = 0;
    int plng = 0;

    size_t n_points = points.size();
    for (size_t i=0; i<n_points; i++) {
        if (levels[i] != -1) {
            encodedLevels << encodeNumber(levels[i]);
            
            pair<double, double> point = points[i];

            int late5 = floor1e5(point.second);
            int lnge5 = floor1e5(point.first);

            int dlat = late5 - plat;
            int dlng = lnge5 - plng;

            plat = late5;
            plng = lnge5;

            encodedPoints << encodeSignedNumber(dlat);
            encodedPoints << encodeSignedNumber(dlng);
        }
    }
    
    auto_ptr<pair<string, string> > r(new pair<string,string>);
    r->first = encodedPoints.str();
    r->second = encodedLevels.str();
    return r;
}

string SkyLinesPolyEncoder::encodeList(list<int>& points) {
    ostringstream encodedList;

    int val = 0;

    for (list<int>::iterator i = points.begin(); i != points.end(); i++) {
        int dval = *i - val;
        val = *i;

        encodedList << encodeSignedNumber(dval);
     }

     return encodedList.str();
}
