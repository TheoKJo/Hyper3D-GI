#include "GIEnginePCH.h"

#include "GamebryoGI.h"

#include "IrradianceVolume.h"
#include "RaytracerProcedure.h"

//#include "Raytracer/Light.h"
//#include "Raytracer/Material.h"
//#include "Raytracer/Scene.h"

#include <NiMesh.h>
#include <NiPoint2.h>
#include <NiPointLight.h>
#include <NiStandardMaterial.h>

#include <NiMaterialProperty.h>
#include <NiTexturingProperty.h>
#include <NiSourceTexture.h>
#include <NiDX9TextureData.h>
#include <NiDX9SourceTextureData.h>

#include <crtdbg.h>

#include <fstream>

#include "squish-1.10/squish.h"

//#pragma comment(lib, "squish-1.10/lib/squishd.lib")
//#pragma comment(lib, "squish-1.10/lib/squish.lib")
//#pragma comment(lib, "squishd.lib")

/*!
 * 임시
 */

using namespace GIEngine;

unsigned int GetNumberOfBytes( const D3DFORMAT &Format )
{
	switch (Format)
	{
	case D3DFMT_DXT1:
		return 0; // 4-bites
	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_P8:
	case D3DFMT_L8:
	case D3DFMT_A4L4:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		return 1;
	case D3DFMT_X4R4G4B4:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_R5G6B5:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_A8P8:
	case D3DFMT_A8L8:
	case D3DFMT_V8U8:
	case D3DFMT_L6V5U5:
	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_D15S1:
	case D3DFMT_D16:
	case D3DFMT_L16:
	case D3DFMT_INDEX16:
	case D3DFMT_CxV8U8:
	case D3DFMT_G8R8_G8B8:
	case D3DFMT_R8G8_B8G8:
	case D3DFMT_R16F:
		return 2;
	case D3DFMT_R8G8B8:
		return 3;
	case D3DFMT_A2W10V10U10:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8L8V8U8:
	case D3DFMT_Q8W8V8U8:
	case D3DFMT_V16U16:
		//case D3DFMT_W11V11U10: // Dropped in DX9.0
	case D3DFMT_UYVY:
	case D3DFMT_YUY2:
	case D3DFMT_G16R16:
	case D3DFMT_D32:
	case D3DFMT_D24S8:
	case D3DFMT_D24X8:
	case D3DFMT_D24X4S4:
	case D3DFMT_D24FS8:
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_INDEX32:
	case D3DFMT_MULTI2_ARGB8:
	case D3DFMT_G16R16F:
	case D3DFMT_R32F:
		return 4;
	case D3DFMT_A16B16G16R16:
	case D3DFMT_Q16W16V16U16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_G32R32F:
		return 8;
	case D3DFMT_A32B32G32R32F:
		return 16; // !
	}
	return 0;
}

unsigned char* CreatePixelDataFromD3DTexture( D3DTexturePtr pkD3DTexture )
{
	//D3DSurfacePtr pkMipLevel;
	D3DSURFACE_DESC desc;

	HRESULT hResult = pkD3DTexture->GetLevelDesc(0, &desc);
	if(FAILED(hResult))
		return NULL;

	D3DLOCKED_RECT rect;
	hResult = pkD3DTexture->LockRect(0, &rect, NULL, D3DLOCK_READONLY);
	if(FAILED(hResult))
		return NULL;

	int EnumFlag = -1;

	switch( desc.Format )
	{
	case D3DFMT_DXT1:
		EnumFlag = squish::kDxt1;
		break;
	case D3DFMT_DXT3:
		EnumFlag = squish::kDxt3;
		break;
	case D3DFMT_DXT5:
		EnumFlag = squish::kDxt5;
		break;
	}
	if( EnumFlag != -1 )
	{
		BYTE* pData = new BYTE[desc.Width * desc.Height * 4];
		squish::DecompressImage( pData, desc.Width , desc.Height, rect.pBits, EnumFlag );
		pkD3DTexture->UnlockRect(0);
		return pData;
	}
	unsigned int Bytes = GetNumberOfBytes( desc.Format );
	if( Bytes == 0 )
	{
		pkD3DTexture->UnlockRect(0);
		return NULL;
	}
	else
	{
		// TODO : 완전 임시코드, 제대로 작동 안함!
		BYTE* pData = new BYTE[desc.Width * desc.Height * 4];
		// Read data from locked rect
		for(UINT y=0; y<desc.Height; ++y)
		{
			// Get a pointer to this "row" of pixels
			BYTE* pBits = (BYTE*)rect.pBits + y*rect.Pitch;
			memcpy(pData, pBits, desc.Width*Bytes); // Again, assume 32-bit
		}

		// Unlock the texture
		// Do whatever with pData - it now contains the texture data
		pkD3DTexture->UnlockRect(0);
		return pData;
	}
	NIASSERT( false );

	return NULL;

}

GamebryoGISceneManager::GamebryoGISceneManager()
	//: mScene(NULL), mRTScene(NULL), 
	//GetRaytracer()Procedure(NULL), mIrradianceProcedure(NULL), 
	//mCullBackface(true)
{
}

GamebryoGISceneManager::~GamebryoGISceneManager()
{
}

