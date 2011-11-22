/*!
 * \file		PhotonMap.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:14
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "Photon.h"
#include <vector>

namespace GIEngine { 
	class GIScene;
	namespace PhotonMapping {

class GIPhotonMap
{
public:
	GIPhotonMap( GIScene *Scene, unsigned int ApproxPhotonSize );
	~GIPhotonMap();

	void InsertPhoton( const GIPhoton &Photon );
	unsigned int GetPhotonCount() const { return mPhotonArray.size(); }

	bool CreatePhotonKDTree();
	bool HasKDTree();

	std::vector<GIPhoton>& GetPhotonArray() { return mPhotonArray; }

private:
	//mKDTree
	GIScene *mScene;
	std::vector<GIPhoton> mPhotonArray;
};

};};