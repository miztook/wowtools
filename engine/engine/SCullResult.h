#pragma once

#include <vector>

class IRenderer;

struct SCullResult
{
	std::vector<const IRenderer*>		VisibleRenderers;
};