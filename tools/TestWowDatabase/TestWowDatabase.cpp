#include <crtdbg.h>
#include <stdio.h>
#include <regex>
#include <iostream>
#include <list>

#include "CFileSystem.h"
#include "wowEnvironment.h"
#include "wowDatabase.h"
#include "function.h"
#include "stringext.h"

#include "wowDbFile.h"

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testWowDatabase83();
void dumpWowDatabase(CFileSystem* fs, const wowDatabase* wowDB);
void testWowDatabaseClassic();

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	testWowDatabase83();
	//testWowDatabase81();
	//testWowDatabaseClassic();

	getchar();
	return 0;
}

void testWowDatabase83()
{
	CFileSystem* fs = new CFileSystem(R"(E:\World Of Warcraft)");
	wowEnvironment* wowEnv = new wowEnvironment(fs);
	wowDatabase* wowDB = new wowDatabase(wowEnv);

	if (!wowEnv->init("wow"))
		printf("init fail!\n");
	else
		printf("wowEnv init success! %s, %s, %s\n", wowEnv->getProduct(), wowEnv->getLocale(), wowEnv->getVersionString());

	if (!wowEnv->loadCascListFiles())
		printf("listfile fail!\n");
	else
		printf("listfile success!\n");


	if (!wowDB->init())
	{
		printf("wowDB init fail!\n");
	}
	else
	{
		printf("wowDB init success!\n");
		dumpWowDatabase(fs, wowDB);
	}

	delete wowDB;
	delete wowEnv;
	delete fs;
}

