#include <Game/World.hpp>
#include <Game/Foreach.hpp>
#include <Game/TextNode.hpp>
#include <Game/ParticleNode.hpp>
#include <Game/SoundNode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>


World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds)
: mTarget(outputTarget)
, mSceneTexture()
, mWorldView(outputTarget.getDefaultView())
, mTextures() 
, mFonts(fonts)
, mSounds(sounds)
, mSceneGraph()
, mSceneLayers()
, mTileSize(16u)
, mWorldBounds(0.f, 0.f, mTileSize * 100.f, mTileSize * 100.f)
, mSpawnPosition(mWorldBounds.width / 2.f, mWorldBounds.height / 2.f)
, mPlayerCharacter(nullptr)
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();
	setupView();
}

void World::update(sf::Time dt)
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

void World::draw()
{
		mTarget.setView(mWorldView);
		mTarget.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

bool World::hasAlivePlayer() const
{
	return !mPlayerCharacter->isMarkedForRemoval();
}

void World::loadTextures()
{
	mTextures.load(Textures::Characters,	"Media/Textures/characters.png");
	mTextures.load(Textures::Tiles,			"Media/Textures/tiles.png");
}

void World::setupView()
{
	sf::Vector2u visibleArea(mTileSize * 10u, mTileSize * 10u);
	auto zoom = std::min(visibleArea.x, visibleArea.y) / std::min(mWorldView.getSize().x, mWorldView.getSize().y);	
	mWorldView.setCenter(mSpawnPosition);	
	mWorldView.zoom(zoom);
}

void World::adaptViewPosition()
{
    // Keep camera's position and view bounds inside the world bounds
    sf::Vector2f borderDistance = sf::Vector2f(getViewBounds().width / 2.f, getViewBounds().height / 2.f);
    
	sf::Vector2f position = mPlayerCharacter->getPosition();
    mWorldView.setCenter(position);
	position.x = std::max(position.x, mWorldBounds.left + borderDistance.x);
	position.x = std::min(position.x, mWorldBounds.left + mWorldBounds.width - borderDistance.x);
	position.y = std::max(position.y, mWorldBounds.top + borderDistance.y);
	position.y = std::min(position.y, mWorldBounds.top + mWorldBounds.height - borderDistance.y);
	mWorldView.setCenter(position);
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the world bounds, at least borderDistance units from the border
	const float borderDistance = 12.f;

	sf::Vector2f position = mPlayerCharacter->getPosition();
	position.x = std::max(position.x, mWorldBounds.left + borderDistance);
	position.x = std::min(position.x, mWorldBounds.left + mWorldBounds.width - borderDistance);
	position.y = std::max(position.y, mWorldBounds.top + borderDistance);
	position.y = std::min(position.y, mWorldBounds.top + mWorldBounds.height - borderDistance);
	mPlayerCharacter->setPosition(position);
}

void World::adaptPlayerVelocity()
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

void World::handleCollisions()
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

void World::updateSounds()
{
	// Set listener's position to player position
	mSounds.setListenerPosition(mPlayerCharacter->getWorldPosition());

	// Remove unused sounds
	mSounds.removeStoppedSounds();
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == Main) ? Category::SceneLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Add player's character
	std::unique_ptr<Character> player(new Character(Character::Player, mTextures, mFonts));
	mPlayerCharacter = player.get();
	mPlayerCharacter->setPosition(mSpawnPosition);
	mSceneLayers[Main]->attachChild(std::move(player));
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}
