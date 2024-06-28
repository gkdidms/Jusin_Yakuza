#pragma once

#include "GameObject.h"

#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CVIBuffer_Cube;
class CCollider;
END

BEGIN(Client)

class CDecal final :
    public CGameObject
{
public:
    typedef struct tDecalObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        int         iMaterialNum;
        CTexture* pTexture;
        XMMATRIX    vStartPos;
    }DECALOBJ_DESC;

protected:
    CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDecal(const CDecal& rhs);
    virtual ~CDecal() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

    /* 모델 로컬 공간으로 Decal 이동시켜야해서 world matrix 받아오기 */
public:
    void            Get_Model_Transform(CTransform* pTransform) { m_pModelTransformCom = pTransform; Safe_AddRef(m_pModelTransformCom); }


private:
    CTexture*       m_pTextureCom = { nullptr };
    CShader*        m_pShaderCubeCom = { nullptr };
    CShader*        m_pShaderPosCom = { nullptr };
    CVIBuffer_Cube* m_pVIBufferCubeCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferRectCom = { nullptr };
    CTransform*     m_pModelTransformCom;

private:
    int                 m_iMaterialNum;

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

END