#include "UICarchase.h"

#include"GameInstance.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	

#include "CarChase_Monster.h"
#include "Highway_Taxi.h"   
#include "Highway_Kiryu.h"

CUICarchase::CUICarchase()
    :CUIScene{}
{
}

CUICarchase::CUICarchase(const CUICarchase& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUICarchase::Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP)
{
    CARCCHASE_UI_DESC* pTarget = Find_TargetUI(iIndex);
    if (nullptr == pTarget)
        return E_FAIL;

    _vector vTargetPos = TargetMatrix.r[3];
    pTarget->TargetingUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);

    vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) - 60.f);
    pTarget->HPBarBackUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);
    pTarget->HPBarUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);
    pTarget->HPBarUI->Get_TransformCom()->Set_Scale(iHP, 6.f, 0.f);

    return S_OK;
}

HRESULT CUICarchase::Remove_Target(_uint iIndex)
{
    CARCCHASE_UI_DESC* pTarget = Find_TargetUI(iIndex);
    if (nullptr == pTarget)
        return E_FAIL;

    Safe_Release(pTarget->HPBarBackUI);
    Safe_Release(pTarget->HPBarUI);
    //Safe_Release(pTarget->pMonsterAddr);
    Safe_Release(pTarget->TargetingUI);

    m_Targets.erase(iIndex);

    return S_OK;
}

HRESULT CUICarchase::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUICarchase::Tick(const _float& fTimeDelta)
{
    Update_Player();

    __super::Tick(fTimeDelta);

    m_EventUI[AIM]->Tick(fTimeDelta);
    m_EventUI[INFO]->Tick(fTimeDelta);

    Move_Aim(fTimeDelta);

    for (auto& pTarget : m_Targets)
    {
        pTarget.second.HPBarBackUI->Tick(fTimeDelta);
        pTarget.second.HPBarUI->Tick(fTimeDelta);
        pTarget.second.TargetingUI->Tick(fTimeDelta);
    }

    return S_OK;
}

HRESULT CUICarchase::Late_Tick(const _float& fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_EventUI[AIM]->Late_Tick(fTimeDelta);
    m_EventUI[INFO]->Late_Tick(fTimeDelta);

    for (auto& pTarget : m_Targets)
    {
        pTarget.second.HPBarBackUI->Late_Tick(fTimeDelta);
        pTarget.second.HPBarUI->Late_Tick(fTimeDelta);
        pTarget.second.TargetingUI->Late_Tick(fTimeDelta);
    }

    if (!m_isAnimFin)
        m_isAnimFin = true;

    Coll_Aim();

    return S_OK;
}

void CUICarchase::Action()
{
}

void CUICarchase::OverAction()
{
}

HRESULT CUICarchase::Add_TargetingRound(_uint iIndex, class CCarChase_Monster* pMonster)
{
    if (nullptr != Find_TargetUI(iIndex))
        return E_FAIL;

    CARCCHASE_UI_DESC Desc{};

    CUI_Object* pTargeting = dynamic_cast<CUI_Object*>(m_EventUI[TARGETING]->Clone(nullptr));
    if (nullptr == pTargeting)
        return E_FAIL;

    CUI_Object* pHpBarBack = dynamic_cast<CUI_Object*>(m_EventUI[HPBAR_BACK]->Clone(nullptr));
    if (nullptr == pHpBarBack)
        return E_FAIL;

    CUI_Object* pHpBar = dynamic_cast<CUI_Object*>(m_EventUI[HPBAR]->Clone(nullptr));
    if (nullptr == pHpBar)
        return E_FAIL;

    Desc.TargetingUI = pTargeting;
    Desc.HPBarBackUI = pHpBarBack;
    Desc.HPBarUI = pHpBar;
    Desc.pMonsterAddr = pMonster;

    m_Targets.emplace(iIndex, Desc);
    //Safe_AddRef(pMonster);
    
    return S_OK;
}

