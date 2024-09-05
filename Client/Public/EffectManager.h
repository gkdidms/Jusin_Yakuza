#pragma once

#include "Base.h"
#include "CineCamera.h"
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CEffectManager final : public CBase
{
	DECLARE_SINGLETON(CEffectManager)

private:
	CEffectManager();
	virtual ~CEffectManager() = default;

    //����ƼŬ
public:
    //����� �Ѹ¾����� ����Ʈ
    void Blood_Effect(CEffect::EFFECT_DESC& EffectDesc);
    //�ڵ���, ������̿� �Ѹ����� ������ ����ũ
    void Car_HitSpark(CEffect::EFFECT_DESC& EffectDesc);
    //�� ������ ���� ��
    void Car_Fire(CEffect::EFFECT_DESC& EffectDesc);
    //�� ������
    void Car_Explosion(CEffect::EFFECT_DESC& EffectDesc);
    //�ѽ�
    void Shot_Flash(CEffect::EFFECT_DESC& EffectDesc);
    //�ó׸� �ʹ� ���� ������
    void Cine_NoseBlood(CEffect::EFFECT_DESC& EffectDesc);
    //�ó׸� �Կ��� ����
    void Cine_MouseBlood(CEffect::EFFECT_DESC& EffectDesc);
    //�ڵ��� �޺�(Ʈ���Ϲ���)
    void Car_BackTrail(CEffect::EFFECT_DESC& EffectDesc);
    //�ڵ��� â�� ����
    void Car_GlassBroke(CEffect::EFFECT_DESC& EffectDesc);
    //��
    void Money(CEffect::EFFECT_DESC& EffectDesc);
    //�︮���� ��
    void Heli_Fire(CEffect::EFFECT_DESC& EffectDesc);
    //�︮���� ����
    void Heli_Exp(CEffect::EFFECT_DESC& EffectDesc);
    //�̻�������
    void Heli_BulletExp(CEffect::EFFECT_DESC& EffectDesc);
    //�ı��� 2Ÿ ����Ʈ
    void KRC_Hand(CEffect::EFFECT_DESC& EffectDesc);

    void Player_Attack_Effect(CEffect::EFFECT_DESC& EffectDesc);
    void Enemy_Attack_Effect(CEffect::EFFECT_DESC& EffectDesc);

    void Cine_BloodEffect(CEffect::EFFECT_DESC& EffectDesc, _uint iEffectType);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void							Free() override;
};
END
