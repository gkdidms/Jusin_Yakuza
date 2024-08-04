#include "Highway_Kiryu.h"

#include "GameInstance.h"
#include "Anim.h"
#include "Mesh.h"
#include "Bone.h"

#include "SocketModel.h"
#include "Gun_Cz75.h"

CHighway_Kiryu::CHighway_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CHighway_Kiryu::CHighway_Kiryu(const CHighway_Kiryu& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CHighway_Kiryu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHighway_Kiryu::Initialize(void* pArg)
{
	CARCHASE_KIRYU_DESC* pDesc = static_cast<CARCHASE_KIRYU_DESC*>(pArg);
	pTaxiMatrix = pDesc->pTaxiWorldMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_wstrModelName = TEXT("Kiryu_CarChase");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	if (FAILED(Add_Objects()))
		return E_FAIL;

	Set_HandAnimIndex(HAND_GUN);
	On_Separation_Hand(m_isLeft ? 1 : 2);

	return S_OK;
}

void CHighway_Kiryu::Priority_Tick(const _float& fTimeDelta)
{
	if(m_isLeft)
		m_pGun_L->Priority_Tick(fTimeDelta);
	else
		m_pGun_R->Priority_Tick(fTimeDelta);
}

void CHighway_Kiryu::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Key_Input();

	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
	Play_CurrentAnimation(fTimeDelta);

	m_ModelMatrix = *pTaxiMatrix;

	if(m_isLeft)
		m_pGun_L->Tick(fTimeDelta);
	else
		m_pGun_R->Tick(fTimeDelta);
}

void CHighway_Kiryu::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if(m_isLeft)
		m_pGun_L->Late_Tick(fTimeDelta);
	else
		m_pGun_R->Late_Tick(fTimeDelta);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CHighway_Kiryu::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //����
		else
			m_pShaderCom->Begin(0);		//����Ʈ

		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

void CHighway_Kiryu::Change_Animation()
{
}

void CHighway_Kiryu::Key_Input()
{
	/* ���� ��ġ �����ϴ� Ű ��ǲ */
	if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
	{

	}
	if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
	{

	}
	if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
	{

	}
	if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
	{
	}

	// �߻�
	if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
	{
		Change_Behavior(SHOT);
	}

	// ��Ʈ���� ���
	if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == HOLD)
	{
		if (0 >= m_fHitEye)
		{
			m_fHitEye = 0.f;
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
		}
		else
		{
			m_isHitEyeCharging = false;
			m_fHitEye -= HITEYE_DECREASE_SPEED * m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player"));
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.2f);
		}
	}
	else if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == AWAY)
	{
		m_isHitEyeCharging = true;
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	}

	if (m_isHitEyeCharging)
	{
		if (100.f < m_fHitEye)
			m_fHitEye = 100.f;
		m_fHitEye += HITEYE_DECREASE_SPEED * m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player"));
	}

	// ����/����
	if (m_pGameInstance->GetKeyState(DIK_E) == HOLD)
	{
		Change_Behavior(HIDE);
	}
	else if (m_pGameInstance->GetKeyState(DIK_E) == AWAY)
	{
		Change_Behavior(AIMING);
	}

	// �ڸ� �ٲٱ� (�÷��̾ ���� �����ϴ� ���� �ƴ϶� ���������� �߰��� ��ư)
	if (m_pGameInstance->GetKeyState(DIK_C) == HOLD)
	{
		Change_Behavior(SWAP);
	}
}

void CHighway_Kiryu::Play_CurrentAnimation(_float fTimeDelta)
{
	switch (m_eCurrentBehavior)
	{
	case CHighway_Kiryu::AIMING:
	{
		Play_Animing(fTimeDelta);
		break;
	}
	case CHighway_Kiryu::HIDE:
	{
		Play_HideReload(fTimeDelta);
		break;
	}
	case CHighway_Kiryu::HIT:
	{
		Play_Hit(fTimeDelta);
		break;
	}
	case CHighway_Kiryu::SHOT:
	{
		Play_Shot(fTimeDelta);
		break;
	}
	case CHighway_Kiryu::SWAP:
	{
		Play_Swap(fTimeDelta);
		break;
	}
	}

	//if (m_bTest)
	//{
	//	m_pModelCom->Play_Animation_Rotation_SeparationBone(fTimeDelta, "kosi_c_n", 2, m_fTest, false);
	//}
	//else
		//m_pModelCom->Play_Animation(fTimeDelta, false);

	m_pModelCom->Play_Animation(fTimeDelta, false);

	// �ִϸ��̼��� �̻��ؼ�, �ϹݽŸ� Shot �ִ����� ���� �������Ѵ�.
	m_pModelCom->Play_Animation_Separation(fTimeDelta, (m_isLeft ? 31 : 66), m_pAnimCom, true, 3);
	m_pModelCom->Play_Animation_Separation(fTimeDelta, (m_isLeft ? 34 : 70), m_pAnimCom, true, 4);
}

