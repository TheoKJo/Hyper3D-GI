/*!
 * \file		PhotonMap.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:14
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"
#include "../GIEnginePCH.h"

#include "PhotonMap.h"
#include "PhotonMapKDTree.h"

using namespace GIEngine::PhotonMapping;

GIPhotonMap::GIPhotonMap( GIEngine::GIScene *Scene, unsigned int ApproxPhotonSize )
	: mScene(Scene), mPhotonMapKDTree(NULL)
{
	mPhotonArray.reserve( ApproxPhotonSize );
}

void GIPhotonMap::InsertPhoton( const GIPhoton &Photon )
{
	if( Photon.IsLivingPhoton() == false && Photon.IsValidPhoton() )
		mPhotonArray.push_back( Photon );
}

bool GIPhotonMap::CreatePhotonKDTree()
{
	if( GetPhotonCount() == 0 )
		return false;

	GIPhotonMapKDTree *PhotonMapKDTree = new GIPhotonMapKDTree();
	PhotonMapKDTree->ConstructKDTree( GetPhotonArray(), GetPhotonCount() );

	SafeDelete( &mPhotonMapKDTree );
	mPhotonMapKDTree = PhotonMapKDTree;

	return true;
}