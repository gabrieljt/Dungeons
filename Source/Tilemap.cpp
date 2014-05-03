#include <Game/Tilemap.hpp>
#include <Game/Utility.hpp>
#include <Game/ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cassert>


Tilemap::Tilemap(const TextureHolder& textures)
: SceneNode(Category::Tilemap)
, mTileset(textures.get(Textures::Tiles))
, mSize(200u, 200u)
, mBounds(0.f, 0.f, mSize.x * Tile::Size, mSize.y * Tile::Size)
, mImage()
, mMap()
{
	// sample SQUARE map.
	for (auto x = 0u; x < mSize.x; ++x)
		for (auto y = 0u; y < mSize.y; ++y)
		{
			Tile::ID id(x, y);
			if (x % 2 != 0 && y % 2 != 0)
			{				
				addTile(id, Tile::Type::Wall);
			}
			else
				addTile(id, Tile::Type::Floor);
		}

	mImage.setPrimitiveType(sf::Quads);
    mImage.resize(mSize.x * mSize.y * 4);
	for (auto x = 0u; x < mSize.x; ++x)
		for (auto y = 0u; y < mSize.y; ++y)
		{
			auto tile = mMap[Tile::ID(x,y)];
			auto type = tile->getType();

			// find its position in the tileset texture
			auto tu = type % (mTileset.getSize().x / Tile::Size);
			auto tv = type / (mTileset.getSize().x / Tile::Size);

			// get a pointer to the current tile's quad
			sf::Vertex* quad = &mImage[(x + y * mSize.x) * 4];

			// define its 4 corners
			quad[0].position = sf::Vector2f(x * Tile::Size, y * Tile::Size);
			quad[1].position = sf::Vector2f((x + 1) * Tile::Size, y * Tile::Size);
			quad[2].position = sf::Vector2f((x + 1) * Tile::Size, (y + 1) * Tile::Size);
			quad[3].position = sf::Vector2f(x * Tile::Size, (y + 1) * Tile::Size);

			// define its 4 texture coordinates
			quad[0].texCoords = sf::Vector2f(tu * Tile::Size, tv * Tile::Size);
			quad[1].texCoords = sf::Vector2f((tu + 1) * Tile::Size, tv * Tile::Size);
			quad[2].texCoords = sf::Vector2f((tu + 1) * Tile::Size, (tv + 1) * Tile::Size);
			quad[3].texCoords = sf::Vector2f(tu * Tile::Size, (tv + 1) * Tile::Size);
		}
}

void Tilemap::addTile(Tile::ID id, Tile::Type type)
{
	TilePtr tile(new Tile(id, type));
	tile->setPosition(id.first * Tile::Size, id.second * Tile::Size);
	mMap[id] = tile;
}

Tilemap::TilePtr Tilemap::getTile(Tile::ID id)
{
	assert(mMap.find(id) != mMap.end());
	return mMap[id];
}

Tilemap::TilePtr Tilemap::getTile(sf::Vector2f position)
{
	Tile::ID id(position.x / Tile::Size, position.y / Tile::Size);
	return getTile(id);
}

void Tilemap::getNeighbours(Tile::ID id, std::vector<TilePtr>& neighbours)
{
	TilePtr tile = getTile(id);
	neighbours.push_back(getTile(Tile::ID(id.first - 1u, id.second - 1u)));
	neighbours.push_back(getTile(Tile::ID(id.first - 1u, id.second)));
	neighbours.push_back(getTile(Tile::ID(id.first - 1u, id.second + 1u)));
	neighbours.push_back(getTile(Tile::ID(id.first, id.second - 1u)));
	neighbours.push_back(getTile(Tile::ID(id.first, id.second + 1u)));
	neighbours.push_back(getTile(Tile::ID(id.first + 1u, id.second - 1u)));
	neighbours.push_back(getTile(Tile::ID(id.first + 1u, id.second)));
	neighbours.push_back(getTile(Tile::ID(id.first + 1u, id.second + 1u)));			
}

void Tilemap::getNeighbours(sf::Vector2f position, std::vector<TilePtr>& neighbours)
{
	Tile::ID id(position.x / Tile::Size, position.y / Tile::Size);
	getNeighbours(id, neighbours);
}

sf::FloatRect Tilemap::getBoundingRect() const
{
	return mBounds;
}

void Tilemap::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the transform
	states.transform *= getTransform();
    
	// apply the tileset texture
	states.texture = &mTileset;

	// draw the vertex array
	target.draw(mImage, states);
}
	
void Tilemap::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// TODO: update tiles and vertex array?
}
