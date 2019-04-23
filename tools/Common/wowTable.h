#pragma once

#include <stdint.h>
#include <vector>
#include <map>
#include <functional>

#include "wowDbFile.h"
#include <cassert>

class wowDatabase;

#define BUILD_ID2INDEX_MAP(field)		\
for (uint32_t i = 0; i < (uint32_t)RecordList.size(); ++i)	\
{ Id2IndexMap[RecordList[i].##field] = i; }	

class BaseTable
{
public:
	bool hasKey() const { return !Id2IndexMap.empty(); }

	std::map<uint32_t, uint32_t> Id2IndexMap;

protected:
	bool loadData(const wowDatabase* database, const char* tableName);

	std::function<void (const std::vector<VAR_T>& val)> onLoadItem;
};

/*
AnimationData
CharBaseSection
CharComponentTextureLayouts
CharComponentTextureSections
CharHairGeoSets
CharSections
CharacterFacialHairStyles
ChrClasses
ChrCustomization
ChrRaces
ComponentModelFileData
ComponentTextureFileData
CreatureDisplayInfo
CreatureDisplayInfoExtra
CreatureModelData
CreatureType
HelmetGeosetData
Item
ItemAppearance
ItemClass
ItemDisplayInfo
ItemDisplayInfoMaterialRes
ItemModifiedAppearance
ItemSet
ItemSparse
ItemSubClass
ModelFileData
Mount
MountXDisplay
NpcModelItemSlotDisplayInfo
ParticleColor
TextureFileData


*/
class AnimationDataTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		std::string Name;
	};

	std::vector<SRecord>  RecordList;

public:
	AnimationDataTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 2);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Name = val[1].Get<std::string>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "AnimationData"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CharBaseSectionTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t LayoutResType;
		uint32_t VariationEnum;
		uint32_t ResolutionVariationEnum;
	};

	std::vector<SRecord>  RecordList;

public:
	CharBaseSectionTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 4);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.LayoutResType = val[1].Get<uint32_t>();
			r.VariationEnum = val[2].Get<uint32_t>();
			r.ResolutionVariationEnum = val[3].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CharBaseSection"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CharComponentTextureLayoutsTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t Width;
		uint16_t Height;
	};

	std::vector<SRecord>  RecordList;

public:
	CharComponentTextureLayoutsTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 3);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Width = val[1].Get<uint16_t>();
			r.Height = val[2].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CharComponentTextureLayouts"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CharComponentTextureSectionsTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t LayoutID;
		uint16_t Section;
		uint16_t X;
		uint16_t Y;
		uint16_t Width;
		uint16_t Height;
	};

	std::vector<SRecord>  RecordList;

public:
	CharComponentTextureSectionsTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 7);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.LayoutID = val[1].Get<uint16_t>();
			r.Section = val[2].Get<uint16_t>();
			r.X = val[3].Get<uint16_t>();
			r.Y = val[4].Get<uint16_t>();
			r.Width = val[5].Get<uint16_t>();
			r.Height = val[6].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CharComponentTextureSections"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CharHairGeoSetsTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t RaceID;
		uint16_t SexID;
		uint16_t VariationID;
		uint16_t VariationType;
		uint16_t GeoSetID;
		uint16_t GeoSetType;
		uint16_t ShowScalp;
		uint32_t ColorIndex;
	};

	std::vector<SRecord>  RecordList;

public:
	CharHairGeoSetsTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 9);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.RaceID = val[1].Get<uint16_t>();
			r.SexID = val[2].Get<uint16_t>();
			r.VariationID = val[3].Get<uint16_t>();
			r.VariationType = val[4].Get<uint16_t>();
			r.GeoSetID = val[5].Get<uint16_t>();
			r.GeoSetType = val[6].Get<uint16_t>();
			r.ShowScalp = val[7].Get<uint16_t>();
			r.ColorIndex = val[8].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CharHairGeoSets"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CharSectionsTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t RaceID;
		uint16_t SexID;
		uint16_t SectionType;
		uint32_t TextureName[3];
		uint16_t Flags;
		uint16_t VariationIndex;
		uint16_t ColorIndex;
	};

	std::vector<SRecord>  RecordList;

