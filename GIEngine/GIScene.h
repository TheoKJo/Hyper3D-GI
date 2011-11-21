/*!
 * License
 */
#pragma once

#include "../GIEngineCore/RtScene.h"

namespace GIEngine {

//class KDTreeStructure;

/*!
 * 
 * \date 2011.02.07
 * \author Hybrid	
 */
class GIScene : public RtScene
{
public:
	GIScene();
	~GIScene();

	//KDTreeStructure* GetKDTree();

private:
	////! Internal Structures
	//RtScene *mRTScene; //!< Composition

	//KDTreeStructure *mKDTree;
};

};