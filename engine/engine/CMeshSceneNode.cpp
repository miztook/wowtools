#include "CMeshSceneNode.h"
#include "CMesh.h"

CMeshRenderer::CMeshRenderer(const CMesh * mesh, const ISceneNode* node)
	: IRenderer(node), Mesh(mesh)
{
	Box = Mesh->getBoundingBox();
}

aabbox3df CMeshRenderer::getBoundingBox() const
{
	return getLocalToWorldMatrix().transformBox(Box);
}

CMeshSceneNode::CMeshSceneNode(ISceneNode* parent)
	: ISceneNode(parent)
{

}

void CMeshSceneNode::addMesh(CMesh* pMesh)
{
	if (!pMesh)
		return;

	auto meshRenderer = new CMeshRenderer(pMesh, this);
	m_RendererList.push_back(meshRenderer);
}

void CMeshSceneNode::removeMesh(CMesh* pMesh)
{
	if (!pMesh)
		return;

	for (auto itr = m_RendererList.begin(); itr != m_RendererList.end();)
	{
		auto meshRenderer = static_cast<CMeshRenderer*>(*itr);
		if (meshRenderer && meshRenderer->getMesh() == pMesh)
		{
			m_RendererList.erase(itr++);
			delete meshRenderer;
		}
		else
		{
			++itr;
		}
	}
}