HRESULT CUICarchase::Update_Player()
{
    _uint Level = m_pGameInstance->Get_CurrentLevel();

    CHighway_Taxi* Taxi = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(Level, TEXT("Layer_Taxi"), 0));

    CHighway_Kiryu* pKiryu = static_cast<CHighway_Kiryu*>(Taxi->Get_Kiryu());

    //���� ���� + ui ������Ʈ
    //1 . ĳ���� , 2��Ʈ���� , 3�ڵ����� , 4�Ѿ�
    vector<CUI_Object*>* pUI =  dynamic_cast<CGroup*>(m_EventUI[INFO])->Get_pPartObjects();

    _float Upoint = m_pGameInstance->Lerp(-1.f, 0.f, pKiryu->Get_HP() / pKiryu->Get_MaxHP());
    dynamic_cast<CUI_Texture*>((*pUI)[PLAYERHP])->Change_Point(_float4(0.f,0.f,Upoint,0.f),_float4(0.f,0.f,Upoint,0.f));

    Upoint = m_pGameInstance->Lerp(-1.f, 0.f, pKiryu->Get_HitEye() / pKiryu->Get_MaxHitEye());
    dynamic_cast<CUI_Texture*>((*pUI)[HITEYE])->Change_Point(_float4(0.f,0.f,Upoint,0.f),_float4(0.f,0.f,Upoint,0.f));

    Upoint = m_pGameInstance->Lerp(-1.f, 0.f, Taxi->Get_CarHp() / Taxi->Get_CarMaxHp());
    dynamic_cast<CUI_Texture*>((*pUI)[CARHP])->Change_Point(_float4(0.f, 0.f, Upoint, 0.f), _float4(0.f, 0.f, Upoint, 0.f));

    Upoint = m_pGameInstance->Lerp(-1.f, 0.f, Taxi->Get_CarHp() / Taxi->Get_CarMaxHp());

    wstring NumAmmo = to_wstring(pKiryu->Get_Ammo());
    dynamic_cast<CText*>((*pUI)[AMMO])->Set_Text(NumAmmo);

    return S_OK;
}

void CUICarchase::Move_Aim(const _float& fTimeDelta)
{
    _vector vPos = m_EventUI[AIM]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

    if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
    {
        _float fY = XMVectorGetY(vPos);
        fY += fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetY(vPos, fY);
    }
    if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
    {
        _float fX = XMVectorGetX(vPos);
        fX += fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetX(vPos, fX);
    }
    if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
    {
        _float fX = XMVectorGetX(vPos);
        fX -= fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetX(vPos, fX);
    }
    if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
    {
        _float fY = XMVectorGetY(vPos);
        fY -= fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetY(vPos, fY);
    }

    m_EventUI[AIM]->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
}

CUICarchase::CARCCHASE_UI_DESC* CUICarchase::Find_TargetUI(_uint iIndex)
{
    auto pTarget = m_Targets.find(iIndex);
    
    if (pTarget == m_Targets.end())
        return nullptr;

    return &(pTarget->second);
}

//�浹����?
void CUICarchase::Coll_Aim()
{
    if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
    {

        //�Ʒ��� ui���� Ȯ���� �ڵ� ��������
        //_uint Level = m_pGameInstance->Get_CurrentLevel();

        //CHighway_Taxi* Taxi = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(Level, TEXT("Layer_Taxi"), 0));

        //CHighway_Kiryu* pKiryu = static_cast<CHighway_Kiryu*>(Taxi->Get_Kiryu());

        //pKiryu->Shot();
        //pKiryu->Damage();

        _float4 vAimPos;
        XMStoreFloat4(&vAimPos, m_EventUI[AIM]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
        _float3 vAimScale = m_EventUI[AIM]->Get_TransformCom()->Get_Scaled();
        //������ ��Ȯ�ϰ� ���� �־�� ��.
        for (auto& pTarget : m_Targets)
        {
            _float4 vTargetPos;
            XMStoreFloat4(&vTargetPos, pTarget.second.TargetingUI->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

            _float3 vTargetScale;
            vTargetScale = pTarget.second.TargetingUI->Get_TransformCom()->Get_Scaled();

            _float fAimMinX = vAimPos.x - vAimScale.x * 0.5f;
            _float fAimMaxX = vAimPos.x + vAimScale.x * 0.5f;

            _float fAimMinY = vAimPos.y - vAimScale.y * 0.5f;
            _float fAimMaxY = vAimPos.y + vAimScale.y * 0.5f;

            _float fTargetMinX = vTargetPos.x - vTargetScale.x * 0.5f;
            _float fTargetMaxX = vTargetPos.x + vTargetScale.x * 0.5f;

            _float fTargetMinY = vTargetPos.y - vTargetScale.y * 0.5f;
            _float fTargetMaxY = vTargetPos.y + vTargetScale.y * 0.5f;

            if (fAimMinX > fTargetMinX && fAimMaxX < fTargetMaxX
                && fAimMinY > fTargetMinY && fAimMaxY < fTargetMaxY)
            {
                //�浹
                pTarget.second.TargetingUI->Show_UI();
                pTarget.second.pMonsterAddr->Set_Coll();
                
                return;
            }
        }
    }
}

CUICarchase* CUICarchase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUICarchase* pInstance = new CUICarchase();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        MSG_BOX("Make Failed :CUICarchase");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUICarchase::Free()
{
    __super::Free();

    for (auto& pTarget : m_Targets)
    {
        Safe_Release(pTarget.second.HPBarBackUI);
        Safe_Release(pTarget.second.HPBarUI);
        //Safe_Release(pTarget.second.pMonsterAddr);
        Safe_Release(pTarget.second.TargetingUI);
    }
    m_Targets.clear();
    
}
