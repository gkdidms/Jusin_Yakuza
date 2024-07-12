#pragma once
#include "Base.h"
#include "Client_Defines.h"
BEGIN(Engine)

END

BEGIN(Client)
class CItemData :
    public CBase
{
public:
    typedef struct tItemDesc {
        wstring szItemName;
        wstring TextureName;
        _uint iHealAmount;
        _uint iStaminaAmount;
    }ITEMDESC;
private:
    CItemData();
    CItemData(const CItemData& rhs);
    virtual ~CItemData() = default;
public:
    HRESULT Initialize() ;
private:
    wstring m_szItemName = { TEXT("") };//�������̸�
    wstring m_TextureName={TEXT("")};//Ŭ���� ������Ʈ �̸�
    _uint m_iHealAmount = { 0 };//����
    _uint m_iStaminaAmount = { 0 };//���׹̳� ȸ����
public:
    static CItemData* Create();
    virtual void Free();
};
END