void dumpWowDatabase(CFileSystem* fs, const wowDatabase* wowDB)
{
	std::string dir = fs->getWorkingDirectory();
	normalizeDirName(dir);

	dir += "Database/";
	Q_MakeDirForFileName(dir.c_str());

	//CharBaseSection
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CharBaseSection.txt").c_str(), false);
		for (const auto& r : wowDB->m_CharBaseSectionTable.RecordList)
		{
			wf->writeLine("ID: %u, LayoutResType: %u, VariationEnum: %u, ResolutionVariationEnum: %u",
				r.ID, r.LayoutResType, r.VariationEnum, r.ResolutionVariationEnum);
		}
		delete wf;
	}

	//CharComponentTextureLayouts
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CharComponentTextureLayouts.txt").c_str(), false);
		for (const auto& r : wowDB->m_CharComponentTextureLayoutsTable.RecordList)
		{
			wf->writeLine("ID: %u, Width: %d, Height: %d",
				r.ID, r.Width, r.Height);
		}
		delete wf;
	}

	//CharComponentTextureSections
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CharComponentTextureSections.txt").c_str(), false);
		for (const auto& r : wowDB->m_CharComponentTextureSectionsTable.RecordList)
		{
			wf->writeLine("ID: %u, LayoutID: %d, Section: %d, X: %d, Y: %d, Width: %d, Height: %d",
				r.ID, r.LayoutID, r.Section, r.X, r.Y, r.Width, r.Height);
		}
		delete wf;
	}

	//CharHairGeoSets
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CharHairGeoSets.txt").c_str(), false);
		for (const auto& r : wowDB->m_CharHairGeoSetsTable.RecordList)
		{
			wf->writeLine("ID: %u, RaceID: %d, SexID: %d, VariationID: %d, VariationType: %d, GeoSetID: %d, GeosetType: %d, ShowScalp: %d, ColorIndex: %u",
				r.ID, r.RaceID, r.SexID, r.VariationID, r.VariationType, r.GeoSetID, r.GeoSetType, r.ShowScalp, r.ColorIndex);
		}
		delete wf;
	}

	//CharSections
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CharSections.txt").c_str(), false);
		for (const auto& r : wowDB->m_CharSectionsTable.RecordList)
		{
			wf->writeLine("ID: %u, RaceID: %d, SexID: %d, SectionType: %d, TextureName[0]: %u, TextureName[1]: %u, TextureName[2]: %u, TextureName[3]: %u, Flags: %d, VariationIndex: %d, ColorIndex: %d",
				r.ID, r.RaceID, r.SexID, r.SectionType, r.TextureName[0], r.TextureName[1], r.TextureName[2], r.TextureName[3], r.Flags, r.VariationIndex, r.ColorIndex);
		}
		delete wf;
	}

	//CharacterFacialHairStyles
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CharacterFacialHairStyles.txt").c_str(), false);
		for (const auto& r : wowDB->m_CharacterFacialHairStylesTable.RecordList)
		{
			wf->writeLine("ID: %u, Geoset[0]: %d, Geoset[1]: %d, Geoset[2]: %d,Geoset[3]: %d, Geoset[4]: %d, RaceID: %d, SexID: %d, VariationID: %d",
				r.ID, r.Geoset[0], r.Geoset[1], r.Geoset[2], r.Geoset[3], r.Geoset[4], r.RaceID, r.SexID, r.VariationID);
		}
		delete wf;
	}

	//ChrClasses
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ChrClasses.txt").c_str(), false);
		for (const auto& r : wowDB->m_ChrClassesTable.RecordList)
		{
			wf->writeLine("ID: %u, Name: %s",
				r.ID, r.Name.c_str());
		}
		delete wf;
	}

	//ChrCustomization
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ChrCustomization.txt").c_str(), false);
		for (const auto& r : wowDB->m_ChrCustomizationTable.RecordList)
		{
			wf->writeLine("ID: %u, Name: %s, RaceID: %u",
				r.ID, r.Name.c_str(), r.RaceId);
		}
		delete wf;
	}

	//ChrRaces
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ChrRaces.txt").c_str(), false);
		for (const auto& r : wowDB->m_ChrRacesTable.RecordList)
		{
			wf->writeLine(
"ID: %u, Flags: %u, MaleDisaplayID: %u, FemaleDispalyID: %u, HighResMaleDisplayId: %u, HighResFemaleDisplayId: %u, CharComponentTexLayoutID: %d, ClientPrefix: %s, CharComponentTexLayoutHiResID: %d, BaseRaceID: %d",
				r.ID, r.Flags, r.MaleDisplayID, r.FemaleDisplayID, r.HighResMaleDisplayID, r.HighResFemaleDisplayID, r.CharComponentTexLayoutID, r.ClientPrefix.c_str(), r.CharComponentTexLayoutHiResID, r.BaseRaceID);
		}
		delete wf;
	}

	//ComponentModelFileData
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ComponentModelFileData.txt").c_str(), false);
		for (const auto& r : wowDB->m_ComponentModelFileDataTable.RecordList)
		{
			wf->writeLine("ID: %u, GenderIndex: %d, ClassID: %d, RaceID: %d, PositionIndex: %d",
				r.ID, r.GenderIndex, r.ClassID, r.RaceID, r.PositionIndex);
		}
		delete wf;
	}

	//ComponentTextureFileData
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ComponentTextureFileData.txt").c_str(), false);
		for (const auto& r : wowDB->m_ComponentTextureFileDataTable.RecordList)
		{
			wf->writeLine("ID: %u, GenderIndex: %d, ClassID: %d, RaceID: %d",
				r.ID, r.GenderIndex, r.ClassID, r.RaceID);
		}
		delete wf;
	}

	//CreatureDisplayInfo
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CreatureDisplayInfo.txt").c_str(), false);
		for (const auto& r : wowDB->m_CreatureDisplayInfoTable.RecordList)
		{
			wf->writeLine("ID: %u, ModelID: %d, ExtendedDisplayInfoID: %u, Texture[0]: %u, Texture[1]: %u, Texture[2]: %u, ParticleColorID: %d",
				r.ID, r.ModelID, r.ExtendedDisplayInfoID, r.Texture[0], r.Texture[1], r.Texture[2], r.ParticleColorID);
		}
		delete wf;
	}

	//CreatureDisplayInfoExtra
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CreatureDisplayInfoExtra.txt").c_str(), false);
		for (const auto& r : wowDB->m_CreatureDisplayInfoExtraTable.RecordList)
		{
			wf->writeLine("ID: %u, Skin: %d, Face: %d, HairStyle: %d, HairColor: %d, FacialHair: %d",
				r.ID, r.Skin, r.Face, r.HairStyle, r.HairColor, r.FacialHair);
		}
		delete wf;
	}

	//CreatureModelData
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CreatureModelData.txt").c_str(), false);
		for (const auto& r : wowDB->m_CreatureModelDataTable.RecordList)
		{
			wf->writeLine("ID: %u, FileID: %u",
				r.ID, r.FileID);
		}
		delete wf;
	}

	//CreatureType
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "CreatureType.txt").c_str(), false);
		for (const auto& r : wowDB->m_CreatureTypeTable.RecordList)
		{
			wf->writeLine("ID: %u, Name: %s, Flags: %d",
				r.ID, r.Name.c_str(), r.Flags);
		}
		delete wf;
	}

	//HelmetGeosetData
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "HelmetGeosetData.txt").c_str(), false);
		for (const auto& r : wowDB->m_HelmetGeosetDataTable.RecordList)
		{
			wf->writeLine("ID: %u, RaceID: %d, GeosetGroup: %d, GeosetVisDataID: %d",
				r.ID, r.RaceID, r.GeosetGroup, r.GeosetVisDataID);
		}
		delete wf;
	}

	//Item
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "Item.txt").c_str(), false);
		for (const auto& r : wowDB->m_ItemTable.RecordList)
		{
			wf->writeLine("ID: %u, Class: %d, SubClass: %d, Type: %d, Sheath: %d",
				r.ID, r.Class, r.SubClass, r.Type, r.Sheath);
		}
		delete wf;
	}

	//ItemAppearance
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ItemAppearance.txt").c_str(), false);
		for (const auto& r : wowDB->m_ItemAppearanceTable.RecordList)
		{
			wf->writeLine("ID: %u, ItemDisplayInfoID: %u",
				r.ID, r.ItemDisplayInfoID);
		}
		delete wf;
	}

	//ItemClass
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ItemClass.txt").c_str(), false);
		for (const auto& r : wowDB->m_ItemClassTable.RecordList)
		{
			wf->writeLine("ID: %u, Name: %s, Col0: %d",
				r.ID, r.Name.c_str(), r.Col0);
		}
		delete wf;
	}

	//ItemDisplayInfo
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ItemDisplayInfo.txt").c_str(), false);
		for (const auto& r : wowDB->m_ItemDisplayInfoTable.RecordList)
		{
			wf->writeLine("ID: %u, Model0: %u, Model1: %u, TextureItemID0: %u, TextureItemID1: %u, HelmetGeosetVis0: %u, HelmetGeosetVis1: %u",
				r.ID, r.Model[0], r.Model[1], r.TextureItemID[0], r.TextureItemID[1], r.HelmetGeosetVis[0], r.HelmetGeosetVis[1]);
		}
		delete wf;
	}

	//ItemDisplayInfoMaterialRes
	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "ItemDisplayInfoMaterialRes.txt").c_str(), false);
		for (const auto& r : wowDB->m_ItemDisplayInfoMaterialResTable.RecordList)
		{
			wf->writeLine("ID: %u, ItemDisplayInfoID: %u, TextureFileDataID: %u",
				r.ID, r.ItemDisplayInfoID, r.TextureFileDataID);
		}
		delete wf;
	}
}

void testWowDatabaseClassic()
{
	CFileSystem* fs = new CFileSystem(R"(E:\World Of Warcraft)");
	wowEnvironment* wowEnv = new wowEnvironment(fs);
	wowDatabase* wowDB = new wowDatabase(wowEnv);

	if (!wowEnv->init("wow_classic"))
		printf("init fail!\n");
	else
		printf("wowEnv init success! %s, %s, %s\n", wowEnv->getProduct(), wowEnv->getLocale(), wowEnv->getVersionString());

	if (!wowEnv->loadCascListFiles())
		printf("listfile fail!\n");
	else
		printf("listfile success!\n");

	
	const DBFile* file = wowDB->loadDBFile("CharSections");
	if (file)
	{
		printf("load dbfile success!\n");
		delete file;
	}
	else
	{
		printf("load dbfile fail!\n");
	}

	delete wowDB;
	delete wowEnv;
	delete fs;
}