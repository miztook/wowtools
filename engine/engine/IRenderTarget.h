#pragma once

#include "base.h"
#include "vector2d.h"

class ITexture;

#define MAX_COLOR_ATTACHMENTS 4			//MRT

struct SRTCreateParam
{
	SRTCreateParam(ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt)
	{
		memset(ColorFmts, 0, sizeof(ColorFmts));

		NumColors = 1;
		ColorFmts[0] = colorFmt;
		HasDepth = true;
		DepthFmt = depthFmt;
	}

	SRTCreateParam(ECOLOR_FORMAT colorFmt)
	{
		memset(ColorFmts, 0, sizeof(ColorFmts));

		NumColors = 1;
		ColorFmts[0] = colorFmt;
		HasDepth = false;
		DepthFmt = ECF_UNKNOWN;
	}

	SRTCreateParam(ECOLOR_FORMAT colorFmts[3], ECOLOR_FORMAT depthFmt)
	{
		memset(ColorFmts, 0, sizeof(ColorFmts));

		NumColors = 3;
		ColorFmts[0] = colorFmts[0];
		ColorFmts[1] = colorFmts[1];
		ColorFmts[2] = colorFmts[2];
		HasDepth = true;
		DepthFmt = depthFmt;
	}

	int NumColors;
	ECOLOR_FORMAT ColorFmts[MAX_COLOR_ATTACHMENTS];
	ECOLOR_FORMAT DepthFmt;
	bool HasDepth;
};

class IRenderTarget
{
private:
	DISALLOW_COPY_AND_ASSIGN(IRenderTarget);

public:
	IRenderTarget(const SRTCreateParam& param, bool bUseDepthTexture)
		: NumColorAttachments(param.NumColors),
		HasDepthAttachment(param.HasDepth), UseDepthTexture(bUseDepthTexture),
		VideoBuilt(false)
	{
		DepthFormat = param.DepthFmt;
		for (int i = 0; i < MAX_COLOR_ATTACHMENTS; ++i)
			ColorFormats[i] = param.ColorFmts[i];
	}

	virtual ~IRenderTarget() { }

public:
	const vector2di& getSize() const { return TextureSize; }
	ECOLOR_FORMAT getColorFormat(int index) const { ASSERT(index >= 0 && index < MAX_COLOR_ATTACHMENTS); return ColorFormats[index]; }
	ECOLOR_FORMAT getDepthFormat() const { return DepthFormat; }
	int getNumColorAttachments() const { return NumColorAttachments; }
	bool hasDepthAttachment() const { return HasDepthAttachment; }

	virtual bool isValid() const = 0;
	virtual ITexture* getRTTexture(int index) const = 0;
	virtual ITexture* getDepthTexture() const = 0;
	virtual bool writeToRTTexture() = 0;

protected:
	vector2di	TextureSize;
	ECOLOR_FORMAT	ColorFormats[MAX_COLOR_ATTACHMENTS];
	ECOLOR_FORMAT	DepthFormat;
	const int		NumColorAttachments;
	const bool		HasDepthAttachment;
	bool	UseDepthTexture;			//use depth texture or surface
	bool	VideoBuilt;
};