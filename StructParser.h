#pragma once

#include <string>
#include "util.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "cppparser/cppparser.h"
#include "util.h"




bool initParser(const std::string& filename = "AcPhysics.cpp");
const StructFieldMap& getStructFieldMap();