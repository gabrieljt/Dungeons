#include <Game/Character.hpp>
#include <Game/DataTables.hpp>
#include <Game/Utility.hpp>
#include <Game/CommandQueue.hpp>
#include <Game/SoundNode.hpp>
#include <Game/ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>


using namespace std::placeholders;

namespace
{
	const std::vector<CharacterData> Table = initializeCharacterData();
}

Character::Character(Type type, const TextureHolder& textures, const FontHolder& fonts)
: Entity(Table[type].hitpoints)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mTravelledDistance(0.f)
, mDirectionIndex(0)
, mHealthDisplay(nullptr)
{
	centerOrigin(mSprite);

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	updateTexts();
}

void Character::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

void Character::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	updateTexts();
	updateIdleAnimation();

	if (isDestroyed())
	{
		return;
	}

	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
}

unsigned int Character::getCategory() const
{
	if (isAllied())
		return Category::PlayerCharacter;
	else
		return Category::EnemyCharacter;
}

sf::FloatRect Character::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Character::isMarkedForRemoval() const
{
	return isDestroyed();
}

void Character::remove()
{
	Entity::remove();
}

bool Character::isAllied() const
{
	return mType == Player;
}

float Character::getMaxSpeed() const
{
	return Table[mType].speed;
}

void Character::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();
	
	Command command;
	command.category = Category::SoundEffect;
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition] (SoundNode& node, sf::Time)
		{
			node.playSound(effect, worldPosition);
		});

	commands.push(command);
}

void Character::updateMovementPattern(sf::Time dt)
{
	const std::vector<Direction>& directions = Table[mType].directions;
	if (!directions.empty())
	{
		// Moved long enough in current direction: Change direction
		if (mTravelledDistance > directions[mDirectionIndex].distance)
		{
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}

		// Compute velocity from direction
		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);

		setVelocity(vx, vy);

		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

void Character::updateTexts()
{
	if (isDestroyed())
		mHealthDisplay->setString("");
	else
		mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());
}

void Character::updateIdleAnimation()
{
	// check sprite sheet
	if (Table[mType].hasIdleAnimation)
	{
		sf::IntRect textureRect = Table[mType].textureRect;

		// Idle left: Texture rect offset once
		if (getVelocity().x < 0.f)
			textureRect.left += textureRect.width;

		// Idle right: Texture rect offset twice
		else if (getVelocity().x > 0.f)
			textureRect.left += 2 * textureRect.width;

		mSprite.setTextureRect(textureRect);
	}
}
