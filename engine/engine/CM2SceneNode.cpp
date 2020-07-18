#include "CM2SceneNode.h"

CM2SceneNode::CM2SceneNode(std::shared_ptr<wowM2File> file)
	: M2File(file)
{

}

CM2SceneNode::~CM2SceneNode()
{
	for (IRenderer* renderer : m_RendererList)
	{
		delete renderer;
	}
	m_RendererList.clear();
}
