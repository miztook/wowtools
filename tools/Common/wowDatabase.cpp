#include "wowDatabase.h"
#include "wowEnvironment.h"
#include "CFileSystem.h"
#include "stringext.h"
#include "function.h"

#include "pugixml.hpp"

#define DATABASEFILE "database.xml"


wowDatabase::wowDatabase(const wowEnvironment* env)
	: Environment(env)
{
}

wowDatabase::~wowDatabase()
{
}

bool wowDatabase::init()
{
	if (!initFromXml())
		return false;

	return true;
}

const CTableStruct* wowDatabase::getTableStruct(const char* name) const
{
	auto itr = DbStructureMap.find(name);
	if (itr == DbStructureMap.end())
		return nullptr;
	return &itr->second;
}

bool wowDatabase::initFromXml()
{
	const CFileSystem* fs = Environment->getFileSystem();
	const auto& version = Environment->getVersion();

	std::string dir = fs->getDataDirectory();
	normalizeDirName(dir);
	dir += std_string_format("%d.%d", version[0], version[1]);
	normalizeDirName(dir);

	std::string xmlFile = dir + DATABASEFILE;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(xmlFile.c_str());
	if (result.status != pugi::status_ok)
		return false;

	auto root = doc.first_child();
	for (auto dbtable : root.children("table"))
	{
		CTableStruct tblStruct;

		tblStruct.name = dbtable.attribute("name").as_string();

		int fieldId = 0;
		for (auto dbfield : dbtable.children("field"))
		{
			CFieldStruct fieldStruct;
			fieldStruct.id = fieldId;

			auto attr_name = dbfield.attribute("name");
			auto attr_type = dbfield.attribute("type");
			auto attr_key = dbfield.attribute("primary");
			auto attr_arraySize = dbfield.attribute("arraySize");
			auto attr_index = dbfield.attribute("createIndex");
			auto attr_commonData = dbfield.attribute("commonData");
			auto attr_relationshipData = dbfield.attribute("relationshipData");

			if (!attr_name.empty() && !attr_type.empty())
			{
				fieldStruct.name = attr_name.as_string();
				fieldStruct.type = attr_type.as_string();

				fieldStruct.isKey = !attr_key.empty();
				fieldStruct.needIndex = !attr_index.empty();
				fieldStruct.arraySize = attr_arraySize.as_uint();

				fieldStruct.isCommonData = attr_commonData.as_bool();
				fieldStruct.isRelationshipData = attr_relationshipData.as_bool();

				//add
				tblStruct.fields.emplace_back(fieldStruct);
			}

			++fieldId;
		}

		DbStructureMap[tblStruct.name] = tblStruct;
	}
	return true;
}