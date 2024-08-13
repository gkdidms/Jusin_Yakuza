#include "EffectManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffectManager)

CEffectManager::CEffectManager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CEffectManager::Blood_Effect(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit_Blood0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit_Blood1"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit_Blood2"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Blood_Splash(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_BloodSplash"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_HitSpark(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_CarHitSpark0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_CarHitSpark1"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_Fire(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_FireA"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_Explosion(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_ExpA"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Shot_Flash(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_ExpB"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Cine_NoseBlood(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Blood0"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Cine_MouseBlood(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Blood6"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_BackTrail(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Trail_CarBackLight"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_GlassBroke(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Mesh_glass0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Mesh_glass1"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Mesh_glass2"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Player_Attack_Effect(CEffect::EFFECT_DESC& EffectDesc)
{
    // �÷��̾� Ÿ�� ����Ʈ (== ���� �ǰ� ����Ʈ)
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Glow0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part2"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part3"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part4"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Distortion0"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Enemy_Attack_Effect(CEffect::EFFECT_DESC& EffectDesc)
{    
    // ���� Ÿ�� ����Ʈ (== �÷��̾� �ǰ� ����Ʈ)
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part1"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part2"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part3"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Glow0"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Free()
{
    Safe_Release(m_pGameInstance);
}