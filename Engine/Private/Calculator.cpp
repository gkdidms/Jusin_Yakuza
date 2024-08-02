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

vector<_int> CCalculator::Compute_LPS(const string& pattern) {
    _int m = pattern.size();
    vector<_int> lps(m, 0);
    _int len = 0; // ���� LPS ����
    _int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len - 1]; // ���� LPS ���̷� ���ư���
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}

// KMP �˰����� ����Ͽ� ���ڿ� �˻�
_bool CCalculator::Find_String(const string& text, const string& pattern) {
    _int n = text.size();
    _int m = pattern.size();
    vector<_int> lps = Compute_LPS(pattern);
    _int i = 0; // text�� �ε���
    _int j = 0; // pattern�� �ε���

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            return true; // ������ �ؽ�Ʈ�� ���Ե�
        }
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1]; // LPS �迭 ���
            }
            else {
                i++;
            }
        }
    }

    return false; // ������ �ؽ�Ʈ�� ���Ե��� ����
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