public:
	CharSectionsTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 10);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.RaceID = val[1].Get<uint16_t>();
			r.SexID = val[2].Get<uint16_t>();
			r.SectionType = val[3].Get<uint16_t>();
			for (uint32_t i = 0; i < 3; ++i)
			{
				r.TextureName[i] = val[4 + i].Get<uint32_t>();
			}
			r.Flags = val[7].Get<uint16_t>();
			r.VariationIndex = val[8].Get<uint16_t>();
			r.ColorIndex = val[9].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CharSections"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CharacterFacialHairStylesTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t RaceID;
		uint16_t SexID;
		uint16_t VariantID;
		uint32_t Geoset[5];
	};

	std::vector<SRecord>  RecordList;

public:
	CharacterFacialHairStylesTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 9);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.RaceID = val[1].Get<uint16_t>();
			r.SexID = val[2].Get<uint16_t>();
			r.VariantID = val[3].Get<uint16_t>();
			for (uint32_t k = 0; k < 5; ++k)
			{
				r.Geoset[k] = val[4 + k].Get<uint32_t>();
			}
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CharacterFacialHairStyles"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ChrClassesTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		std::string Name;
	};

	std::vector<SRecord>  RecordList;

public:
	ChrClassesTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 2);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Name = val[1].Get<std::string>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ChrClasses"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ChrCustomizationTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		std::string Name;
		uint32_t Field1;
		uint16_t Field2;
		uint16_t Field3;
		int Field4[3];
		uint32_t RaceId;
	};

	std::vector<SRecord>  RecordList;

public:
	ChrCustomizationTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 9);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Name = val[1].Get<std::string>();
			r.Field1 = val[2].Get<uint32_t>();
			r.Field2 = val[3].Get<uint16_t>();
			r.Field3 = val[4].Get<uint16_t>();
			for (uint32_t i = 0; i < 3; ++i)
			{
				r.Field4[i] = val[5 + i].Get<int>();
			}
			r.RaceId = val[8].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ChrCustomization"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ChrRacesTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t Flags;
		uint32_t MaleDisplayID;
		uint32_t FemaleDisplayID;
		uint16_t CharComponentTexLayoutID;
		std::string ClientPrefix;
		uint32_t HighResMaleDisplayId;
		uint32_t HighResFemaleDisplayId;
		uint16_t CharComponentTexLayoutHiResID;
		int BaseRaceID;
		int MaleModelFallbackRaceID;
		int FemaleModelFallbackRaceID;
		int MaleTextureFallbackRaceID;
		int FemaleTextureFallbackRaceID;
	};

	std::vector<SRecord>  RecordList;

public:
	ChrRacesTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 14);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Flags = val[1].Get<uint32_t>();
			r.MaleDisplayID = val[2].Get<uint32_t>();
			r.FemaleDisplayID = val[3].Get<uint32_t>();
			r.CharComponentTexLayoutID = val[4].Get<uint16_t>();
			r.ClientPrefix = val[5].Get<std::string>();
			r.HighResMaleDisplayId = val[6].Get<uint32_t>();
			r.HighResFemaleDisplayId = val[7].Get<uint32_t>();
			r.CharComponentTexLayoutHiResID = val[8].Get<uint16_t>();
			r.BaseRaceID = val[9].Get<int>();
			r.MaleModelFallbackRaceID = val[10].Get<int>();
			r.FemaleModelFallbackRaceID = val[11].Get<int>();
			r.MaleTextureFallbackRaceID = val[12].Get<int>();
			r.FemaleTextureFallbackRaceID = val[13].Get<int>();

			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ChrRaces"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ComponentModelFileDataTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t GenderIndex;
		uint16_t ClassID;
		uint16_t RaceID;
		uint16_t PositionIndex;
	};

	std::vector<SRecord>  RecordList;

public:
	ComponentModelFileDataTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 5);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.GenderIndex = val[1].Get<uint16_t>();
			r.ClassID = val[2].Get<uint16_t>();
			r.RaceID = val[3].Get<uint16_t>();
			r.PositionIndex = val[4].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ComponentModelFileData"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ComponentTextureFileDataTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t GenderIndex;
		uint16_t ClassID;
		uint16_t RaceID;
	};

	std::vector<SRecord>  RecordList;

public:
	ComponentTextureFileDataTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 4);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.GenderIndex = val[1].Get<uint16_t>();
			r.ClassID = val[2].Get<uint16_t>();
			r.RaceID = val[3].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ComponentTextureFileData"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CreatureDisplayInfoTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t ModelID;
		uint32_t ExtendedDisplayInfoID;
		uint32_t Texture[3];
		uint16_t ParticleColorID;
	};

	std::vector<SRecord>  RecordList;

