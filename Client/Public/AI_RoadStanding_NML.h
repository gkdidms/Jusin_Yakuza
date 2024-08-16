#pragma once
#include "AI_RoadStanding.h"

#include "Client_Defines.h"

BEGIN(Client)
class CAI_RoadStanding_NML:
    public CAI_RoadStanding
{
public:
    enum MOTION_TYPE { MOTION_A, MOTION_B, MOTION_END };

private:
    CAI_RoadStanding_NML();
    CAI_RoadStanding_NML(const CAI_RoadStanding_NML& rhs);
    virtual ~CAI_RoadStanding_NML() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    _uint m_iMotionType = { 0 };

private:
    virtual void Ready_Root();

private:
    //서있는 상태
    virtual CBTNode::NODE_STATE Stand() override;

public:
    static CAI_RoadStanding_NML* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END