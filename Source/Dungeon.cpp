#include <Game/Dungeon.hpp>
#include <Game/Tile.hpp>
#include <Game/Foreach.hpp>
#include <Game/TextNode.hpp>
#include <Game/ParticleNode.hpp>
#include <Game/SoundNode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>


Dungeon::Dungeon(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, const unsigned int level)
: mTarget(outputTarget)
, mSceneTexture()
, mDungeonView(outputTarget.getDefaultView())
, mTextures() 
, mFonts(fonts)
, mSounds(sounds)
, mSceneGraph()
, mSceneLayers()
, mLevel(level)
, mTileSize(16u)
, mDungeonBounds()
, mSpawnPosition()
, mPlayerCharacter(nullptr)
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();
	setupView();
}

void Dungeon::update(sf::Time dt)
{	
	mPlayerCharacter->setVelocity(0.f, 0.f);
	adaptViewPosition();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	handleCollisions();

	mSceneGraph.removeWrecks();

	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
}

void Dungeon::draw()
{
		mTarget.setView(mDungeonView);
		mTarget.draw(mSceneGraph);
}

CommandQueue& Dungeon::getCommandQueue()
{
	return mCommandQueue;
}

bool Dungeon::hasAlivePlayer() const
{
	return !mPlayerCharacter->isMarkedForRemoval();
}

void Dungeon::loadTextures()
{
	mTextures.load(Textures::Characters,	"Media/Textures/characters.png");
	mTextures.load(Textures::Tiles,			"Media/Textures/tiles.png");
}

void Dungeon::setupView()
{
	sf::Vector2u visibleArea(mTileSize * 10u, mTileSize * 10u); //10x10 cells, each cell has 16x16 pixels
	auto zoom = std::min(visibleArea.x, visibleArea.y) / std::min(mDungeonView.getSize().x, mDungeonView.getSize().y);	
	mDungeonView.setCenter(mSpawnPosition);	
	mDungeonView.zoom(zoom);
}

void Dungeon::adaptViewPosition()
{
    // Keep camera's position and view bounds inside the world bounds
    sf::Vector2f borderDistance = sf::Vector2f(getViewBounds().width / 2.f, getViewBounds().height / 2.f);
    
	sf::Vector2f position = mPlayerCharacter->getPosition();
    mDungeonView.setCenter(position);
	position.x = std::max(position.x, mDungeonBounds.left + borderDistance.x);
	position.x = std::min(position.x, mDungeonBounds.left + mDungeonBounds.width - borderDistance.x);
	position.y = std::max(position.y, mDungeonBounds.top + borderDistance.y);
	position.y = std::min(position.y, mDungeonBounds.top + mDungeonBounds.height - borderDistance.y);
	mDungeonView.setCenter(position);
}

void Dungeon::adaptPlayerPosition()
{
	// Keep player's position inside the world bounds, at least borderDistance units from the border
	const float borderDistance = 12.f;

	sf::Vector2f position = mPlayerCharacter->getPosition();
	position.x = std::max(position.x, mDungeonBounds.left + borderDistance);
	position.x = std::min(position.x, mDungeonBounds.left + mDungeonBounds.width - borderDistance);
	position.y = std::max(position.y, mDungeonBounds.top + borderDistance);
	position.y = std::min(position.y, mDungeonBounds.top + mDungeonBounds.height - borderDistance);
	mPlayerCharacter->setPosition(position);
}

void Dungeon::adaptPlayerVelocity()
{
	sf::Vector2f velocity = mPlayerCharacter->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerCharacter->setVelocity(velocity / std::sqrt(2.f));
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void Dungeon::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	FOREACH(SceneNode::Pair pair, collisionPairs)
	{
		/*
		if (matchesCategories(pair, Category::PlayerCharacter, Category::EnemyCharacter))
		{
			auto& player = static_cast<Character&>(*pair.first);
			auto& enemy = static_cast<Character&>(*pair.second);

			// Collision: Player damage = enemy's remaining HP
			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}
		*/
	}
}

void Dungeon::updateSounds()
{
	// Set listener's position to player position
	mSounds.setListenerPosition(mPlayerCharacter->getWorldPosition());

	// Remove unused sounds
	mSounds.removeStoppedSounds();
}

void Dungeon::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == Main) ? Category::SceneLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// TODO: generate random dungeon (pixels) and map (tiles) bounds
	const sf::Vector2u tilemapSize = sf::Vector2u(100u, 100u); // 100x100 cells
	mDungeonBounds = sf::FloatRect(0.f, 0.f, mTileSize * tilemapSize.x, mTileSize * tilemapSize.y); //1600x1600 pixels
	// TODO: generate LEVEL!!!!
	std::unique_ptr<Tile> floorTile(new Tile(Tile::Floor, mTextures, mFonts));	
	auto tile = floorTile.get();
	tile->setPosition(0.f, 0.f);
	mSceneLayers[Background]->attachChild(std::move(floorTile));
	std::unique_ptr<Tile> wallTile(new Tile(Tile::Wall, mTextures, mFonts));	
	tile = wallTile.get();
	tile->setPosition(16.f, 0.f);
	mSceneLayers[Main]->attachChild(std::move(wallTile));

	//TODO: generate random spawn position
	mSpawnPosition = sf::Vector2f(mDungeonBounds.width / 2.f, mDungeonBounds.height / 2.f);

	// Add player's character
	std::unique_ptr<Character> player(new Character(Character::Player, mTextures, mFonts));
	mPlayerCharacter = player.get();
	mPlayerCharacter->setPosition(mSpawnPosition);
	mSceneLayers[Main]->attachChild(std::move(player));
}

sf::FloatRect Dungeon::getViewBounds() const
{
	return sf::FloatRect(mDungeonView.getCenter() - mDungeonView.getSize() / 2.f, mDungeonView.getSize());
}
