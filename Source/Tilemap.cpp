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
, mSize()
, mBounds()
, mImage()
, mMap()
, mRooms()
{
	generateMap();	
	generateMapImage();
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
	assert(validateTile(id));
	return mMap[id];
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

void Tilemap::getRoom(Tile::ID id, std::vector<TilePtr>& tiles)
{
	if (validateTile(id))
	{
		for (auto roomItr = mRooms.begin(); roomItr != mRooms.end(); ++roomItr)
		{			
			sf::IntRect room = *roomItr->get();
			if (room.contains(id.first, id.second))
			{
				for (auto x = room.left; x < room.left + room.width; ++x)
					for (auto y = room.top; y < room.top + room.height; ++y)
					{
						tiles.push_back(getTile(Tile::ID(x, y)));
					}
			}
		}
	}
}

void Tilemap::getRoom(sf::Vector2f position, std::vector<TilePtr>& tiles)
{
	Tile::ID id(position.x / Tile::Size, position.y / Tile::Size);
	getRoom(id, tiles);
}

sf::Vector2i Tilemap::getRandomRoomCenter()
{
	unsigned int index = randomInt(mRooms.size());
	assert(index < mRooms.size());
	return getCenter(*mRooms[index]);
}

sf::FloatRect Tilemap::getBoundingRect() const
{
	return mBounds;
}

void Tilemap::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = &mTileset;
	target.draw(mImage, states);
}
	
void Tilemap::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// TODO: update tiles and vertex array? not now.
}

bool Tilemap::validateTile(Tile::ID id)
{
    return id.first >= 0 && id.first < mSize.x && id.second >= 0 && id.second < mSize.y;
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
			if (id.first == 0u || id.first == mSize.x - 1u || id.second == 0u || id.second == mSize.y - 1u)
				addTile(id, Tile::Type::Wall);
			else
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
	// TODO: improve generation...
	int randomFactor = 10;
	int randomFactorX = 1 + randomInt(randomFactor);
	int randomFactorY = 1 + randomInt(randomFactor);
	mSize.x = 10u + 10 * randomFactorX;
	mSize.y = 10u + 10 * randomFactorY;
	mBounds = sf::FloatRect(0.f, 0.f, mSize.x * Tile::Size, mSize.y * Tile::Size);
	// Fills the map, easier to manage
	for (auto x = 0u; x < mSize.x; ++x)
		for (auto y = 0u; y < mSize.y; ++y)
		{
			Tile::ID id(x, y);	
			addTile(id, Tile::Type::None);
		}

	auto maxRooms 		= std::max(mSize.x, mSize.y) / std::min(randomFactorX, randomFactorY);
	auto roomMinSize 	= 3u;
	auto roomMaxSize 	= 10u;
	auto numberRooms 	= 0u;
	// Generate Rooms (Floor)
	for (auto i = 0u; i < maxRooms; ++i)
	{
        auto width 	= roomMinSize + randomInt(roomMaxSize);
        auto height = roomMinSize + randomInt(roomMaxSize);
        auto x 		= randomInt(mSize.x - width - 1);
        auto y 		= randomInt(mSize.y - height - 1);

        sf::IntRect newRoom(x, y, width, height);
		bool failed = false;
		for (auto roomItr = mRooms.begin(); roomItr != mRooms.end(); ++roomItr)
		{			
			sf::IntRect room = *roomItr->get();
			if (newRoom.intersects(room))
			{
				failed = true;
				break;
			}
		}
		if (!failed)
		{
			auto newRoomCenter = getCenter(newRoom);
			if (numberRooms > 0)
			{
				auto previousRoomCenter = getCenter(*mRooms[numberRooms - 1]);
				
	            if (randomInt(1) == 1)
	            {
	                createTunnelH(previousRoomCenter.x, newRoomCenter.x, previousRoomCenter.y);
	                createTunnelV(previousRoomCenter.y, newRoomCenter.y, newRoomCenter.x);
	            }
	            else
	            {
	                createTunnelV(previousRoomCenter.y, newRoomCenter.y, previousRoomCenter.x);
	                createTunnelH(previousRoomCenter.x, newRoomCenter.x, newRoomCenter.y);
				}				
			}	
			createRoom(newRoom);			
			++numberRooms;	
		}			
	}
	// Generate Walls
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
		}
}

void Tilemap::generateMapImage()
{
	mImage.setPrimitiveType(sf::Quads);
    mImage.resize(mSize.x * mSize.y * 4);
	for (auto x = 0u; x < mSize.x; ++x)
		for (auto y = 0u; y < mSize.y; ++y)
		{
			auto tilesetIndex = mMap[Tile::ID(x,y)]->getTilesetIndex();

			auto tu = tilesetIndex % (mTileset.getSize().x / Tile::Size);
			auto tv = tilesetIndex / (mTileset.getSize().x / Tile::Size);

			sf::Vertex* quad = &mImage[(x + y * mSize.x) * 4];

			quad[0].position = sf::Vector2f(x * Tile::Size, y * Tile::Size);
			quad[1].position = sf::Vector2f((x + 1) * Tile::Size, y * Tile::Size);
			quad[2].position = sf::Vector2f((x + 1) * Tile::Size, (y + 1) * Tile::Size);
			quad[3].position = sf::Vector2f(x * Tile::Size, (y + 1) * Tile::Size);

			quad[0].texCoords = sf::Vector2f(tu * Tile::Size, tv * Tile::Size);
			quad[1].texCoords = sf::Vector2f((tu + 1) * Tile::Size, tv * Tile::Size);
			quad[2].texCoords = sf::Vector2f((tu + 1) * Tile::Size, (tv + 1) * Tile::Size);
			quad[3].texCoords = sf::Vector2f(tu * Tile::Size, (tv + 1) * Tile::Size);
		}
}
