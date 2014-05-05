#include <Game/Dungeon.hpp>
#include <Game/Foreach.hpp>
#include <Game/Utility.hpp>
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
, mCommandQueue()
, mTilemap()
, mSpawnPosition()
, mPlayerCharacter(nullptr)
, mEnemySpawnPoints()
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
	destroyEntitiesOutsideView();

	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	handleCollisions();

	mSceneGraph.removeWrecks();
	spawnEnemies();

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
	//mTextures.load(Textures::Characters,			"Media/Textures/characters-sf.png");
	mTextures.load(Textures::Characters,			"Media/Textures/characters.png");
	mTextures.load(Textures::SlimeCharacters,		"Media/Textures/slimes.png");
	mTextures.load(Textures::Tiles,					"Media/Textures/tiles.png");
}

void Dungeon::setupView()
{
	auto visibleArea = Tile::Size * 10u; // i x i cells
	auto zoom = visibleArea / std::min(mView.getSize().x, mView.getSize().y);	
	mView.setCenter(mSpawnPosition);	
	mView.zoom(zoom);
}

void Dungeon::adaptViewPosition()
{
    auto borderDistance = sf::Vector2f(getViewBounds().width / 2.f, getViewBounds().height / 2.f);
	auto dungeonBounds = mTilemap->getBoundingRect();
	auto position = mPlayerCharacter->getPosition();
    mView.setCenter(position);
	position.x = std::max(position.x, dungeonBounds.left + borderDistance.x);
	position.x = std::min(position.x, dungeonBounds.left + dungeonBounds.width - borderDistance.x);
	position.y = std::max(position.y, dungeonBounds.top + borderDistance.y);
	position.y = std::min(position.y, dungeonBounds.top + dungeonBounds.height - borderDistance.y);
	mView.setCenter(position);
}

