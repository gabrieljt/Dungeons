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
		Type 					getType() const;
		bool					isWalkable() const;


	private:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		const ID				mId;				
		const Type				mType;
};

#endif // GAME_TILE_HPP
