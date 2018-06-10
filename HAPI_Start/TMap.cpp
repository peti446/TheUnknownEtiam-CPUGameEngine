#include <HAPI_Types.h>
#include "TMap.hpp"
#include "Entity.hpp"
#include "TileNoColision.hpp"
#include "TileColision.hpp"
#include "GraphicsSystem.hpp"

TMap::TMap()
{
}


TMap::~TMap()
{
	for (Layer l : this->m_layers)
	{
		for (std::vector<Entity*> tileCont : l.m_tiles)
		{
			for (Entity* e : tileCont)
			{
				delete e;
			}
		}
	}
}

bool TMap::loadMap(const std::string& filePath, const GRAPHICS::GraphicsSystem& gs)
{
	std::string::size_type extensionTypeIndex = filePath.rfind(".");
	if (extensionTypeIndex == std::string::npos)
	{
		std::cerr << "No file extension found!" << std::endl;
		return false;
	}
 
	std::string fileExtension = filePath.substr(extensionTypeIndex + 1);
	if (fileExtension != "tmx" && fileExtension != "xml" && fileExtension != "xaml")
	{
		std::cerr << "Map file extension invalid! Can only accept tmx, xml and xaml. Dont even try to rename then if the format is nor correct it will not load anyways" << std::endl;
		return false;
	}

	CHapiXML map(filePath);

	if (!map.HasData())
	{
		std::cerr << "Well, it seems like the map is empty. Why do you load an empty map !?!?!. Could not load map." << std::endl;
		return false;
	}

	CHapiXMLNode* mapNode = map.GetFirstNodeWithName("map");

	if (mapNode == nullptr)
	{
		std::cerr << "Could not load the map, as the root `<map>` does not exits in the file." << std::endl;
		return false;
	}

	CHapiXMLAttribute width;
	CHapiXMLAttribute height;
	CHapiXMLAttribute TileWidth;
	CHapiXMLAttribute TileHeight;
	if (!mapNode->GetAttributeWithName("width", width) || !mapNode->GetAttributeWithName("height", height)
		||
		!mapNode->GetAttributeWithName("tilewidth", TileWidth) || !mapNode->GetAttributeWithName("tileheight", TileHeight))
	{
		std::cerr << "The `<map>` node needs to have a width, height, tilewidth and tileheight attribute, it seems that one was missing. Could not load map." << std::endl;
		return false;
	}

	this->m_MapWidth = width.AsInt();
	this->m_MapHeight = height.AsInt();
	this->m_TileWidth = TileWidth.AsInt();
	this->m_TileHeight = TileHeight.AsInt();

	std::vector<CHapiXMLNode*> layers = map.GetAllNodesWithName("layer");
	this->m_layers.reserve(layers.size());

	for (CHapiXMLNode* layer : layers)
	{
		CHapiXMLNode* properties = nullptr;
		CHapiXMLNode* data = nullptr;

		for (CHapiXMLNode* node : layer->GetChildren())
		{
			if (node->GetName() == "data")
			{
				data = node;
			}
			else if(node->GetName() == "properties")
			{
				properties = node;
			}
		}

		if (data == nullptr || data->GetChildren().size() != (this->m_MapHeight * this->m_MapWidth))
		{
			CHapiXMLAttribute name;
			if (layer->GetAttributeWithName("name", name))
				std::cerr << "The `<data>` node inside " << name.AsString() << " does not exists, or the amount of tiles is incorrect based on the map size. Could not load map." << std::endl;
			return false;
		}
		bool EntityLayer = false;

		if (properties != nullptr)
		{
			for (CHapiXMLNode* Property : properties->GetChildren())
			{
				CHapiXMLAttribute name;
				if (Property->GetAttributeWithName("name", name))
				{
					if (name.AsString() == "PlayerLayer")
					{
						CHapiXMLAttribute value;
						if (Property->GetAttributeWithName("value", value))
						{
							EntityLayer = value.AsString() == "true" ? true : false;
						}
					}
				}
			}
		}

		std::vector<CHapiXMLNode*> LayerTiles = data->GetChildren();
		Layer layer;
		layer.m_tiles.reserve(LayerTiles.size());

		for (int tileNum = 0; tileNum < LayerTiles.size(); tileNum++)
		{
			CHapiXMLAttribute id;
			if (!LayerTiles[tileNum]->GetAttributeWithName("gid", id))
			{ 
				std::cerr << "A tile was missing a gid attribute, could not load map." << std::endl;
				return false;
			}
			int frameNum = id.AsInt() - 1;
			std::vector<Entity*> entitiesOnTile;
			//So no memeory alocation need to be done when pushing back 
			//entitiesOnTile.reserve(5);
			if (frameNum != -1)
			{
				Entity* e = nullptr;
				if (EntityLayer)
				{
					TileColision* tc = new TileColision("ExteriorTilesSheet", gs.getRect("ExteriorTilesSheet"), frameNum);
					e = tc;
				}
				else
				{
					TileNoColision* tnc = new TileNoColision("ExteriorTilesSheet", gs.getRect("ExteriorTilesSheet"), frameNum);
					e = tnc;
				}
				int y = tileNum / this->m_MapWidth;
				int x = tileNum % this->m_MapWidth;
				e->setPosition(x*m_TileWidth, y*m_TileHeight, false);
				entitiesOnTile.push_back(e);
			}
			layer.m_tiles.push_back(entitiesOnTile);
		}

		if (EntityLayer)
		{
			this->m_playerLayer = this->m_layers.size();
		}
		this->m_layers.push_back(layer);
	}

	return true;
}

Layer & TMap::getPlayerLayer()
{
	return this->m_layers[this->m_playerLayer];
}

std::vector<Layer>& TMap::getMap()
{
	return this->m_layers;
}

const unsigned int TMap::getMapWidth() const
{
	return this->m_MapHeight;
}

const unsigned int TMap::getMapHeight() const
{
	return this->m_MapHeight;
}

const unsigned int TMap::getTileWidth() const
{
	return this->m_TileWidth;
}

const unsigned int TMap::getTileHeight() const
{
	return this->m_TileHeight;
}
