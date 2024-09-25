#pragma once
#include "Monster.h"

/*
* 의자 들고있는 몬스터
*/

// 소파 콜라이더는 애님툴에서 naka1_r_n 뼈에 붙임 (쇼파가 갖고있는거 X)

BEGIN(Client)
class CWPHYakuza :
    public CMonster
{
private:
    CWPHYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWPHYakuza(const CWPHYakuza& rhs);
    virtual ~CWPHYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    virtual HRESULT Add_Components() override;
    HRESULT Add_Objects();
    virtual void Change_Animation() override;

private:
    class CSofa* m_pSofa = { nullptr };

public:
    static CWPHYakuza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
