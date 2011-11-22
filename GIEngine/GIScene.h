/*!
 * \file		GIScene.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:13
 * copyright	Hybrid (creavol@gmail.com)
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