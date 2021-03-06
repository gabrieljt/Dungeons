#ifndef GAME_DUNGEON_HPP
#define GAME_DUNGEON_HPP

#include <Game/ResourceHolder.hpp>
#include <Game/ResourceIdentifiers.hpp>
#include <Game/SceneNode.hpp>
#include <Game/SpriteNode.hpp>
#include <Game/Character.hpp>
#include <Game/Tile.hpp>
#include <Game/Tilemap.hpp>
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

class Dungeon : private sf::NonCopyable
{
	public:
											Dungeon(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds);
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
		void								addEnemies();
		void								addEnemy(Character::Type type, float relX, float relY);
    	void								spawnEnemies();
        
        void 								destroyEntitiesOutsideView();
		sf::FloatRect						getViewBounds() const;
		sf::FloatRect						getBattlefieldBounds() const;


	private:
		enum Layer
		{
			Background,
			Main,
			LayerCount
		};

		template <typename EntityType>
		struct SpawnPoint 
		{
			SpawnPoint(EntityType type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
            , attached()
            , hitpoints()
			{
			}

			EntityType type;
			float x;
			float y;
            bool attached;
            int hitpoints;
		};
    
        typedef SpawnPoint<Character::Type>         CharacterSpawnPoint;


	private:
		sf::RenderTarget&					mTarget;
		sf::RenderTexture					mSceneTexture;
		sf::View							mView;
		TextureHolder						mTextures;
		FontHolder&							mFonts;
		SoundPlayer&						mSounds;

		SceneNode							mSceneGraph;
		std::array<SceneNode*, LayerCount>	mSceneLayers;
		CommandQueue						mCommandQueue;

		Tilemap*							mTilemap;

		sf::Vector2f						mSpawnPosition;		
		Character*							mPlayerCharacter;

		std::vector<CharacterSpawnPoint>    mEnemySpawnPoints;

		BloomEffect							mBloomEffect;
};

#endif // GAME_DUNGEON_HPP
