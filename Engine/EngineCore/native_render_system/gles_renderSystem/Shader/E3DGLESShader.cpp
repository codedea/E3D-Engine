﻿//
//  Shader.cpp
//
//  Created by 潘庆云 on 2017/3/30.
//

#include "E3DGLESShader.hpp"
#include <src/Source/E3DDebug.h>
#include <src/RenderSystem/Material/E3DMaterial.hpp>
#include <src/Source/Helpers.h>
#include <src/Scene/E3DSceneManager.hpp>
#include "../E3DGLESRenderSystem.hpp"
#include <src/RenderSystem/Texture/E3DCubeMapTexture.hpp>
#include <src/RenderSystem/Texture/E3DTexture2D.hpp>

namespace E3DEngine
{
	
	void GLES_Shader::InitShaderVar()
	{
		uniformSetFunc["float"] = &Shader::createFloat1Uniform;
		uniformSetFunc["float[]"] = &Shader::createFloat1ArrayUniform;
		uniformSetFunc["vec2"] = &Shader::createFloat2Uniform;
		uniformSetFunc["vec3"] = &Shader::createFloat3Uniform;
		uniformSetFunc["vec4"] = &Shader::createFloat4Uniform;
		uniformSetFunc["vec2[]"] = &Shader::createFloat2ArrayUniform;
		uniformSetFunc["vec3[]"] = &Shader::createFloat3ArrayUniform;
		uniformSetFunc["vec4[]"] = &Shader::createFloat4ArrayUniform;
		uniformSetFunc["mat2"] = &Shader::createMatrix2Uniform;
		uniformSetFunc["mat3"] = &Shader::createMatrix3Uniform;
		uniformSetFunc["mat4"] = &Shader::createMatrix4Uniform;
		uniformSetFunc["int"] = &Shader::createInt1Uniform;
		uniformSetFunc["sampler2D"] = &Shader::createSampler2DUniform;
		uniformSetFunc["samplerCube"] = &Shader::createSamplerCubeUniform;
	}

	void GLES_Shader::LoadShader(const char *vertexShader, const char *fragmentShader)
	{
		ShaderProgram = ES2::CreateProgram();
		GLuint &&vertexShaderHandle = compileShader(vertexShader, GL_VERTEX_SHADER);
		GLuint &&fragmentShaderHandle = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

		ES2::AttachShader(ShaderProgram, vertexShaderHandle);
		ES2::AttachShader(ShaderProgram, fragmentShaderHandle);
		//bindAttribLoaction(STATIC_VERTEX);
		//bindAttribLoaction(DYNAMIC_VERTEX);
		ES2::LinkProgram(ShaderProgram);
		GLint linkSuccess;
		ES2::GetProgramiv(ShaderProgram, GL_LINK_STATUS, &linkSuccess);
		if (linkSuccess != GL_TRUE)
		{
			GLint logLength;
			ES2::GetShaderiv(ShaderProgram, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0)
			{
				GLchar *log = (GLchar *)malloc(logLength);
				ES2::GetShaderInfoLog(ShaderProgram, logLength, &logLength, log);
				Debug::Log(ell_Error, "Shader: LINK ERROR shader link log: %s" , log);
			}
			ES2::DeleteShader(ShaderProgram);
		}
		
		if (vertexShaderHandle != 0)
		{
			ES2::DeleteShader(vertexShaderHandle);
		}
		if (fragmentShaderHandle != 0)
		{
			ES2::DeleteShader(fragmentShaderHandle);
		}
		loadUniformLocation();
		loadAttribLocation(STATIC_VERTEX);
		loadAttribLocation(DYNAMIC_VERTEX);
	}


