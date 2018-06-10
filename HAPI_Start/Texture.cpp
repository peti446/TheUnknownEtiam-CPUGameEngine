#include "Texture.hpp"


namespace GRAPHICS
{

	Texture::Texture() :
		m_TextureRectangle(0, 30, 0, 30),
		m_textureFirstByte(nullptr),
		m_textureFirstPixel(nullptr),
		m_alphaType(eAlphaType::NormalAlpha)
	{
	}

	Texture::~Texture()
	{
		//Delete the pointer of the texture
		delete[] this->m_textureFirstByte;
	}

	Rectangle Texture::getRect(int frameNum) const
	{
		//Default frame to use when there isent any frame information
		Rectangle TextureRectangle(this->m_TextureRectangle);

		//Checks if frame information is avilable
		if (!this->m_frames.empty())
		{
			//Make the rectangle width and height equals to the frame height and width
			TextureRectangle = Rectangle(0, this->m_frames[frameNum].getWidth(), 0, this->m_frames[frameNum].getHeight());
		}

		return TextureRectangle;
	}

	bool Texture::LoadTexture(const std::string& path, const std::string& xmlFile)
	{
		//Load the texture
		int width = 0;
		int height = 0;
		bool sucesfull = HAPI.LoadTexture(path, &this->m_textureFirstByte, width, height);
		if (!sucesfull)
		{
			//If the texture could not been loaded, create a pink square
			//Alocate the memory for the square
			this->m_textureFirstByte = new BYTE[this->m_TextureRectangle.getWidth() * this->m_TextureRectangle.getHeight() * 4];
			//Set the memoty to pink
			BYTE* temp = this->m_textureFirstByte;
			for (int i = 0; i < this->m_TextureRectangle.getHeight(); i++)
			{
				for (int i2 = 0; i2 < this->m_TextureRectangle.getWidth(); i2++)
				{
					//Set pink pixel
					temp[0] = 255;
					temp[1] = 0;
					temp[2] = 255;
					//Advance pointer
					temp += 4;
				}
			}
			//Loaded my own texture so i know the alpha there isent one
			this->m_alphaType = eAlphaType::FullOpaque;
		}
		else
		{
			this->m_TextureRectangle = Rectangle(0, width, 0, height);
			BYTE* temp = this->m_textureFirstByte;
			unsigned char firstAlpha = temp[3];
			// Texture loaded so check the alpha type
			for (int y = 0; y < this->m_TextureRectangle.getHeight(); y++)
			{
				for (int x = 3; x < this->m_TextureRectangle.getWidth(); x += 4)
				{
					//Check if it has an alpha value between 0 and 255 (not including them)
					if (temp[x] > 0 && temp[x] < 255)
					{
						//Yep it got normal alpha, so just exit the loop as we
						this->m_alphaType = eAlphaType::NormalAlpha;
						goto GoToNormalAlpha;
					}
					else
					{
						//Check if there is a anternance between fully alpha an fully tran sparent
						if (temp[x] != firstAlpha)
						{
							this->m_alphaType = eAlphaType::NormalAlpha;
							goto GoToNormalAlpha;
						}
					}
				}
				//Advance pointer
				temp += this->m_TextureRectangle.getHeight() * 4;
			}
			//Set the correct enum if the loop exits normally (no goto) si the texture will be fully opaque or fully transparent
			this->m_alphaType = firstAlpha == 255 ? eAlphaType::FullOpaque : eAlphaType::FullyTransparent;
		}
	GoToNormalAlpha:
		//Conver Bytes to pixels
		this->m_textureFirstPixel = (HAPI_TColour*)this->m_textureFirstByte;

		if (xmlFile != "")
		{
			CHapiXML animFrames(xmlFile);
			std::vector<CHapiXMLNode*> nodes = animFrames.GetAllNodesWithName("subtexture");
			this->m_frames.reserve(nodes.size());
			for (auto node : nodes)
			{
				int left, right, top, buttom;
				CHapiXMLAttribute atribute;
				if (!node->GetAttributeWithName("x", atribute))
					break;

				left = atribute.AsInt();

				if (!node->GetAttributeWithName("y", atribute))
					break;
				
				top = atribute.AsInt();

				if (!node->GetAttributeWithName("width", atribute))
					break;
				
				right = atribute.AsInt();

				if (!node->GetAttributeWithName("height", atribute))
					break;
					
				buttom = atribute.AsInt();

				this->m_frames.push_back(Rectangle(left, right + left, top, buttom + top));
			}
		}
		return sucesfull;
	}

