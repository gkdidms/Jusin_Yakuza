#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject_Manager :
    public CBase
{
private:
    CGameObject_Manager();
    virtual ~CGameObject_Manager() = default;

public:
    HRESULT Initialize(_uint iMaxLevel);
    void Priority_Tick(const _float& fTimeDelta);
    void Tick(const _float& fTimeDelta);
    void Late_Tick(const _float& fTimeDelta);
    void Render();

public:
    HRESULT Add_GameObject_Prototype(const wstring strGameObjectTag, class CGameObject* pGameObject);
    HRESULT Add_GameObject(_uint iLevelIndex, const wstring strGameObjectTag, const wstring strLayerTag, void* pArg);
    class CGameObject* Clone_Object(const wstring strGameObjectTag, void* pArg);
    void Clear(_uint iLevelIndex);

public:
    class CComponent* Get_GameObject_Component(_uint iLevelIndex, const wstring strLayerTag, const wstring strComponentTag, _uint iIndex);
    class CGameObject* Get_GameObject(_uint iLevelIndex, const wstring strLayerTag, _uint iIndex);
    vector<CGameObject*> Get_GameObjects(_uint iLevelIndex, const wstring strLayerTag);

private:
    map<const wstring, class CGameObject*> m_pPrototypes;
    map<const wstring, class CLayer*>* m_pLayers;

    _uint m_iMaxLevel = { 0 };

private:
    class CGameObject* Find_GameObject_Prototype(const wstring strGameObjectTag);
    class CLayer* Find_Layer(_uint iLevelIndex, const wstring strLayerTag);

public:
    static CGameObject_Manager* Create(_uint iMaxLevel);
    virtual void Free();
};
END
