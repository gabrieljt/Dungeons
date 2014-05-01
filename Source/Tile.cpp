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
	const std::vector<TileData> Table = initializeTileData();
}

Tile::Tile(Type type, const TextureHolder& textures, const FontHolder& fonts, const TileID id)
: Entity(Table[type].hitpoints)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mId(id)
{	
}

void Tile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

void Tile::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (isDestroyed())
	{
		return;
	}

	Entity::updateCurrent(dt, commands);
}

unsigned int Tile::getCategory() const
{
	return Category::Tile;
}

sf::FloatRect Tile::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Tile::isMarkedForRemoval() const
{
	return isDestroyed();
}

void Tile::remove()
{
	Entity::remove();
}

bool Tile::isWalkable() const
{
	return mType == Floor;
}