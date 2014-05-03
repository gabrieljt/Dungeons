#include <Game/Tile.hpp>
#include <Game/DataTables.hpp>
#include <Game/Utility.hpp>
#include <Game/ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


using namespace std::placeholders;

namespace
{
	const std::vector<TileData> Table 	= initializeTileData();
}

const unsigned int Tile::Size = 16u;

Tile::Tile(const ID id, Type type)
: mId(id)
, mType(type)
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
	return sf::FloatRect(mId.first * Tile::Size, mId.second * Tile::Size, Tile::Size, Tile::Size);
}

Tile::ID Tile::getID() const
{
	return mId;
}

Tile::Type Tile::getType() const
{
	return mType;
}

bool Tile::isWalkable() const
{
	return getCategory() == Category::WalkableTile;
}
