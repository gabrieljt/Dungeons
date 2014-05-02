#ifndef GAME_SPRITENODE_HPP
#define GAME_SPRITENODE_HPP

#include <Game/SceneNode.hpp>

#include <SFML/Graphics/Sprite.hpp>


class SpriteNode : public SceneNode
{
	public:
		explicit			SpriteNode(const sf::Texture& texture);
							SpriteNode(const sf::Texture& texture, const sf::IntRect& textureRect);


	protected:
		virtual void		drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	protected:
		sf::Sprite			mSprite;
};

#endif // GAME_SPRITENODE_HPP
