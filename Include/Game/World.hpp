#ifndef GAME_WORLD_HPP
#define GAME_WORLD_HPP

#include <Game/ResourceHolder.hpp>
#include <Game/ResourceIdentifiers.hpp>
#include <Game/SceneNode.hpp>
#include <Game/SpriteNode.hpp>
#include <Game/Character.hpp>
#include <Game/CommandQueue.hpp>
#include <Game/Command.hpp>
#include <Game/BloomEffect.hpp>
#include <Game/SoundPlayer.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <queue>


// Forward declaration
namespace sf
{
	class RenderTarget;
}

class World : private sf::NonCopyable
{
	public:
											World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds);
		void								update(sf::Time dt);
		void								draw();
		
		CommandQueue&						getCommandQueue();

		bool 								hasAlivePlayer() const;


	private:
		void								loadTextures();
		void								setupView();
		void								adaptViewPosition();
		void								adaptPlayerPosition();
		void								adaptPlayerVelocity();
		void								handleCollisions();
		void								updateSounds();

		void								buildScene();
		sf::FloatRect						getViewBounds() const;


	private:
		enum Layer
		{
			Background,
			Main,
			LayerCount
		};


	private:
		sf::RenderTarget&					mTarget;
		sf::RenderTexture					mSceneTexture;
		sf::View							mWorldView;
		TextureHolder						mTextures;
		FontHolder&							mFonts;
		SoundPlayer&						mSounds;

		SceneNode							mSceneGraph;
		std::array<SceneNode*, LayerCount>	mSceneLayers;
		CommandQueue						mCommandQueue;

		unsigned int 						mTileSize;
		sf::FloatRect						mWorldBounds;
		sf::Vector2f						mSpawnPosition;
		Character*							mPlayerCharacter;

		BloomEffect							mBloomEffect;
};

#endif // GAME_WORLD_HPP