void Dungeon::adaptPlayerPosition()
{
	const auto borderDistance = Tile::Size / 2;
	auto dungeonBounds = mTilemap->getBoundingRect();
	auto position = mPlayerCharacter->getPosition();
	position.x = std::max(position.x, dungeonBounds.left + borderDistance);
	position.x = std::min(position.x, dungeonBounds.left + dungeonBounds.width - borderDistance);
	position.y = std::max(position.y, dungeonBounds.top + borderDistance);
	position.y = std::min(position.y, dungeonBounds.top + dungeonBounds.height - borderDistance);
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

void handleTileCollision(Character& character, Tilemap::TilePtr tile)
{
	auto characterBounds 	= character.getBoundingRect();
	auto characterPosition 	= character.getPosition();			
	auto tileBounds 		= tile->getBoundingRect();
	// Tile does not have centered origin
	auto tilePosition 		= tile->getPosition() + sf::Vector2f(Tile::Size / 2, Tile::Size / 2);
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

void Dungeon::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);	
	FOREACH(SceneNode::Pair pair, collisionPairs)
	{	
		if (matchesCategories(pair, Category::Character, Category::Tilemap))
		{
			auto& character = static_cast<Character&>(*pair.first);
			std::vector<Tilemap::TilePtr> neighbours;
			neighbours.push_back(mTilemap->getTile(character.getPosition()));
			mTilemap->getNeighbours(character.getPosition(), neighbours);
			FOREACH (auto tile, neighbours)
			{
				if (!tile->isWalkable() && tile->getBoundingRect().intersects(character.getBoundingRect()))
				{
					handleTileCollision(character, tile);
				}
			}
		}
		if (matchesCategories(pair, Category::PlayerCharacter, Category::EnemyCharacter))
		{
			auto& character = static_cast<Character&>(*pair.first);
			auto& enemy = static_cast<Character&>(*pair.second);

			character.damage(enemy.getHitpoints());
			enemy.destroy();
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
	// Build Map
	std::unique_ptr<Tilemap> tilemap(new Tilemap(mTextures));
	mTilemap = tilemap.get();
	mTilemap->setPosition(0.f, 0.f);
	mSceneLayers[Background]->attachChild(std::move(tilemap));

	// Add player's character
	std::unique_ptr<Character> player(new Character(Character::Player, mTextures, mFonts));
	mPlayerCharacter = player.get();
	mSpawnPosition = mTilemap->getRandomRoomCenter();
	mPlayerCharacter->setPosition(mSpawnPosition);
	mSceneLayers[Main]->attachChild(std::move(player));

	addEnemies();
}

void Dungeon::addEnemy(Character::Type type, float x, float y)
{
	CharacterSpawnPoint spawn(type, x, y);
	mEnemySpawnPoints.push_back(spawn);
}

void Dungeon::addEnemies()
{
	auto roomRandomFactor = 2;
	// TODO: max number of enemies to spawn, better room distribution
	std::vector<Tilemap::TilePtr> roomTiles;
	mTilemap->getRooms(roomTiles);
	for (auto i = 0u; i < roomTiles.size(); i += 1 + randomInt(roomRandomFactor))
	{
		// chance % of spawning enemy; TODO: create function!
		auto chance = 0.05f;
		if ((randomInt(101)) / 100.f >= 1.f - chance)
			addEnemy(Character::Slime, roomTiles[i]->getBoundingRect().left, roomTiles[i]->getBoundingRect().top);			
	}

	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [] (CharacterSpawnPoint lhs, CharacterSpawnPoint rhs)
	{
		return lhs.x < rhs.x && lhs.y < rhs.y;
	});
}

void Dungeon::spawnEnemies()
{
	// TODO: use list instead of vector
	// Spawn all enemies entering the battlefireld area this frame
	std::vector<std::vector<CharacterSpawnPoint>::iterator> spawnedPoints;
	for (auto itr = mEnemySpawnPoints.begin(); itr != mEnemySpawnPoints.end(); ++itr)
	{
		CharacterSpawnPoint spawn = *itr;
		if (getBattlefieldBounds().contains(spawn.x, spawn.y))
		{
			std::unique_ptr<Character> enemy(new Character(spawn.type, mTextures, mFonts));
			enemy->setPosition(spawn.x, spawn.y);
			mSceneLayers[Main]->attachChild(std::move(enemy));

			spawnedPoints.push_back(itr);
		}	
	}
	while (!spawnedPoints.empty())
	{
		mEnemySpawnPoints.erase(spawnedPoints.back());
		spawnedPoints.pop_back();
	}
}

void Dungeon::destroyEntitiesOutsideView()
{
	Command enemyCollector;
	enemyCollector.category = Category::EnemyCharacter;
	enemyCollector.action = derivedAction<Character>([this] (Character& enemy, sf::Time)
	{
		// TODO: save enemy current attributes
		if (!enemy.isDestroyed() && !getBattlefieldBounds().intersects(enemy.getBoundingRect()))
			addEnemy(enemy.getType(), enemy.getPosition().x, enemy.getPosition().y);
	});

	Command command;
	command.category = Category::Character;
	command.action = derivedAction<Entity>([this] (Entity& e, sf::Time)
	{
		if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
		{
			e.destroy();			
		}		
	});
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(command);
}

sf::FloatRect Dungeon::getViewBounds() const
{
	return sf::FloatRect(mView.getCenter() - mView.getSize() / 2.f, mView.getSize());
}

sf::FloatRect Dungeon::getBattlefieldBounds() const
{
	const auto borderDistance 	=  Tile::Size / 2;
	// Return view bounds + some area, where enemies spawn
    auto bounds 				=  getViewBounds();
    bounds.left 				-= 10.f * borderDistance;
    bounds.top 					-= 10.f * borderDistance;
    bounds.width 				+= 20.f * borderDistance;
    bounds.height 				+= 20.f * borderDistance;
    
    return bounds;
}
