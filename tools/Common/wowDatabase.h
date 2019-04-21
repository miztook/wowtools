#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <array>

class wowEnvironment;
class CMemFile;
class DBFile;

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
	~wowDatabase() = default;

public:
	bool init();

	CMemFile* loadDBMemFile(const CTableStruct* table) const;

	const std::map<std::string, CTableStruct>& getDBStructMap() const { return DbStructureMap; }

	const DBFile* loadDBFile(const CTableStruct* table) const;

private:
	bool initFromXml();

private:
	const wowEnvironment*		Environment;
	std::map<std::string, CTableStruct>		DbStructureMap;
};