#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "aabb.h"



void Write(std::vector<AABB>& objects, std::string filename);
std::vector<AABB> Read(std::string filename);


