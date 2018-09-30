//********************************** Effect 3D Engine **************************************************//
//******************* Copyright (c) 2018-9-29  PanQingyun. All rights reserved. ************************//
#include "E3DSceneLoader.h"
#include "../Source/ClassFactory.h"
#include "../Source/EngineDelegate.h"
#include "../Source/FilePath.h"
#include <../../3rd/Mono/mono/metadata/tabledefs.h>

namespace E3DEngine
{
	const std::string NM_GameObject = "GameObject";
	const std::string TP_Camera = "Camera";
	const std::string TP_DLight = "DirectionLight";
	const std::string TP_PLight = "PointLight";
	const std::string TP_SkyBox = "SkyBox";
	const std::string TP_Mesh = "Mesh";
	const std::string TP_Particle = "Particle";
	const std::string TP_Cube = "Cube";
	const std::string TP_Sphere = "Sphere";
	const std::string TP_Empty = "Empty";
	const std::string _typeName = "Type";
	const std::string TP_Terrain = "Terrain";
	const std::string TP_Prefab = "Prefab";

	const std::string _Name = "Name";
	const std::string _FilePath = "FilePath";
	const std::string _SelectID = "SelectID";
	const std::string _LayerMask = "LayerMask";
	const std::string _Active = "Active";
	const std::string _Position = "Posistion";
	const std::string _Rotation = "Rotation";
	const std::string _Scale = "Scale";
	const std::string _Transform = "Transform";
	const std::string _Material = "Material";
	const std::string _Particle = "Particle";
	const std::string _ClearColor = "ClearColor";
	const std::string _Color = "Color";
	const std::string _RenderIndex = "RenderIndex";
	const std::string _Component = "Component";
	const std::string _Range = "Range";

	const std::string _Layer_AllLayer = "AllLayer";
	const std::string _Component_ClassName = "ClassName";
	const std::string _Component_Field = "Field";
	const std::string _Camera_CullMask = "CullingMask";
	const std::string _Camera_CullMask_Everything = "Everything";
	const std::string _ParticleGroupName = "<particle-group>";

	using createGameObjectFun = GameObject*(*)(TiXmlElement *objectElement);
	using createTiXmlElementFun = void(*)(TiXmlElement *objectElement, GameObject *gameObject);

	std::string sceneFolderPath = "";

	std::map<std::string, createGameObjectFun>	createObjectFun;

	void CreateObjects(GameObject * parent, TiXmlElement* rootElem);

	std::string getFileFullPath(string path)
	{
		if (path.empty())
		{
			return "";
		}
		if (path[0] == path[1] && path[1] == '.')
		{
			return sceneFolderPath + "/" + path;
		}
		else
		{
			return Application::AppDataPath + path;
		}
	}

	Color4 createColor(std::string colorStr)
	{
		std::vector<std::string> colorS = StringBuilder::Split(colorStr, ",");
		float r = Convert::ToFloat(colorS[0]);
		float g = Convert::ToFloat(colorS[1]);
		float b = Convert::ToFloat(colorS[2]);
		float a = Convert::ToFloat(colorS[3]);

		return Color4(r, g, b, a);
	}

	vec3f getVector3(std::string vecStr)
	{
		std::vector<std::string> colorS = StringBuilder::Split(vecStr, ",");
		float x = Convert::ToFloat(colorS[0]);
		float y = Convert::ToFloat(colorS[1]);
		float z = Convert::ToFloat(colorS[2]);

		return vec3f(x, y, z);
	}

	void convert2DestType(std::string value, DWORD type, void * retValue)
	{

	}

	void convertTypeSetValue(Component *component, setValue set, string value, string type)
	{
		if (type == stringType) {
			set(component, value);
		}
		else if (type == intType) {
			set(component, Convert::ToInt(value));
		}
		else if (type == floatType) {
			set(component, Convert::ToFloat(value));
		}
		else if (type == doubleType) {
			set(component, Convert::ToDouble(value));
		}
	}