RtLight* GamebryoGISceneManager::ConvertLight( NiLight *pkLight )
{
	NIASSERT( pkLight );
	if( NiIsKindOf( NiPointLight, pkLight ) )
	{
		NiPointLight *pkPointLight = (NiPointLight*)pkLight;

		// TODO: 현재 Scene 에 맞추기 위한 임시.
		if( pkPointLight->GetName().Contains( "ZoneLight" ) )
			return NULL;
		
		RtPointLight *pPointLight = new RtPointLight();

		//pkPointLight->GetDimmer()
		pPointLight->AmbientColor = ConvertColor( pkPointLight->GetAmbientColor(), 1.0f );
		pPointLight->DiffuseColor = ConvertColor( pkPointLight->GetDiffuseColor(), 1.0f );
		pPointLight->SpecularColor = ConvertColor( pkPointLight->GetSpecularColor(), 1.0f );
		pPointLight->Position = ConvertVector( pkPointLight->GetWorldLocation() );

		pPointLight->ConstantAttenuation = pkPointLight->GetConstantAttenuation();
		pPointLight->LinearAttenuation = pkPointLight->GetLinearAttenuation();
		pPointLight->QuadraticAttenuation = pkPointLight->GetQuadraticAttenuation();

		return pPointLight;
	}
	return NULL;
}

RtMaterial* GamebryoGISceneManager::ConvertMaterial( NiMesh *pkMesh )
{
	NiMaterialProperty *pkMatProp = NiDynamicCast(NiMaterialProperty, pkMesh->GetProperty(NiProperty::MATERIAL));
	if( pkMatProp == NULL )
		return NULL;
	
	RtMaterial *pMaterial = new RtMaterial;
	pMaterial->SetMaterialName( pkMatProp->GetName() );
	pMaterial->SetAmbientColor( ConvertColor( pkMatProp->GetAmbientColor() ) );
	pMaterial->SetDiffuseColor( ConvertColor( pkMatProp->GetDiffuseColor() ) );
	pMaterial->SetSpecularColor( ConvertColor( pkMatProp->GetSpecularColor() ) );
	pMaterial->SetShininess( pkMatProp->GetShineness() );

	NiTexturingProperty *pkTexProp = NiDynamicCast(NiTexturingProperty, pkMesh->GetProperty(NiProperty::TEXTURING));
	if( pkTexProp && pkTexProp->GetBaseMap() )
	{
		RtTexture *pTexture = FetchTextureData( pkTexProp->GetBaseMap()->GetTexture() );
		pMaterial->SetAlbedoTexture( pTexture );
	}

	return pMaterial;
}
//
//RtMaterial* GamebryoGISceneManager::ConvertMaterial( NiMaterial *pkMaterial, const NiTransform &ParentTransform )
//{
//	NIASSERT( pkMaterial );
//	//if( NiIsKindOf( NiPointLight, pkLight ) )
//	//{
//	//	NiPointLight *pkPointLight = (NiPointLight*)pkLight;
//
//	//	RtPointLight *pPointLight = new RtPointLight();
//
//	//	//pkPointLight->GetDimmer()
//	//	pPointLight->AmbientColor = RtVector4( pkPointLight->GetAmbientColor().r, pkPointLight->GetAmbientColor().g, pkPointLight->GetAmbientColor().b, 1.0f );
//	//	pPointLight->DiffuseColor = RtVector4( pkPointLight->GetDiffuseColor().r, pkPointLight->GetDiffuseColor().g, pkPointLight->GetDiffuseColor().b, 1.0f );
//	//	pPointLight->SpecularColor = RtVector4( pkPointLight->GetSpecularColor().r, pkPointLight->GetSpecularColor().g, pkPointLight->GetSpecularColor().b, 1.0f );
//	//	pPointLight->Position = RtVector3( pkPointLight->GetWorldLocation().x, pkPointLight->GetWorldLocation().y, pkPointLight->GetWorldLocation().z );
//	//	return pPointLight;
//	//}
//	//return NULL;
//
//
//	//RtMaterial *pMaterial = new RtMaterial;
//	//pMaterial->SetAlbedoTexture( 
//
//	// 지원하는 Material 일 경우
//	if( NiIsKindOf( NiStandardMaterial, pkMaterial ) )
//	{
//		NiStandardMaterial *pkStdMaterial = (NiStandardMaterial*)pkMaterial;
//		//NiTexturingProperty *pkTexProp = NiDynamicCast(NiTexturingProperty, pkMesh->GetProperty(NiProperty::TEXTURING));
//		//pMaterialProperty
//		//pkTexProp->GetBaseMap()->
//		
//		/*RtTexture *pAlbedoTexture = new RtTexture;
//		pMaterial->SetAlbedoTexture( pAlbedoTexture );
//		FetchTextureData( pkTexProp->GetBaseTexture(), pAlbedoTexture );
//		pkStdMaterial->GetBase*/
//
//	}
//	else
//		return NULL; //!< 지원하지 않는 material
//	return NULL;
//}

