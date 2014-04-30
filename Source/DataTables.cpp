#include <Game/DataTables.hpp>
#include <Game/Character.hpp>
#include <Game/Particle.hpp>


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<CharacterData> initializeCharacterData()
{
	std::vector<CharacterData> data(Character::TypeCount);

	data[Character::Player].hitpoints = 100;
	data[Character::Player].speed = 200.f;
	data[Character::Player].texture = Textures::Characters;
	data[Character::Player].textureRect = sf::IntRect(2, 0, 22, 24);
	data[Character::Player].hasWalkAnimation = false;

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(Particle::ParticleCount);
	return data;
}
