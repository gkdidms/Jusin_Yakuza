#include "Collision_Manager.h"
#include "LandObject.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize()
{
    return S_OK;
}

HRESULT CCollision_Manager::Add_ImpulseResolution(CLandObject* pObejct)
{
    Safe_AddRef(pObejct);
    m_ImpulseResolutionObjects.push_back(pObejct);

    return S_OK;
}

void CCollision_Manager::Tick()
{
    ImpulseResolution();
}

void CCollision_Manager::ImpulseResolution()
{
    // 여기에 충돌처리 코드 구현
    for (size_t i = 0; i < m_ImpulseResolutionObjects.size(); i++)
    {
        for (size_t j = 0; j < m_ImpulseResolutionObjects.size(); j++)
        {
            if (i == j) continue;

            m_ImpulseResolutionObjects[i]->ImpulseResolution(m_ImpulseResolutionObjects[j]);
        }
    }

    Clear();
}

void CCollision_Manager::Clear()
{
    for (auto& pComponents : m_ImpulseResolutionObjects)
        Safe_Release(pComponents);

    m_ImpulseResolutionObjects.clear();
}

void CCollision_Manager::Free()
{
    for (auto& pObject : m_ImpulseResolutionObjects)
    {
        Safe_Release(pObject);
    }
    m_ImpulseResolutionObjects.clear();
}
