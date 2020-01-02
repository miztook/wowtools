#pragma once

#include "base.h"
#include "IVideoResource.h"
#include "IImage.h"

enum E_TEXTURE_TYPE : int
{
	ETT_IMAGE = 0,
	ETT_RENDERTARGET,
	ETT_DEPTHSTENCIL,

	ETT_COUNT
};

class ITexture : public IVideoResource
{
public:
	virtual ~ITexture() {}

public:
	explicit ITexture(bool mipmap) : HasMipMaps(mipmap), TextureSize(0, 0), Type(ETT_IMAGE)
	{
		ColorFormat = ECF_UNKNOWN;
		SampleCount = 0;
		IsCube = false;
		NumMipmaps = 1;

		m_pImage = nullptr;
	}

public:
	const dimension2d& getSize() const { return TextureSize; }
	ECOLOR_FORMAT getColorFormat() const { return ColorFormat; }
	uint32_t getNumMipmaps() const { return NumMipmaps; }
	uint8_t getSampleCount() const { return SampleCount; }
	E_TEXTURE_TYPE getType() const { return (E_TEXTURE_TYPE)Type; }
	bool	isCube() const { return IsCube; }

	void loadImage(std::shared_ptr<IImage> image);

protected:
	std::shared_ptr<IImage>		m_pImage;

	dimension2d		TextureSize;
	ECOLOR_FORMAT	ColorFormat;
	uint32_t		NumMipmaps;
	uint8_t	Type;
	uint8_t	SampleCount;
	const bool	HasMipMaps;
	bool	IsCube;
	
};

inline void ITexture::loadImage(std::shared_ptr<IImage> image)
{
	if (m_pImage)
		return;

	m_pImage = image;

	if (m_pImage)
	{
		NumMipmaps = HasMipMaps ? m_pImage->getNumMipLevels() : 1;
		TextureSize = m_pImage->getDimension();
		ColorFormat = m_pImage->getColorFormat();
		IsCube = m_pImage->isCube();
	}
	else
	{
		NumMipmaps = 1;
		TextureSize = dimension2d(1, 1);
		ColorFormat = ECF_UNKNOWN;
		IsCube = false;
	}
}