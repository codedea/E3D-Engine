#include "E3DBox.h"
#include "../Scene/E3DSceneManager.hpp"
#include "../Object/E3DTransform.hpp"
#include "../RenderSystem/E3DRenderSystem.hpp"

void E3DEngine::Box::Create(float l, float w, float h)
{
	Transform->SetNeedUpdate(false);
	IsActive = false;
	m_vecVertex.resize(24);
	// 前
	m_vecVertex[0].SetPosition(-l / 2, h / 2, w / 2);
	m_vecVertex[0].SetColor(1, 0, 1, 1);
	m_vecVertex[0].SetNormal(0, 0 ,1);
	m_vecVertex[1].SetPosition(-l / 2, -h / 2, w / 2);
	m_vecVertex[1].SetColor(1, 0, 1, 1);
	m_vecVertex[1].SetNormal(0, 0, 1);
	m_vecVertex[2].SetPosition(l / 2, -h / 2, w / 2);
	m_vecVertex[2].SetColor(1, 0, 1, 1);
	m_vecVertex[2].SetNormal(0, 0, 1);
	m_vecVertex[3].SetPosition(l / 2, h / 2, w / 2);
	m_vecVertex[3].SetColor(1, 0, 1, 1);
	m_vecVertex[3].SetNormal(0, 0, 1);
	// 上
	m_vecVertex[4].SetPosition(-l / 2, h / 2, w / 2);
	m_vecVertex[4].SetColor(1, 1, 1, 1);
	m_vecVertex[4].SetNormal(0, 1, 0);
	m_vecVertex[5].SetPosition(l / 2, h / 2, w / 2);
	m_vecVertex[5].SetColor(1, 1, 1, 1);
	m_vecVertex[5].SetNormal(0, 1, 0);
	m_vecVertex[6].SetPosition(l / 2, h / 2, -w / 2);
	m_vecVertex[6].SetColor(1, 1, 1, 1);
	m_vecVertex[6].SetNormal(0, 1, 0);
	m_vecVertex[7].SetPosition(-l / 2, h / 2, -w / 2);
	m_vecVertex[7].SetColor(1, 1, 1, 1);
	m_vecVertex[7].SetNormal(0, 1, 0);
	// 后
	m_vecVertex[8].SetPosition(-l / 2, h / 2, -w / 2);
	m_vecVertex[8].SetColor(0, 1, 1, 1);
	m_vecVertex[8].SetNormal(0, 0, -1);
	m_vecVertex[9].SetPosition(l / 2, h / 2, -w / 2);
	m_vecVertex[9].SetColor(0, 1, 1, 1);
	m_vecVertex[9].SetNormal(0, 0, -1);
	m_vecVertex[10].SetPosition(l / 2, -h / 2, -w / 2);
	m_vecVertex[10].SetColor(0, 1, 1, 1);
	m_vecVertex[10].SetNormal(0, 0, -1);
	m_vecVertex[11].SetPosition(-l / 2, -h / 2, -w / 2);
	m_vecVertex[11].SetColor(0, 1, 1, 1);
	m_vecVertex[11].SetNormal(0, 0, -1);
	// 下
	m_vecVertex[12].SetPosition(-l / 2, -h / 2, -w / 2);
	m_vecVertex[12].SetColor(1, 1, 0, 1);
	m_vecVertex[12].SetNormal(0, -1, 0);
	m_vecVertex[13].SetPosition(l / 2, -h / 2, -w / 2);
	m_vecVertex[13].SetColor(1, 1, 0, 1);
	m_vecVertex[13].SetNormal(0, -1, 0);
	m_vecVertex[14].SetPosition(l / 2, -h / 2, w / 2);
	m_vecVertex[14].SetColor(1, 1, 0, 1);
	m_vecVertex[14].SetNormal(0, -1, 0);
	m_vecVertex[15].SetPosition(-l / 2, -h / 2, w / 2);
	m_vecVertex[15].SetColor(1, 1, 0, 1);
	m_vecVertex[15].SetNormal(0, -1, 0);
	// 左
	m_vecVertex[16].SetPosition(-l / 2, h / 2, w / 2);
	m_vecVertex[16].SetColor(1, 0, 0, 1);
	m_vecVertex[16].SetNormal(-1, 0, 0);
	m_vecVertex[17].SetPosition(-l / 2, h / 2, -w / 2);
	m_vecVertex[17].SetColor(1, 0, 0, 1);
	m_vecVertex[17].SetNormal(-1, 0, 0);
	m_vecVertex[18].SetPosition(-l / 2, -h / 2, -w / 2);
	m_vecVertex[18].SetColor(1, 0, 0, 1);
	m_vecVertex[18].SetNormal(-1, 0, 0);
	m_vecVertex[19].SetPosition(-l / 2, -h / 2, w / 2);
	m_vecVertex[19].SetColor(1, 0, 0, 1);
	m_vecVertex[19].SetNormal(-1, 0, 0);
	// 右
	m_vecVertex[20].SetPosition(l / 2, h / 2, w / 2);
	m_vecVertex[20].SetColor(0, 1, 0, 1);
	m_vecVertex[20].SetNormal(1, 0, 0);
	m_vecVertex[21].SetPosition(l / 2, -h / 2, w / 2);
	m_vecVertex[21].SetColor(0, 1, 0, 1);
	m_vecVertex[21].SetNormal(1, 0, 0);
	m_vecVertex[22].SetPosition(l / 2, -h / 2, -w / 2);
	m_vecVertex[22].SetColor(0, 1, 0, 1);
	m_vecVertex[22].SetNormal(1, 0, 0);
	m_vecVertex[23].SetPosition(l / 2, h / 2, -w / 2);
	m_vecVertex[23].SetColor(0, 1, 0, 1);
	m_vecVertex[23].SetNormal(1, 0, 0);

	size.x = l;
	size.y = w;
	size.z = h;

	m_vecIndex.resize(36);
	UINT index[36] = 
	{
		0	, 1	, 2,	2,	3,	0,
		4	, 5	, 6,	6,	7,	4,
		8	, 9	,10,	10,	11, 8,
		12	,13	,14,	14,	15,	12,
		16	,17	,18,	18,	19,	16,
		20	,21	,22,	22, 23, 20
	};

	memcpy(m_vecIndex.data(), index, sizeof(UINT) * 36);
}


