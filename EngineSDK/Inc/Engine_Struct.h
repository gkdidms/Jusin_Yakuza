#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	/* 엔진을 초기화하기위해 필요한 데이터 여러개를 묶었다. */
	typedef struct
	{
		bool			isWindowed;
		HWND			hWnd;
		HINSTANCE		hInst;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
	}ENGINE_DESC;

	typedef struct tLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eType = { TYPE_END };

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		/* 빛의 기본적인 색상. */
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHT_DESC;

	// 타임별로 Value를 저장한다.
	struct KEYFRAME
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		double			Time;
	};

	/* Model Export용 구조체들*/
	struct BAiVertexWeight
	{
		_uint			mVertexId;
		_float			mWeight;
	};

	struct BAiFace
	{
		_uint* mIndices;
	};

	struct BAiNode
	{
		_char				mName[MAX_PATH];
		_int				mParentBoneIndex;
		XMFLOAT4X4			mTransformationMatrix;
	};

	struct VectorKey
	{
		_double				mTime;
		XMFLOAT3			mValue;
	};

	struct QuatKey
	{
		_double				mTime;
		XMFLOAT4			mValue;
	};

	struct BAiNodeAnim
	{
		_char				mName[MAX_PATH];
		_uint				mNumScalingKeys;
		_uint				mNumRotationKeys;
		_uint				mNumPositionKeys;

		VectorKey* mScalingKeys;
		QuatKey* mRotationKeys;
		VectorKey* mPositionKeys;
	};

	struct BAiBone
	{
		_char				mName[MAX_PATH];
		_uint				mNumWeights;

		BAiVertexWeight* mWeights;

		XMFLOAT4X4			mOffsetMatrix;
	};

	struct BAiMesh
	{
		_char				mName[MAX_PATH];
		_uint				mNumVertices;
		_uint				mNumFaces;

		_uint				mMaterialIndex;
		_uint				mNumBones;

		_float3* mVertices;
		_float3* mNormals;
		_float2* mTextureCoords[AI_MAX_NUMBER_OF_TEXTURECOORDS];
		_float3* mTangents;

		BAiFace* mFaces;
		BAiBone** mBones;
	};

	struct BAiAnimation
	{
		_char				mName[MAX_PATH];
		_double				mDuration;
		_double				mTicksPerSecond;
		_uint				mNumChannels;
		BAiNodeAnim** mChannels;
	};

	typedef struct ENGINE_DLL tVexPos
	{
		XMFLOAT3 vPosition;

		static const _uint iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	} VTXPOS;

	typedef struct ENGINE_DLL tVtxPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOINT;
	
	typedef struct ENGINE_DLL tVexPosTex 
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vTexcoord;

		static const _uint iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	} VTXPOSTEX;

	typedef struct ENGINE_DLL tVexNorTex
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;

		static const _uint iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	} VTXNORTEX;

	typedef struct ENGINE_DLL tVexMesh
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;
		XMFLOAT3 vTangent;

		static const _uint iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	} VTXMESH;

	typedef struct ENGINE_DLL tVexAnimMesh
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;
		XMFLOAT3 vTangent;

		XMUINT4 vBlendIndices; // 정점에게 영향을 주는 뼈의 인덱스
		XMFLOAT4 vBlendWeights; // 정점에게 영향을 주는 뼈의 영향을 주는 정도 (가중치)

		static const _uint iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	} VTXANIMMESH;

	typedef struct
	{
		class CTexture* pMaterialTextures[AI_TEXTURE_TYPE_MAX];
	} MESH_MATERIAL;

	typedef struct ENGINE_DLL tVtxMatRix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT2			vLifeTime;
	}VTXMATRIX;

	typedef struct ENGINE_DLL tVtxInstance_Rect
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_RECT;

	typedef struct ENGINE_DLL tVtxInstance_Point
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_POINT;

	typedef struct ENGINE_DLL tagIndex32
	{
		_ulong  _0;
		_ulong  _1;
		_ulong  _2;

	}INDEX32;
}
#endif // Engine_Struct_h__
