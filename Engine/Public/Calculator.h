#pragma once
#include "Base.h"
BEGIN(Engine)
class CCalculator :
    public CBase
{
private:
    CCalculator();
    virtual ~CCalculator() = default;

public:
    HRESULT Initialize();
public:
    _float Lerp(_float Start, _float End, _float Factor);
    _uint Lerp(_uint Start, _uint End, _float Factor);
public:
    static CCalculator* Create();
    virtual void Free() override;
};

END