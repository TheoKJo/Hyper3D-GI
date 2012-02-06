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

class GIPhotonMapKDTree;

class GIPhotonMap
{
public:
	GIPhotonMap( GIScene *Scene, unsigned int ApproxPhotonSize );
	~GIPhotonMap();

	void InsertPhoton( const GIPhoton &Photon );
	unsigned int GetPhotonCount() const { return m_PhotonArray.size(); }

	bool CreatePhotonKDTree();
	GIPhotonMapKDTree* GetPhotonMapKDTree() { return m_PhotonMapKDTree; }

	const std::vector<GIPhoton>& GetPhotonArray() const { return m_PhotonArray; }
	std::vector<GIPhoton>& GetPhotonArray() { return m_PhotonArray; }

private:
	//mKDTree
	GIScene *m_Scene;
	std::vector<GIPhoton> m_PhotonArray;
	GIPhotonMapKDTree *m_PhotonMapKDTree;
};

};};