	void Texture::draw(int x, int y, HAPI_TColour* screen, const Rectangle& screenRect, int frameNum, float porcentWidth, float porcentHeight)
	{
		switch (this->m_alphaType)
		{
		case eAlphaType::NormalAlpha:
			//Draw normally as it got different alpha values
			this->drawNormal(x, y, screen, screenRect, frameNum, porcentWidth, porcentHeight);
			break;
		case eAlphaType::FullOpaque:
			//Draw with no alpha a lot quicker!
			this->drawNoAlpha(x, y, screen, screenRect, frameNum, porcentWidth, porcentHeight);
			break;
		case eAlphaType::FullyTransparent:
			//What should a draw if it is fully transparent !?!?!
			break;
		}
	}

	void Texture::rezise(const int width, const int height)
	{
		//Create the pixel padding (const for now)
		float paddingPixelesW = (0.48f)*this->m_TextureRectangle.getWidth();
		float paddingPixelesH = (0.48f)*this->m_TextureRectangle.getHeight();

		//Create the rectangles and get the texture of that rectangle from the main
		std::vector<Rectangle> slices = this->createSlices(this->m_TextureRectangle.getWidth(), this->m_TextureRectangle.getHeight(),
														   paddingPixelesH, paddingPixelesH,
														   paddingPixelesW, paddingPixelesW);
		//Gets the texture data from each slice and stores them in a vector
		std::vector<std::vector<HAPI_TColour>> colours;
		for (Rectangle r : slices)
		{
			colours.push_back(this->drawToVector(r));
		}
		//Create new slices (rectangles) that mach the new size of the texture
		std::vector<Rectangle> newSlices = this->createSlices(width, height,
															  paddingPixelesH, paddingPixelesH,
															  paddingPixelesW, paddingPixelesW);
		//Create the new texture pointer
		HAPI_TColour* newTexture = new HAPI_TColour[width*height];
		//Apply the texture slices to the texture to meet the "Slice 9 Method"
		for (int i = 0; i < newSlices.size(); i++)
		{
			//Gets the amount of time to draw a square form the original image to the new sized image in both directions
			int amountOfRectanglesDown = ((int)std::ceilf((float)newSlices[i].getHeight() / (float)slices[i].getHeight()));
			int amountOfRectanglesLeft = ((int)std::ceilf((float)newSlices[i].getWidth() / (float)slices[i].getWidth()));
			//Draw in each direction
			for (int y = 0; y < amountOfRectanglesDown; y++)
			{
				for (int x = 0; x <= amountOfRectanglesLeft; x++)
				{
					//Gets the original texture rectangle
					Rectangle r = slices[i];
					//We need the difference in position from the old texture to the new one,
					//so we can position the given rectangle to where it should be on the new texture
					int diffLeft = (newSlices[i].getLeft() - slices[i].getLeft());
					int diffTop = (newSlices[i].getTop() - slices[i].getTop());
					//Apart from the differents we need to add the current offsets so we draw the square where it needs to based on the amount
					r.translate(diffLeft + (x * r.getWidth()), diffTop + (y * r.getHeight()));
					//Clips the rectangle so we dont get out of our respective new sized slice (rectangle)
					r.ClipTo(newSlices[i]);
					//Draw it!
					this->drawVectorToPointerArray(width, r, slices[i].getWidth(), colours[i], newTexture);
				}
			}
		}

		//Cllean up the old texture
		delete[] this->m_textureFirstByte;
		//Apply the new texture and new size to the class
		this->m_TextureRectangle = Rectangle(0, width, 0, height);
		this->m_textureFirstByte = (BYTE*)newTexture;
		this->m_textureFirstPixel = newTexture;
	}

	void Texture::drawNoAlpha(int x, int y, HAPI_TColour * screen, const Rectangle & screenRect, int frameNum, float porcentWidth, float porcentHeight)
	{
		Rectangle TextureRectangle;
		if (!this->setUpTextureDrawRect(x, y, TextureRectangle, screenRect, frameNum, porcentWidth, porcentHeight)) {
			return;
		}

		//Get the screen width 
		int screenW = screenRect.getWidth();
		//Temporally pointers to the screen and texture to start at the correct offset
		HAPI_TColour* tempTexturePixels = this->m_textureFirstPixel + (TextureRectangle.getTop()*this->m_TextureRectangle.getWidth()) + TextureRectangle.getLeft();
		HAPI_TColour* tempScreenPixels = screen + (y * screenW) + x;
		//Hight and width of the square that actually needs to get rendered
		int h = TextureRectangle.getHeight();
		int w = TextureRectangle.getWidth();
		int textureEndOfLineAdd = this->m_TextureRectangle.getWidth() - w;
		int screenEndOfLine = screenW - w;
		size_t acl = sizeof(HAPI_TColour) * w;
		//Loops to render the texture
		for (int yT = 0; yT < h; yT++)
		{
			std::memcpy(tempScreenPixels,tempTexturePixels, acl);
			//Advance the pointer one line (the one we just copied)
			tempTexturePixels += w;
			tempScreenPixels += w;
			//Advance the pointers to the correct row and column
			tempTexturePixels += textureEndOfLineAdd;
			tempScreenPixels += screenEndOfLine;
		}
	}

