#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <array>

class wowEnvironment;

class CFieldStruct
{
public:
	CFieldStruct() :
		isKey(false), needIndex(false)
		, arraySize(1), id(0)
		, isCommonData(false), isRelationshipData(false)
	{}

public:
	std::string name;
	std::string type;
	uint32_t arraySize;
	int id;
	bool isKey;
	bool needIndex;

	int pos;
	bool isCommonData;
	bool isRelationshipData;
};

class CTableStruct
{
public:
	std::string name;
	std::vector<CFieldStruct> fields;
};

class wowDatabase
{
public:
	explicit wowDatabase(const wowEnvironment* env);
	~wowDatabase();

public:
	bool init();

	const CTableStruct* getTableStruct(const char* name) const;

private:
	bool initFromXml();

private:
	const wowEnvironment*		Environment;
	std::map<std::string, CTableStruct> DbStructureMap;
};