void E3DEngine::Box::SetMaterial(Material * material)
{
	m_pRenderer = GetRenderSystem()->GetRenderManager()->GetRenderer(material->ID);
	if (m_pRenderer->RenderIndex != eRI_None && m_pRenderer->RenderIndex != RenderIndex)
	{
		// TODO 同样的材质，不同渲染层级，需要重新创建一个Renderer
		//m_pRenderer = GetRenderSystem()->GetRenderManager()->CreateVertexRender(material->mMaterialID);
	}
	m_pRenderer->SetMaterial(material);
	m_pRenderer->IsStaticDraw = false;
	SetActive(true);
}

vec3f E3DEngine::Box::GetBounds()
{
	return size;
}

void E3DEngine::Box::SetActive(bool isActive)
{
	if (isActive == IsActive)
	{
		return;
	}
	IsActive = isActive;
	Renderer* mRenderer = static_cast<Renderer*>(m_pRenderer);
	mRenderer->RemoveInRenderer(ID);
	if (isActive)
	{
		mRenderer->RecordCurrentVextexStartIndex(ID);
		mRenderer->RecordCurrentIndexStartIndex(ID);
		for (int i = 0; i < m_vecVertex.size(); i ++)
		{
			mRenderer->FillVertex(m_vecVertex[i]);
		}

		for (int i = 0; i < m_vecIndex.size(); i ++)
		{
			mRenderer->FillIndex(m_vecIndex[i]);
		}
		mRenderer->FillEnd();
		mRenderer->VertexCountAdd(ID, m_vecVertex.size());
		mRenderer->IndexCountAdd(ID, m_vecIndex.size());
	}
	m_pRenderer->TransformChange();
}


void E3DEngine::Box::AfterUpdate(float deltaTime)
{
	
}

void E3DEngine::Box::TransformChange()
{
	if (pCamera && !pCamera->boundingBoxFrustum(Transform->Position, 100))
	{
		return;
	}
	m_pRenderer->SetTransform(Transform);
	Renderer* mRenderer = static_cast<Renderer*>(m_pRenderer);
	int vertexStartIndex = mRenderer->GetRendererBuffer(ID)->VertextStartIndex;
	std::vector<Vertex>::iterator it = m_vecVertex.begin();
	for (int i = 0; i < m_vecVertex.size(); i++)
	{
		m_vecVertex[i].SetTransformPosition(Transform->Position.x, Transform->Position.y, Transform->Position.z);
		m_vecVertex[i].SetTransformScale(Transform->Scale.x, Transform->Scale.y, Transform->Scale.z);;
		m_vecVertex[i].SetTransformRotate(Transform->RotationEuler.x, Transform->RotationEuler.y, Transform->RotationEuler.z);
		mRenderer->Vertices[vertexStartIndex + i] = m_vecVertex[i];
	}
	mRenderer->TransformChange();
}

void E3DEngine::Box::CreateBehaviour()
{
	m_pBehaviour->SetImage(MonoScriptManager::GetInstance().GetEngineImage());
	NEW_INSTANCE(Box);
	setBehaviourDefaultValue();
}

void E3DEngine::Box::setBehaviourDefaultValue()
{
	GameObject::setBehaviourDefaultValue();
}

