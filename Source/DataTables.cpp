#include <Game/DataTables.hpp>
#include <Game/Character.hpp>
#include <Game/Tile.hpp>
#include <Game/Particle.hpp>


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<CharacterData> initializeCharacterData()
{
	std::vector<CharacterData> data(Character::TypeCount);

	data[Character::Player].hitpoints = 40;
	data[Character::Player].speed = 96.f;
	data[Character::Player].texture = Textures::Characters;
	//data[Character::Player].textureRect = sf::IntRect(16, 48, 16, 16);
	data[Character::Player].textureRect = sf::IntRect(18, 48, 12, 16);
	//data[Character::Player].textureRect = sf::IntRect(2, 0, 19, 24);
	data[Character::Player].hasIdleAnimation = false;

	return data;
}

std::vector<TileData> initializeTileData()
{
	std::vector<TileData> data(Tile::TypeCount);

	data[Tile::Floor].texture = Textures::Tiles;
	data[Tile::None].tilesetIndex = 16;

	data[Tile::Floor].texture = Textures::Tiles;
	data[Tile::Floor].tilesetIndex = 3;

	data[Tile::Wall].texture = Textures::Tiles;
	data[Tile::Wall].tilesetIndex = 21;

	data[Tile::TunnelFloor].texture = Textures::Tiles;
	data[Tile::TunnelFloor].tilesetIndex = 0;

	data[Tile::TunnelWall].texture = Textures::Tiles;
	data[Tile::TunnelWall].tilesetIndex = 19;

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(Particle::ParticleCount);
	return data;
}