unsigned int GamebryoGISceneManager::MeshToTriangleList( GIScene *Scene, NiMesh *pkMesh, unsigned int &VertexIndex, 
											RtTriangle *outTriangleList, std::list<RtMaterial*> &MaterialList )
{
	NIASSERT( outTriangleList != NULL );
	if( pkMesh == NULL )
		return 0;

	unsigned int TriangleCount = pkMesh->GetTotalPrimitiveCount();
	if( TriangleCount == 0 || pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES )
	{
		assert( false );
		return 0;
	}

	// 임시
	//assert( pkMesh->GetSubmeshCount() == 1 );

	const unsigned int FirstVertexIndex = VertexIndex;
	unsigned int SubMeshCount = pkMesh->GetSubmeshCount();

	NiMaterialProperty *pkMatProp = NiDynamicCast(NiMaterialProperty, pkMesh->GetProperty(NiProperty::MATERIAL));
	int MaterialIndex = -1;
	if( pkMatProp )
	{
		//MaterialIndex = Scene->FindMaterialIndex( (const char*)pkMatProp->GetName() );
		unsigned int index = 0;
		for( std::list<RtMaterial*>::iterator iter = MaterialList.begin(); iter != MaterialList.end(); iter++ )
		{
			if( pkMatProp->GetName() == (*iter)->GetMaterialName() )
			{
				MaterialIndex = index;
				break;
			}
			index++;
		}
		
		if( MaterialIndex < 0 ) // 이미 생성한 material이 아니라면, 새로 생성(변환)
		{
			MaterialIndex = MaterialList.size();
			RtMaterial *pMaterial = ConvertMaterial( pkMesh );
			MaterialList.push_back( pMaterial );
		}
	}

	//for( unsigned int SubMeshIndex = 0; SubMeshIndex < SubMeshCount; SubMeshIndex++ )
	unsigned int SubMeshIndex = 0;
	{
	// Vertex Position
		/*NiDataStreamElementLock kPositionLock2( pkMesh,
			NiCommonSemantics::POSITION(), SubMeshIndex,
			NiDataStreamElement::F_FLOAT32_4, 
			NiDataStream::LOCK_TOOL_READ);*/
		//NIASSERT(kPositionLock2.IsLocked());

		NiDataStreamElementLock kPositionLock( pkMesh,
			NiCommonSemantics::POSITION(), SubMeshIndex,
			NiDataStreamElement::F_FLOAT32_3, 
			NiDataStream::LOCK_TOOL_READ);
		NIASSERT(kPositionLock.IsLocked());
		NiTStridedRandomAccessIterator<NiPoint3> kPositionIter = kPositionLock.begin<NiPoint3>();
		NiTStridedRandomAccessIterator<NiPoint3> kPositionIterEnd = kPositionLock.end<NiPoint3>();

		unsigned int PositionCount = kPositionLock.count();

	// Vertex Index of Triangle
		NiDataStreamElementLock kIndexLock( pkMesh,
			NiCommonSemantics::INDEX(), SubMeshIndex,
			NiDataStreamElement::F_UINT16_1, 
			NiDataStream::LOCK_TOOL_READ);
		NIASSERT(kIndexLock.IsLocked());
		NiTStridedRandomAccessIterator<NiUInt16> kIndexIter = kIndexLock.begin<NiUInt16>();
		NiTStridedRandomAccessIterator<NiUInt16> kIndexIterEnd = kIndexLock.end<NiUInt16>();

	// Normal
		NiDataStreamElementLock kNormalLock( pkMesh,
			NiCommonSemantics::NORMAL(), SubMeshIndex,
			NiDataStreamElement::F_FLOAT32_3, 
			NiDataStream::LOCK_TOOL_READ);
		bool bNormal = kNormalLock.IsLocked();
		NiTStridedRandomAccessIterator<NiPoint3> kNormalIter;
		NiTStridedRandomAccessIterator<NiPoint3> kNormalIterEnd;

		bool bGlobalNormal = false;
		NiPoint3 GlobalNormal;
		if( bNormal )
		{
			kNormalIter = kNormalLock.begin<NiPoint3>();
			kNormalIterEnd = kNormalLock.end<NiPoint3>();
			if( kNormalLock.count() == 1 )
			{
				GlobalNormal = pkMesh->GetWorldRotate() * kNormalIter[0];
				GlobalNormal /= GlobalNormal.Length();
				bGlobalNormal = true;
			}
			unsigned int ss = kNormalLock.count();
			NIASSERT( kNormalLock.count() == PositionCount || kNormalLock.count() == 1 );
		}

	// Vertex Color
		NiDataStreamElementLock kColorLock( pkMesh, 
			NiCommonSemantics::COLOR(), SubMeshIndex,
			NiDataStreamElement::F_FLOAT32_4, 
			NiDataStream::LOCK_TOOL_READ );
		bool bColor = kColorLock.IsLocked();
		NiTStridedRandomAccessIterator<NiColor> kColorIter;
		NiTStridedRandomAccessIterator<NiColor> kColorIterEnd;
		if( bColor )
		{
			kColorIter = kColorLock.begin<NiColor>();
			kColorIterEnd = kColorLock.end<NiColor>();
			unsigned int colorCount = kColorLock.count();
			NIASSERT( PositionCount == kColorLock.count() );
		}

	// Texture Coords
		NiDataStreamElementLock kTexCoordsLock( pkMesh,
			NiCommonSemantics::TEXCOORD(), SubMeshIndex,
			NiDataStreamElement::F_FLOAT32_2, 
			NiDataStream::LOCK_TOOL_READ);
		bool bTexCoords = kTexCoordsLock.IsLocked();
		NiTStridedRandomAccessIterator<NiPoint2> kTexCoordsIter;
		NiTStridedRandomAccessIterator<NiPoint2> kTexCoordsIterEnd;
		if( bTexCoords )
		{
			kTexCoordsIter = kTexCoordsLock.begin<NiPoint2>();
			kTexCoordsIterEnd = kTexCoordsLock.end<NiPoint2>();
			unsigned int ss = kTexCoordsLock.count();
			NIASSERT( PositionCount == kTexCoordsLock.count() );
		}

		//assert( kIndexLock.count() == TriangleCount * 3 );

		for( unsigned int i = 0; i < PositionCount; i++ )
		{
			Scene->GetVertexPositionList()[VertexIndex] = ConvertVector( pkMesh->GetWorldTransform() * kPositionIter[i] );
			if( bGlobalNormal )
				Scene->GetVertexNormalList()[VertexIndex] = ConvertVector( GlobalNormal );
			else
				Scene->GetVertexNormalList()[VertexIndex] = ConvertVector( pkMesh->GetWorldRotate() * kNormalIter[i] );
			VertexIndex++;
		}

		for( unsigned int i = 0; i < TriangleCount; i++ )
		{
			unsigned int v0_index = kIndexIter[i*3+0];
			unsigned int v1_index = kIndexIter[i*3+1];
			unsigned int v2_index = kIndexIter[i*3+2];

			assert( v0_index < PositionCount );
			assert( v1_index < PositionCount );
			assert( v2_index < PositionCount );

			NiPoint3 Position0 = kPositionIter[v0_index];
			NiPoint3 Position1 = kPositionIter[v1_index];
			NiPoint3 Position2 = kPositionIter[v2_index];

			Position0 = pkMesh->GetWorldTransform() * Position0;
			Position1 = pkMesh->GetWorldTransform() * Position1;
			Position2 = pkMesh->GetWorldTransform() * Position2;
			
			outTriangleList[i].TriangleNumber = -1;
			outTriangleList[i].MaterialIndex = MaterialIndex;
			outTriangleList[i].vg0.Vertex = RtVector3( Position0.x, Position0.y, Position0.z );
			outTriangleList[i].vg1.Vertex = RtVector3( Position1.x, Position1.y, Position1.z );
			outTriangleList[i].vg2.Vertex = RtVector3( Position2.x, Position2.y, Position2.z );
			outTriangleList[i].vg0.VertexNum = FirstVertexIndex + v0_index;
			outTriangleList[i].vg1.VertexNum = FirstVertexIndex + v1_index;
			outTriangleList[i].vg2.VertexNum = FirstVertexIndex + v2_index;

			if( bNormal )
			{
				if( bGlobalNormal )
				{
					outTriangleList[i].vg0.Normal = RtVector3( GlobalNormal.x, GlobalNormal.y, GlobalNormal.z );
					outTriangleList[i].vg1.Normal = RtVector3( GlobalNormal.x, GlobalNormal.y, GlobalNormal.z );
					outTriangleList[i].vg2.Normal = RtVector3( GlobalNormal.x, GlobalNormal.y, GlobalNormal.z );
				}
				else
				{
					NiPoint3 Normal0 = pkMesh->GetWorldRotate() * kNormalIter[v0_index];
					NiPoint3 Normal1 = pkMesh->GetWorldRotate() * kNormalIter[v1_index];
					NiPoint3 Normal2 = pkMesh->GetWorldRotate() * kNormalIter[v2_index];
					Normal0 /= Normal0.Length();
					Normal1 /= Normal1.Length();
					Normal2 /= Normal2.Length();

					outTriangleList[i].vg0.Normal = RtVector3( Normal0.x, Normal0.y, Normal0.z );
					outTriangleList[i].vg1.Normal = RtVector3( Normal1.x, Normal1.y, Normal1.z );
					outTriangleList[i].vg2.Normal = RtVector3( Normal2.x, Normal2.y, Normal2.z );
				}
			}
			if( bTexCoords )
			{
				NiPoint2 TexCoords0 = kTexCoordsIter[v0_index];
				NiPoint2 TexCoords1 = kTexCoordsIter[v1_index];
				NiPoint2 TexCoords2 = kTexCoordsIter[v2_index];

				outTriangleList[i].vg0.TexCoords = RtVector2( TexCoords0.x, TexCoords0.y );
				outTriangleList[i].vg1.TexCoords = RtVector2( TexCoords1.x, TexCoords1.y );
				outTriangleList[i].vg2.TexCoords = RtVector2( TexCoords2.x, TexCoords2.y );
			}
		}

		kPositionLock.Unlock();
		kIndexLock.Unlock();
		kNormalLock.Unlock();
		kTexCoordsLock.Unlock();
	}

	return TriangleCount;
}