	void Texture::drawNormal(int x, int y, HAPI_TColour * screen, const Rectangle & screenRect, int frameNum, float porcentWidth, float porcentHeight)
	{
		Rectangle TextureRectangle;
		if (!this->setUpTextureDrawRect(x, y, TextureRectangle, screenRect, frameNum, porcentWidth, porcentHeight)) {
			return;
		}

		//Get the screen width 
		int screenW = screenRect.getWidth();
		//Temporally pointers to the screen and texture to start at the correct offset
		HAPI_TColour* tempTexturePixels = this->m_textureFirstPixel + (TextureRectangle.getTop()*this->m_TextureRectangle.getWidth()) + TextureRectangle.getLeft();
		HAPI_TColour* tempScreenPixels = screen + (y * screenW) + x;
		//Hight and width of the square that actually needs to get rendered
		int h = TextureRectangle.getHeight();
		int w = TextureRectangle.getWidth();
		int textureEndOfLineAdd = this->m_TextureRectangle.getWidth() - w;
		int screenEndOfLineAdd = screenW - w;

		//Loops to render the texture
		for (int yT = 0; yT < h; yT++)
		{
			for (int xT = 0; xT < w; xT++)
			{
				//Check if the current pixel is not compleatly transparent
				if (tempTexturePixels->alpha > 0)
				{
					// Check if the pixel is not compleatly opaque
					if (tempTexturePixels->alpha < 255)
					{
						//Calculate each pixel color based on the alpha value
						tempScreenPixels->red = tempScreenPixels->red + ((tempTexturePixels->alpha*(tempTexturePixels->red - tempScreenPixels->red)) >> 8);
						tempScreenPixels->green = tempScreenPixels->green + ((tempTexturePixels->alpha*(tempTexturePixels->green - tempScreenPixels->green)) >> 8);
						tempScreenPixels->blue = tempScreenPixels->blue + ((tempTexturePixels->alpha*(tempTexturePixels->blue - tempScreenPixels->blue)) >> 8);
					}
					else
					{
						//Copy the hole pixel information over
						*tempScreenPixels = *tempTexturePixels;
					}
				}
				//Advance the pointers
				tempTexturePixels++;
				tempScreenPixels++;
			}
			//Advance the pointers to the correct row and column
			tempTexturePixels += textureEndOfLineAdd;
			tempScreenPixels += screenEndOfLineAdd;
		}
	}
	
	std::vector<HAPI_TColour> Texture::drawToVector(const Rectangle & renderInfo) const
	{
		HAPI_TColour* tempTexturePixels = this->m_textureFirstPixel + (renderInfo.getTop()*this->m_TextureRectangle.getWidth()) + renderInfo.getLeft();
		std::vector<HAPI_TColour> storedTextrue;
		int h = renderInfo.getHeight();
		int w = renderInfo.getWidth();
		int textureEndOfLineAdd = this->m_TextureRectangle.getWidth() - w;
		//Loops to render the texture
		for (int yT = 0; yT < h; yT++)
		{
			for (int xT = 0; xT < w; xT++)
			{
				storedTextrue.push_back(*tempTexturePixels);
				//Advance the pointers
				tempTexturePixels++;
			}
			//Advance the pointers to the correct new row position
			tempTexturePixels += textureEndOfLineAdd;
		}

		return storedTextrue;
	}
	
	void Texture::drawVectorToPointerArray(const int width, const Rectangle & renderInfo, const int colorWidth, std::vector<HAPI_TColour> colours, HAPI_TColour* const pointer) const
	{
		HAPI_TColour* tempPointer = pointer + (renderInfo.getTop()*width) + renderInfo.getLeft();
		int h = renderInfo.getHeight();
		int w = renderInfo.getWidth();
		int textureEndOfLineAdd = width - w;
		//Loops to render the texture
		for (int yT = 0; yT < h; yT++)
		{
			for (int xT = 0; xT < w; xT++)
			{
				*tempPointer = colours[xT + (yT * colorWidth)];
				//Advance the pointers
				tempPointer++;
			}
			//Advance the pointers to the correct new row position
			tempPointer += textureEndOfLineAdd;
		}
	}
	
