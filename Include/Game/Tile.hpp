#ifndef GAME_TILE_HPP
#define GAME_TILE_HPP

#include <Game/SceneNode.hpp>
#include <Game/ResourceIdentifiers.hpp>
#include <Game/TextNode.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>


class Tile : public SceneNode
{
	public:
		typedef std::pair<unsigned int, unsigned int> ID;
		static const unsigned int Size;


	public:
		enum Type
		{
			None,
			Floor,
			Wall,
			TunnelFloor,
			TunnelWall,
			TypeCount,
		};

	
	public:
								Tile(const ID id, Type type);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;

		ID 						getID() const;
		Type 					getType() const;
		unsigned int 			getTilesetIndex() const;
		bool					isWalkable() const;


	private:
		const ID				mId;				
		const Type				mType;
};

#endif // GAME_TILE_HPP
