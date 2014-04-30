#include <Game/TextNode.hpp>
#include <Game/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

    
TextNode::TextNode(const FontHolder& fonts, const std::string& text, unsigned int characterSize)
{
	mText.setFont(fonts.get(Fonts::Main));
	mText.setCharacterSize(characterSize);
	setString(text);
}

void TextNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mText, states);
}

void TextNode::setString(const std::string& text)
{
	mText.setString(text);
	centerOrigin(mText);
}