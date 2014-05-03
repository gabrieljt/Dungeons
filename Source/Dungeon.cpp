#include <Game/Dungeon.hpp>
#include <Game/Foreach.hpp>
#include <Game/TextNode.hpp>
#include <Game/ParticleNode.hpp>
#include <Game/SoundNode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>


Dungeon::Dungeon(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds)
: mTarget(outputTarget)
, mSceneTexture()
, mView(outputTarget.getDefaultView())
, mTextures() 
, mFonts(fonts)
, mSounds(sounds)
, mSceneGraph()
, mSceneLayers()
, mCommandQueue()
, mTilemap()
, mDungeonBounds()
, mSpawnPosition()
, mPlayerCharacter(nullptr)
, mBloomEffect()
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();
	setupView();
}

void Dungeon::update(sf::Time dt)
{	
	adaptViewPosition();

	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	handleCollisions();

	mSceneGraph.removeWrecks();

	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
	mPlayerCharacter->setVelocity(0.f, 0.f);	
}

void Dungeon::draw()
{
	mTarget.setView(mView);
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
	auto visibleArea = Tile::Size * 5u; // 5x5 cells
	auto zoom = visibleArea / std::min(mView.getSize().x, mView.getSize().y);	
	mView.setCenter(mSpawnPosition);	
	mView.zoom(zoom);
}

void Dungeon::adaptViewPosition()
{
    sf::Vector2f borderDistance = sf::Vector2f(getViewBounds().width / 2.f, getViewBounds().height / 2.f);
    
	sf::Vector2f position = mPlayerCharacter->getPosition();
    mView.setCenter(position);
	position.x = std::max(position.x, mDungeonBounds.left + borderDistance.x);
	position.x = std::min(position.x, mDungeonBounds.left + mDungeonBounds.width - borderDistance.x);
	position.y = std::max(position.y, mDungeonBounds.top + borderDistance.y);
	position.y = std::min(position.y, mDungeonBounds.top + mDungeonBounds.height - borderDistance.y);
	mView.setCenter(position);
}

void Dungeon::adaptPlayerPosition()
{
	const auto borderDistance = Tile::Size / 2;

	sf::Vector2f position = mPlayerCharacter->getPosition();
	position.x = std::max(position.x, mDungeonBounds.left + borderDistance);
	position.x = std::min(position.x, mDungeonBounds.left + mDungeonBounds.width - borderDistance);
	position.y = std::max(position.y, mDungeonBounds.top + borderDistance);
	position.y = std::min(position.y, mDungeonBounds.top + mDungeonBounds.height - borderDistance);
	mPlayerCharacter->setPosition(position);
}

void Dungeon::adaptPlayerVelocity()
{
	// If moving diagonally, reduce velocity (to have always same velocity)
	sf::Vector2f velocity = mPlayerCharacter->getVelocity();
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerCharacter->setVelocity(velocity / std::sqrt(2.f));		
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

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
		if (matchesCategories(pair, Category::Character, Category::UnwalkableTile))
		{
			auto& character 		= static_cast<Character&>(*pair.first);
			auto& tile 				= static_cast<Tile&>(*pair.second);
			auto characterBounds 	= character.getBoundingRect();
			auto characterPosition 	= character.getPosition();			
			auto tileBounds 		= tile.getBoundingRect();
			auto tilePosition 		= tile.getPosition();

			// check X axis penetration through left or right
			auto penetrationX		= std::min(std::abs(tileBounds.left + tileBounds.width - characterBounds.left) 
												, std::abs(characterBounds.left + characterBounds.width - tileBounds.left));
			// check X axis penetration through up or down
			auto penetrationY		= std::min(std::abs(tileBounds.top + tileBounds.height - characterBounds.top)
												, std::abs(characterBounds.top + characterBounds.height - tileBounds.top));
			// the least penetrating axis
			auto penetratingAxis 	= std::min(penetrationX, penetrationY);
			auto penetratingX 		= penetratingAxis < penetrationY;

			if (penetratingX)
			{
				// Colliding Left
				if (characterPosition.x > tilePosition.x)
					character.setPosition(characterPosition.x + penetrationX, characterPosition.y);
				// Colliding Right
				else
					character.setPosition(characterPosition.x - penetrationX, characterPosition.y);
			}
			else
			{
				// Colliding Top 
				if (characterPosition.y > tilePosition.y)
					character.setPosition(characterPosition.x, characterPosition.y + penetrationY);
				// Colliding Bottom
				else
					character.setPosition(characterPosition.x, characterPosition.y - penetrationY);
			}			
		}		
	}	
}

void Dungeon::updateSounds()
{
	mSounds.setListenerPosition(mPlayerCharacter->getWorldPosition());

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

	std::unique_ptr<Tilemap> tilemap(new Tilemap(mTextures));
	mSceneLayers[Background]->attachChild(std::move(tilemap));
	//TODO: generate random spawn position (get cell?)
	Tile::ID tileId(5u, 5u);
	mSpawnPosition = sf::Vector2f(tileId.first * Tile::Size, tileId.second * Tile::Size);

	// Add player's character
	std::unique_ptr<Character> player(new Character(Character::Player, mTextures, mFonts));
	mPlayerCharacter = player.get();
	mPlayerCharacter->setPosition(mSpawnPosition);
	mSceneLayers[Main]->attachChild(std::move(player));
}

void Dungeon::addTile(Tile::ID id, Tile::Type type) 
{
	std::unique_ptr<Tile> tilePtr(new Tile(id, type, mTextures));	
	auto tile = tilePtr.get();
	// Tile has centered origin	
	tile->setPosition(id.first * Tile::Size, id.second * Tile::Size);
	mSceneLayers[Main]->attachChild(std::move(tilePtr));
}

sf::FloatRect Dungeon::getViewBounds() const
{
	return sf::FloatRect(mView.getCenter() - mView.getSize() / 2.f, mView.getSize());
}
