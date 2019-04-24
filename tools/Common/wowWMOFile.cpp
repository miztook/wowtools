#include "wowWMOFile.h"

#include "CMemFile.h"
#include "function.h"

wowWMOFile::wowWMOFile()
{
}

wowWMOFile::~wowWMOFile()
{
}

bool wowWMOFile::loadFile(CMemFile* file)
{
	char tmp[QMAX_PATH];
	const char* name = file->getFileName();
	getFullFileNameNoExtensionA(name, tmp, QMAX_PATH);
	Name = tmp;

	char fourcc[5];
	
	uint32_t size;
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
		else if (strcmp(fourcc, "MOHD") == 0)
		{

		}
		else if (strcmp(fourcc, "MOTX") == 0)
		{

		}
		else if (strcmp(fourcc, "MOMT") == 0)
		{

		}
		else if (strcmp(fourcc, "MOGN") == 0)
		{

		}
		else if (strcmp(fourcc, "MOGI") == 0)
		{

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
		else
		{
			assert(false);
		}

	}

	return false;
}
