#pragma once

#include "IImage.h"
#include "SColor.h"

class CCImage : public IImage
{
public:
	CCImage(ECOLOR_FORMAT format, const dimension2d& size, const void* data, bool deletaData);
	CCImage(ECOLOR_FORMAT format, const dimension2d& size);
	virtual ~CCImage();

public:
	virtual ECOLOR_FORMAT getColorFormat() const override { return Format; }
	virtual const dimension2d& getDimension() const override { return Size; }
	virtual uint32_t getNumMipLevels() const override { return Size.getNumMipLevels(); }

	virtual bool isCube() const override { return false; }
	virtual uint32_t	 getNumFaces() const override { return 1; }
	virtual bool copyMipmapData(uint32_t face, uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height) const override;

public:
	//
	void* lock() const { return Data; }
	void unlock() const {}

	uint32_t getPitch() const { return Pitch; }
	SColor getPixel(uint32_t x, uint32_t y) const;
	void setPixel(uint32_t x, uint32_t y, SColor color);

	//scale”√À´œﬂ–‘
	void copyToScaling(void* target, uint32_t width, uint32_t height, ECOLOR_FORMAT format = ECF_A8R8G8B8, uint32_t pitch = 0) const;
	void copyToScaling(CCImage* target) const;
	void copyTo(CCImage* target, const vector2di& pos = vector2di(0, 0)) const;
	bool checkHasAlpha() const;

protected:
	uint8_t*		Data;
	dimension2d		Size;
	uint32_t		Pitch;
	ECOLOR_FORMAT	Format;

	bool		DeleteData;
};