public:
	CreatureDisplayInfoTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 7);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.ModelID = val[1].Get<uint16_t>();
			r.ExtendedDisplayInfoID = val[2].Get<uint32_t>();
			for (uint32_t i = 0; i < 3; ++i)
			{
				r.Texture[i] = val[3 + i].Get<uint32_t>();
			}
			r.ParticleColorID = val[6].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CreatureDisplayInfo"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CreatureDisplayInfoExtraTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t Skin;
		uint16_t Face;
		uint16_t HairStyle;
		uint16_t HairColor;
		uint16_t FacialHair;
	};

	std::vector<SRecord>  RecordList;

public:
	CreatureDisplayInfoExtraTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 6);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Skin = val[1].Get<uint16_t>();
			r.Face = val[2].Get<uint16_t>();
			r.HairStyle = val[3].Get<uint16_t>();
			r.HairColor = val[4].Get<uint16_t>();
			r.FacialHair = val[5].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CreatureDisplayInfoExtra"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CreatureModelDataTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t FileID;
	};

	std::vector<SRecord>  RecordList;

public:
	CreatureModelDataTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 2);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.FileID = val[1].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CreatureModelData"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class CreatureTypeTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		std::string Name;
		uint16_t Flags;
	};

	std::vector<SRecord>  RecordList;

public:
	CreatureTypeTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 3);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Name = val[1].Get<std::string>();
			r.Flags = val[2].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "CreatureType"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class HelmetGeosetDataTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		int RaceID;
		uint16_t GeosetGroup;
		uint32_t GeosetVisDataID;
	};

	std::vector<SRecord>  RecordList;

public:
	HelmetGeosetDataTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 4);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.RaceID = val[1].Get<int>();
			r.GeosetGroup = val[2].Get<uint16_t>();
			r.GeosetVisDataID = val[3].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "HelmetGeosetData"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint16_t Class;
		uint16_t SubClass;
		uint16_t Type;
		uint16_t Sheath;
	};

	std::vector<SRecord>  RecordList;

public:
	ItemTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 5);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Class = val[1].Get<uint16_t>();
			r.SubClass = val[2].Get<uint16_t>();
			r.Type = val[3].Get<uint16_t>();
			r.Sheath = val[4].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "Item"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemAppearanceTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t ItemDisplayInfoID;
	};

	std::vector<SRecord>  RecordList;

public:
	ItemAppearanceTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 2);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.ItemDisplayInfoID = val[1].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ItemAppearance"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemClassTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t Col0;
		std::string Name;
		uint32_t ID;
	};

	std::vector<SRecord>  RecordList;

public:
	ItemClassTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 3);
			SRecord r;
			r.Col0 = val[0].Get<uint32_t>();
			r.Name = val[1].Get<std::string>();
			r.ID = val[2].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ItemClass"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemDisplayInfoTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t Model[2];
		uint32_t TextureItemID[2];
		uint16_t GeosetGroup[6];
		uint32_t ParticleColorID;
		uint32_t HelmetGeosetVis[2];
	};

	std::vector<SRecord>  RecordList;

public:
	ItemDisplayInfoTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 14);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			for (uint32_t i = 0; i < 2; ++i)
			{
				r.Model[i] = val[1+i].Get<uint32_t>();
			}
			for (uint32_t i = 0; i < 2; ++i)
			{
				r.TextureItemID[i] = val[3+i].Get<uint32_t>();
			}			
			for (uint32_t i = 0; i < 6; ++i)
			{
				r.GeosetGroup[i] = val[5+i].Get<uint16_t>();
			}
			r.ParticleColorID = val[11].Get<uint32_t>();
			for (uint32_t i = 0; i < 2; ++i)
			{
				r.HelmetGeosetVis[i] = val[12+i].Get<uint32_t>();
			}
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ItemDisplayInfo"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemDisplayInfoMaterialResTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t ItemDisplayInfoID;
		uint32_t TextureFileDataID;
	};

	std::vector<SRecord>  RecordList;

public:
	ItemDisplayInfoMaterialResTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 3);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.ItemDisplayInfoID = val[1].Get<uint32_t>();
			r.TextureFileDataID = val[2].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ItemDisplayInfoMaterialRes"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemModifiedAppearanceTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t ItemID;
		uint32_t ItemAppearanceID;
		uint16_t ItemLevel;
	};

	std::vector<SRecord>  RecordList;

