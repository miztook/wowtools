#include "wowWMOFile.h"

#include "CMemFile.h"
#include "function.h"

#include "wowEnvironment.h"

wowWMOFile::wowWMOFile(const wowEnvironment* wowEnv)
	: WowEnvironment(wowEnv)
{
}

wowWMOFile::~wowWMOFile()
{
}

bool wowWMOFile::loadFile(const char* filename)
{
	CMemFile* memFile = WowEnvironment->openFile(filename);
	if (!memFile)
		return false;

	char tmp[QMAX_PATH];
	const char* name = memFile->getFileName();
	getFullFileNameNoExtensionA(name, tmp, QMAX_PATH);
	Name = tmp;

	char fourcc[5];
	
	uint32_t size;
	while (!memFile->isEof())
	{
		memFile->read(fourcc, 4);
		memFile->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		uint32_t nextpos = memFile->getPos() + size;

		if (strcmp(fourcc, "MVER") == 0)				//version
		{
			uint32_t version;
			memFile->read(&version, size);
			assert(version == 17);
		}
		else if (strcmp(fourcc, "MOHD") == 0)
		{
			assert(size == sizeof(Header));
			memFile->read(&Header, sizeof(Header));

			MaterialList.resize(Header.nMaterials);
			GroupList.resize(Header.nGroups);
			LightList.resize(Header.nLights);
			DoodadSets.resize(Header.nDoodadSets);
		}
		else if (strcmp(fourcc, "MOTX") == 0)
		{
			TextureFileNameBlock.resize(size);
			memFile->read(TextureFileNameBlock.data(), size);
		}
		else if (strcmp(fourcc, "MOMT") == 0)
		{
			assert(size == Header.nMaterials * sizeof(WMO::wmoMaterial));
			MaterialList.resize(Header.nMaterials);
			for (uint32_t i = 0; i < Header.nMaterials; ++i)
			{
				WMO::wmoMaterial m;
				memFile->read(&m, sizeof(WMO::wmoMaterial));

				MaterialList[i].flags = m.flags;
				MaterialList[i].shaderType = (E_WMO_SHADER)m.shadertype;
				MaterialList[i].alphatest = m.alphatest != 0;

				MaterialList[i].color0.set(m.col1A, m.col1R, m.col1G, m.col1B);
				MaterialList[i].tex1FileId = m.tex1;
				MaterialList[i].color1.set(m.col2A, m.col2R, m.col2G, m.col2B);
				MaterialList[i].tex2FileId = m.tex2;

				MaterialList[i].color2.set(m.col3A, m.col3R, m.col3G, m.col3B);
			}
		}
		else if (strcmp(fourcc, "MOGN") == 0)
		{
			GroupNameBlock.resize(size);
			memFile->read(GroupNameBlock.data(), size);
		}
		else if (strcmp(fourcc, "MOGI") == 0)
		{
			GroupList.resize(Header.nGroups);
			for (uint32_t i = 0; i < Header.nGroups; ++i)
			{
				WMO::wmoGroupInfo g;
				memFile->read(&g, sizeof(WMO::wmoGroupInfo));
				GroupList[i].index = i;
				GroupList[i].flags = g.flags;
				GroupList[i].box.setByMinMax(WMO::fixCoordinate(g.min), WMO::fixCoordinate(g.max));
				if (g.nameIndex >= 0 && g.nameIndex < (int32_t)GroupNameBlock.size())
					GroupList[i].name = (const char*)&GroupNameBlock[g.nameIndex];
			}
		}
		else if (strcmp(fourcc, "MOSB") == 0)
		{

		}
		else if (strcmp(fourcc, "MOPV") == 0)
		{

		}
		else if (strcmp(fourcc, "MOPT") == 0)
		{

		}
		else if (strcmp(fourcc, "MOPR") == 0)
		{

		}
		else if (strcmp(fourcc, "MOLT") == 0)
		{
			assert(size == Header.nLights * sizeof(WMO::wmoLight));
			LightList.resize(Header.nLights);
			for (uint32_t i = 0; i < Header.nLights; ++i)
			{
				WMO::wmoLight li;
				memFile->read(&li, sizeof(WMO::wmoLight));

				LightList[i].lighttype = (E_LIGHT_TYPE)li.lighttype;
				LightList[i].color.set(li.colA, li.colR, li.colG, li.colB);
				LightList[i].position = WMO::fixCoordinate(li.pos);
				LightList[i].intensity = li.intensity;
				LightList[i].attenStart = li.attenStart;
				LightList[i].attenEnd = li.attenEnd;
			}
		}
		else if (strcmp(fourcc, "MODS") == 0)
		{

		}
		else if (strcmp(fourcc, "MODN") == 0)
		{

		}
		else if (strcmp(fourcc, "MODD") == 0)
		{

		}
		else if (strcmp(fourcc, "MFOG") == 0)
		{
			uint32_t nFog = size / sizeof(WMO::wmoFog);
			FogList.resize(nFog);
			for (uint32_t i = 0; i < nFog; ++i)
			{
				WMO::wmoFog f;
				memFile->read(&f, sizeof(WMO::wmoFog));

				FogList[i].position = WMO::fixCoordinate(f.pos);
				FogList[i].radius1 = f.r1;
				FogList[i].radius2 = f.r2;
				FogList[i].fogstart = f.fogstart;
				FogList[i].fogend = f.fogend;
				FogList[i].color.set(f.colA, f.colR, f.colG, f.colB);
			}
		}
		else if (strcmp(fourcc, "MOGP") == 0)
		{

		}
		else if (strcmp(fourcc, "MCVP") == 0)
		{

		}
		else if (strcmp(fourcc, "GFID") == 0)
		{

		}
		else if (strcmp(fourcc, "MODI") == 0)
		{

		}
		else
		{
			assert(false);
		}

		memFile->seek((int32_t)nextpos);
	}

	//load groups and bounding box
	Box.clear();
	for (SWMOGroup& group : GroupList)
	{
		if (!loadGroupFile(memFile, group))
		{
			assert(false);
			return false;
		}
		Box.addInternalBox(group.box);

		//group box
		for (auto& batch : group.batchList)
		{
			batch.box.clear();
			for (uint32_t k = batch.vertexStart; k <= batch.vertexEnd; ++k)
			{
				batch.box.addInternalPoint(group.vertices[k].Pos);
			}
		}
	}

	delete memFile;

	return true;
}

