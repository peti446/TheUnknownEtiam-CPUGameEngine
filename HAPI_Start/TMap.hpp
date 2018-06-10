#pragma once

#include <vector>
#include <string>

class Entity;
namespace GRAPHICS {
	class GraphicsSystem;
};
struct Layer {
	std::vector<std::vector<Entity*>> m_tiles;
};

class TMap
{
public:

	TMap();
	~TMap();

	bool loadMap(const std::string& filePath, const GRAPHICS::GraphicsSystem& gs);

	Layer& getPlayerLayer();
	std::vector<Layer>& getMap();

	const unsigned int getMapWidth() const;
	const unsigned int getMapHeight() const;
	const unsigned int getTileWidth() const;
	const unsigned int getTileHeight() const;
private:
	unsigned int m_MapWidth;
	unsigned int m_MapHeight;
	unsigned int m_TileWidth;
	unsigned int m_TileHeight;

	size_t m_playerLayer;

	std::vector<Layer> m_layers;
};