	// TODO
	void setComponentFieldValue(MonoClass * klass, const char* fieldName, Component * component, const char* valueStr)
	{
		MonoClassField * field = mono_class_get_field_from_name(klass, fieldName);
		MonoObject * monoObj = component->GetMonoBehaviour()->GetMonoObject();
		if (field == nullptr)
		{
			return;
		}
		MonoType *type = mono_field_get_type(field);
		int typeType = mono_type_get_type(type);
		switch (typeType)
		{
		case MONO_TYPE_STRING:
		{
			mono_field_set_value(monoObj, field, mono_string_new(MonoScriptManager::GetInstance().GetEngineDomain(), valueStr));
		}
		break;
		case MONO_TYPE_I4:
		{
			int retValue = Convert::ToInt(valueStr);
			mono_field_set_value(monoObj, field, &retValue);
		}
		break;
		case MONO_TYPE_BOOLEAN:
		{
			int retValue = Convert::ToBoolean(valueStr) ? 1 : 0;
			mono_field_set_value(monoObj, field, &retValue);
		}
		break;
		case MONO_TYPE_R4:
		{
			float retValue = Convert::ToFloat(valueStr);
			mono_field_set_value(monoObj, field, &retValue);
		}
		default:
			break;
		}

	}

	void setComponentFieldValue(TiXmlElement *fieldElement, Component *component)
	{
		if (fieldElement == nullptr)
		{
			return;
		}
		component->registProperty();
		MonoClass *klass = component->GetMonoBehaviour()->GetClass();
		for (TiXmlAttribute * attribute = fieldElement->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
		{
			const char* fieldName = attribute->Name();
			const char* valueStr = attribute->Value();
			if (component->m_setMethodMap.find(fieldName) != component->m_setMethodMap.end())
			{
				convertTypeSetValue(component, (setValue)(component->m_setMethodMap[fieldName]), valueStr, component->m_propertyTypeMap[fieldName]);
			}
			else
			{
				setComponentFieldValue(klass, fieldName, component, valueStr);
			}
		}

		setComponentFieldValue(fieldElement->NextSiblingElement(), component);
	}

	void createComponent(TiXmlElement *objectElement, GameObject *go)
	{
		if (objectElement == nullptr)
		{
			return;
		}

		std::string comName = *objectElement->Attribute(_Component_ClassName);
		Component * component = go->AddComponent(comName.c_str());
		setComponentFieldValue(objectElement->FirstChildElement(_Component_Field), component);
		component->OnCreate();
		component->OnCreateComplete();
		createComponent(objectElement->NextSiblingElement(_Component), go);
	}

	void parseTransform(TiXmlElement *objectElement, CTransform * transform)
	{
		if (transform == nullptr)
		{
			return;
		}
		if (objectElement == nullptr)
		{
			return;
		}

		transform->SetPosition(getVector3(*objectElement->Attribute(_Position)));
		transform->SetRotation(getVector3(*objectElement->Attribute(_Rotation)));
		transform->SetScale(getVector3(*objectElement->Attribute(_Scale)));
	}

	void setRenderIndex(TiXmlElement *objectElement, GameObject * go)
	{
		if (objectElement->Attribute(_RenderIndex) == nullptr)
		{
			return;
		}
		std::string  rd = *objectElement->Attribute(_RenderIndex);
		StringBuilder::Trim(rd);
		go->SetRenderIndex(Convert::ToInt(rd));
	}

	void setCameraCullMask(TiXmlElement *objectElement, GameObject *go)
	{
		if (objectElement == nullptr)
		{
			return;
		}
		if (objectElement->Attribute(_Camera_CullMask) == nullptr)
		{
			return;
		}
		std::string cullMask = *objectElement->Attribute(_Camera_CullMask);

		unsigned int layer = 0;
		if (cullMask == _Camera_CullMask_Everything)
		{
			go->SetLayerMask(-1);
		}
		else
		{
			std::vector<std::string> layers = StringBuilder::Split(cullMask, "|");
			for (auto &layerMsk : layers)
			{
				int _layer = Convert::ToInt(layerMsk);
				layer |= (1 << _layer);
			}
			go->SetLayerMask(layer);
		}
	}

	GameObject* createCamera(TiXmlElement *objectElement)
	{
		Camera *pCamera = Camera::CreateCamera();

		SceneManager::GetCurrentScene()->AddCamera(pCamera);
		pCamera->SetClearColor(createColor(*objectElement->Attribute(_ClearColor)));
		setCameraCullMask(objectElement, pCamera);
		parseTransform(objectElement->FirstChildElement(_Transform), pCamera->Transform);
		CreateObjects(pCamera, objectElement);
		return pCamera;
	}

	Renderer * createRenderer(TiXmlElement *objectElement)
	{
		std::string _path = *objectElement->Attribute(_FilePath);
		int _id = Convert::ToInt(*objectElement->Attribute(_SelectID));

		Material *m = GetRenderSystem()->GetMaterialManager()->CreateMaterial(getFileFullPath(_path), _id);
		m->mConfigPath = _path;
		m->mConfigID = _id;
		Renderer * renderer = GetRenderSystem()->GetRenderManager()->GetRenderer(m->ID);
		renderer->SetMaterial(m);
		return renderer;
	}

	GameObject* createSkyBox(TiXmlElement *objectElement)
	{
		Renderer * rd = createRenderer(objectElement->FirstChildElement(_Material));
		SkyBox *skb = new SkyBox();
		skb->Create(50, 50, 50);
		skb->SetRenderer(rd);
		CreateObjects(skb, objectElement);
		return skb;
	}

	GameObject* createMesh(TiXmlElement *objectElement)
	{
		std::string _path = *objectElement->Attribute(_FilePath);
		int _id = Convert::ToInt(*objectElement->Attribute(_SelectID));

		Mesh *mesh = Mesh::Create(getFileFullPath(_path), _id);
		mesh->mConfigPath = _path;
		setRenderIndex(objectElement, mesh);
		CreateObjects(mesh, objectElement);
		return mesh;
	}

	GameObject* createDLight(TiXmlElement *objectElement)
	{
		Light *light = Light::Create(LightType::eDIRECTION_LIGHT);
		Color4 color = createColor(*objectElement->Attribute(_Color));
		light->Color = vec4f(color.r, color.g, color.b, color.a);
		CreateObjects(light, objectElement);
		return light;
	}

	GameObject *createEmpty(TiXmlElement *objectElement)
	{
		GameObject * go = new GameObject();

		return go;
	}

	GameObject *createCube(TiXmlElement *objectElement)
	{
		Box * box = new Box();
		box->Create(1, 1, 1);
		Renderer * rd = createRenderer(objectElement->FirstChildElement(_Material));

		box->SetRenderer(rd);
		CreateObjects(box, objectElement);

		return box;
	}

	GameObject *createSphere(TiXmlElement *objectElement)
	{
		Sphere * sphere = new Sphere();
		sphere->Create(1);
		Renderer * rd = createRenderer(objectElement->FirstChildElement(_Material));
		sphere->SetRenderer(rd);
		CreateObjects(sphere, objectElement);

		return sphere;
	}

	GameObject *createPointLight(TiXmlElement *objectElement)
	{
		Light *light = Light::Create(LightType::ePOINT_LIGHT);
		Color4 color = createColor(*objectElement->Attribute(_Color));
		light->Color = vec4f(color.r, color.g, color.b, color.a);
		if (objectElement->Attribute(_Range) != nullptr)
		{
			static_cast<PointLight*>(light)->Range = Convert::ToFloat(*objectElement->Attribute(_Range));
		}
		CreateObjects(light, objectElement);

		return light;
	}

	GameObject *createPrefab(TiXmlElement *objectElement)
	{
		std::string filePath = sceneFolderPath + "/" + (*objectElement->Attribute(_FilePath));
		return LoadPrefab(filePath);
	}

	GameObject * LoadPrefab(std::string filePath)
	{
		Prefab *prefab = new Prefab();
		
		prefab->SetFilePath(filePath);
		TiXmlDocument * doc = new TiXmlDocument(filePath.c_str());
		bool loadOkay = doc->LoadFile();
		if (!loadOkay)
		{
			return prefab;
		}
		std::string folderPath = sceneFolderPath;
		sceneFolderPath = GetFolder(filePath);

		TiXmlElement* rootElem = doc->RootElement();
		CreateObjects(prefab, rootElem);
		sceneFolderPath = folderPath;
		return prefab;
	}

	GameObject * createParticle(TiXmlElement *objectElement)
	{
		GameObject * go = new GameObject();
		go->CreateBehaviour();
		std::string _path = *objectElement->Attribute(_FilePath);
		go->mConfigPath = _path;
		std::vector<ParticleGroup*> *particles = ParticleSystem::GetInstance().ActiveParticle(sceneFolderPath + "/" + _path);
		for (auto & particle : *particles)
		{
			particle->mName = _ParticleGroupName;
			go->AddChild(particle);
		}

		return go;
	}

	void setLayerMask(TiXmlElement *objectElement, GameObject * go)
	{
		if (objectElement == nullptr)
		{
			return;
		}
		if (objectElement->Attribute(_LayerMask) == nullptr)
		{
			return;
		}
		std::string layerStr = *objectElement->Attribute(_LayerMask);
		if (layerStr == _Layer_AllLayer)
		{
			go->SetLayerMask(-1);
		}
		else
		{
			std::vector<std::string> layers = StringBuilder::Split(layerStr, "|");
			int layer = 0;
			for (int i = 0; i < layers.size(); i++)
			{
				if (StringBuilder::Trim(layers[i]) == "")
					return;
				layer |= (1 << Convert::ToInt(layers[i]));
			}
			go->SetLayerMask(layer);
		}
	}

	void setGameObjectActive(TiXmlElement *objectElement, GameObject * go)
	{
		if (objectElement == nullptr)
		{
			return;
		}
		if (objectElement->Attribute(_Active) == nullptr)
		{
			return;
		}
		string activeStr = *objectElement->Attribute(_Active);
		bool active = Convert::ToBoolean(StringBuilder::Trim(activeStr));
		go->SetActive(active);
	}

	void setObjectCommonValue(GameObject * go, TiXmlElement *objectElement, std::string _type)
	{
		go->mName = *objectElement->Attribute(_Name);
		go->mTypeName = _type;
		parseTransform(objectElement->FirstChildElement(_Transform), go->Transform);
		if (_type == TP_Particle)
		{
			go->Transform->SetIsBillBoard(true);
		}
		createComponent(objectElement->FirstChildElement(_Component), go);
		setLayerMask(objectElement, go);
		setGameObjectActive(objectElement, go);
	}

	void LoadSceneObjects(string sceneFilePath)
	{
		TiXmlDocument * doc = new TiXmlDocument(sceneFilePath.c_str());
		bool loadOkay = doc->LoadFile();
		if (!loadOkay)
		{
			return;
		}
		sceneFolderPath = GetFolder(sceneFilePath);

		TiXmlElement* rootElem = doc->RootElement();
		CreateObjects(nullptr, rootElem);
		delete doc;

	}

	void CreateObjects(GameObject * parent, TiXmlElement* rootElem)
	{
		for (TiXmlNode * item = rootElem->FirstChild(); item != nullptr; item = item->NextSibling())
		{
			if (item->Value() != NM_GameObject)
			{
				continue;
			}
			std::string _type = *item->ToElement()->Attribute(_typeName);
			if (createObjectFun.find(_type) == createObjectFun.end())
			{
				continue;
			}
			GameObject *go = createObjectFun[_type](item->ToElement());
			if (go != nullptr)
			{
				setObjectCommonValue(go, item->ToElement(), _type);
				if (parent == nullptr)
				{
					ADD_IN_SCENE(go);
				}
				else
				{
					parent->AddChild(go);
				}
			}
		}
	}

	std::string getLayerMaskElement(int layerMask)
	{
		if (layerMask == -1)
		{
			return _Layer_AllLayer;
		}
		std::string retLayeStr = "";
		TableManager *tbMgr = TableRegister::GetTableManager((LAYER_CONFIAG_NAME).c_str());
		if (tbMgr == nullptr)
		{
			return _Layer_AllLayer;
		}
		std::vector< LayerConfig*> *layers = tbMgr->GetTable<LayerConfig>();
		for (auto &layer : *layers)
		{
			if ((1 << layer->ID) & layerMask)
			{
				if (retLayeStr == "")
				{
					retLayeStr = Convert::ToString(layer->ID);
				}
				else
				{
					retLayeStr = retLayeStr + "|" + Convert::ToString(layer->ID);
				}
			}
		}

		SAFE_DELETE(layers);
		return retLayeStr;
	}

	void exportInnerComponent(TiXmlElement *objectElement, std::string className, std::vector<Component *> &componentList)
	{
		string kName = ClassFactory::GetInstance().getTypeNameByClassName(className);
		bool isInEngine = false;
		if (kName == "NAN")
		{
			return;
		}
		for (auto component : componentList)
		{
			TiXmlElement *componentElement = new TiXmlElement(_Component);
			std::map<std::string, std::string> &fieldMap = component->m_propertyTypeMap;
			for (auto fieldPair : fieldMap)
			{
				getValue get = (getValue)(component->m_getMethodMap[fieldPair.first]);

				if (fieldPair.second == stringType)
				{
					componentElement->SetAttribute(fieldPair.first, Convert::ToString(get(component)));
				}
				else if (fieldPair.second == intType)
				{
					componentElement->SetAttribute(fieldPair.first, Convert::ToString(object_cast<int>(get(component))));
				}
				else if (fieldPair.second == floatType)
				{
					componentElement->SetAttribute(fieldPair.first, Convert::ToString(object_cast<float>(get(component))));
				}
			}
			objectElement->LinkEndChild(componentElement);
		}
	}

	void exportOuterComponent(TiXmlElement *objectElement, std::string className, std::vector<Component *> componentList)
	{
		for (auto component : componentList)
		{
			MonoObject * mono_obj = component->GetMonoBehaviour()->GetMonoObject();
			MonoClass * mono_class = component->GetMonoBehaviour()->GetClass();
			MonoClassField * field = nullptr;
			void * iter = nullptr;
			TiXmlElement *componentElement = new TiXmlElement(_Component);
			while (field = mono_class_get_fields(mono_class, &iter))
			{
				const char * name = mono_field_get_name(field);
				MonoType *mono_type = mono_field_get_type(field);
				void *fieldValue = nullptr;
				mono_field_get_value(mono_obj, field, fieldValue);
				int type = mono_type_get_type(mono_type);
				int flags = mono_field_get_flags(field);
				if ((flags & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK) != FIELD_ATTRIBUTE_PUBLIC)
				{
					continue;
				}
				if (type == MONO_TYPE_I4)
				{
					componentElement->SetAttribute(name, Convert::ToString(*(int*)fieldValue));
				}
				else if (type == MONO_TYPE_BOOLEAN)
				{
					componentElement->SetAttribute(name, (*(int*)fieldValue) == 1 ? "true" : "false");
				}
				else if (type == MONO_TYPE_STRING)
				{
					componentElement->SetAttribute(name, (const char*)fieldValue);
				}
				else if (type == MONO_TYPE_R4)
				{
					componentElement->SetAttribute(name, Convert::ToString(*(float*)fieldValue));
				}
			}
			objectElement->LinkEndChild(componentElement);
		}
	}

	void saveComponentElement(TiXmlElement *objectElement, GameObject *gameObject)
	{
		std::map<std::string, std::vector<Component*>> &componentsMap = gameObject->GetAllComponents();

		for (auto componentListPair : componentsMap)
		{
			std::string componentName = componentListPair.first;
			std::string className = "";
			std::string nameSpaceName = "";
			std::string full_name = std::string(componentName);
			int dotPos = full_name.find_last_of(".");

			if (dotPos == std::string::npos)
			{
				className = componentName;
			}
			else
			{
				className = full_name.substr(dotPos + 1);
				nameSpaceName = full_name.substr(0, dotPos);
			}

			if (nameSpaceName == E3D_NAME_SPACE)
			{ // ���涨��
				exportInnerComponent(objectElement, className, componentListPair.second);
			}
			else
			{ // �Զ���
				exportOuterComponent(objectElement, className, componentListPair.second);
			}
		}
	}

	void saveMaterialElement(TiXmlElement *objectElement, Material *material)
	{
		TiXmlElement *materialElement = new TiXmlElement(_Material);
		materialElement->SetAttribute(_FilePath, material->mConfigPath);
		materialElement->SetAttribute(_SelectID, material->mConfigID);

		objectElement->LinkEndChild(materialElement);
	}

	void saveTransformElement(TiXmlElement *objectElement, CTransform *transform)
	{
		TiXmlElement *transformElement = new TiXmlElement(_Transform);
		transformElement->SetAttribute(_Position, Convert::ToString(transform->Position));
		transformElement->SetAttribute(_Rotation, Convert::ToString(transform->RotationEuler));
		transformElement->SetAttribute(_Scale, Convert::ToString(transform->Scale));
		objectElement->LinkEndChild(transformElement);
	}

	void saveGameObjectElement(TiXmlElement *objectElement, GameObject *gameObject)
	{
		objectElement->SetAttribute(_Name, gameObject->mName);
		objectElement->SetAttribute(_typeName, gameObject->mTypeName);
		objectElement->SetAttribute(_LayerMask, getLayerMaskElement(gameObject->GetLayerMask()));
		objectElement->SetAttribute(_Active, Convert::ToString(gameObject->IsActive));
		if (gameObject->GetRenderer() != nullptr)
		{
			objectElement->SetAttribute(_RenderIndex, gameObject->GetRenderer()->RenderIndex);
		}	
		if (gameObject->mTypeName == TP_Particle || gameObject->mTypeName == TP_Mesh)
		{
			objectElement->SetAttribute(_FilePath, gameObject->mConfigPath);
		}

		saveTransformElement(objectElement, gameObject->Transform);
		saveComponentElement(objectElement, gameObject);

		// create childs
		for (auto gameObj : gameObject->GetChilds())
		{
			GameObject *childObject = gameObj.second;
			if (gameObject->mTypeName == TP_Particle && childObject->mName == _ParticleGroupName)
			{ // Particle Group ���ܱ���
				continue;
			}
			TiXmlElement *childElement = new TiXmlElement(NM_GameObject);
			saveGameObjectElement(childElement, childObject);
			objectElement->LinkEndChild(childElement);
		}
	}

	void SaveCurrentScene()
	{
		Scene *pScene = SceneManager::GetInstance().GetCurrentScene();

		// ����rootObject���Ӽ�
		std::map<UINT, GameObject*> &objects = pScene->GetRootObject()->GetChilds();

		TiXmlDocument *doc = new TiXmlDocument();
		TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");
		doc->LinkEndChild(decl);
		TiXmlElement *rootElement = new TiXmlElement("Scene");
		doc->LinkEndChild(rootElement);

		for (auto gameObj : objects)
		{
			TiXmlElement *childElement = new TiXmlElement(NM_GameObject);
			GameObject *childObject = gameObj.second;
			saveGameObjectElement(childElement, childObject);
			rootElement->LinkEndChild(childElement);
		}

		doc->SaveFile(pScene->GetScenePath());
	}


	void RegistScnObjCreateFunc()
	{
		createObjectFun[TP_Camera] = createCamera;
		createObjectFun[TP_DLight] = createDLight;
		createObjectFun[TP_SkyBox] = createSkyBox;
		createObjectFun[TP_Mesh] = createMesh;
		createObjectFun[TP_Particle] = createParticle;
		createObjectFun[TP_Empty] = createEmpty;
		createObjectFun[TP_Cube] = createCube;
		createObjectFun[TP_Sphere] = createSphere;
		createObjectFun[TP_PLight] = createPointLight;
		createObjectFun[TP_Prefab] = createPrefab;
	}

}