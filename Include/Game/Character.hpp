#ifndef GAME_CHARACTER_HPP
#define GAME_CHARACTER_HPP

#include <Game/Entity.hpp>
#include <Game/Command.hpp>
#include <Game/ResourceIdentifiers.hpp>
#include <Game/TextNode.hpp>
#include <Game/Animation.hpp>

#include <SFML/Graphics/Sprite.hpp>


class Character : public Entity
{
	public:
		enum Type
		{
			Player,						
			TypeCount
		};


	public:
								Character(Type type, const TextureHolder& textures, const FontHolder& fonts);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;
		virtual void			remove();
		virtual bool 			isMarkedForRemoval() const;
		bool					isAllied() const;
		float					getMaxSpeed() const;

		void					playLocalSound(CommandQueue& commands, SoundEffect::ID effect);


	private:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);
		void					updateMovementPattern(sf::Time dt);

		void					updateTexts();
		void					updateIdleAnimation();


	private:
		Type					mType;
		sf::Sprite				mSprite;
		Animation				mIdle;

		float					mTravelledDistance;
		std::size_t				mDirectionIndex;
};

#endif // GAME_CHARACTER_HPP
