#pragma once

#include <string>
#include <HAPI_lib.h>
using namespace HAPISPACE;

#include "Rectangle.hpp"


namespace GRAPHICS
{

	class Texture
	{
	public:
		/*
			Destructors and constructors
		*/
		Texture();
		~Texture();

		/**
		* Loads a texture in from a specific path
		* Parameters:
		* const int framNum - The current frame number
		*
		* Returns:
		* Rectangle current sprite texture rectangle
		**/
		Rectangle getRect(int frameNum) const;

		/**
		* Loads a texture in from a specific path
		* Parameters:
		* const std::string path - The path of the file to load
		*
		* Returns:
		* void
		**/
		bool LoadTexture(const std::string& path, const std::string& xmlFile);

		/**
		* Draws a texture to a specific location in the screen, will use fast blit or normal blit depending on the texture (if texture is fully opaque or if it has alpha values)
		* Parameters:
		* int x  - The X position of the texture in the screen
		* int y  - The Y position of the texture in the screen
		* HAPI_TColour* screen - The pointer to the first pixel of the screen
		* Rectangle& screenRect - A rectangle class that stores information about the screen
		*
		* Returns:
		* void
		**/
		void draw(int x, int y, HAPI_TColour* screen, const Rectangle& screenRect, int frameNum, float porcentWidth, float porcentHeight);

		/**
		* Rezises the imagen to the given height using Silce 9 Method
		* Parameters:
		* const int width - The new width
		* const int height - The new height
		*
		* Returns:
		* void
		**/
		void rezise(const int width, const int height);
	protected:
		/**
		* Draws a texture to a specific location in the screen, this is the fast drawing as no alpha calculations are required.
		* Parameters:
		* int x  - The X position of the texture in the screen
		* int y  - The Y position of the texture in the screen
		* HAPI_TColour* screen - The pointer to the first pixel of the screen
		* Rectangle& screenRect - A rectangle class that stores information about the screen
		*
		* Returns:
		* void
		**/
		void drawNoAlpha(int x, int y, HAPI_TColour* screen, const Rectangle& screenRect, int frameNum, float porcentWidth, float porcentHeight);
		/**
		* Draws a texture to a specific location in the screen, slower drawing as it need to calculate the alpha value and then blit the color to screen
		* Parameters:
		* int x  - The X position of the texture in the screen
		* int y  - The Y position of the texture in the screen
		* HAPI_TColour* screen - The pointer to the first pixel of the screen
		* Rectangle& screenRect - A rectangle class that stores information about the screen
		*
		* Returns:
		* void
		**/
		void drawNormal(int x, int y, HAPI_TColour* screen, const Rectangle& screenRect, int frameNum, float porcentWidth, float porcentHeight);
	private:
		/**
		* Creates a vector with 9 slices (retangle) with the dimensions of the given data
		*
		* Returns:
		* vector of retangles that contains slice width, height and position information
		**/
		std::vector<Rectangle> createSlices(const int width, const int height,const float topPadding, const float bottomPadding, const float leftPadding, const float rightPadding) const;
		
		/**
		* Copies a part of the texture to a vector
		* Parameters:
		* Rectangle& screenRect - A rectangle class that stores information about wich part to save in the vector
		*
		* Returns:
		* void
		**/
		std::vector<HAPI_TColour> drawToVector(const Rectangle& renderInfo) const;

		void drawVectorToPointerArray(const int width, const Rectangle& renderInfo, const int colorWidth, std::vector<HAPI_TColour> colours, HAPI_TColour* const pointer) const;

		/**
		* Clips the current texture rect to the screen
		*
		* Returns:
		* Rectangle - The new rectangle in texture space used to draw
		**/
		bool setUpTextureDrawRect(int& x, int& y, Rectangle& TextureRectangle, const Rectangle & screenRect, int frameNum, float porcentWidth, float porcentHeight);

		//Creates the alpha type enum
		enum eAlphaType
		{
			FullyTransparent, FullOpaque, NormalAlpha
		};

		//Pointer to the first byte/first pixel of the texture.
		BYTE* m_textureFirstByte;
		HAPI_TColour* m_textureFirstPixel;

		//Check for alpha type
		eAlphaType m_alphaType;

		//The width and height of the texture
		Rectangle m_TextureRectangle;

		//All tectures rectangles
		std::vector<Rectangle> m_frames;
	};
}