GIScene* GamebryoGISceneManager::ConvertScene( NiNode *pScene )
{
	GIScene *pGIScene = CreateScene();
	RefineTriangles( pScene, pGIScene );
	RefineLights( pScene, pGIScene );
	//RefineMaterials( pScene, pGIScene );

	// ??
	pGIScene->ConvertAll();

	return pGIScene;
}

void GamebryoGISceneManager::RefineTriangles( NiNode *pkScene, GIScene *pGIScene )
{
	NIASSERT( pGIScene != NULL && pkScene != NULL );
	unsigned int TriangleCount = CountTrianglesRecursive( pkScene );
	unsigned int VertexCount = CountVerticesRecursive( pkScene );

	pGIScene->InitializeVertex( VertexCount );

	pGIScene->InitializeTriangle( TriangleCount );
	RtTriangle *pTriangleList = pGIScene->GetTriangleList();

	std::list<RtMaterial*> MaterialList;
	unsigned int VertexIndex = 0;
	unsigned int RefinedTriangleCount = RefineTrianglesRecursive( pGIScene, pkScene, VertexIndex, pTriangleList, MaterialList );
	NIASSERT( RefinedTriangleCount == TriangleCount );
	NIASSERT( VertexIndex == VertexCount );

	pGIScene->InitializeMaterial( MaterialList.size() );
	RtMaterial **MaterialArray = pGIScene->GetMaterialArray();

	if( MaterialArray == NULL )
		return;

	unsigned int i = 0;
	for( std::list<RtMaterial*>::iterator iter = MaterialList.begin(); iter != MaterialList.end(); iter++ )
	{
		MaterialArray[i++] = *iter;
	}
}

void GamebryoGISceneManager::RefineLights( NiNode *pkScene, GIScene *pGIScene )
{
	std::list<RtLight*> LightList;
	unsigned int LightCount = RefineLightsRecursive( pkScene, LightList );
	NIASSERT( LightCount == LightList.size() );

	pGIScene->InitializeLight( LightCount );
	RtLight** LightArray = pGIScene->GetLightArray();
	
	if( LightArray == NULL )
		return;

	unsigned int i = 0;
	for( std::list<RtLight*>::iterator iter = LightList.begin(); iter != LightList.end(); ++iter )
	{
		LightArray[i++] = (*iter);
	}
}

