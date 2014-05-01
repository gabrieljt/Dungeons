#include <Game/GameState.hpp>
#include <Game/MusicPlayer.hpp>

#include <SFML/Graphics/RenderWindow.hpp>


GameState::GameState(StateStack& stack, Context context)
: State(stack, context)
, mDungeon(*context.window, *context.fonts, *context.sounds)
, mPlayer(*context.player)
{
}

void GameState::draw()
{
	mDungeon.draw();
}

bool GameState::update(sf::Time dt)
{
	mDungeon.update(dt);

	CommandQueue& commands = mDungeon.getCommandQueue();
	mPlayer.handleRealtimeInput(commands);

	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	// Game input handling
	CommandQueue& commands = mDungeon.getCommandQueue();
	mPlayer.handleEvent(event, commands);

	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPop();

	return true;
}
