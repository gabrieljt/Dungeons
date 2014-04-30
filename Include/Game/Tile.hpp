#ifndef GAME_TILE_HPP
#define GAME_TILE_HPP

#include <Game/Entity.hpp>
#include <Game/Command.hpp>
#include <Game/ResourceIdentifiers.hpp>

#include <SFML/Graphics/Sprite.hpp>


class Tile : public Entity
{
	public:
		enum Type
		{
			Floor,
			Wall,
			TypeCount,
		};

	
	public:
								Tile(Type type, const TextureHolder& textures, const FontHolder& fonts, const unsigned int id = 0u);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;
		virtual void			remove();
		virtual bool 			isMarkedForRemoval() const;	

		bool					isWalkable() const;


	private:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);


	private:
		Type					mType;
		sf::Sprite				mSprite;		

		const unsigned int 		mId;
};

#endif // GAME_TILE_HPP