//
//void GamebryoGISceneManager::RefineMaterials( NiNode *pkScene, GIScene *pGIScene )
//{
//	NIASSERT( pGIScene != NULL && pkScene != NULL );
//
//	std::list<RtMaterial*> MaterialList;
//
//	unsigned MaterialCount = NiMaterial::GetMaterialCount();
//	pGIScene->InitializeMaterial( MaterialCount );
//	RtMaterial** MaterialArray = pGIScene->GetMaterialArray();
//
//	unsigned int MaterialIndex = 0;
//	NiMaterialIterator iter = NiMaterial::GetFirstMaterialIter();
//	for( unsigned int i = 0; i < MaterialCount; i++ )
//	{
//		NiMaterial *curNiMaterial = NiMaterial::GetNextMaterial( iter );
//		if( curNiMaterial == NULL )
//			break;
//
//		//MaterialArray[MaterialIndex] = curNiMaterial;
//		MaterialIndex++;
//
//			//FetchTextureData( 
//
//		//// 지원하는 Material 일 경우
//		//if( NiIsKindOf( NiStandardMaterial, curNiMaterial ) )
//		//{
//		//	NiStandardMaterial *curStdMaterial = (NiStandardMaterial*)curNiMaterial;
//		//	NiTexturingProperty* pkTexProp = NiDynamicCast(NiTexturingProperty, pkMesh->GetProperty(NiProperty::TEXTURING));
//		//	NiSourceTexture* pkSourceTexture = NiDynamicCast(NiSourceTexture, pkTexProp->GetBaseTexture());
//		//	if (pkSourceTexture)
//		//	{
//
//		//		pkSourceTexture->GetSourcePixelData()
//		//			//pkSourceTexture->LoadPixelDataFromFile();
//		//	}
//
//		//	NiFixedString FixedString;
//		//	curStdMaterial->GetTextureNameFromTextureEnum( NiStandardMaterial::MAP_BASE, FixedString, 0 );
//		//	/*curStdMaterial->MapIndexFromTextureEnum( NiStandardMaterial::MAP_BASE, 
//		//	MaterialArray[MaterialIndex].SetMaterialName( 
//		//	NiTexture::GetStreamableRTTIName(*/
//
//		//	RefinedMaterialIndex++;
//		//}
//		/*NiBinaryStream BS;
//		NiFixedString::LoadCStringAsFixedString( BS, curMaterial->GetName() );
//		MaterialArray[i].SetMaterialName( BS.ReadCString() );*/
//		//MaterialArray[i].SetMaterialName( curMaterial->GetName() );
//
//		//curNiMaterial->GetCurrentShader(
//
//		// 임시
//		/*char *strMaterial = new char[255];
//		NiStream stream;
//		stream.SaveFixedString( curMaterial->GetName() );*/
//		//stream.LoadCString( strMaterial );
//		//MaterialArray[i].SetMaterialName( strMaterial );
//		//delete[] strMaterial;
//	}
//}

//void GamebryoGISceneManager::InitializeLights()
//{
	//assert( mScene );
	//unsigned int LightCount = CountLightRecursive( mScene );
	///*mRTScene->InitializeLights( LightCount );
	//RtLight** LightArray = mRTScene->GetLightArray();*/

	//NiTransform transform;
	//transform.MakeIdentity();
	//RefineLightRecursive( mScene, transform );

	//mRTScene->InitializeLights( mLightList.size() );
	//RtLight** LightArray = mRTScene->GetLightArray();

	//unsigned int i = 0;
	//for( std::list<RtLight*>::iterator iter = mLightList.begin(); iter != mLightList.end(); ++iter )
	//{
	//	LightArray[i++] = (*iter);
	//}
//}

//void GamebryoGISceneManager::InitializeMaterials()
//{
	//assert( mScene );
	///*unsigned MaterialCount = NiMaterial::GetMaterialCount();
	//mRTScene->InitializeMaterials( MaterialCount );*/

	//mRTScene->InitializeMaterials( mMaterialList.size() );
	//RtMaterial* MaterialArray = mRTScene->GetMaterialArray();

	//unsigned int i = 0;
	//for( std::list<RtMaterial*>::iterator iter = mMaterialList.begin(); iter != mMaterialList.end(); ++iter )
	//{
	//	MaterialArray[i++] = *(*iter);
	//}

	//unsigned int MaterialIndex = 0;
	//NiMaterialIterator iter = NiMaterial::GetFirstMaterialIter();
	//for( unsigned int i = 0; i < MaterialCount; i++ )
	//{
	//	NiMaterial *curNiMaterial = NiMaterial::GetNextMaterial( iter );
	//	if( curNiMaterial == NULL )
	//		break;
	//	if( NiIsKindOf( NiStandardMaterial, curNiMaterial ) )
	//	{
	//		NiStandardMaterial *curStdMaterial = (NiStandardMaterial*)curNiMaterial;

	//		//NiTexturingProperty* pkTexProp = NiDynamicCast(NiTexturingProperty, pkMesh->GetProperty(NiProperty::TEXTURING));
	//		////pkTexProp->Get
	//		//NiSourceTexture* pkSourceTexture = NiDynamicCast(NiSourceTexture, pkTexProp->GetBaseTexture());
	//		//pkSourceTexture->GetName()
	//		//if (pkSourceTexture)
	//		//{

	//		//	pkSourceTexture->GetSourcePixelData()
	//		//		//pkSourceTexture->LoadPixelDataFromFile();
	//		//}

	//		//NiFixedString FixedString;
	//		//curStdMaterial->GetTextureNameFromTextureEnum( NiStandardMaterial::MAP_BASE, FixedString, 0 );
	//		/*curStdMaterial->MapIndexFromTextureEnum( NiStandardMaterial::MAP_BASE, 
	//		MaterialArray[MaterialIndex].SetMaterialName( 
	//		NiTexture::GetStreamableRTTIName(*/

	//		MaterialIndex++;
	//	}
	//	/*NiBinaryStream BS;
	//	NiFixedString::LoadCStringAsFixedString( BS, curMaterial->GetName() );
	//	MaterialArray[i].SetMaterialName( BS.ReadCString() );*/
	//	//MaterialArray[i].SetMaterialName( curMaterial->GetName() );

	//	//curNiMaterial->GetCurrentShader(

	//	// 임시
	//	/*char *strMaterial = new char[255];
	//	NiStream stream;
	//	stream.SaveFixedString( curMaterial->GetName() );*/
	//	//stream.LoadCString( strMaterial );
	//	//MaterialArray[i].SetMaterialName( strMaterial );
	//	//delete[] strMaterial;
	//}
