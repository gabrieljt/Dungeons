#ifndef GAME_TILE_HPP
#define GAME_TILE_HPP

#include <Game/SpriteNode.hpp>
#include <Game/ResourceIdentifiers.hpp>
#include <Game/TextNode.hpp>


class Tile : public SpriteNode
{
	public:
		typedef std::pair<unsigned int, unsigned int> ID;
		static const unsigned int Size;


	public:
		enum Type
		{
			Floor,
			Wall,
			TypeCount,
		};

	
	public:
								Tile(const ID id, Type type, const TextureHolder& textures);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;

		ID 						getID() const;
		bool					isWalkable() const;


	private:
		const ID				mId;				
		Type					mType;
};

#endif // GAME_TILE_HPP
