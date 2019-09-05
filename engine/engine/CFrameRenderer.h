#pragma once

#include "ISceneNode.h"
#include <vector>
#include <map>

struct SRenderUnit;

class CFrameRenderer
{
public:
	CFrameRenderer();
	~CFrameRenderer();

public:
	void addSceneNode(ISceneNode* node);
	void skipSceneNode(ISceneNode* node);

	void addRenderUnit(const SRenderUnit* unit);
	void renderAll();

private:
	const SRenderUnit*			CurrentUnit;

	struct SEntry
	{
		ISceneNode*		node;
		bool skip;

		bool operator<(const SEntry& c) const
		{
			uint32_t a = node->getTransform()->getHiearchyCount();
			uint32_t b = c.node->getTransform()->getHiearchyCount();

			if (a != b)
				return a < b;
			else
				return node < c.node;
		}
	};

	std::vector<SEntry>		m_SceneNodes;

	//
	std::map<int, std::vector<const SRenderUnit*>>  m_RenderUnitMap;
};