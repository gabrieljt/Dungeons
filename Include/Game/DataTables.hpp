#ifndef GAME_DATATABLES_HPP
#define GAME_DATATABLES_HPP

#include <Game/ResourceIdentifiers.hpp>

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <vector>
#include <functional>


class Character;

struct Direction
{
	Direction(float angle, float distance)
	: angle(angle)
	, distance(distance)
	{
	}

	float angle;
	float distance;
};

struct CharacterData
{
	int								hitpoints;
	float							speed;
	Textures::ID					texture;
	sf::IntRect						textureRect;
	std::vector<Direction>			directions;
	bool							hasIdleAnimation;
};

struct TileData
{
	Textures::ID					texture;
	unsigned int 					tilesetIndex;
};

struct ParticleData
{
	sf::Color						color;
	sf::Time						lifetime;
};

std::vector<CharacterData>	initializeCharacterData();
std::vector<TileData>		initializeTileData();
std::vector<ParticleData>	initializeParticleData();

#endif // GAME_DATATABLES_HPP