bool wowWMOFile::loadGroupFile(CMemFile* pMemFile, SWMOGroup& group)
{
	char path[QMAX_PATH];
	getFullFileNameNoExtensionA(this->Name.c_str(), path, QMAX_PATH);

	char filename[QMAX_PATH];
	Q_sprintf(filename, QMAX_PATH, "%s_%03d.wmo", path, (int32_t)group.index);

	CMemFile* file = WowEnvironment->openFile(filename);
	if (!file)
		return false;

	WMO::wmoGroupHeader header;

	char fourcc[5];
	uint32_t size;

	uint32_t nTcoords = 0;

	while (!file->isEof())
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		uint32_t nextpos = file->getPos() + size;

		if (strcmp(fourcc, "MVER") == 0)				//version
		{
			uint32_t version;
			file->read(&version, size);
			assert(version == 17);
		}
		else if (strcmp(fourcc, "MOGP") == 0)
		{
			size = sizeof(WMO::wmoGroupHeader);
			nextpos = file->getPos() + size;
			file->read(&header, sizeof(WMO::wmoGroupHeader));
		}
		else if (strcmp(fourcc, "MOPY") == 0)
		{

		}
		else if (strcmp(fourcc, "MOVI") == 0)
		{
			uint16_t icount = (uint16_t)(size / sizeof(uint16_t));
			group.indices.resize(icount);
			file->read(group.indices.data(), icount * sizeof(uint16_t));
		}
		else if (strcmp(fourcc, "MOVT") == 0)
		{
			uint32_t vcount = size / sizeof(vector3df);
			group.vertices.resize(vcount);
			for (uint32_t i = 0; i < vcount; ++i)
			{
				vector3df tmp;
				file->read(&tmp, sizeof(vector3df));
				group.vertices[i].Pos = WMO::fixCoordinate(tmp);
			}
		}
		else if (strcmp(fourcc, "MONR") == 0)
		{
			uint32_t vcount = (uint32_t)group.vertices.size();
			assert(size == vcount * sizeof(vector3df));
			for (uint32_t i = 0; i < vcount; ++i)
			{
				vector3df tmp;
				file->read(&tmp, sizeof(vector3df));
				group.vertices[i].Normal = WMO::fixCoordinate(tmp);
				group.vertices[i].Normal.normalize();
			}
		}
		else if (strcmp(fourcc, "MOTV") == 0)
		{
			uint32_t vcount = (uint32_t)group.vertices.size();
			assert(size == vcount * sizeof(vector2df));
			if (nTcoords == 0)
			{
				for (uint32_t i = 0; i < vcount; ++i)
				{
					vector2df tmp;
					file->read(&tmp, sizeof(vector2df));
					group.vertices[i].TCoords0 = tmp;
				}
			}
			else
			{
				for (uint32_t i = 0; i < vcount; ++i)
				{
					vector2df tmp;
					file->read(&tmp, sizeof(vector2df));
					group.vertices[i].TCoords1 = tmp;
				}
			}
			++nTcoords;
			assert(nTcoords <= 2);
		}
		else if (strcmp(fourcc, "MOCV") == 0)
		{
			group.hasVertexColor = true;
			uint32_t vcount = (uint32_t)group.vertices.size();
			assert(size == vcount * sizeof(SColor));
			for (uint32_t i = 0; i < vcount; ++i)
			{
				SColor color;
				file->read(&color, sizeof(SColor));
				group.vertices[i].Color = color;
			}
		}
		else if (strcmp(fourcc, "MOBA") == 0)
		{
			uint32_t numBatches = (uint32_t)(size / sizeof(WMO::wmoGroupBatch));
			group.batchList.resize(numBatches);
			for (uint32_t i = 0; i < numBatches; ++i)
			{
				WMO::wmoGroupBatch batch;
				file->read(&batch, sizeof(WMO::wmoGroupBatch));

				group.batchList[i].indexStart = batch.indexStart;
				group.batchList[i].indexCount = batch.indexCount;
				group.batchList[i].vertexStart = batch.vertexStart;
				group.batchList[i].vertexEnd = batch.vertexEnd;
				group.batchList[i].matId = batch.matId;

				assert(batch.matId < Header.nMaterials);
			}
		}
		else if (strcmp(fourcc, "MOLR") == 0)
		{
			uint32_t nLights = (uint32_t)(size / sizeof(uint16_t));
			group.lightList.resize(nLights);
			file->read(group.lightList.data(), nLights * sizeof(uint16_t));
		}
		else if (strcmp(fourcc, "MODR") == 0)
		{
			uint32_t nDoodads = (uint32_t)(size / sizeof(uint16_t));
			group.doodadList.resize(nDoodads);
			file->read(group.doodadList.data(), nDoodads * sizeof(uint16_t));
		}
		else if (strcmp(fourcc, "MOBN") == 0)			//bsp node
		{

		}
		else if (strcmp(fourcc, "MOBR") == 0)				//bsp triangle
		{

		}
		else if (strcmp(fourcc, "MLIQ") == 0)
		{
		}
		else if (strcmp(fourcc, "MOBS") == 0)
		{
		}
		else if (strcmp(fourcc, "MDAL") == 0)
		{
		}
		else if (strcmp(fourcc, "MOTA") == 0)
		{
		}
		else if (strcmp(fourcc, "MOPL") == 0)
		{
		}
		else
		{
			//MessageBoxA(nullptr, fourcc, "", 0);
			assert(false);
		}

		file->seek((int32_t)nextpos);
	}

	delete file;

	return true;
}
