#ifndef GAME_SOUNDNODE_HPP
#define GAME_SOUNDNODE_HPP

#include <Game/SceneNode.hpp>
#include <Game/ResourceIdentifiers.hpp>


class SoundPlayer;

class SoundNode : public SceneNode
{
	public:
		explicit				SoundNode(SoundPlayer& player);
		void					playSound(SoundEffect::ID sound, sf::Vector2f position);

		virtual unsigned int	getCategory() const;


	private:
		SoundPlayer&			mSounds;
};

#endif // GAME_SOUNDNODE_HPP
