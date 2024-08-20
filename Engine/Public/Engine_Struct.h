#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	struct ENGINE_DLL CAMERAOBJ_DESC
	{
		_float4 vEye, vFocus;
		_float fFovY, fAspect, fNear, fFar;
		_bool	bLerp; /* ���������Ұ��� */
		_float	fStayTime;
		_float	fMoveTime;
		_float	fMoveSpeed;
	};

	struct ENGINE_DLL CAMERAOBJ_IO
	{
		int					iCameraNum;
		bool				bFirstLerp;
		CAMERAOBJ_DESC*		pCamObjDesc;
	};

	struct ENGINE_DLL TRIGGER_DESC
	{
		XMFLOAT4X4				vTransform;
		int						iTriggerType; // Ʈ���� ����
		bool					bMoveScene; // cinemachine �� scene ��ȯ�Ǵ� ��� true
		int						iCineNum;
		int						iLevelNum; // ���������� �Ѿ ��� sceneNum �ֱ�
		int						iYonedaKnife;
		int						iTriggerID;
	};

	struct ENGINE_DLL TRIGGER_IO
	{
		int				iTriggerNum; // Ʈ���� ����
		TRIGGER_DESC*	pTriggers;
	};

	struct ENGINE_DLL ROUTE_IO
	{
		int				iCellNums;
		_float4			vPosition;
		int				iPointOption;
		int				iRouteNums;
		int*			pRouteID;
	};


	struct ENGINE_DLL DECAL_DESC_IO
	{
		int						iMaterialNum;
		XMFLOAT4X4				vTransform;
	};

	struct ENGINE_DLL COLLIDER_DESC
	{
		XMFLOAT4X4				vTransform;
		_float3					vCenter;
		_float3					vExtents;
		_float3					vQuaternion;
	};

	struct ENGINE_DLL COLLIDER_IO
	{
		int						iColliderNum;
		COLLIDER_DESC*			pColliderDesc;
	};

	// Object ���� Collider
	struct ENGINE_DLL OBJCOLLIDER_DESC
	{
		int						iColliderType; // OBB, AABB, SPHERE
		_float3					vCenter;
		_float3					vExtents;
		_float3					vQuaternion;
	};


	/* map ������� */
	struct ENGINE_DLL OBJECTPLACE_DESC
	{
		XMFLOAT4X4			vTransform;
		XMFLOAT4X4			vOffsetTransform;
		char				strLayer[MAX_PATH];
		char				strModelCom[MAX_PATH]; /* model �̸� */
		int					iShaderPassNum; /* shader pass ��ȣ */
		int					iObjType; /* object ���� ���� - construction, item ��.. */
		int					iObjPropertyType; /* object �� Ư¡ */

		int					iNPCDirection; /* object �� Ư¡ */
		int					iGroupMonster; //bool ����
		int					iGroupNum;


		int					iNaviNum;
		int					iNaviRoute;

		int					iDecalNum;
		DECAL_DESC_IO*		pDecals;

		int					iColliderNum;
		OBJCOLLIDER_DESC*	pObjColliders;
	};


	struct ENGINE_DLL MAP_TOTALINFORM_DESC
	{
		int						iLevelIndex;
		XMFLOAT2				vPlaneSize;
		int						iNumMapObj;
		OBJECTPLACE_DESC*		pMapObjDesc;
	};




	struct ENGINE_DLL DECAL_DESC
	{
		_char					sTextureFullPath[MAX_PATH];
		int						iMaterialNum;

		/* mesh ��� ����ϴ��� */
		int						iMeshNum;
		int*					pMeshIndices;
	};




	/* ������ �ʱ�ȭ�ϱ����� �ʿ��� ������ �������� ������. */
	typedef struct
	{
		bool			isWindowed;
		HWND			hWnd;
		HINSTANCE		hInst;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
	}ENGINE_DESC;


	//typedef struct tLightDesc
	//{
	//	enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

	//	TYPE		eType = { TYPE_END };

	//	XMFLOAT4	vDirection;
	//	XMFLOAT4	vPosition;
	//	float		fRange;

	//	/* ���� �⺻���� ����. */
	//	XMFLOAT4	vDiffuse;
	//	XMFLOAT4	vAmbient;
	//	XMFLOAT4	vSpecular;
	//}LIGHT_DESC;

	struct ENGINE_DLL LIGHT_DESC
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eType = { TYPE_END };

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		/* ���� �⺻���� ����. */
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	};

	struct ENGINE_DLL LIGHT_DESC_IO
	{
		int						iNumLights;
		LIGHT_DESC*				pLightDesc;
	};




	// Ÿ�Ӻ��� Value�� �����Ѵ�.
	struct KEYFRAME
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		double			Time;
	};

	/* Model Export�� ����ü��*/
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

	typedef struct ENGINE_DLL tVTXCUBE
	{
		XMFLOAT3 vPosition;  // 12����Ʈ

		XMFLOAT3 vTexcoord;  // 12����Ʈ

		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[2];

	}VTXCUBE;

	typedef struct ENGINE_DLL tVTXCUBEOcculusion
	{
		XMFLOAT3 vPosition;  // 12����Ʈ
		XMFLOAT3 vTexcoord;  // 12����Ʈ

		float    padding1;   // 4����Ʈ �е� (���� ����)
		float    padding2;   // 4����Ʈ �е� (���� ����)

		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[4];

	}VTXCUBE_OCCULUSION;


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

		XMUINT4 vBlendIndices; // �������� ������ �ִ� ���� �ε���
		XMFLOAT4 vBlendWeights; // �������� ������ �ִ� ���� ������ �ִ� ���� (����ġ)

		static const _uint iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	} VTXANIMMESH;

	typedef struct ENGINE_DLL tVexAnimBone
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;
		XMFLOAT3 vTangent;

		static const _uint iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	} VTXANIMBONE;

	typedef struct
	{
		char strMaterialName[MAX_PATH];
		class CTexture* pMaterialTextures[AI_TEXTURE_TYPE_MAX];
	} MESH_MATERIAL;

	typedef struct ENGINE_DLL tVtxMatRix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT4			vDirection;
		XMFLOAT2			vLifeTime;
		_float2					vRectSize;	
	}VTXMATRIX;

	typedef struct ENGINE_DLL tComputeMatRix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT4			vDirection;
		XMFLOAT2			vLifeTime;
		_float					vRectSize;
		_float4					vCamPos;
	}COMPUTEMATRIX;


	typedef struct ENGINE_DLL tVtxInstance_Rect
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_RECT;

	typedef struct ENGINE_DLL tVtxInstance_Point
	{
		static const unsigned int		iNumElements = { 9 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[9];
	}VTXINSTANCE_POINT;

	typedef struct ENGINE_DLL tVtxInstance_Mesh
	{
		static const unsigned int		iNumElements = { 11 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[11];
	}VTXINSTANCE_MESH;


	typedef struct ENGINE_DLL tagIndex32
	{
		_ulong  _0;
		_ulong  _1;
		_ulong  _2;

	}INDEX32;
}
#endif // Engine_Struct_h__
