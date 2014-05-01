#include <Game/Dungeon.hpp>
#include <Game/Foreach.hpp>
#include <Game/TextNode.hpp>
#include <Game/ParticleNode.hpp>
#include <Game/SoundNode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>


Dungeon::Dungeon(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds)
: mTarget(outputTarget)
, mSceneTexture()
, mView(outputTarget.getDefaultView())
, mTextures() 
, mFonts(fonts)
, mSounds(sounds)
, mSceneGraph()
, mSceneLayers()
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
	const auto borderDistance = Tile::Size / 2u - 1u;

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
		if (matchesCategories(pair, Category::Character, Category::WallTile))
		{
			auto& player = static_cast<Character&>(*pair.first);
			auto& wall = static_cast<Tile&>(*pair.second);
			// TODO: collision!
			player.damage(wall.getHitpoints());
			wall.destroy();
		}		
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

	// TODO: generate "random" dungeon (pixels) and map (tiles) bounds
	const auto tilemapSize = 10u; // 10x10 cells
	mDungeonBounds = sf::FloatRect(0.f, 0.f, Tile::Size * tilemapSize, Tile::Size * tilemapSize); //1600x1600 pixels
	
	// TODO: generate LEVEL!!!!
	for (auto x = 1u; x < tilemapSize - 1u; ++x)
		for (auto y = 1u; y < tilemapSize - 1u; ++y)
		{
				Tile::TileID id(x, y);
				addTile(id, Tile::Type::Floor);
		}

	for (auto i = 0u; i < tilemapSize; ++i)
	{
		Tile::TileID firstRow(i, 0);
		Tile::TileID lastRow(i, tilemapSize - 1);
		Tile::TileID firstColumn(0, i);
		Tile::TileID lastColumn(tilemapSize - 1, i);
		addTile(firstRow, Tile::Type::Wall);
		addTile(lastRow, Tile::Type::Wall);
		addTile(firstColumn, Tile::Type::Wall);
		addTile(lastColumn, Tile::Type::Wall);
	}

	//TODO: generate random spawn position (get cell?)
	Tile::TileID tileId(5u, 5u);
	mSpawnPosition = sf::Vector2f(tileId.first * Tile::Size, tileId.second * Tile::Size);

	// Add player's character
	std::unique_ptr<Character> player(new Character(Character::Player, mTextures, mFonts));
	mPlayerCharacter = player.get();
	mPlayerCharacter->setPosition(mSpawnPosition);
	mSceneLayers[Main]->attachChild(std::move(player));
}

void Dungeon::addTile(Tile::TileID id, Tile::Type type) 
{
	std::unique_ptr<Tile> tilePtr(new Tile(type, mTextures, mFonts, id));	
	auto tile = tilePtr.get();
	tile->setPosition(id.first * Tile::Size, id.second * Tile::Size);
	mSceneLayers[Main]->attachChild(std::move(tilePtr));
}

sf::FloatRect Dungeon::getViewBounds() const
{
	return sf::FloatRect(mView.getCenter() - mView.getSize() / 2.f, mView.getSize());
}
