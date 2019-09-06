#pragma once

#include "ISceneNode.h"
#include "RenderStruct.h"
#include <vector>
#include <map>
#include <algorithm>

class CRenderUnitQueue
{
public:
	using COMPARE_FUNC = bool (*)(const SRenderUnit& a, const SRenderUnit& b);

public:
	CRenderUnitQueue(COMPARE_FUNC func) : CompareFunc(func) {}

	void sort()
	{
		std::sort(RenderUnits.begin(), RenderUnits.end(), CompareFunc);
	}

	static COMPARE_FUNC getCompareFunc(int renderQueue);

	std::vector<SRenderUnit>	RenderUnits;
	COMPARE_FUNC  CompareFunc;
};

class CFrameRenderer
{
public:
	CFrameRenderer();
	~CFrameRenderer();

public:
	void addSceneNode(ISceneNode* node);
	void skipSceneNode(ISceneNode* node);
	void clearAllSceneNodes();
	void tickAllSceneNodes(uint32_t tickTime);

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
	std::map<int, CRenderUnitQueue>  m_RenderQueueMap;
};