//}
////
//int GamebryoGISceneManager::GetMaterialIndex( NiMaterial *pMaterial )
//{
//	//int foundIndex = -1;
//	for( unsigned int i = 0; i < mRTScene->GetMaterialCount(); i++ )
//	{
//		if( pMaterial->GetName().Equals( mRTScene->GetMaterialArray()[i].GetMaterialName() ) )
//		{
//			return i;
//		}
//	}
//	return -1;
//}
//
//int GamebryoGISceneManager::GetMaterialIndex( NiMaterialProperty *pMaterialProperty, NiTexturingProperty *pTextureProperty )
//{
//	int index = 0;
//	for( std::list<RtMaterial*>::iterator iter = mMaterialList.begin(); iter != mMaterialList.end(); ++iter )
//	{
//		RtMaterial *pMaterial = (*iter);
//		if( pMaterialProperty->GetName().Equals( pMaterial->GetMaterialName() ) )
//		{
//			return index;
//		}
//		index++;
//	}
//
//	RtMaterial *pMaterial = new RtMaterial;
//	mMaterialList.push_back( pMaterial );
//	pMaterial->SetMaterialName( (const char*)pMaterialProperty->GetName() );
//
//	RtVector4 AmbientColor( pMaterialProperty->GetAmbientColor().r, pMaterialProperty->GetAmbientColor().g, pMaterialProperty->GetAmbientColor().b );
//	RtVector4 DiffuseColor( pMaterialProperty->GetDiffuseColor().r, pMaterialProperty->GetDiffuseColor().g, pMaterialProperty->GetDiffuseColor().b );
//	RtVector4 SpecularColor( pMaterialProperty->GetSpecularColor().r, pMaterialProperty->GetSpecularColor().g, pMaterialProperty->GetSpecularColor().b );
//	float Shininess = pMaterialProperty->GetShineness();
//
//	pMaterial->SetAmbientColor( AmbientColor );
//	pMaterial->SetDiffuseColor( DiffuseColor );
//	pMaterial->SetSpecularColor( SpecularColor );
//	pMaterial->SetShininess( Shininess );
//
//	if( pTextureProperty )
//	{
//		//pTextureProperty->GetBaseMap()->GetTexture()->Get
//		RtTexture *pAlbedoTexture = new RtTexture();
//		FetchTextureData( pAlbedoTexture, pTextureProperty->GetBaseTexture() );
//		pMaterial->SetAlbedoTexture( pAlbedoTexture );
//	}
//	
//	return index;
//}
//
RtTexture* GamebryoGISceneManager::FetchTextureData( NiTexture *inTexture )
{
	NiSourceTexture *pSourceTexture = NiDynamicCast( NiSourceTexture, inTexture );

	const NiFixedString fs = pSourceTexture->GetFilename();

	if( pSourceTexture == NULL )
		return NULL;
	
	if( pSourceTexture->GetSourceRendererDataIsPersistent() )
	{
		NiPersistentSrcTextureRendererData *pPSTRD= pSourceTexture->GetPersistentSourceRendererData();
		NiDX9PersistentSrcTextureRendererData* pkPersistentSrcRendererData = (NiDX9PersistentSrcTextureRendererData*)pSourceTexture->GetPersistentSourceRendererData();
		NIASSERT( pPSTRD->GetPixels() != NULL );
		//pPSTRD->GetPixels()[0] = 0;
		//pPSTRD->GetPixels()
		return NULL;
	}

	RtTexture *outTexture = new RtTexture;
	
	////pBaseTexture->SetStatic( false );
	pSourceTexture->LoadPixelDataFromFile();
	//unsigned int Width = pSourceTexture->GetWidth();
	//unsigned int Height = pSourceTexture->GetHeight();
	//outTexture->Initialize( Width, Height, RtTexture::ETF_RGB );
	//pMaterial->SetAlbedoTexture( pAlbedoTexture );

	NiPixelData *pPixelData = pSourceTexture->GetSourcePixelData();
	if( pPixelData != NULL )
	{
		//unsigned char *pPixelData = pPixelData->GetPixels();
		outTexture->Initialize( inTexture->GetWidth(), inTexture->GetHeight(), RtTexture::ETF_RGBA );
		/*for( unsigned int y = 0; y > inTexture->GetHeight(); y++ )
			for( unsigned int x = 0; x > inTexture->GetWidth(); x++ )*/
		{
			//unsigned char *pPixel = pSourceTexture->GetSourcePixelData()->operator()(x, y );

			//NiPixelData::

			//(*pPixelData)( x, y );
			/*NiPixelData::GetPixelStride(
			pPixelData->*/
			/*outTexture->GetData()[(x + y*inTexture->GetWidth())*4] =
			pPixelData[*/
			
		}
		//pPixelData->GetPixelFormat().GetPaletteHasAlpha()
		NiPixelData *pPixelDataTemp = NiNew NiPixelData( pPixelData->GetWidth(), pPixelData->GetHeight(), NiPixelFormat::RGBA32 );
		NiImageConverter::GetImageConverter()->ConvertPixelData( *pPixelData, NiPixelFormat::RGBA32, pPixelDataTemp, false );
		memcpy( outTexture->GetData(), pPixelDataTemp->GetPixels(), outTexture->GetWidth() * outTexture->GetHeight() * sizeof(BYTE) * 4 );

		return outTexture;
	}
	
	bool t = pSourceTexture->GetSourceRendererDataIsPersistent();
	NiPersistentSrcTextureRendererData *pPSTRD= pSourceTexture->GetPersistentSourceRendererData();
	if( pPSTRD == NULL ) 
	{
		NiTexture::RendererData *pRD = pSourceTexture->GetRendererData();
		
		/*if( pRD->GetReplacementData( pBaseTexture ) != NULL )
		{
			pPixelData = pRD->GetReplacementData( pBaseTexture );
		}*/

		assert( NiIsKindOf( NiDX9TextureData, pRD ) );

		NiDX9TextureData *pDX9TextureData = (NiDX9TextureData*)pRD; 
		NiDX9SourceTextureData *pDX9SourceTextureData = pDX9TextureData->GetAsSourceTexture();
		D3DBaseTexturePtr pD3DBaseTexture = pDX9SourceTextureData->GetD3DTexture();
		assert( pD3DBaseTexture );
		/*pDX9SourceTextureData->
		pD3DBaseTexture->*/

		D3DTexturePtr pkD3DTexture = (D3DTexturePtr)pD3DBaseTexture;
		BYTE *pDate = CreatePixelDataFromD3DTexture( pkD3DTexture );
		outTexture->Initialize( inTexture->GetWidth(), inTexture->GetHeight(), RtTexture::ETF_RGBA );
		if( pDate != NULL )
			memcpy( outTexture->GetData(), pDate, outTexture->GetWidth() * outTexture->GetHeight() * sizeof(BYTE) * 4 );

		//outTexture->GetData()[0] = 0;
		return outTexture;

		/*ID3D10Texture2D::GetDesc( &
			ID3D9Texture2D::
		
		LPDIRECT3DBASETEXTURE9::GetPrivateData
		pD3DBaseTexture->GetPrivateData()*/

		/*NiDX9TextureData
		pPSTRD = (NiDX9TextureData*)pRD;*/
	}
	//int a= 0;
	return outTexture;
	
	//if( false )
	//{
	//	//BaseTexture->GetPixelFormat();
	//	assert( pPixelData != NULL );

	//	unsigned char *pPixels = pPixelData->GetPixels();
	//	assert( pPixels != NULL );
	//	unsigned int PixelStride = pPixelData->GetPixelStride();

	//	for( unsigned int y = 0; y < Height; y++ )
	//		for( unsigned int x = 0; x < Width; x++ )
	//	{
	//		pAlbedoTexture->GetData()[(x + y * Width)*3+0] = pPixels[(x + y * Width)*PixelStride+0];
	//		pAlbedoTexture->GetData()[(x + y * Width)*3+1] = pPixels[(x + y * Width)*PixelStride+1];
	//		pAlbedoTexture->GetData()[(x + y * Width)*3+2] = pPixels[(x + y * Width)*PixelStride+2];
	//	}
	//}
}

