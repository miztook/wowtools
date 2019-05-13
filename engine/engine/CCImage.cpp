#include "CCImage.h"
#include "CBlit.h"

CCImage::CCImage(ECOLOR_FORMAT format, const dimension2d& size)
	: IImage(EIT_IMAGE), Size(size), Format(format)
{
	Pitch = ROUND_4BYTES(getBytesPerPixelFromFormat(Format) * Size.width);
	Data = new uint8_t[Size.height * Pitch];

	DeleteData = true;
}

CCImage::CCImage(ECOLOR_FORMAT format, const dimension2d& size, const void* data, bool deletaData)
	: IImage(EIT_IMAGE), Size(size), Format(format)
{
	Pitch = ROUND_4BYTES(getBytesPerPixelFromFormat(Format) * Size.width);
	Data = (uint8_t*)data;

	ASSERT(Data);

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

void CCImage::copyToScaling(void* target, uint32_t width, uint32_t height, ECOLOR_FORMAT format/*=ECF_A8R8G8B8*/, uint32_t pitch/*=0 */) const
{
	if (!target || !width || !height)
		return;

	const uint32_t bpp = getBytesPerPixelFromFormat(format);
	if (0 == pitch)
		pitch = width*bpp;

	if (Format == format && Size.width == width && Size.height == height)
	{
		if (pitch == Pitch)
		{
			memcpy(target, Data, height*pitch);
		}
		else
		{
			uint8_t* tgtpos = (uint8_t*)target;
			uint8_t* srcpos = Data;
			const uint32_t bwidth = width*bpp;
			const uint32_t rest = pitch - bwidth;
			for (uint32_t y = 0; y < height; ++y)
			{
				// copy scanline
				memcpy(tgtpos, srcpos, bwidth);
				// clear pitch
				memset(tgtpos + bwidth, 0, rest);
				tgtpos += pitch;
				srcpos += Pitch;
			}
		}
		return;
	}

	if (Size.width > width || Size.height > height)
	{
		CBlit::resizeNearest(Data, Size.width, Size.height, Pitch, Format, target, width, height, pitch, format);
	}
	else
	{
		switch (Format)
		{
		case ECF_A8R8G8B8:
			CBlit::resizeBilinearA8R8G8B8(Data, Size.width, Size.height, target, width, height, format);
			//CBlit::resizeBicubicA8R8G8B8(Data, Size.Width, Size.Height, target, width, height, format);
			break;
		default:
			ASSERT(false);
			break;
		}
	}
}

void CCImage::copyToScaling(CCImage* target) const
{
	if (!target)
		return;

	const dimension2d& targetSize = target->getDimension();

	if (targetSize == Size)
	{
		copyTo(target);
		return;
	}

	copyToScaling(target->lock(), targetSize.width, targetSize.height, target->getColorFormat());
	target->unlock();
}

void CCImage::copyTo(CCImage* target, const vector2di& pos /*= core::vector2di(0,0) */) const
{
	bool bRes = CBlit::Blit(target, vector2di(0, 0), target->getDimension(),
		this, pos);

	ASSERT(bRes);
}

bool CCImage::checkHasAlpha() const
{
	bool alpha = false;
	for (uint32_t x = 0; x < Size.width; ++x)
	{
		for (uint32_t y = 0; y < Size.height; ++y)
		{
			SColor color = getPixel(x, y);
			alpha = (color.getAlpha() != 255);
			if (alpha)
				break;
		}
	}
	return alpha;
}

SColor CCImage::getPixel(uint32_t x, uint32_t y) const
{
	if (x >= Size.width || y >= Size.height)
		return SColor(0);

	switch (Format)
	{
	case ECF_A1R5G5B5:
		return SColor::A1R5G5B5toA8R8G8B8(((uint16_t*)Data)[y*Size.width + x]);
	case ECF_R5G6B5:
		return SColor::R5G6B5toA8R8G8B8(((uint16_t*)Data)[y*Size.width + x]);
	case ECF_A8R8G8B8:
		return ((uint32_t*)Data)[y*Size.width + x];
	case ECF_R8G8B8:
	{
		uint8_t* p = Data + (y * 3)*Size.width + (x * 3);
		return SColor(p[0], p[1], p[2]);
	}
	default:
		break;
	}

	return SColor(0);
}

void CCImage::setPixel(uint32_t x, uint32_t y, SColor color)
{
	if (x >= Size.width || y >= Size.height)
		return;

	switch (Format)
	{
	case ECF_A1R5G5B5:
	{
		uint16_t * dest = (uint16_t*)(Data + (y * Pitch) + (x << 1));
		*dest = SColor::A8R8G8B8toA1R5G5B5(color.color);
	}
	break;

	case ECF_R5G6B5:
	{
		uint16_t * dest = (uint16_t*)(Data + (y * Pitch) + (x << 1));
		*dest = SColor::A8R8G8B8toR5G6B5(color.color);
	}
	break;

	case ECF_R8G8B8:
	{
		uint8_t* dest = Data + (y * Pitch) + (x * 3);
		dest[0] = (uint8_t)color.getRed();
		dest[1] = (uint8_t)color.getGreen();
		dest[2] = (uint8_t)color.getBlue();
	}
	break;

	case ECF_A8R8G8B8:
	{
		uint32_t * dest = (uint32_t*)(Data + (y * Pitch) + (x << 2));
		*dest = color.color;
	}
	break;
	default:
		break;
	}
}

bool CCImage::copyMipmapData(uint32_t face, uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height) const
{
	copyToScaling(dest, width, height, Format, pitch);

	return true;
}
