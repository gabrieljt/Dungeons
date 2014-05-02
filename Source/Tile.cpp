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

Tile::Tile(Type type, const TextureHolder& textures, const FontHolder& fonts, const TileID id)
: Entity(Table[type].hitpoints)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mId(id)
, mIdDisplay()
{
	centerOrigin(mSprite);
	if (Utility::Debug)
	{
		std::unique_ptr<TextNode> idDisplay(new TextNode(fonts, ""));
		mIdDisplay = idDisplay.get();
		attachChild(std::move(idDisplay));
		updateTexts();	
	}
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

void Tile::updateTexts()
{	
	mIdDisplay->setString(toString(mId.first) + toString(mId.second));
	mIdDisplay->setPosition(0.f, 0.f);
	mIdDisplay->setRotation(-getRotation());
	mIdDisplay->setCharacterSize(5u);
}
