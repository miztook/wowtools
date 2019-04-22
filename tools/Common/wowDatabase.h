#pragma once

#include "wowTable.h"
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

	const std::map<std::string, CTableStruct>& getDBStructMap() const { return DbStructureMap; }
	const CTableStruct* getDBStruct(const char* name) const;

	const DBFile* loadDBFile(const CTableStruct* table) const;

private:
	bool initFromXml();
	CMemFile* loadDBMemFile(const CTableStruct* table) const;
	bool loadAllTables();

public:
	//tables
	AnimationDataTable	m_AnimationDataTable;
	CharBaseSectionTable	m_CharBaseSectionTable;
	CharComponentTextureLayoutsTable	m_CharComponentTextureLayoutsTable;
	CharComponentTextureSectionsTable	m_CharComponentTextureSectionsTable;
	CharHairGeoSetsTable	m_CharHairGeoSetsTable;
	CharSectionsTable	m_CharSectionsTable;
	CharacterFacialHairStylesTable		m_CharacterFacialHairStylesTable;
	ChrClassesTable		m_ChrClassesTable;
	ChrCustomizationTable	m_ChrCustomizationTable;
	ChrRacesTable		m_ChrRacesTable;
	ComponentModelFileDataTable		m_ComponentModelFileDataTable;
	ComponentTextureFileDataTable	m_ComponentTextureFileDataTable;
	CreatureDisplayInfoTable	m_CreatureDisplayInfoTable;
	CreatureDisplayInfoExtraTable	m_CreatureDisplayInfoExtraTable;
	CreatureModelDataTable		m_CreatureModelDataTable;
	CreatureTypeTable	m_CreatureTypeTable;
	HelmetGeosetDataTable	m_HelmetGeosetDataTable;
	ItemTable	m_ItemTable;
	ItemAppearanceTable		m_ItemAppearanceTable;
	ItemClassTable		m_ItemClassTable;
	ItemDisplayInfoTable	m_ItemDisplayInfoTable;
	ItemDisplayInfoMaterialResTable		m_ItemDisplayInfoMaterialResTable;
	ItemModifiedAppearanceTable		m_ItemModifiedAppearanceTable;

private:
	const wowEnvironment*		Environment;
	std::map<std::string, CTableStruct>		DbStructureMap;

};