//bool GamebryoGISceneManager::BuildStructure( const char *strStructureFilename )
//{
//	if( mKDTree != NULL )
//		delete mKDTree;
//	mKDTree = GetRaytracer()->BuildStructure( mRTScene, strStructureFilename );
//	//mKDTree = GetRaytracer()->BuildStructure( mRTScene, false );
//	return mKDTree!=NULL;
//}

//void GamebryoGISceneManager::Render( NiCamera *pCamera, unsigned int Width, unsigned int Height, /* AcclStructure, */ float *outPixelData, unsigned int ThreadCount /*= 1*/ )
//{
//	NIASSERT( pCamera != NULL && outPixelData != NULL );
//
//	RtCamera curRTCamera;
//	ConvertCamera( pCamera, Width, Height, &curRTCamera );
//
//	unsigned int RayCount = Width * Height;
//
//	if( mKDTree == NULL || mKDTree->IsBuilt() == false )
//	{
//		for( unsigned int i = 0; i < RayCount; i++ )
//		{
//			outPixelData[i*4+0] = 0.0f;
//			outPixelData[i*4+1] = 0.0f;
//			outPixelData[i*4+2] = 0.0f;
//			outPixelData[i*4+3] = 0.0f;
//		}
//		return;
//	}
//	assert( mKDTree != NULL && mKDTree->IsBuilt() );
//
//	RtRay *RayArray = new RtRay[RayCount];
//	GetRaytracer()->GenerateRays( RayArray, &curRTCamera );
//
//	if( ThreadCount <= 1 )
//	{
//		RtVector4 RenderedColor = GetRaytracer()->BackgroundColor;
//		for( unsigned int i = 0; i < RayCount; i++ )
//		{
//			// Linear
//			//GetRaytracer()->ShootRay( RayArray[i], &RenderedColor );
//			// KD-Tree
//			GetRaytracer()->ShootRay( RayArray[i], &RenderedColor, mKDTree );
//			RenderedColor.CopyToFloatArray( &outPixelData[i*4] );
//		}
//	}
//	else
//	{
//		GetRaytracer()Procedure = NiNew RaytracerProcedure[ThreadCount];
//
//		unsigned int RayCountPerThread = RayCount/ThreadCount;
//		int RestRayCount = (int(RayCount) - int(RayCountPerThread * ThreadCount));
//		NIASSERT( 0 <= RestRayCount );
//
//		RaytracerProcedure::GetRaytracer() = this->GetRaytracer();
//		RaytracerProcedure::mKDTree = this->mKDTree;
//
//		NiThread **ThreadArray = new NiThread*[ThreadCount];
//		unsigned int RayIndex = 0;
//		for( unsigned int i = 0; i < ThreadCount; i++ )
//		{
//			RaytracerProcedure *pRaytracerProcedure = &((RaytracerProcedure*)GetRaytracer()Procedure)[i];
//
//			pRaytracerProcedure->RayArray = &RayArray[RayIndex];
//			pRaytracerProcedure->RayCount = RayCountPerThread;
//			pRaytracerProcedure->outPixel = &outPixelData[RayIndex*4];
//
//			ThreadArray[i] = NiThread::Create( pRaytracerProcedure );
//			RayIndex+= RayCountPerThread;
//		}
//		RaytracerProcedure *pLastRaytracerProcedure = &((RaytracerProcedure*)&GetRaytracer()Procedure)[ThreadCount-1];
//		pLastRaytracerProcedure->RayCount += RestRayCount;
//		
//		for( unsigned int i = 0; i < ThreadCount; i++ )
//			ThreadArray[i]->Resume();
//
//		for( unsigned int i = 0; i < ThreadCount; i++ )
//		{
//			ThreadArray[i]->WaitForCompletion();
//		}
//		delete[] ThreadArray;
//	}
//}

