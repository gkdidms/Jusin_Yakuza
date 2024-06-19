#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
END

class CObjPlace_Manager final : public CBase
{
	DECLARE_SINGLETON(CObjPlace_Manager);

private:
	CObjPlace_Manager();
	virtual ~CObjPlace_Manager() = default;

public:
	HRESULT					Initialize();
	void					Priority_Tick(const _float& fTimeDelta);
	void					Tick(const _float& fTimeDelta);
	void					Late_Tick(const _float& fTimeDelta);
	void					Render();

public:
	void					Set_GameObjectsList(); /* imgui�� ������ ���̵� */
	void					Save_GameObject(int iLevel); /* ���� map�� ������ object ����*/
	void					Load_GameObject(int iNum);
	void					Edit_GameObject(int iNumObject);

	bool					Add_CloneObject_Imgui(_uint iLayerIndex, _uint iObjectIndex);
	/* Object ���� ���� */
	void					Set_Map_Object();


private:
	void					Load_ModelName();



private:
	string					modifyString(string& input);



private:
	/* object�̸�, cgameobject */
	multimap<wstring, CGameObject*>				m_GameObjects; /* �߰��� ������Ʈ ���� */
	vector<char*>								m_ObjectNames_Map0; /* �߰��� ������Ʈ �̸��� */
	vector<char*>								m_ObjectNames_Map1; /* �߰��� ������Ʈ �̸��� */
	vector<char*>								m_FileNames; /* �ʸ��� ������ bin ����Ʈ�� */

	vector<char*>								m_Layers;
	vector<char*>								m_ModelNames;

	class CGameInstance*						m_pGameInstance = { nullptr };

private:
	_uint										m_iObjectNums = 0;

	/* ��ġ���� bool ���� */
	_bool										m_bDoing_Place_Object = false;


public:
	virtual void Free() override;
};

