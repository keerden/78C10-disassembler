#ifndef REGISTERSET_H
#define REGISTERSET_H

#include <cstdint>
#include <string>
#include <map>
#include <list>

typedef std::map<uint8_t, std::string> regMap;

class RegisterSet
{
  private:
    std::map<std::string, regMap *> regset;

  public:
    ~RegisterSet();
    void add(regMap *r, std::string name);
    regMap *get(std::string name);
    bool load(std::string file);
    void print();
};

#endif