//NiColor GamebryoGISceneManager::CalcIrradiance( NiPoint3 Position, NiPoint3 Direction, unsigned int RayCount, unsigned int ThreadCount )
//{
//	RtRay *RayArray = new RtRay[RayCount];
//	float *CosWeightArray = new float[RayCount];
//	GenerateHemisphericalSampleArray( CosWeightArray, RayArray, ConvertVector( Position ), ConvertVector( Direction ), RayCount );
//
//	/*_CrtMemState s1;
//	_CrtMemCheckpoint( &s1 );
//
//	static int MemoryIndex = 0;
//	static _CrtMemState previous;
//	if( MemoryIndex == 0 )
//	{
//		_CrtMemCheckpoint( &previous );
//		MemoryIndex++;
//	}
//	else if( MemoryIndex == 1 )
//	{
//		_CrtMemState current, difference;
//		_CrtMemCheckpoint( &current );
//		_CrtMemDifference( &difference, &previous, &current );
//		_CrtMemDumpStatistics( &difference );
//	}*/
//
//	mIrradianceProcedure = NiNew IrradianceProcedure[ThreadCount];
//
//	unsigned int RayCountPerThread = RayCount/ThreadCount;
//	int RestRayCount = (int(RayCount) - int(RayCountPerThread * ThreadCount));
//	NIASSERT( 0 <= RestRayCount );
//
//	float *outPixel = new float[ThreadCount*4];
//	for( unsigned int i = 0; i < ThreadCount*4; i++ )
//		outPixel[i] = 0.0f;
//
//	IrradianceProcedure::GetRaytracer() = this->GetRaytracer();
//	IrradianceProcedure::mKDTree = this->mKDTree;
//
//	NiThread **ThreadArray = new NiThread*[ThreadCount];
//	unsigned int RayIndex = 0;
//	for( unsigned int i = 0; i < ThreadCount; i++ )
//	{
//		IrradianceProcedure *pIrradianceProcedure = &((IrradianceProcedure*)mIrradianceProcedure)[i];
//
//		pIrradianceProcedure->CosWeightArray = CosWeightArray;
//		pIrradianceProcedure->RayArray = &RayArray[RayIndex];
//		pIrradianceProcedure->RayCount = RayCountPerThread;
//		pIrradianceProcedure->outPixel = &outPixel[i*4];
//
//		ThreadArray[i] = NiThread::Create( pIrradianceProcedure );
//		RayIndex += RayCountPerThread;
//	}
//	IrradianceProcedure *pLastRaytracerProcedure = &((IrradianceProcedure*)&mIrradianceProcedure)[ThreadCount-1];
//	pLastRaytracerProcedure->RayCount += RestRayCount;
//
//	for( unsigned int i = 0; i < ThreadCount; i++ )
//		ThreadArray[i]->Resume();
//
//	for( unsigned int i = 0; i < ThreadCount; i++ )
//	{
//		ThreadArray[i]->WaitForCompletion();
//		NiDelete ThreadArray[i];
//	}
//	delete[] ThreadArray;
//	NiDelete[] mIrradianceProcedure;
//	delete[] RayArray;
//	
//	float rec = 1.0f/(float(RayCount));
//	NiColor ReturnColor( 0.0f, 0.0f, 0.0f );
//	for( unsigned int i = 0; i < ThreadCount; i++ )
//	{
//		ReturnColor.r += outPixel[i*4+0];
//		ReturnColor.g += outPixel[i*4+1];
//		ReturnColor.b += outPixel[i*4+2];
//	}
//	ReturnColor.r *= rec;
//	ReturnColor.g *= rec;
//	ReturnColor.b *= rec;
//	delete[] outPixel;
//
//	return ReturnColor;
//}
//
RtCamera* GamebryoGISceneManager::ConvertCamera( NiCamera *pkCamera, unsigned int Width, unsigned int Height )
{
	NIASSERT( pkCamera != NULL );

	RtCamera *pRtCamera = new RtCamera;

	pRtCamera->Width = Width;
	pRtCamera->Height = Height;
	
	pRtCamera->NearDistance = FLOAT_EPSILON*1.1f;
	pRtCamera->FarDistance = 1e16f;
	pRtCamera->Fovy = 45.0f;

	pRtCamera->Origin = RtVector3( pkCamera->GetWorldLocation().x, pkCamera->GetWorldLocation().y, pkCamera->GetWorldLocation().z );
	pRtCamera->Direction = RtVector3( pkCamera->GetWorldDirection().x, pkCamera->GetWorldDirection().y, pkCamera->GetWorldDirection().z );
	pRtCamera->UpDirection = RtVector3( pkCamera->GetWorldUpVector().x, pkCamera->GetWorldUpVector().y, pkCamera->GetWorldUpVector().z );
	return pRtCamera;
}