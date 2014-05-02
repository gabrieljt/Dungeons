#ifndef GAME_TILE_HPP
#define GAME_TILE_HPP

#include <Game/Command.hpp>
#include <Game/ResourceIdentifiers.hpp>
#include <Game/SpriteNode.hpp>
#include <Game/TextNode.hpp>

#include <SFML/Graphics/Sprite.hpp>


class Tile : public SpriteNode
{
	public:
		typedef std::pair<unsigned int, unsigned int> TileID;
		static const unsigned int Size;


	public:
		enum Type
		{
			Floor,
			Wall,
			TypeCount,
		};

	
	public:
								Tile(Type type, const TextureHolder& textures, const TileID);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;

		bool					isWalkable() const;


	private:
		Type					mType;

		const TileID			mId;		
		TextNode* 				mIdDisplay;
};

#endif // GAME_TILE_HPP
