#include "CM2SceneNode.h"
#include "wowM2File.h"

CM2Renderer::CM2Renderer(CM2SceneNode* node)
	: IRenderer(node)
{
	M2File = node->M2File.get();
	WowSkinFile = node->WowSkinFile;
}

aabbox3df CM2Renderer::getBoundingBox() const
{
	return M2File->BoundingBox;
}

CM2SceneNode::CM2SceneNode(std::shared_ptr<wowM2File> file)
	: M2File(file), M2Renderer(this)
{
	WowSkinFile = &M2File->SkinFile;

	m_RendererList.push_back(&M2Renderer);
}

CM2SceneNode::~CM2SceneNode()
{
	m_RendererList.clear();
}

std::list<SRenderUnit*> CM2SceneNode::render(const IRenderer* renderer, const CCamera* cam)
{
	std::list<SRenderUnit*> unitList;

	return unitList;
}