public:
	ItemModifiedAppearanceTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 4);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.ItemID = val[1].Get<uint32_t>();
			r.ItemAppearanceID = val[2].Get<uint32_t>();
			r.ItemLevel = val[3].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ItemModifiedAppearance"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemSetTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		std::string Name;
		int Item[17];
	};

	std::vector<SRecord>  RecordList;

public:
	ItemSetTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 19);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Name = val[1].Get<std::string>();
			for (uint32_t i = 0; i < 17; ++i)
			{
				r.Item[i] = val[2 + i].Get<int>();
			}
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ItemSet"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemSparseTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint64_t AllowableRace;
		std::string Description;
		std::string DisplayLang3;
		std::string DisplayLang2;
		std::string DisplayLang1;
		std::string Name;
	};

	std::vector<SRecord>  RecordList;

public:
	ItemSparseTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 7);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.AllowableRace = val[1].Get<uint64_t>();
			r.Description = val[2].Get<std::string>();
			r.DisplayLang3 = val[3].Get<std::string>();
			r.DisplayLang2 = val[4].Get<std::string>();
			r.DisplayLang1 = val[5].Get<std::string>();
			r.Name = val[6].Get<std::string>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ItemSparse"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ItemSubClassTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t Col0;
		uint16_t ID;
		uint16_t SubClassID;
		std::string Name;
		std::string VerboseName;
	};

	std::vector<SRecord>  RecordList;

public:
	ItemSubClassTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 5);
			SRecord r;
			r.Col0 = val[0].Get<uint32_t>();
			r.ID = val[1].Get<uint16_t>();
			r.SubClassID = val[2].Get<uint16_t>();
			r.Name = val[3].Get<std::string>();
			r.VerboseName = val[4].Get<std::string>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ItemSubClass"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ModelFileDataTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ModelID;
		uint32_t ID;
	};

	std::vector<SRecord>  RecordList;

public:
	ModelFileDataTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 2);
			SRecord r;
			r.ModelID = val[0].Get<uint32_t>();
			r.ID = val[1].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ModelFileData"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class MountTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		std::string Name;
	};

	std::vector<SRecord>  RecordList;

public:
	MountTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 2);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.Name = val[1].Get<std::string>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "Mount"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class MountXDisplayTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t MountID;
		uint32_t DisplayID;
	};

	std::vector<SRecord>  RecordList;

public:
	MountXDisplayTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 3);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.MountID = val[1].Get<uint32_t>();
			r.DisplayID = val[2].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "MountXDisplay"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class NpcModelItemSlotDisplayInfoTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		uint32_t CreatureDisplayInfoExtraID;
		uint32_t ItemDisplayInfoID;
		uint16_t ItemType;
	};

	std::vector<SRecord>  RecordList;

public:
	NpcModelItemSlotDisplayInfoTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 4);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			r.CreatureDisplayInfoExtraID = val[1].Get<uint32_t>();
			r.ItemDisplayInfoID = val[2].Get<uint32_t>();
			r.ItemType = val[3].Get<uint16_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "NpcModelItemSlotDisplayInfo"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class ParticleColorTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t ID;
		int StartColor[3];
		int MidColor[3];
		int EndColor[3];
	};

	std::vector<SRecord>  RecordList;

public:
	ParticleColorTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 10);
			SRecord r;
			r.ID = val[0].Get<uint32_t>();
			for (uint32_t i = 0; i < 3; ++i)
			{
				r.StartColor[i] = val[1+i].Get<int>();
			}
			for (uint32_t i = 0; i < 3; ++i)
			{
				r.MidColor[i] = val[4+i].Get<int>();
			}
			for (uint32_t i = 0; i < 3; ++i)
			{
				r.EndColor[i] = val[7+i].Get<int>();
			}
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "ParticleColor"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};

class TextureFileDataTable : public BaseTable
{
public:
	struct SRecord
	{
		uint32_t TextureID;
		uint32_t ID;
	};

	std::vector<SRecord>  RecordList;

public:
	TextureFileDataTable()
	{
		onLoadItem = [this](const std::vector<VAR_T>& val)
		{
			assert(val.size() == 2);
			SRecord r;
			r.TextureID = val[0].Get<uint32_t>();
			r.ID = val[1].Get<uint32_t>();
			RecordList.emplace_back(r);
		};
	}

public:
	bool loadData(const wowDatabase* database)
	{
		if (!BaseTable::loadData(database, "TextureFileData"))
			return false;
		BUILD_ID2INDEX_MAP(ID);
		return true;
	}
};