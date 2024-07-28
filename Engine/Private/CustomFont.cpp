#include "CustomFont.h"
#include "GameInstance.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice},
	m_pContext {pContext},
	m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCustomFont::Initialize(const wstring& strFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pContext);
	m_pFont = new SpriteFont(m_pDevice, strFontFilePath.c_str());

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	// ���� ���� ����
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // �ҽ��� ���� ���� ���
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // ����� ���� ���� �����Ͽ� ���
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // ��� ���� ä�� Ȱ��ȭ

	// ���� ���� ����
	if (FAILED(m_pDevice->CreateBlendState(&blendDesc, &m_pBlendState)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCustomFont::Render(const wstring& strText, const _float2& vPosition, _fvector vColor)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pContext->OMSetDepthStencilState(nullptr, 0);
	_float		fTemp = {};
	m_pContext->OMSetBlendState(nullptr, &fTemp, 0);
	m_pContext->RSSetState(nullptr);

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), vPosition, vColor, 0.f, _float2(0.f, 0.f), _float2(0.5f, 0.5f), SpriteEffects_None, 0.5f);
	m_pBatch->End();

	return S_OK;
}

HRESULT CCustomFont::Align_Render(const wstring& strText, const _float2& vPosition, _fvector vColor, _uint iAlignment)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	// ������ ���� ���¸� ���
	m_pContext->OMSetBlendState(m_pBlendState, nullptr, 0xFFFFFFFF);

	m_pBatch->Begin();

	// �ؽ�Ʈ�� ���� ���ڸ� �������� �����Ͽ� �� ��� ����

	vector<LineInfo> lines;
	wstring line;
	wstringstream iss(strText);
	while (getline(iss, line)) {
		LineInfo lineInfo;
		lineInfo.text = line;
		_vector vSize = m_pFont->MeasureString(line.c_str());;
		XMStoreFloat2(&lineInfo.size, vSize);

		if (lines.empty()) {
			lineInfo.cumulativeHeight = 0.f;
		}
		else {
			lineInfo.cumulativeHeight = lines.back().cumulativeHeight + lineInfo.size.y;
		}

		lines.push_back(lineInfo);
	}

	float startX = vPosition.x; // ���� ���� �⺻��

	// ���� ������ ���� X ��ǥ ����
	if (iAlignment == MIDDLE) { // �߾� ����
		for (const LineInfo& lineInfo : lines) {
			startX = vPosition.x - lineInfo.size.x*0.5f / 2.f;
			m_pFont->DrawString(m_pBatch, lineInfo.text.c_str(), _float2(startX, vPosition.y + lineInfo.cumulativeHeight), vColor, 0.f, _float2(0.f, 0.f), _float2(0.5f, 0.5f));
		}
	}
	else if (iAlignment == RIGHT) { // ������ ����
		for (const LineInfo& lineInfo : lines) {
			startX = vPosition.x - lineInfo.size.x*0.5f;
			m_pFont->DrawString(m_pBatch, lineInfo.text.c_str(), _float2(startX, vPosition.y + lineInfo.cumulativeHeight), vColor,0.f,_float2(0.f,0.f),_float2(0.5f,0.5f));
		}
	}
	else { // ���� ���� (�⺻��)
		for (const LineInfo& lineInfo : lines) {
			m_pFont->DrawString(m_pBatch, lineInfo.text.c_str(), _float2(startX, vPosition.y + lineInfo.cumulativeHeight), vColor, 0.f, _float2(0.f, 0.f), _float2(0.5f, 0.5f));
		}
	}

	m_pBatch->End();

	return S_OK;
}
HRESULT CCustomFont::Perspective_Render(const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, const _float& fTimeDelta)
{
	m_pBatch->Begin();

	_float2 vScale = _float2(0.5f * fScale - fTimeDelta, 0.5f * fScale - fTimeDelta);
	m_pFont->DrawString(m_pBatch, strText.c_str(), vPosition, vColor, 0.f, _float2(0.f, 0.f), vScale, SpriteEffects_None, 0.5f);
	m_pBatch->End();

	return S_OK;
}

HRESULT CCustomFont::Blend_Render(const wstring& strText, const _float2& vPosition, _fvector vColor)
{
	m_pContext->OMSetBlendState(m_pBlendState, nullptr, 0xFFFFFFFF);

	m_pBatch->Begin();
	m_pFont->DrawString(m_pBatch, strText.c_str(), vPosition, vColor, 0.f, _float2(0.f, 0.f), _float2(0.5f, 0.5f), SpriteEffects_None, 0.5f);
	m_pBatch->End();

	return S_OK;
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		MSG_BOX("Failed To Created : CCustomFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pFont);
	Safe_Delete(m_pBatch);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pBlendState);
	Safe_Release(m_pGameInstance);
}
