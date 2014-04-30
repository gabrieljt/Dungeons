#ifndef GAME_TEXTNODE_HPP
#define GAME_TEXTNODE_HPP

#include <Game/ResourceHolder.hpp>
#include <Game/ResourceIdentifiers.hpp>
#include <Game/SceneNode.hpp>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>


class TextNode : public SceneNode
{
	public:
		explicit			TextNode(const FontHolder& fonts, const std::string& text, unsigned int characterSize = 20u);

		void				setString(const std::string& text);


	private:
		virtual void		drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		sf::Text			mText;
};

#endif // GAME_TEXTNODE_HPP
