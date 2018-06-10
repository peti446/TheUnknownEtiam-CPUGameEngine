#pragma once

#include <unordered_map>
#include <string>
#include <HAPI_lib.h>
using namespace HAPISPACE;
#include "Rectangle.hpp"
#include "Vector2.h"

namespace GRAPHICS
{
	class Texture;

	class GraphicsSystem
	{
	public:
		//Constructor and destructor
		GraphicsSystem(int height, int width);
		~GraphicsSystem();


		/**
		* Starts the graphics system.
		**/
		bool startSystem();

		/*
		* Gets the width of the screen
		* Returns: int - the width
		*/
		const int getWidth() const;

		/*
		* Gets the height of the screen
		*
		* Returns: int - the screen height
		*/
		const int getHeight() const;

		/*
		* Sets the current camera position to the new one in world space
		*
		* Returns:
		* void
		*/
		void setCameraPosition(int x, int y);

		/*
		* Translates the camera by the positions given
		*
		* Returns:
		* void
		*/
		void translateCamera(int dx, int dy);

		/*
		* Gets the camera rectangle
		*
		* Returns: copy of Rectangle - the camera rectangle
		*/
		const Rectangle getCameraRect() const;

		/*
		* Gets the camera worldspace position
		*
		* Returns: Vector2 - position in wordspace of the camera
		*/
		const Vector2 getCameraPosition() const;

		/**
		* Clears the screen to a specific colour.
		* Parameters:
		* const Hapi_TColour colour - The color to wich the screen wil be set (DEFAULT: HAPI_TColour::Black)
		*
		* Returns:
		* void
		**/
		void ClearScreen(const HAPI_TColour colour = HAPI_TColour::BLACK);

		/**
		* Loads a texture from a specific path
		* Parameters:
		* const std::string key - The key under witch the texture will be saved.
		* const std::steing path - The path of the texture
		*
		* Returns:
		* const std::string& - The Key
		**/
		const std::string& LoadTexture(const std::string& key, const std::string& path, const std::string& xml = "");

		/**
		* Unloads the specified textures
		* Parameters:
		* const std::string key - The texture key to unload.
		*
		* Returns:
		* void
		**/
		void UnloadTexture(const std::string& key);

		/**
		* Draws a texture to a specific location in the screen
		* Parameters:
		* std::string key - The identifier of the texture
		* int x  - The X position of the texture in the screen
		* int y  - The Y position of the texture in the screen
		*
		* Returns:
		* void
		**/
		void Draw(const std::string& key, const int x, const int y, int frameNum = 0, float porcentWidth = 1.0f, float porcentHeight = 1.0f) const;
		void Draw(const std::string& key, const Vector2 pos, int frameNum = 0, float porcentWidth = 1.0f, float porcentHeight = 1.0f) const;

		/**
		* Gets the rectangle for a texture.
		* Parameters:
		* std::string key - The identifier of the texture
		* int frameNum  - The frame number of the sprite sheet (default 0 if no sheed is used)/
		*
		* Returns:
		* The texture Rectangle
		**/
		Rectangle getRect(const std::string& key, int frameNum = 0) const;

		/**
		* Rezises the given texture using 9sliced method (Note: it will override the texture)
		* Parameters:
		* std::string key - The identifier of the texture
		* const int newWidth  - The new width of the texture 
		* const int newHeight  -  The new height of the texture
		*
		* Returns:
		* void
		**/
		void resizeTexture(const std::string& key, const int newWidth, const int newHeight) const;
	protected:

	private:
		//Pointers to the first byte of the screen and to the first pixel of the screen
		BYTE* m_screenPointer;
		HAPI_TColour* m_screenPixelPointer;

		//The original screen rectangle
		Rectangle m_screenRect;
		Rectangle m_cameraRect;

		//The unordered map where all the textures will be stored
		std::unordered_map<std::string, Texture*> m_textures;
	};
}