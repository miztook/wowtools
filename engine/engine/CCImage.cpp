#include "CCImage.h"

CCImage::CCImage(ECOLOR_FORMAT format, const vector2di& size)
	: IImage(EIT_IMAGE), Size(size), Format(format)
{
	Pitch = ROUND_4BYTES(getBytesPerPixelFromFormat(Format) * Size.width);
	Data = new uint8_t[Size.height * Pitch];

	DeleteData = true;
}

CCImage::CCImage(ECOLOR_FORMAT format, const vector2di& size, const void* data, bool deletaData)
	: IImage(EIT_IMAGE), Size(size), Format(format)
{
	Pitch = ROUND_4BYTES(getBytesPerPixelFromFormat(Format) * Size.width);
	Data = (uint8_t*)data;

	assert(Data);

	DeleteData = deletaData;
}

CCImage::~CCImage()
{
	if (Data && DeleteData)
	{
		delete[] Data;
		Data = 0;
	}
}