#pragma once

#include "Transform.h"

BEGIN(Engine)
class ENGINE_DLL CGameObject abstract :
    public CBase
{
public:
    typedef struct tGameObjectDesc: public CTransform::TRANSFORM_DESC
    {
        _uint iObjectIndex;
    } GAMEOBJECT_DESC;

protected:
    CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject(const CGameObject& rhs);
    virtual ~CGameObject() = default;

public:
    class CTransform* m_pTransformCom = { nullptr };

public:
    _bool isDead() { return m_isDead; }
    _bool isObjectDead() { return m_isObjectDead; }

public:
    void        Set_Dead() { m_isDead = true; }                 // °´Ã¼ ÀÚÃ¼°¡ Á×¾úÀ» ¶§ (°´Ã¼¸¦ »èÁ¦ÇÑ´Ù)
    void        Set_ObjectDead() { m_isObjectDead = true; }     // °´Ã¼°¡ Á×¾ú´ÂÁö (°´Ã¼ »èÁ¦X)
    float       Get_ObjID() { return m_iObjectIndex; }
    void        Set_ObjID(float fID) { m_iObjectIndex = fID; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Tick(const _float& fTimeDelta);
    virtual void Tick(const _float& fTimeDelta);
    virtual void Late_Tick(const _float& fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT Render_Bloom() { return S_OK; }
    virtual HRESULT Render_LightDepth() { return S_OK; }

public:
    class CComponent* Get_Component(wstring strComponentTag);
    CTransform* Get_TransformCom() { return m_pTransformCom; }

protected:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

protected:
    map<const wstring, class CComponent*> m_Components;

protected:
    _uint m_iCurrentLevel = { 0 };
    _bool m_isDead = { false };
    _bool m_isObjectDead = { false };

    _uint   m_iObjectIndex = { 0 };

protected:
    vector<_float> m_Casecade;

protected:
    HRESULT Add_Component(_uint iLevelIndex, const wstring strComponentPrototypeTag, const wstring strComponentTag, class CComponent** pComponent, void* pArg = nullptr);

private:
    class CComponent* Find_Component(const wstring StrComponentTag);

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

END

