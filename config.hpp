#pragma once
#include <string>
#include "Exchange.hpp"
//#include "ExchangeHandler.hpp"

using namespace std;
typedef unsigned int uint;
typedef const string& cstring;

const uint maxNum = 999999999;
const double comission_taker = 0.001; // 0.1 %
const double comission_maker = 0.0009; // 0.09 %
std::map<uint, vector<std::string>> allFill;

const std::string orderExSim_ip_str = "127.0.0.1";
const char* orderExSim_ip = orderExSim_ip_str.c_str();
const int orderExSim_port = 8082;