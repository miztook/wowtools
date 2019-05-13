#include "COpenGLTexture.h"

#include "COpenGLDriver.h"
#include "COpenGLHelper.h"
#include "COpenGLMaterialRenderComponent.h"

COpenGLTexture::COpenGLTexture(COpenGLDriver* driver, bool mipmap)
	: ITexture(mipmap), Driver(driver), GLTexture(0)
{

}

COpenGLTexture::~COpenGLTexture()
{
	releaseVideoResources();
}