#pragma once 
#include <vector>
#include <string>
//#include <fstream>
//#include <thread>
//#include <chrono>
//#include <cstdio>
//#include "stdlib.h"
#include <iostream> 
#include <algorithm>
#include <sstream>
#include <boost/filesystem.hpp>
#include <random>
//#include <Windows.h>

inline std::vector<std::string> splitStr(const std::string& data, const std::string& separator)
{
	std::vector<std::string> splitV;
	auto beginIterator = data.begin();
	while (beginIterator != data.end())
	{
		auto endLineIterator = std::find_first_of(beginIterator, data.end(), separator.begin(), separator.end());
		std::string targetStr;
		targetStr = std::string(beginIterator, endLineIterator);
		splitV.push_back(targetStr);
		if (endLineIterator == data.end()) break;
		beginIterator = endLineIterator + separator.size();
	}
	return splitV;
}
	
inline bool write_to_file(const std::string& file_data, const std::string& file_name,
	const std::ios_base::openmode& mode)
{
	std::ofstream file;
	file.open(file_name, mode);

	if (file.is_open())
	{
		file << file_data;
		file.close();
		return true;
	}
	else
	{
		std::cout << "Failed to open/create file: " << file_name << std::endl;
		return false;
	}
}

inline void writeLogToFile(const std::string& data1, const bool addn = 0) // , int& is_all_okay
{
	std::string configName = "events.log";
	std::string path = "";// "logs";
	std::stringstream filePath;

	if (path != "")
		filePath << path << "/" << configName;
	else
		filePath << configName;

	//if (!boost::filesystem::exists(filePath.str()) && path != "")
	//	boost::filesystem::create_directories(path);
	std::string data = data1;
	//std::vector<std::string> command = utils::splitStr(filePath, "_");
	//std::string command_type = command[3] + "_" + command[4];
	//std::cout << command[5];
	if (addn)
		data += "\n";
	write_to_file(data, filePath.str(), std::ios_base::app);
}
	
void slp()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

inline std::string allTypeCommand() // , int& is_all_okay
{
	std::string configName = "ev.log";
	std::string path = "";// "logs";
	std::stringstream filePath;

	filePath << path << "/" << configName;
	boost::filesystem::create_directories(path);
	return filePath.str();
}

double drandom(double x, double delta)
{
	double lower_bound = 0;
	double upper_bound = 10000;
	std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
	std::default_random_engine re;
	double a_random_double = unif(re);

	return 0;
}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double random(int x, int delta, int multi = 1)
{
	int a = x - delta;
	int b = x + delta;
	srand(time(NULL));
	//if (a > 0) return a + rand() % (b - a);
	//else return a + rand() % (abs(a) + b);
	return (a + rand() % (b - a))/multi;
}

int random()
{
	int a = 50;
	int b = 150;
	srand(time(NULL));
	if (a > 0) return a + rand() % (b - a);
	else return a + rand() % (abs(a) + b);
}

//void DataReadJustThat()
//{
//	std::string data;
//	std::ifstream file("exchange.txt");
//	while (getline(file, data)) 
//	{
//	std::cout << data << endl << endl;
//	ExchangeHandler(data);
//	cout << endl;
//	}
//	file.close();
//}

std::string slurp(std::ifstream& in) {
	std::ostringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

//void DataRead()
//{
//	string data;
//	ifstream file("exchange.txt");
//	std::ofstream ofs;
//
//		while (true)
//	{
//			
//		data = slurp(file);
//		cout << data << endl;
//		ofs.open("exchange.txt", std::ofstream::out | std::ofstream::trunc);
//
//		auto tmp = SimEx1::splitStr(data, "\n");
//		for (const auto& tmpp : tmp)
//		{
//			ExchangeHandler(tmpp);
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//	}
//}

void WriteDataToFile()
{
	std::ofstream out;
	out.open("exchange.txt");

	while (true)
	{
		if (out.is_open())
		{
			out << "create_order 5894 1 BTC-USDT ..." << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

//// Driver program for receiving data from buffer 
//string getData(tcp::socket& socket)
//{
//	boost::asio::streambuf buf;
//	read_until(socket, buf, "\n");
//	string data = buffer_cast<const char*>(buf.data());
//	return data;
//}
//
//// Driver program to send data 
//void sendData(tcp::socket& socket, const string& message)
//{
//	write(socket,
//		buffer(message + "\n"));
//}
