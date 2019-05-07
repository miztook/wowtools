#pragma once

#include "base.h"

class IImage
{
public:
	IImage(E_IMAGE_TYPE type) : m_ImageType(type) {}
	virtual ~IImage() = default;

public:
	virtual ECOLOR_FORMAT getColorFormat() const = 0;
	virtual const vector2du& getDimension() const = 0;
	virtual uint32_t getNumMipLevels() const = 0;

	virtual bool isCube() const = 0;
	virtual uint32_t	 getNumFaces() const = 0;

	virtual bool copyMipmapData(uint32_t face, uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height) const = 0;

	E_IMAGE_TYPE getImageType() const { return m_ImageType; }

private:
	E_IMAGE_TYPE	m_ImageType;

};
