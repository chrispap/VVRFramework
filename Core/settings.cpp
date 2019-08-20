#include <vvr/settings.h>
#include <string>
#include <map>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstring>

#define MAX_LINE_LEN 1024

using namespace vvr;
using namespace std;

static char* trimLeft(char* s)
{
    while (isspace(*s)) s++;
    return s;
}

static char* trimRight(char* s)
{
    int len = std::strlen(s);
    if (!len) return s;
    char* pos = s + len - 1;
    while (pos >= s && isspace(*pos)) {
        *pos = '\0';
        pos--;
    }
    return s;
}

static char* trim(char* s)
{
    return trimRight(trimLeft(s));
}

Settings::Settings()
{
}

Settings::Settings(string path) : m_path(path)
{
    reload();
}

void Settings::reload()
{
    FILE *file = fopen(m_path.c_str(), "r");
    if (!file) throw string("Cannot open config file <") + m_path + ">";

    char Line[MAX_LINE_LEN], key[128], val[MAX_LINE_LEN - 128];
    while (fgets(Line, MAX_LINE_LEN, file)) {
        if (Line[0] == '#') continue;
        if (sscanf(Line, "%[^=]=%[^\n]", key, val) == 2) {
            transform(key, key + std::strlen(key), key, ::tolower);
            char *k = trim(key);
            char *v = trim(val);
            m_map[k] = trim(v);
        }
    }

    fclose(file);
}

void Settings::merge(const Settings& other)
{
    //! Keeps original vals in key-conflict.
    m_map.insert(other.m_map.begin(), other.m_map.end());
}

void Settings::getKeys(vector<string> &keys) const
{
    for (map<string, string>::const_iterator it = m_map.begin(); it != m_map.end(); ++it) {
        keys.push_back(it->first);
    }
}

void Settings::write(const string &filename) const
{
    FILE *file;
    file = fopen(filename.c_str(), "w");

    for (map<string, string>::const_iterator it = m_map.begin(); it != m_map.end(); ++it) {
        fprintf(file, "%s = %s\n", it->first.c_str(), it->second.c_str());
    }

    fclose(file);
}

void Settings::set(const string &Key, const string &newVal, bool create_if_not_exist)
{
    string key(Key);
    transform(key.begin(), key.end(), key.begin(), ::tolower);
    if (m_map.find(key) == m_map.end() && create_if_not_exist == false) {
        throw string("Setting <") + key + "> not in map.";
    }
    m_map[key] = newVal;
}

bool Settings::exists(const string &key) const
{
    return m_map.count(key);
}

string Settings::getStr(const string &Key) const
{
    string key(Key);
    transform(key.begin(), key.end(), key.begin(), ::tolower);
    if (m_map.find(key) == m_map.end()) {
        throw string("Setting <") + key + "> not in map.";
    }
    return m_map.at(key);
}

double Settings::getDbl(const string &key) const
{
    return atof(getStr(key).c_str());
}

bool Settings::getBool(const string &key) const
{
    return getInt(key);
}

int Settings::getInt(const string &key) const
{
    return atoi(getStr(key).c_str());
}
