#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Instance_Point final :
    public CVIBuffer_Instance
{
public:
    typedef struct tINSTANCE_POINT_DESC :public CVIBuffer_Instance::INSTANCE_DESC
    {

    }INSTANCE_POINT_DESC;
private:
    CVIBuffer_Instance_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Instance_Point(const CVIBuffer_Instance_Point& rhs);
    virtual ~CVIBuffer_Instance_Point() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    HRESULT Initialize_Prototype(string FilePath) ;
    virtual HRESULT Initialize(void* pArg) override;
public:
    void Save_Data(ofstream* out);
    HRESULT Load_Data(string Filepath);
public:
    static CVIBuffer_Instance_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string FilePath);
    static CVIBuffer_Instance_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};
END