void CHighway_Kiryu::Play_Animing(_float fTimeDelta)
{
	// ���� ��� (������ �Ͼ�� ��)
	// [71] [mngcar_c_car_gun_sitl_en]
	// [74] [mngcar_c_car_gun_sitr_en]
	// 
	// [5] [mngcar_c_car_gun_aiml_b_hi]
	// [16] [mngcar_c_car_gun_aiml_f_hi]
	// [27] [mngcar_c_car_gun_aiml_l_hi]
	// [40] [mngcar_c_car_gun_aimr_b_hi]
	// [51] [mngcar_c_car_gun_aimr_f_hi]
	// [62] [mngcar_c_car_gun_aimr_r_hi]
	// 
	// [7] [mngcar_c_car_gun_aiml_b_lp]
	// [6] [mngcar_c_car_gun_aiml_b_l45]
	// [29] [mngcar_c_car_gun_aiml_l_lp]		// �� ��Ǹ� ���� �߸��Ȱ��� �㸮�� ��Ʋ������
	// [18] [mngcar_c_car_gun_aiml_f_r45]
	// [17] [mngcar_c_car_gun_aiml_f_lp]
	// 
	// [42] [mngcar_c_car_gun_aimr_b_lp]
	// [41] [mngcar_c_car_gun_aimr_b_l45]
	// [64] [mngcar_c_car_gun_aimr_r_lp]
	// [53] [mngcar_c_car_gun_aimr_f_r45]
	// [52] [mngcar_c_car_gun_aimr_f_lp]

	// ���Ϳ� ���Ӷ��� ������ ���� ����������� �߰�����
	_uint iAnimIndex = 29;
	switch (m_iStageDir)
	{
	case DIR_F:
		iAnimIndex = (m_isLeft ? 17 : 52);
		break;
	case DIR_B:
		iAnimIndex = (m_isLeft ? 7 : 42);
		break;
	case DIR_L:
		iAnimIndex = (m_isLeft ? 29 : 64);
		break;
	case DIR_R:
		iAnimIndex = (m_isLeft ? 29 : 64);
		break;
	}

	if (!m_isStarted)
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 71 : 74), 4.f);
	else
		m_pModelCom->Set_AnimationIndex(iAnimIndex, 4.f);

	if (m_pModelCom->Get_AnimFinished())
		m_isStarted = true;
}

void CHighway_Kiryu::Play_HideReload(_float fTimeDelta)
{
	// ���� ��� (���� ���� ��)
	// [73] [mngcar_c_car_gun_sitl_st]
	// [76] [mngcar_c_car_gun_sitr_st]
	// [72] [mngcar_c_car_gun_sitl_lp]
	// [75] [mngcar_c_car_gun_sitr_lp]
	if (!m_isStarted)
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 73 : 76), 4.f);
	else
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 72 : 75), 4.f);

	if (m_pModelCom->Get_AnimFinished())
		m_isStarted = true;
}

void CHighway_Kiryu::Play_Hit(_float fTimeDelta)
{
	/*
	* [2] [mngcar_c_car_gun_aiml_b_dam]
	* [13] [mngcar_c_car_gun_aiml_f_dam]
	* [24] [mngcar_c_car_gun_aiml_l_dam]
	* 
	* [37] [mngcar_c_car_gun_aimr_b_dam]
	* [48] [mngcar_c_car_gun_aimr_f_dam]
	* [59] [mngcar_c_car_gun_aimr_r_dam]
	*/

	_uint iAnimIndex = 2;
	switch (m_iStageDir)
	{
	case DIR_F:
		iAnimIndex = (m_isLeft ? 13 : 48);
		break;
	case DIR_B:
		iAnimIndex = (m_isLeft ? 2 : 37);
		break;
	case DIR_L:
		iAnimIndex = (m_isLeft ? 24 : 59);
		break;
	case DIR_R:
		iAnimIndex = (m_isLeft ? 24 : 59);
		break;
	}

	m_pModelCom->Set_AnimationIndex(iAnimIndex, 4.f);

	if (m_pModelCom->Get_AnimFinished())
		Change_Behavior(AIMING);

}

