#include <Game/Tilemap.hpp>
#include <Game/Utility.hpp>
#include <Game/ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <iostream>


Tilemap::Tilemap(const TextureHolder& textures)
: SceneNode(Category::Tilemap)
, mSize(10u, 10u)
, mBounds(0.f, 0.f, mSize.x * Tile::Size, mSize.y * Tile::Size)
, mImage()
, mMap()
{
	// sample SQUARE map.
	for (auto x = 1u; x < mSize.x - 1u; ++x)
		for (auto y = 1u; y < mSize.y - 1u; ++y)
		{
			if (x != 3u || y != 3u)
			{
				Tile::ID id(x, y);
				addTile(id, Tile::Type::Floor, textures);
			}
		}
	addTile(Tile::ID(3u, 3u), Tile::Type::Wall, textures);
	for (auto i = 0u; i < mSize.x; ++i)
	{
		Tile::ID firstRow(i, 0u);
		Tile::ID lastRow(i, mSize.x - 1u);
		Tile::ID firstColumn(0u, i);
		Tile::ID lastColumn(mSize.y - 1u, i);
		addTile(firstRow, Tile::Type::Wall, textures);
		addTile(lastRow, Tile::Type::Wall, textures);
		addTile(firstColumn, Tile::Type::Wall, textures);
		addTile(lastColumn, Tile::Type::Wall, textures);
	}

	// TODO: generate VertexArray
	for (auto x = 0; x < mSize.x; ++x)
		for (auto y = 0; y < mSize.y; ++y)
		{
			auto tilePtr = std::move(mMap[Tile::ID(x,y)]);
			auto tile = tilePtr.get();
			std::cout << tile->getID().first << " " << tile->getID().second << " || " << tile->getPosition().x << " " << tile->getPosition().y << std::endl;
		}
}

void Tilemap::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	// TODO: draw VertexArray
}
	
void Tilemap::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// TODO: update tiles and vertex array?
}

void Tilemap::addTile(Tile::ID id, Tile::Type type, const TextureHolder& textures)
{
	// TODO: assert inserted
	TilePtr tilePtr(new Tile(id, type, textures));
	auto tile = tilePtr.get();
	tile->setPosition(id.first * Tile::Size, id.second * Tile::Size);
	mMap[id] = std::move(tilePtr);
}