#include "Calculator.h"

CCalculator::CCalculator()
{
}

HRESULT CCalculator::Initialize()
{
	return S_OK;
}

_float CCalculator::Lerp(_float Start, _float End, _float Factor)
{
	return Start + (End - Start) * Factor;
}

_uint CCalculator::Lerp(_uint Start, _uint End, _float Factor)
{
	return Start + (End - Start) * Factor;
}

CCalculator* CCalculator::Create()
{
	CCalculator* pInstance = new CCalculator();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCalculator::Free()
{
}
