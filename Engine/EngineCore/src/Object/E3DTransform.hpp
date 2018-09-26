//********************************** Effect 3D Engine **************************************************//
//******************* Copyright (c) 2017-2-8  PanQingyun. All rights reserved. *************************//
#ifndef Transform_hpp
#define Transform_hpp

#include <stdio.h>
#include "E3DObject.h"
#include "../Source/vmath.h"

namespace E3DEngine
{
#define NULL_RETURN(OBJ)\
	if (OBJ == nullptr)\
	{\
		return; \
	}

#define NOT_ACTIVE_RETURN(OBJ)\
	if (OBJ != 0)\
	{\
		if (!OBJ->IsActive)\
		{\
			return; \
		}\
	}

    class GameObject;
	class E3D_EXPORT_DLL CTransform : public Object
    {
    public:
        CTransform();
		void SetMatrix(mat4f matrix);
        void Update();
        void SetPosition(vec3f position);
		void SetPosition(float x, float y, float z);
        void SetRotation(Quatf rotation);
        void SetRotation(mat4f rotateMatrix);
		void SetRotation(float x, float y,float z);
		void SetRotation(vec3f rotate);
		void SetRotation(float x, float y, float z, float w);
		void SetScale(vec3f scale);
		void SetScale(float x, float y, float z);
        void GenBillBoard(vec3f center, vec3f cameraForward);
		void RemoveChild(unsigned int objID);
		void Identity();
        void SetBillBoardNormal(vec3f pos,vec3f scale);
        void SetIsBillBoard(bool bill);
		void SetNeedUpdate(bool bIsNeed);
        
		vec3f GetForward();
		vec3f GetUp();
		vec3f GetRight();
        vec3f GetPosition();
		vec3f GetScale();
		vec3f GetLocalPosition();
		vec3f GetLocalScale();
		void AddChild(unsigned int objID ,CTransform * child);
		mat4f GetParentMatrix();
		void Destory();

	protected:
		void  translation();
		void  rotate();
		void  scale();

	protected:
		virtual void CreateBehaviour();
		virtual void setBehaviourDefaultValue();

    public:
        GameObject * gameObject;
        mat4f WorldMatrix;
        mat4f billboardMatrix;
        vec3f Position;
        vec3f Scale;
		Quatf Rotation;
		vec3f RotationEuler;
		bool  IsTransPosFirst;
		CTransform * Parent;
		std::map<unsigned int, CTransform*> Childs;
	private:
		bool  m_bStateChange;
        bool  m_IsBillBoard;
		bool  m_bNeedUpdate;
		vec3f m_vForward;
		vec3f m_vUp;
		vec3f m_vRight;
    };
}

#endif /* Transform_hpp */
