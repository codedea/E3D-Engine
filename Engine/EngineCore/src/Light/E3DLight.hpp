﻿//
//  Light.hpp
//
//  Created by 潘庆云 on 2017/3/21.
//

#ifndef E3DLight_hpp
#define E3DLight_hpp

#include "../Object/E3DGameObject.h"

#include <iostream>

namespace E3DEngine
{
	enum LightType
	{
		eDIRECTION_LIGHT = 1,
		ePOINT_LIGHT,
		eSPOT_LIGHT,
	};

	class Light : public GameObject
	{
	public:
		static Light * Create(LightType type);
	public:
		LightType   Type;
		vec4f		Color;				// 颜色
		float		Intensity;			// 强度

	public:
		virtual void setBehaviourDefaultValue() override;

	public:
		virtual void CreateShadow() { }
		Light()
		{
			shadowCamera = nullptr;
		}
		virtual ~Light()
		{
			if (shadowCamera != nullptr)
			{
				SAFE_DELETE(shadowCamera);
			}
		}

	protected:
		Camera * shadowCamera;
	};

	class PointLight : public Light
	{
	public:
		PointLight();
		float Range;

	public:
		virtual void CreateBehaviour() override;
		virtual void setBehaviourDefaultValue();
		virtual void CreateShadow() override;
	};

	class DirectionLight : public Light
	{
	public:
		DirectionLight();

	public:
		virtual void CreateBehaviour() override;
		virtual void setBehaviourDefaultValue();
		virtual void CreateShadow() override;
	};

	class SpotLight : public Light
	{
	public:
		SpotLight();
		float Range;
		int   SpotAngle;

	public:
		virtual void CreateBehaviour() override;
		virtual void setBehaviourDefaultValue();
		virtual void CreateShadow() override;
	};
	
}
#endif /* Light_hpp */
