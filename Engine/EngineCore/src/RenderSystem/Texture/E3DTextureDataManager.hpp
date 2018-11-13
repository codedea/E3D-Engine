﻿//
//  TextureDataManager.hpp
//
//  Created by 潘庆云 on 2017/5/24.
//

#ifndef __E3D_TEXTURE_DATA_MANAGER_HPP__
#define __E3D_TEXTURE_DATA_MANAGER_HPP__

#include "src/Source/Interface.h"

namespace E3DEngine
{	
	enum CLAMP_TYPE
	{
		CLAMP_TO_EDGE = 0,
		REPEAT = 1,
		MIRRORED_REPEAT = 2,
	};

	enum TextureType
	{
		eTEXTURE_2D,
		eRENDER_TEXTURE,
		eCUBMAP_TEXTURE,
	};

	enum FILTER_TYPE
	{
		LINEAR = 0,
		NEAREST = 1,
		NEAREST_MIPMAP_NEAREST = 2,
		LINEAR_MIPMAP_NEAREST = 3,
		NEAREST_MIPMAP_LINEAR = 4,
		LINEAR_MIPMAP_LINEAR = 5
	};

	enum PixelFormat
	{
		Unknow,
		L8,			// 1 byte pixel format, 1 byte luminance
		L8A8,		// 2 byte pixel format, 1 byte luminance, 1 byte alpha
		R8G8B8,		// 24-bit pixel format, 8 bits for red, green and blue.
		R8G8B8A8	// 32-bit pixel format, 8 bits for red, green, blue and alpha.
	};

	typedef enum
	{
		RENDER_BUFFER,
		RENDER_TO_TEXTURE,
		RENDER_DEPTH,
	} RenderTargeType;

	struct E3D_EXPORT_DLL TextureData
	{
		CLAMP_TYPE clampType;
		FILTER_TYPE filterType;
		std::string fileName;
		std::string uniformName;

		unsigned int rgbModule;
		void *	imgData;
		int		width;
		int		height;
		bool	useMipMap;
		int		configID;
		int		target;

		TextureData()
		{
			clampType = CLAMP_TYPE::CLAMP_TO_EDGE;
			filterType = FILTER_TYPE::LINEAR;
			imgData = nullptr;
			width = 0;
			height = 0;
			rgbModule = 0;
			useMipMap = false;
			configID = 0;
		}

		~TextureData()
		{
			if (imgData != nullptr)
			{
				free(imgData);
			}
		}
	};
	class Render2Texture;
	class Texture;
	class E3D_EXPORT_DLL TextureDataManager : public IManager
	{
	public:
		// -----------------------------------------------
		// 创建纹理缓冲区
		// @return 纹理缓冲区ID
		// @param 图片名字
		//-----------------------------------------------
		virtual unsigned int GetTextureBuffer(std::string imageName);

		// -----------------------------------------------
		// 创建RTT
		//-----------------------------------------------
		Render2Texture* CreateRender2Texture(TextureData *data);

		Render2Texture* CreateRender2TextureSingleThread(TextureData *data);

		// -----------------------------------------------
		// 创建图像数据
		// @return 图像内部数据RGB
		// @param 图片名字
		//-----------------------------------------------
		virtual TextureData * GetTextureDataFromFile(std::string imageName, TextureData * InData = nullptr);

		Texture *GetTexture(TextureType type, TextureData* tData);

		// -----------------------------------------------
		// 清理
		//-----------------------------------------------
		virtual void Cleanup();

	protected:
		virtual Render2Texture* createRender2Texture() { return nullptr; }
		virtual Texture *createTexture2D(TextureData *data) { return nullptr; }
		virtual Texture *createCubeTexture(std::string filePath, int selectID);
	protected:
		std::map<std::string, Texture*> m_mapTextures;
	};
}

#endif /* TextureDataManager_hpp */
