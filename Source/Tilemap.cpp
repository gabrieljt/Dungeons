#include <Game/Tilemap.hpp>
#include <Game/Foreach.hpp>
#include <Game/Utility.hpp>
#include <Game/ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <algorithm>
#include <cassert>


Tilemap::Tilemap(const TextureHolder& textures)
: SceneNode(Category::Tilemap)
, mTileset(textures.get(Textures::Tiles))
, mSize(50u, 50u)
, mBounds(0.f, 0.f, mSize.x * Tile::Size, mSize.y * Tile::Size)
, mImage()
, mMap()
, mRooms()
{
	// fills the map, easier to build vertex array
	for (auto x = 0u; x < mSize.x; ++x)
		for (auto y = 0u; y < mSize.y; ++y)
		{
			Tile::ID id(x, y);	
			addTile(id, Tile::Type::None);
		}

	generateMap();

	mImage.setPrimitiveType(sf::Quads);
    mImage.resize(mSize.x * mSize.y * 4);
	for (auto x = 0u; x < mSize.x; ++x)
		for (auto y = 0u; y < mSize.y; ++y)
		{
			auto tile = mMap[Tile::ID(x,y)];
			auto type = tile->getType();			
			if (type == Tile::Type::None)
			{
				std::vector<Tilemap::TilePtr> neighbours;
				getNeighbours(tile->getID(), neighbours);
				FOREACH (auto neighbour, neighbours)
				{
					if (neighbour->isWalkable())
					{
						addTile(tile->getID(), Tile::Type::Wall);
						tile = mMap[Tile::ID(x,y)];
					}	
				}
			}
			auto tilesetIndex = tile->getTilesetIndex();

			// find its position in the tileset texture
			auto tu = tilesetIndex % (mTileset.getSize().x / Tile::Size);
			auto tv = tilesetIndex / (mTileset.getSize().x / Tile::Size);

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
	if (mMap.find(id) != mMap.end() && validateTile(id))
		mMap.erase(id);
	TilePtr tile = std::make_shared<Tile>(id, type);
	tile->setPosition(id.first * Tile::Size, id.second * Tile::Size);
	mMap[id] = tile;
}

Tilemap::TilePtr Tilemap::getTile(Tile::ID id)
{	
	if (validateTile(id))
		return mMap[id];
	else
		return nullptr;
}

Tilemap::TilePtr Tilemap::getTile(sf::Vector2f position)
{	
	Tile::ID id(position.x / Tile::Size, position.y / Tile::Size);
	return getTile(id);
}

void Tilemap::getNeighbours(Tile::ID id, std::vector<TilePtr>& neighbours)
{
	if (validateTile(Tile::ID(id.first - 1u, id.second - 1u)))
		neighbours.push_back(getTile(Tile::ID(id.first - 1u, id.second - 1u)));

	if (validateTile(Tile::ID(id.first - 1u, id.second)))
		neighbours.push_back(getTile(Tile::ID(id.first - 1u, id.second)));

	if (validateTile(Tile::ID(id.first - 1u, id.second + 1u)))
		neighbours.push_back(getTile(Tile::ID(id.first - 1u, id.second + 1u)));

	if (validateTile(Tile::ID(id.first, id.second - 1u)))
		neighbours.push_back(getTile(Tile::ID(id.first, id.second - 1u)));

	if (validateTile(Tile::ID(id.first, id.second + 1u)))
		neighbours.push_back(getTile(Tile::ID(id.first, id.second + 1u)));

	if (validateTile(Tile::ID(id.first + 1u, id.second - 1u)))
		neighbours.push_back(getTile(Tile::ID(id.first + 1u, id.second - 1u)));

	if (validateTile(Tile::ID(id.first + 1u, id.second)))
		neighbours.push_back(getTile(Tile::ID(id.first + 1u, id.second)));

	if (validateTile(Tile::ID(id.first + 1u, id.second + 1u)))
		neighbours.push_back(getTile(Tile::ID(id.first + 1u, id.second + 1u)));					
}

void Tilemap::getNeighbours(sf::Vector2f position, std::vector<TilePtr>& neighbours)
{
	Tile::ID id(position.x / Tile::Size, position.y / Tile::Size);
	getNeighbours(id, neighbours);
}

unsigned int Tilemap::getNumberRooms()
{
	return mRooms.size();
}

sf::Vector2i Tilemap::getRoomCenter(unsigned int index)
{
	assert(index < mRooms.size());
	return getCenter(*mRooms[index]);
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

bool Tilemap::validateTile(Tile::ID id)
{
    return mMap.find(id) != mMap.end() &&
    		id.first >= 0 && id.first < mSize.x && id.second >=0 && id.second < mSize.y;
}

void Tilemap::createRoom(sf::IntRect bounds)
{	
	Tile::ID id(bounds.left, bounds.top);
	BoundsPtr room(new sf::IntRect(bounds.left, bounds.top, bounds.width, bounds.height));
	mRooms.push_back(std::move(room));
	for (auto x = bounds.left; x < bounds.left + bounds.width; ++x)
		for (auto y = bounds.top; y < bounds.top + bounds.height; ++y)
		{
			Tile::ID id(x,y);
			addTile(id, Tile::Type::Floor);
		}
}

void Tilemap::createTunnelH(int x1, int x2, int y)
{
	for (auto x = std::min(x1, x2); x < std::max(x1, x2) + 1; ++x)
	{		
		addTile(Tile::ID(x, y), Tile::Type::Floor);
	}
}
void Tilemap::createTunnelV(int y1, int y2, int x)
{
	for (auto y = std::min(y1, y2); y < std::max(y1, y2) + 1; ++y)
	{		
		addTile(Tile::ID(x, y), Tile::Type::Floor);
	}
}

void Tilemap::generateMap()
{
	auto maxRooms 		= std::min(mSize.x, mSize.y) / 10u;
	auto roomMaxSize 	= std::max(mSize.x, mSize.y) / 10u;
	auto roomMinSize 	= std::min(mSize.x, mSize.y) / 10u;
	auto numberRooms 	= 0u;

	for (auto i = 0u; i < maxRooms; ++i)
	{
        auto width 	= randomInt(roomMinSize, roomMaxSize);
        auto height = randomInt(roomMinSize, roomMaxSize);
        // Random position without going out of the boundaries of the map
        auto x 		= randomInt(0, mSize.x - width - 1);
        auto y 		= randomInt(0, mSize.y - height - 1);

        sf::IntRect newRoom(x, y, width, height);
		bool isSameRoom = false;
		for (auto roomItr = mRooms.begin(); roomItr != mRooms.end(); ++roomItr)
		{			
			auto room = *roomItr->get();
			if (newRoom.intersects(room));
			{
				isSameRoom = true;
				break;
			}
		}

		auto newRoomCenter = getCenter(newRoom);
		if (numberRooms > 0)
		{
			auto previousRoomCenter = getCenter(*mRooms[numberRooms - 1]);
			
			// draw a coin (random number that is either 0 or 1)
            if (randomInt(1) == 1)
            {
                // first move horizontally, then vertically
                createTunnelH(previousRoomCenter.x, newRoomCenter.x, previousRoomCenter.y);
                createTunnelV(previousRoomCenter.y, newRoomCenter.y, newRoomCenter.x);
            }
            else
            {
                // first move vertically, then horizontally
                createTunnelV(previousRoomCenter.y, newRoomCenter.y, previousRoomCenter.x);
                createTunnelH(previousRoomCenter.x, newRoomCenter.x, newRoomCenter.y);
			}				
		}	
		// TODO: adjust same room bounds and center
		createRoom(newRoom);			
		++numberRooms;				
	}
}
