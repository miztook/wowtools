#include "CBlit.h"

#include "CCImage.h"

bool CBlit::Blit(CCImage* dest, const vector2di& destPos, const vector2di& destDimension, const CCImage* src, const vector2di& srcPos)
{
	if (!dest || !src ||
		dest->getDimension().width < destPos.x + destDimension.width ||
		dest->getDimension().height < destPos.y + destDimension.height ||
		src->getDimension().width < srcPos.x + destDimension.width ||
		src->getDimension().height < srcPos.y + destDimension.height)
	{
		return false;
	}

	SBlitJob job;

	job.width = destDimension.width;
	job.height = destDimension.height;

	job.srcPitch = src->getPitch();
	job.srcPixelMul = getBytesPerPixelFromFormat(src->getColorFormat());
	job.src = ((uint8_t*)src->lock() + job.srcPitch * srcPos.y + job.srcPixelMul * srcPos.x);

	job.dstPitch = dest->getPitch();
	job.dstPixelMul = getBytesPerPixelFromFormat(dest->getColorFormat());
	job.dst = ((uint8_t*)dest->lock() + job.dstPitch * destPos.y + job.dstPixelMul * destPos.x);

	ECOLOR_FORMAT srcFormat = src->getColorFormat();
	ECOLOR_FORMAT destFormat = dest->getColorFormat();

	if (srcFormat == destFormat)
	{
		executeBlit_TextureCopy_x_to_x(&job);
	}
	else
		if (srcFormat == ECF_A8R8G8B8)
		{
			if (destFormat == ECF_A1R5G5B5)
				executeBlit_TextureCopy_32_to_16(&job);
			else if (destFormat == ECF_R8G8B8)
				executeBlit_TextureCopy_32_to_24(&job);
			else
			{
				src->unlock();
				dest->unlock();
				return false;
			}
		}
		else
			if (srcFormat == ECF_A1R5G5B5)
			{
				if (destFormat == ECF_R8G8B8)
					executeBlit_TextureCopy_16_to_24(&job);
				else if (destFormat == ECF_A8R8G8B8)
					executeBlit_TextureCopy_16_to_32(&job);
				else
				{
					src->unlock();
					dest->unlock();
					return false;
				}
			}
			else
				if (srcFormat == ECF_R8G8B8)
				{
					if (destFormat == ECF_A1R5G5B5)
						executeBlit_TextureCopy_24_to_16(&job);
					else if (destFormat == ECF_A8R8G8B8)
						executeBlit_TextureCopy_24_to_32(&job);
					else
					{
						src->unlock();
						dest->unlock();
						return false;
					}
				}
				else
				{
					src->unlock();
					dest->unlock();
					return false;
				}

	src->unlock();
	dest->unlock();

	return true;
}
