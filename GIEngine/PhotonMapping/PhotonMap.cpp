#include "GIEnginePCH.h"
#include "../GIEnginePCH.h"

#include "PhotonMap.h"
#include "PhotonMapKDTree.h"

using namespace GIEngine::PhotonMapping;

GIPhotonMap::GIPhotonMap( GIEngine::GIScene *Scene, unsigned int ApproxPhotonSize )
	: mScene(Scene)
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

	/*GIPhotonMapKDTree *PhotonMapKDTree = new GIPhotonMapKDTree();
	PhotonMapKDTree->ConstructKDTree( GetPhotonArray(), GetPhotonCount() );*/

	//PhotonMapKDTree->

	return true;
}