	std::vector<Rectangle> Texture::createSlices(const int width, const int height,
												 const float topPadding, const float bottomPadding, const float leftPadding, const float rightPadding) const
	{
		int lPaddingPos = (int)std::floorf(leftPadding);
		int rPaddingPos = (int)std::floorf(float(width) - std::floorf(rightPadding));
		int tPaddingPos = (int)std::floorf(topPadding);
		int bPaddingPos = (int)std::floorf(float(height) - std::floorf(bottomPadding));
		std::vector<Rectangle> slices;
		slices.reserve(9);
		//Rectangle(LEFT, RIGHt, TOP, BOTTOM)
		slices.push_back(Rectangle(0,				lPaddingPos,	 0,				tPaddingPos)); // TopLeft
		slices.push_back(Rectangle(lPaddingPos,		rPaddingPos,	 0,				tPaddingPos)); //Top Middle
		slices.push_back(Rectangle(rPaddingPos,		width,			 0,				tPaddingPos)); //TopRight
		slices.push_back(Rectangle(0,				lPaddingPos,	 tPaddingPos,	bPaddingPos)); //MiddleLeft
		slices.push_back(Rectangle(lPaddingPos,		rPaddingPos,	 tPaddingPos,	bPaddingPos)); // Middle Center
		slices.push_back(Rectangle(rPaddingPos,		width,			 tPaddingPos,	bPaddingPos)); //MiddleRight
		slices.push_back(Rectangle(0,				lPaddingPos,	 bPaddingPos, height)); //BottomLeft
		slices.push_back(Rectangle(lPaddingPos,		rPaddingPos,	 bPaddingPos, height)); // BottomCenter
		slices.push_back(Rectangle(rPaddingPos,		width,			 bPaddingPos, height)); // BottomRight
		return slices;
	}
	
	bool Texture::setUpTextureDrawRect(int & x, int & y, Rectangle& TextureRectangle, const Rectangle & screenRect, int frameNum, float porcentWidth, float porcentHeight)
	{
		//Default frame to use when there isent any frame information
		TextureRectangle = Rectangle(this->m_TextureRectangle);

		//Checks if frame information is avilable
		if (!this->m_frames.empty())
		{
			//Make the rectangle width and height equals to the frame height and width
			TextureRectangle = Rectangle(0, this->m_frames[frameNum].getWidth(), 0, this->m_frames[frameNum].getHeight());
		}

		//Clamp porcent be between 0 and 2
		porcentWidth = std::max(-1.f, std::min(porcentWidth, 1.0f));
		porcentHeight = std::max(-1.f, std::min(porcentHeight, 1.0f));
		int left = TextureRectangle.getLeft();
		int right = left +  (int)std::ceilf((float)TextureRectangle.getWidth() * porcentWidth);
		int top = TextureRectangle.getTop();
		int bottom = top + (int)std::ceilf((float)TextureRectangle.getHeight() * porcentHeight);

		if (left > right)
		{
			int tleft = left;
			left = right;
			right = tleft;
			left += TextureRectangle.getWidth();
			right += TextureRectangle.getWidth();
			
		}
		if (top > bottom)
		{
			int tTop = top;
			top = bottom;
			bottom = top;
			top += TextureRectangle.getHeight();
			bottom += TextureRectangle.getHeight();
		}


		//Change the with to the new porcentage
		TextureRectangle = Rectangle(left, right, top, bottom);

		//Translate into world space
		TextureRectangle.translate(x, y);
		//Checks if it is outside the camera if so just return as its not needed to draw
		if (TextureRectangle.CompleatlyOutise(screenRect))
		{
			return false;
		}

		//Check if the texture its parcially inside the camera
		if (!screenRect.CompleatlyContains(TextureRectangle))
		{
			//Clip to  the camera rectangle
			TextureRectangle.ClipTo(screenRect);
		}

		int golbalSpaceX = TextureRectangle.getLeft();
		int globalSpaceY = TextureRectangle.getTop();

		//Revert to original state (texture space)
		TextureRectangle.translate(-x, -y);

		//Get screen realtive coordinates to draw on
		x = golbalSpaceX - screenRect.getLeft();
		y = globalSpaceY - screenRect.getTop();

		x = std::max(0, x);
		y = std::max(0, y);

		//Checks if frame information is avilable
		if (!this->m_frames.empty())
		{
			//Tanslate te frame to the correct position in the texture space
			TextureRectangle.translate(this->m_frames[frameNum].getLeft(), this->m_frames[frameNum].getTop());
		}

		return true;
	}
}
