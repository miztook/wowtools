#include "IRenderer.h"
#include "ISceneNode.h"

matrix4 IRenderer::getLocalToWorldMatrix() const
{
	return SceneNode->getTransform()->getAbsoluteTransformation();
}
