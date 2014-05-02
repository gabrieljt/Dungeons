#include <Game/Tile.hpp>
#include <Game/DataTables.hpp>
#include <Game/Utility.hpp>
#include <Game/CommandQueue.hpp>
#include <Game/ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>


using namespace std::placeholders;

namespace
{
	const std::vector<TileData> Table 	= initializeTileData();
}

const unsigned int Tile::Size = 16u;

Tile::Tile(Type type, const TextureHolder& textures, const TileID id)
: SpriteNode(textures.get(Table[type].texture), Table[type].textureRect)
, mType(type)
, mId(id)
{	
}

unsigned int Tile::getCategory() const
{
	switch (mType)
	{
		case Floor:
			return Category::WalkableTile;
		case Wall:
			return Category::UnwalkableTile;
		default:
			return Category::Tile;
	}
}

sf::FloatRect Tile::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Tile::isWalkable() const
{
	return getCategory() == Category::WalkableTile;
}
