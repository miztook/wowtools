#include "CMeshSceneNode.h"
#include "CMesh.h"
#include "Engine.h"
#include "CTextureManager.h"

CMeshRenderer::CMeshRenderer(const CMesh * mesh, ISceneNode* node)
	: IRenderer(node), Mesh(mesh)
{
	Box = Mesh->getBoundingBox();

	CPass* pass = Material.addPass(ELM_ALWAYS);
	pass->Cull = ECM_BACK;
}

aabbox3df CMeshRenderer::getBoundingBox() const
{
	return getLocalToWorldMatrix().transformBox(Box);
}

CMeshSceneNode::CMeshSceneNode()
{
	
}

CMeshSceneNode::~CMeshSceneNode()
{
	for (IRenderer* renderer : m_RendererList)
	{
		delete renderer;
	}
	m_RendererList.clear();
}

CMeshRenderer* CMeshSceneNode::setMesh(const CMesh* pMesh)
{
	if (!pMesh)
		return nullptr;

	for (IRenderer* renderer : m_RendererList)
	{
		delete renderer;
	}
	m_RendererList.clear();

	auto meshRenderer = new CMeshRenderer(pMesh, this);
	meshRenderer->getMaterial().setMainTexture(g_Engine->getTextureManager()->getTextureWhite().get());

	m_RendererList.push_back(meshRenderer);
	return meshRenderer;
}

std::list<SRenderUnit*> CMeshSceneNode::render(const IRenderer* renderer, const CCamera* cam)
{
	const CMeshRenderer* meshRenderer = static_cast<const CMeshRenderer*>(renderer);

	SRenderUnit* unit = new SRenderUnit(renderer);

	const CMesh* mesh = meshRenderer->getMesh();
	unit->vbuffer = mesh->getVertexBuffer();
	unit->ibuffer = mesh->getIndexBuffer();
	unit->primType = mesh->PrimType;
	unit->primCount = mesh->PrimCount;

	unit->drawParam.numVertices = mesh->getVertexBuffer()->getNumVertices();

	std::list<SRenderUnit*> unitList;
	unitList.push_back(unit);
	return unitList;
}