	GLuint GLES_Shader::compileShader(const char* shaderContent, GLenum shaderType)
	{
		if (shaderContent == nullptr)
		{
			return 0;
		}
		int shaderStringLength = (int)strlen(shaderContent);
		GLuint shaderHandle = ES2::CreateShader(shaderType);
		ES2::ShaderSource(shaderHandle, 1, &shaderContent, &shaderStringLength);
		ES2::CompileShader(shaderHandle);
		
		GLint status;
		ES2::GetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE)
		{
			GLint logLength;
			ES2::GetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0)
			{
				GLchar *log = (GLchar *)malloc(logLength);
				ES2::GetShaderInfoLog(shaderHandle, logLength, &logLength, log);
				Debug::Log(ell_Error, "Shader: [ERROR] shader compile log: %s \n ShaderContent:\n %s", log, shaderContent);
			}
			ES2::DeleteShader(shaderHandle);
		}

		return shaderHandle;
	}


	void GLES_Shader::compileShaders()
	{

	}

	void GLES_Shader::UseProgram()
	{
		ES2::UseProgram(ShaderProgram);
	}

	void GLES_Shader::UseNullProgram()
	{
		ES2::UseProgram(NULL_SHADER);
	}

	void GLES_Shader::UpdateProgramUniformValue()
	{		
		for (auto & uniformKeyValue : int1UniformList)
		{
			ES2::Uniform1i(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Value);
		}

		for (auto & uniformKeyValue : float1UniformList)
		{
			ES2::Uniform1f(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Value);
		}

		for (auto & uniformKeyValue : float2UniformList)
		{
			ES2::Uniform2f(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Value1, uniformKeyValue.second.Value2);
		}

		for (auto & uniformKeyValue : float1UniformArrayList)
		{
			if (uniformKeyValue.second.UniformLocation == -1 || uniformKeyValue.second.Value.empty())
			{
				continue;
			}
			ES2::Uniform1fv(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Count, uniformKeyValue.second.Value.data());
		}

		for (auto & uniformKeyValue : float2UniformArrayList)
		{
			if (uniformKeyValue.second.UniformLocation == -1 || uniformKeyValue.second.Value.empty())
			{
				continue;
			}
			ES2::Uniform2fv(uniformKeyValue.second.UniformLocation,uniformKeyValue.second.Count,uniformKeyValue.second.Value.data());
		}

		for (auto & uniformKeyValue : float3UniformArrayList)
		{
			if (uniformKeyValue.second.UniformLocation == -1 || uniformKeyValue.second.Value.empty())
			{
				continue;
			}
			ES2::Uniform3fv(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Count, uniformKeyValue.second.Value.data());
		}

		for (auto & uniformKeyValue : float4UniformArrayList)
		{
			if (uniformKeyValue.second.UniformLocation == -1 || uniformKeyValue.second.Value.empty())
			{
				continue;
			}
			ES2::Uniform4fv(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Count, uniformKeyValue.second.Value.data());
		}

		for (auto & uniformKeyValue : float3UniformList)
		{
			ES2::Uniform3f(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Value1, uniformKeyValue.second.Value2, uniformKeyValue.second.Value3);
		}

		for (auto & uniformKeyValue : float4UniformList)
		{
			ES2::Uniform4f(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Value1, uniformKeyValue.second.Value2, uniformKeyValue.second.Value3, uniformKeyValue.second.Value4);
		}

		for (auto & uniformKeyValue : matrix4UniformList)
		{
			if (uniformKeyValue.second.Data == nullptr)
			{
				continue;
			}
			ES2::UniformMatrix4fv(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Count, uniformKeyValue.second.Transpos, uniformKeyValue.second.Data);
		}

		for (auto & uniformKeyValue : matrix3UniformList)
		{
			if (uniformKeyValue.second.Data == nullptr)
			{
				continue;
			}
			ES2::UniformMatrix3fv(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Count, uniformKeyValue.second.Transpos, uniformKeyValue.second.Data);
		}

		for (auto & uniformKeyValue : matrix2UniformList)
		{
			if (uniformKeyValue.second.Data == nullptr)
			{
				continue;
			}
			ES2::UniformMatrix2fv(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.Count, uniformKeyValue.second.Transpos, uniformKeyValue.second.Data);
		}

		for (auto & uniformKeyValue : sampler2DUniformList)
		{
			if (uniformKeyValue.second.texture == nullptr)
			{
				continue;
			}
			ES2::ActiveTexture(GL_TEXTURE0 + uniformKeyValue.second.TextureIndex);
			ES2::BindTexture(GL_TEXTURE_2D, uniformKeyValue.second.texture->GetTextureBuffer());
			ES2::Uniform1i(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.TextureIndex);
		}

		for (auto & uniformKeyValue : samplerCubeUniformList)
		{
			if (uniformKeyValue.second.texture == nullptr)
			{
				continue;
			}
			ES2::ActiveTexture(GL_TEXTURE0 + uniformKeyValue.second.TextureIndex);
			ES2::BindTexture(GL_TEXTURE_CUBE_MAP, uniformKeyValue.second.texture->GetTextureBuffer());
			ES2::Uniform1i(uniformKeyValue.second.UniformLocation, uniformKeyValue.second.TextureIndex);
		}
	}

	void GLES_Shader::UpdateAttribPointerValue(UINT vertexType)
	{
		std::vector<Attribute> *attrList = nullptr;
		if (vertexType == STATIC_VERTEX)
		{
			attrList = &staticAttributeList;
		}
		else
		{
			attrList = &dynamicAttributeList;
		}

		for (auto & attrib : *attrList)
		{
			ES2::VertexAttribPointer(attrib.AttributeLoaction, attrib.AttributeSize, attrib.VarType, attrib.Normalized, attrib.VertexStructSize, (GLfloat*)nullptr + attrib.StartPosition);
		}

		EnableVertexAttribArray(vertexType);
	}

	void GLES_Shader::DeleteShader()
	{
		ES2::DeleteShader(ShaderProgram);
	}

	void GLES_Shader::EnableVertexAttribArray(UINT vertexType)
	{
		std::vector<Attribute> *attrList = nullptr;
		if (vertexType == STATIC_VERTEX)
		{
			attrList = &staticAttributeList;
		}
		else
		{
			attrList = &dynamicAttributeList;
		}

		for (auto & attrib : *attrList)
		{
			ES2::EnableVertexAttribArray(attrib.AttributeLoaction);
		}
		
	}

	void GLES_Shader::bindAttribLoaction(UINT vertexType)
	{
		std::vector<Attribute> *attrList = nullptr;
		if (vertexType == STATIC_VERTEX)
		{
			attrList = &staticAttributeList;
		}
		else
		{
			attrList = &dynamicAttributeList;
		}

		for (auto & attrib : *attrList)
		{
			ES2::BindAttribLocation(ShaderProgram, attrib.BindLocation, attrib.VarName.c_str());
		}
	}

	void GLES_Shader::loadAttribLocation(UINT vertexType)
	{
		std::vector<Attribute> *attrList = nullptr;
		if (vertexType == STATIC_VERTEX)
		{
			attrList = &staticAttributeList;
		}
		else
		{
			attrList = &dynamicAttributeList;
		}

		for (auto & attrib : *attrList)
		{
			attrib.AttributeLoaction = LoadSelfDefAttribuate(attrib.VarName);
		}
	}

	void GLES_Shader::loadUniformLocation()
	{
		for (auto & uniformKeyValue : int1UniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : float1UniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : float2UniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : float3UniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : float4UniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : matrix2UniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : matrix3UniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : matrix4UniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : float1UniformArrayList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : float2UniformArrayList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : float3UniformArrayList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : float4UniformArrayList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : sampler2DUniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}

		for (auto & uniformKeyValue : samplerCubeUniformList)
		{
			uniformKeyValue.second.UniformLocation = LoadSelfDefUniform(uniformKeyValue.first);
		}
	}


	GLES_Shader::~GLES_Shader()
	{
		DeleteShader();
		float1UniformList.clear();
		float2UniformList.clear();
		float3UniformList.clear();
		float4UniformList.clear();
		matrix4UniformList.clear();
		matrix3UniformList.clear();
		matrix2UniformList.clear();
		int1UniformList.clear();
	}
	

	GLint GLES_Shader::LoadSelfDefUniform(std::string name)
	{
		GLint location = ES2::GetUniformLocation(ShaderProgram, name.c_str());
		uniformLocationMap[name] = location;
		return location;
	}

	GLuint GLES_Shader::LoadSelfDefAttribuate(std::string name)
	{
		GLuint attr = ES2::GetAttribLocation(ShaderProgram, name.c_str());
		return attr;
	}

	void GLES_Shader::AddAttriList(Attribute attri, bool isStatic)
	{
		if (isStatic)
		{
			staticAttributeList.push_back(attri);
		}
		else
		{
			dynamicAttributeList.push_back(attri);
		}

	}

}
