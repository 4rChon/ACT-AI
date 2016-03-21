#pragma once
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "BWAPI.h"
#include "BWTA.h"
#include "boost\filesystem.hpp"

namespace std
{
	template<> struct hash < BWAPI::UnitCommandType >
	{
		size_t operator()(const BWAPI::UnitCommandType &a) const
		{
			return (51 * (std::hash<std::string>()(a.toString())));
		}
	};

	template<> struct equal_to < BWAPI::UnitCommandType >
	{
		bool operator()(const BWAPI::UnitCommandType &lhs, const BWAPI::UnitCommandType &rhs) const
		{
			return lhs.getID() == rhs.getID();
		}
	};

	template<> struct hash < BWAPI::UnitType >
	{
		size_t operator()(const BWAPI::UnitType &a) const
		{
			return (51 * (std::hash<std::string>()(a.toString())));
		}
	};

	template<> struct equal_to < BWAPI::UnitType >
	{
		bool operator()(const BWAPI::UnitType &lhs, const BWAPI::UnitType &rhs) const
		{
			return lhs.getID() == rhs.getID();
		}
	};

	template<> struct hash < BWAPI::UpgradeType >
	{
		size_t operator()(const BWAPI::UpgradeType &a) const
		{
			return (51 * (std::hash<std::string>()(a.toString())));
		}
	};

	template<> struct equal_to < BWAPI::UpgradeType >
	{
		bool operator()(const BWAPI::UpgradeType &lhs, const BWAPI::UpgradeType &rhs) const
		{
			return lhs.getID() == rhs.getID();
		}
	};

	template<> struct hash < BWAPI::TechType >
	{
		size_t operator()(const BWAPI::TechType &a) const
		{
			return (51 * (std::hash<std::string>()(a.toString())));
		}
	};

	template<> struct equal_to < BWAPI::TechType >
	{
		bool operator()(const BWAPI::TechType &lhs, const BWAPI::TechType &rhs) const
		{
			return lhs.getID() == rhs.getID();
		}
	};
	//template<> struct hash < BWTA::BaseLocation* >
	//{
	//	size_t operator()(const BWAPI::TechType &a) const
	//	{
	//		return (51 * (std::hash<std::string>()(a.toString())));
	//	}
	//};

	//template<> struct equal_to < BWTA::BaseLocation* >
	//{
	//	bool operator()(const BWTA::BaseLocation &lhs, const BWTA::BaseLocation &rhs) const
	//	{
	//		return lhs.getPosition() == rhs.getPosition();
	//	}
	//};
}

namespace util
{
	namespace fs = ::boost::filesystem;

	//Normalises an array of values after weighing them with coefficients
	double normaliseValues(std::vector<double> valueArr, std::vector<double> coeffArr);

	//Get all files in root path with ext extension and places them in ret
	void getFiles(const fs::path& root, const std::string& ext, std::vector<fs::path>& ret);
	//void serialise();
	//void deserialise();
}