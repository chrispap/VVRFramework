#ifndef VVR_SETTINGS_H
#define VVR_SETTINGS_H

#include "vvrframework_DLL.h"
#include <string>
#include <map>
#include <vector>

namespace vvr {

class VVRFramework_API Settings
{
    std::string m_path;
    std::map<std::string, std::string> m_map;

public:
    Settings();
    Settings(std::string file_path);

    void reload();
    const std::string& filePath() const { return m_path; }
    void merge(const Settings &other);
    void getKeys(std::vector<std::string> &keys) const;
    void write(const std::string &filename) const;
    void set(const std::string &Key, const std::string &newVal, bool create_if_not_exist=false);
    bool exists(const std::string &key) const;

    //! Get values
    std::string getStr(const std::string &key) const;
    double getDbl(const std::string &key) const;
    int getInt(const std::string &key) const;
    bool getBool(const std::string &key) const;
};

}

#endif