void CHighway_Kiryu::Play_Shot(_float fTimeDelta)
{
	// [31] [mngcar_c_car_gun_aiml_l_shot]
	// [66] [mngcar_c_car_gun_aimr_r_shot]

	m_pModelCom->Set_AnimationIndex((m_isLeft ? 31 : 66), 4.f);

	if (m_pModelCom->Get_AnimFinished())
		Change_Behavior(AIMING);
}

void CHighway_Kiryu::Play_Swap(_float fTimeDelta)
{
	/*
	*	[77] [mngcar_c_car_gun_sync_aimltor]
		[78] [mngcar_c_car_gun_sync_aimrtol]
	*/
	if (!m_isStarted)
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 73 : 76), 4.f);
	else
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 77 : 78), 4.f);

	if (m_isStarted && m_pModelCom->Get_AnimFinished())
	{
		m_isStarted = true;
	}
	else if (m_pModelCom->Get_AnimFinished())
	{
		m_isLeft = !m_isLeft;
		Change_Behavior(AIMING);
	}

}

void CHighway_Kiryu::Change_Behavior(BEHAVIOR_TYPE eType)
{
	// ��� �� ���� �� �ʿ��� �ո� �ٽ� ����
	Off_Separation_Hand();

	// ��� �и� ����
	m_pModelCom->Set_Separation_ParentBone(m_isLeft ? "ude1_r_n" : "ude1_l_n", -1);

	switch (eType)
	{
	case CHighway_Kiryu::AIMING:
	{
		if (m_eCurrentBehavior != SHOT)
			m_isStarted = false;

		//m_pModelCom->Set_Separation_ParentBone("ketu_c_n", 3);
		break;
	}
	case CHighway_Kiryu::HIDE:
	{
		m_isStarted = false;

		//m_pModelCom->Set_Separation_ParentBone("ketu_c_n", -1);
		break;
	}
	case CHighway_Kiryu::HIT:
	{
		m_isStarted = true;
		//m_pModelCom->Set_Separation_ParentBone("ketu_c_n", 3);
		break;
	}
	case CHighway_Kiryu::SHOT:
	{
		m_isStarted = true;
		//m_pModelCom->Set_Separation_ParentBone("ketu_c_n", 3);

		// ��� �и�
		m_pModelCom->Set_Separation_ParentBone(m_isLeft ? "ude1_r_n" : "ude1_l_n", 4);
		m_pModelCom->Set_Separation_SingleBone(m_isLeft ? "ude1_r_n" : "ude1_l_n", -1);
		break;
	}
	case CHighway_Kiryu::SWAP:
	{
		m_isStarted = false;
		//m_pModelCom->Set_Separation_ParentBone("ketu_c_n", -1);
		break;
	}
	}

	m_eCurrentBehavior = eType;

	On_Separation_Hand(m_isLeft ? 1 : 2);
}

void CHighway_Kiryu::HideReload()
{

}

HRESULT CHighway_Kiryu::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kiryu_CarChase"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//Prototype_Component_Anim_KiryuFace
	CAnim* pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_KiryuFace"),
		TEXT("Com_Anim_Face"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//Prototype_Component_Anim_Hand
	pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_Hand"),
		TEXT("Com_Anim_Hand"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//Prototype_Component_Kiryu_CarChase
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Kiryu_CarChase"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CHighway_Kiryu::Add_Objects()
{
	CSocketObject::SOCKETOBJECT_DESC Desc{};
	Desc.pParentMatrix = pTaxiMatrix;
	Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
	Desc.fRotatePecSec = XMConvertToRadians(90.f);
	Desc.fSpeedPecSec = 1.f;
	m_pGun_R = dynamic_cast<CGun_Cz75*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Gun_Cz75"), &Desc));

	Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_l_n");
	m_pGun_L = dynamic_cast<CGun_Cz75*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Gun_Cz75"), &Desc));

	return S_OK;
}

HRESULT CHighway_Kiryu::Bind_ResourceData()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelMatrix)))
		return E_FAIL;
	/*
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", pTaxiMatrix)))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CHighway_Kiryu* CHighway_Kiryu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHighway_Kiryu* pInstance = new CHighway_Kiryu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CHighway_Kiryu::Clone(void* pArg)
{
	CHighway_Kiryu* pInstance = new CHighway_Kiryu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHighway_Kiryu::Free()
{
	__super::Free();

	Safe_Release(m_pGun_R);
	Safe_Release(m_pGun_L);
	Safe_Release(m_pAnimCom);
}

string CHighway_Kiryu::Get_CurrentAnimationName()
{
	return m_pModelCom->Get_AnimationName(m_pModelCom->Get_CurrentAnimationIndex());
}
