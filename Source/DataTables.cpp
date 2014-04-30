#include <Game/DataTables.hpp>
#include <Game/Character.hpp>
#include <Game/Tile.hpp>
#include <Game/Particle.hpp>


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<CharacterData> initializeCharacterData()
{
	std::vector<CharacterData> data(Character::TypeCount);

	data[Character::Player].hitpoints = 100;
	data[Character::Player].speed = 200.f;
	data[Character::Player].texture = Textures::Characters;
	data[Character::Player].textureRect = sf::IntRect(16, 46, 16, 18);
	data[Character::Player].hasIdleAnimation = false;

	return data;
}

std::vector<TileData> initializeTileData()
{
	std::vector<TileData> data(Tile::TypeCount);

	data[Tile::Floor].hitpoints = 1;
	data[Tile::Floor].texture = Textures::Tiles;
	data[Tile::Floor].textureRect = sf::IntRect(0, 0, 16, 16);

	data[Tile::Wall].hitpoints = 1;
	data[Tile::Wall].texture = Textures::Tiles;
	data[Tile::Wall].textureRect = sf::IntRect(80, 32, 16, 16);

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(Particle::ParticleCount);
	return data;
}
