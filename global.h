#ifndef GLOBAL_H
#define GLOBAL_H
#include <string>
#include <filesystem>
#include <iostream>
#include <map>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include "stable-diffusion/stable-diffusion.h"

class Global
{
public:
    Global();
    static Global& Instance();

    void Init();
    void SDVersion();
    int testSD(const std::string& prompt, const std::string& modelPath, const std::string& outputImagePath);
public:
};

#endif // GLOBAL_H
