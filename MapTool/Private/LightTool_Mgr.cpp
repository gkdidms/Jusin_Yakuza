#include "LightTool_Mgr.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"



IMPLEMENT_SINGLETON(CLightTool_Mgr)

CLightTool_Mgr::CLightTool_Mgr()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLightTool_Mgr::Initialize()
{
	return S_OK;
}

void CLightTool_Mgr::Priority_Tick(_float fTimeDelta)
{
}

void CLightTool_Mgr::Tick(_float fTimeDelta)
{
}

void CLightTool_Mgr::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_LightObjects)
		iter->Late_Tick(fTimeDelta);
}

void CLightTool_Mgr::Render()
{
}

void CLightTool_Mgr::Show_LightMgr_IMGUI()
{
	/* LightDesc Ư�� ���� */
	if (0 < m_LightObjects.size())
	{
		static int light_current_idx;

		if (light_current_idx != m_iCurrentLightIndex)
		{
			light_current_idx = m_iCurrentLightIndex;
		}

		if (ImGui::BeginListBox("listbox 0"))
		{
			for (int n = 0; n < m_LightsName.size(); n++)
			{
				/*const bool is_selected = (cell_current_idx == n);*/
				const bool is_selected = (light_current_idx == n);
				if (ImGui::Selectable(m_LightsName[n], is_selected))
				{
					/*cell_current_idx = n;
					m_iCurrentCellIndex = cell_current_idx;*/
					light_current_idx = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		/* �ٸ� ������Ʈ Ŭ���� */
		if (m_iCurrentLightIndex != light_current_idx && m_LightObjects.size() > 0)
		{
			m_iCurrentLightIndex = light_current_idx;
			m_tCurLightDesc = m_LightObjects[m_iCurrentLightIndex]->Get_LightDesc();
		}



		ImGui::Text(u8"����Ʈ �ɼ�");

		ImGui::Text(u8"����Ʈ ����");
		static int lightType = m_tCurLightDesc.eType;
		if (ImGui::RadioButton(u8"TYPE_DIRECTIONAL", m_tCurLightDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL))
		{
			lightType = 0;
			m_tCurLightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"TYPE_POINT", m_tCurLightDesc.eType == LIGHT_DESC::TYPE_POINT))
		{
			lightType = 1;
			m_tCurLightDesc.eType = LIGHT_DESC::TYPE_POINT;
		}

		ImGui::NewLine();

		ImGui::Text(u8"����Ʈ ����");
		ImGui::InputFloat4(u8"����Ʈ ����", reinterpret_cast<float*>(&m_tCurLightDesc.vDirection));
		/*ImGui::InputFloat("x", &m_tCurLightDesc.vDirection.x);
		ImGui::InputFloat("y", &m_tCurLightDesc.vDirection.y);
		ImGui::InputFloat("z", &m_tCurLightDesc.vDirection.z);
		ImGui::InputFloat("w", &m_tCurLightDesc.vDirection.w);*/

		ImGui::NewLine();

		if (m_tCurLightDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL)
		{
			ImGui::Text(u8"����Ʈ ��ġ");
			ImGui::InputFloat4(u8"����Ʈ ��ġ", reinterpret_cast<float*>(&m_tCurLightDesc.vPosition));
		}
		else if (m_tCurLightDesc.eType == LIGHT_DESC::TYPE_POINT)
		{

		}

		ImGui::NewLine();

		ImGui::Text(u8"����Ʈ ����");
		ImGui::InputFloat("x", &m_tCurLightDesc.fRange);

		ImGui::NewLine();
		ImGui::ColorEdit4(u8"vDiffuse ����", (float*)&m_tCurLightDesc.vDiffuse);

		ImGui::NewLine();
		ImGui::ColorEdit4(u8"vAmbient ����", (float*)&m_tCurLightDesc.vAmbient);

		ImGui::NewLine();
		ImGui::ColorEdit4(u8"vSpecular ����", (float*)&m_tCurLightDesc.vSpecular);
	}

	ImGui::Text(u8"����Ʈ �߰� : �߰���ư ������ ���� ���콺 Ŭ�� ");
	if (ImGui::Button(u8"����Ʈ�߰�"))
	{
		m_bLightAdd = true;
	}

	Add_Light_Imgui();


}

void CLightTool_Mgr::Show_FileName()
{
	ImGui::Begin(u8"����Ʈ ����");

	ImGui::Text(u8" ����Ʈ ����Ʈ ");

	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	static int layer_current_idx;
	if (ImGui::BeginListBox("listbox 1"))
	{
		for (int n = 0; n < m_FileNames.size(); n++)
		{
			const bool is_selected = (layer_current_idx == n);
			if (ImGui::Selectable(m_FileNames[n], is_selected))
				layer_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	ImGui::End();
}

void CLightTool_Mgr::Update_FileName()
{
	for (auto& iter : m_FileNames)
		Safe_Delete(iter);

	m_FileNames.clear();
	string path = "../../Client/Bin/DataFiles/LightData/*.dat";

	struct _finddata_t fd;
	intptr_t handle;

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
		return; // ���Ͼ�����
	do
	{
		char* cfilename = new char[MAX_PATH];
		strcpy(cfilename, fd.name);
		m_FileNames.push_back(cfilename);
	} while (_findnext(handle, &fd) == 0);
	_findclose(handle);
}

void CLightTool_Mgr::Select_LightDesc()
{
}

bool CLightTool_Mgr::Add_Light_Imgui()
{
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB) && m_bLightAdd == true)
	{
		_bool		isPick;
		_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

		_matrix			startPos;
		startPos = XMMatrixIdentity();
		startPos.r[3].m128_f32[0] = vTargetPos.m128_f32[0];
		startPos.r[3].m128_f32[1] = vTargetPos.m128_f32[1];
		startPos.r[3].m128_f32[2] = vTargetPos.m128_f32[2];
		startPos.r[3].m128_f32[3] = vTargetPos.m128_f32[3];

		m_tCurLightDesc.vPosition = _float4(vTargetPos.m128_f32[0], vTargetPos.m128_f32[1], vTargetPos.m128_f32[2], vTargetPos.m128_f32[3]);

		CLightObj::LIGHTOBJ_DESC		lightDesc;
		lightDesc.vStartPos = startPos;
		lightDesc.tLightDesc = m_tCurLightDesc;

		m_LightObjects.push_back(dynamic_cast<CLightObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_LightObj"), &lightDesc)));

		m_bLightAdd = false;

		Update_LightsName();

		return true;
	}

	return false;
}

void CLightTool_Mgr::Update_LightsName()
{
	if (0 < m_LightsName.size())
	{
		for (auto& iter : m_LightsName)
			Safe_Delete(iter);

		m_LightsName.clear();
	}

	if (0 < m_LightObjects.size())
	{
		for (int i = 0; i < m_LightObjects.size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "LightObject");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_LightsName.push_back(szName);
		}
	}
}

void CLightTool_Mgr::Free()
{
	for (auto& iter : m_LightObjects)
		Safe_Release(iter);
	m_LightObjects.clear();

	for (auto& iter : m_LightsName)
		Safe_Delete(iter);
	m_LightsName.clear();

	for (auto& iter : m_FileNames)
		Safe_Delete(iter);
	m_FileNames.clear();

	Safe_Release(m_pGameInstance);
}
