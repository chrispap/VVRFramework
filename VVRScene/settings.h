#ifndef VVR_SETTINGS_H
#define VVR_SETTINGS_H

#include "vvrscenedll.h"

#include <string>
#include <map>
#include <vector>

using namespace std;

namespace vvr {

class VVRScene_API Settings
{
    map<string, string> sMap;

public:
    Settings () {}
    Settings (string file);

    void getKeys(vector<string> &keys) const;
    void write(const string &filename) const;
    void set(const string &Key, const string &newVal, bool create_if_not_exist=false);

    string  getStr(const string &key) const;
    double  getDbl(const string &key) const;
    int     getInt(const string &key) const;
    bool    getBool(const string &key) const;
};

}

#endif // VVR_SETTINGS_H
