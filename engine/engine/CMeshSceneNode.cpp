#include "CMeshSceneNode.h"
#include "CMesh.h"

CMeshRenderer::CMeshRenderer(const CMesh * mesh, ISceneNode* node)
	: IRenderer(node), Mesh(mesh)
{
	Box = Mesh->getBoundingBox();
}

aabbox3df CMeshRenderer::getBoundingBox() const
{
	return getLocalToWorldMatrix().transformBox(Box);
}

CMeshSceneNode::CMeshSceneNode()
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

SRenderUnit* CMeshSceneNode::render(const IRenderer* renderer, const CCamera* cam)
{
	const CMeshRenderer* meshRenderer = static_cast<const CMeshRenderer*>(renderer);

	SRenderUnit* unit = new SRenderUnit(renderer);

	const CMesh* mesh = meshRenderer->getMesh();
	unit->vbuffer = mesh->getVertexBuffer();
	unit->ibuffer = mesh->getIndexBuffer();
	unit->primType = mesh->PrimType;
	unit->primCount = mesh->PrimCount;

	unit->drawParam.numVertices = mesh->getVertexBuffer()->getNumVertices();
	unit->matWorld = meshRenderer->getLocalToWorldMatrix();

	return unit;
}
