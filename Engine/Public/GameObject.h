#pragma once

#include "Transform.h"

BEGIN(Engine)
class ENGINE_DLL CGameObject abstract :
    public CBase
{
public:
    typedef struct tGameObjectDesc : public CTransform::TRANSFORM_DESC
    {
        _uint iObjectIndex;
    } GAMEOBJECT_DESC;

    enum TEXTURE_TYPE {
        DIFFUSE,
        NORMAL,
        SURFACE,
        OESHADER,
        SPECULAR,
        TEXTURE_TYPE_END
    };

    struct TEXTURE_OPTION {
        _int g_isRS = { true };
        _int g_isRD = { true };
        _int g_isRM = { true };
        _int g_isRT = { true };
        _int g_isNormal = { true };
        _int g_isMulti = { true };


        _float2 TextureSize[7];
    };

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
    void        Set_Dead() { m_isDead = true; }               // 객체 자체가 죽었을 때 (객체를 삭제한다)
    void        Set_ObjectDead() { m_isObjectDead = true; }   // 객체가 죽었는지 (객체 삭제X)
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
    virtual HRESULT Render_Compute() { return S_OK; }
    virtual void ImpulseResolution(CGameObject* pTargetObject, _float fDistance = 0.5f) {};

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

    //컴퓨트 셰이더 용 텍스쳐버퍼
    ID3D11Buffer* m_pTextureOptionBuffer = { nullptr };
    vector<ID3D11ShaderResourceView*> m_pBufferSRVTexture;
    vector<ID3D11UnorderedAccessView*> m_pBufferUAVTexture;

protected:
    HRESULT Add_Component(_uint iLevelIndex, const wstring strComponentPrototypeTag, const wstring strComponentTag, class CComponent** pComponent, void* pArg = nullptr);
    void Bind_TextureUAV(_uint iType);
    HRESULT Bind_TextureSRV(class CShader* m_pShader, const _char* pName, _uint iType);
    HRESULT Ready_Buffer();

private:
    class CComponent* Find_Component(const wstring StrComponentTag);


public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

END

