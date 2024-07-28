#pragma once
#include"UIScene.h"

//EventUI 0�� =ù��
//EventUI 1�� =�ι�����
//EventUI 2�� =����° ��
 //EventUI 3�� =�̸�
BEGIN(Client)
class CUITalk :
    public CUIScene
{
public:
    enum TEXTCASE {TEXT0 , TEXT1 , TEXT2 ,NAME , TEXTCASE_END};
protected:
    CUITalk();
    CUITalk(const CUITalk& rhs);
    virtual ~CUITalk() = default;

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;

public:
    //����ü�� �޾ƿ��°� �̰Źٲ㼭 �ᵵ��
    void Set_TalkData(wstring Name, wstring TalkData);
    //������ ���� �θ��� StartTalk �ҷ��ֱ�.
    void StartTalk();
private:
    void Talk_Effect(const _float& fTimeDelta);
private:
    wstring m_TalkData;//����ٰ� ���� �����͸� �� �޾ƿͼ� ��������.
    wstring m_EffectText[3];//���� �ؽ�Ʈ�ִϸ��̼ǿ� �� �ǵ��� ����
    _uint m_iIndex = { 0 };
    _uint m_LineIndex = { 0 };
    _float m_PlayTime = { 0.f };
    _bool m_TalkFin = { false };
public:
    static CUITalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
