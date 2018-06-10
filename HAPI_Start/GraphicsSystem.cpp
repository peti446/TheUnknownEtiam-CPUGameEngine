#include "GraphicsSystem.hpp"
#include "Texture.hpp"

namespace GRAPHICS
{
	/*
		Constuctos and destructor
	*/
	GraphicsSystem::GraphicsSystem(int width, int height) : m_screenRect(0, width, 0, height), m_cameraRect(0, width, 0, height)
	{
		//Default values for the pointer
		this->m_screenPointer = nullptr;
		this->m_screenPixelPointer = nullptr;
	}

	GraphicsSystem::~GraphicsSystem()
	{
		//Deletes all the textures at the end of the graphics system (when the porgram exits basicly)
		for (auto it = this->m_textures.begin(); it != this->m_textures.end(); ++it)
		{
			delete it->second;
		}

	}

	//Start the systems
	bool GraphicsSystem::startSystem()
	{
		//Inisialise the window
		int w = this->m_screenRect.getWidth();
		int h = this->m_screenRect.getHeight();
		bool started = HAPI.Initialise(w, h, "The unknown Etiam");

		//Check for changed size
		if (w != this->m_screenRect.getWidth() || h != this->m_screenRect.getHeight())
		{
			this->m_screenRect = Rectangle(0, w, 0, h);
		}

		this->m_cameraRect = Rectangle(this->m_screenRect);
		//Get the first pointer to the screen, and set the first pixel to the screen
		this->m_screenPointer = HAPI.GetScreenPointer();
		this->m_screenPixelPointer = (HAPI_TColour*)this->m_screenPointer;

		//Return if the window was create succesfully or not;
		return started;
	}

	const int GraphicsSystem::getWidth() const
	{
		return this->m_screenRect.getWidth();
	}

	const int GraphicsSystem::getHeight() const
	{
		return this->m_screenRect.getHeight();
	}

	void GraphicsSystem::setCameraPosition(int x, int y)
	{
		this->m_cameraRect.translate(x - this->m_cameraRect.getLeft(), y - this->m_cameraRect.getTop());
	}

	void GraphicsSystem::translateCamera(int dx, int dy)
	{
		this->m_cameraRect.translate(dx, dy);
	}

	const Rectangle GraphicsSystem::getCameraRect() const
	{
		return this->m_cameraRect;
	}

	const Vector2 GraphicsSystem::getCameraPosition() const
	{
		return Vector2((float)this->m_cameraRect.getLeft(), (float)this->m_cameraRect.getTop());
	}

	//Clear the screen to a color
	void GraphicsSystem::ClearScreen(const HAPI_TColour colour)
	{
		// Clears the first line of the screen
		for (int i = 0; i < this->m_screenRect.getWidth(); i++)
		{
			this->m_screenPixelPointer[i] = colour;
		}

		//Copies the first screen line(row) over to the remaining lines(row) for the screen
		size_t toCoy = sizeof(HAPI_TColour) * this->m_screenRect.getWidth();
		for (int i = 1; i < this->m_screenRect.getHeight(); i++)
		{
			memcpy(&this->m_screenPixelPointer[i*this->m_screenRect.getWidth()], this->m_screenPixelPointer, toCoy);
		}
	}


	//Load a texture in to the system with a specific key atachet to it, so it can be drawen later by calling its key.
	const std::string& GraphicsSystem::LoadTexture(const std::string& key, const std::string& path, const std::string& xml)
	{
		//Check if key already exist
		if (this->m_textures.find(key) == this->m_textures.end())
		{
			//Key does not exist so, create a new one, load the texture and add it to the unordered_map
			Texture* tex = new Texture();
			if (!tex->LoadTexture(path, xml))
			{
				std::cout << "Could not find the file: " << path << " for the following texture: " << key << " default pink texture loaded." << std::endl;
			}
			this->m_textures[key] = tex;
		}
		//Return the key at the end
		return key;
	}

	//Unloads a textures
	void GraphicsSystem::UnloadTexture(const std::string & key)
	{
		if (this->m_textures.find(key) != this->m_textures.end())
		{
			Texture* t = this->m_textures.at(key);
			delete t;
			this->m_textures.erase(key);
			return;
		}
		std::cerr << "Could not unload texture with key: " << key << ". Texture does not exists." << std::endl;
	}

	//Draw a texture to a specific location
	void GraphicsSystem::Draw(const std::string & key, const int x, const int y, int frameNum, float porcentWidth, float porcentHeight) const
	{
		//Checks if the key exits, if so draw the texture
		if (this->m_textures.find(key) != this->m_textures.end())
		{

			this->m_textures.at(key)->draw(x, y, this->m_screenPixelPointer, this->m_cameraRect, frameNum, porcentWidth, porcentHeight);
		}
		else
		{
			std::cout << "The texture with key: " << key << " was never loaded... Load it first and then use it (It might be a typo)" << std::endl;
		}
	}
	void GraphicsSystem::Draw(const std::string & key, const Vector2 pos, int frameNum, float porcentWidth, float porcentHeight) const
	{
		this->Draw(key, (int)pos.x, (int)pos.y, frameNum, porcentWidth, porcentHeight);
	}
	Rectangle GraphicsSystem::getRect(const std::string & key, int frameNum) const
	{
		//Checks if the key exits, if so draw the texture
		if (this->m_textures.find(key) == this->m_textures.end())
		{
			std::cerr << "Could not find rectangle from sprite with name: " << key << std::endl;
			return Rectangle();
		}

		return this->m_textures.at(key)->getRect(frameNum);
	}
	void GraphicsSystem::resizeTexture(const std::string & key, const int newWidth, const int newHeight) const
	{
		if (this->m_textures.find(key) == this->m_textures.end()){
			std::cerr << "Could not rezise sprite with name: " << key << " as it does not exists." << std::endl;
			return;
		}
		this->m_textures.at(key)->rezise(newWidth, newHeight);
	}
}
