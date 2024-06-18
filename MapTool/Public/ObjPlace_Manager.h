#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"
#include "GameObject.h"

class CObjPlace_Manager final : public CBase
{
	DECLARE_SINGLETON(CObjPlace_Manager);

private:
	CObjPlace_Manager();
	virtual ~CObjPlace_Manager() = default;

public:
	HRESULT					Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void					Priority_Tick(const _float& fTimeDelta);
	void					Tick(const _float& fTimeDelta);
	void					Late_Tick(const _float& fTimeDelta);
	void					Render();



private:
	/* object이름, cgameobject */
	multimap<wstring, CGameObject*>				m_GameObjects; /* 추가한 오브젝트 저장 */
	vector<char*>								m_ObjectNames; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_FileNames; /* 맵마다 저장한 bin 리스트들 */

	vector<char*>								m_Layers;
	vector<char*>								m_ModelNames;

private:
	_uint										m_iObjectNums = 0;


public:
	virtual void Free() override;
};

