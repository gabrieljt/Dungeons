#include <Game/Utility.hpp>
#include <Game/Animation.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <random>
#include <cmath>
#include <ctime>
#include <cassert>


const bool Utility::Debug = true;

namespace
{
	std::default_random_engine createRandomEngine()
	{
		auto seed = static_cast<unsigned long>(std::time(nullptr));
		return std::default_random_engine(seed);
	}

	auto RandomEngine = createRandomEngine();
}

std::string toString(sf::Keyboard::Key key)
{
	#define GAME_KEYTOSTRING_CASE(KEY) case sf::Keyboard::KEY: return #KEY;

	switch (key)
	{
		GAME_KEYTOSTRING_CASE(Unknown)
		GAME_KEYTOSTRING_CASE(A)
		GAME_KEYTOSTRING_CASE(B)
		GAME_KEYTOSTRING_CASE(C)
		GAME_KEYTOSTRING_CASE(D)
		GAME_KEYTOSTRING_CASE(E)
		GAME_KEYTOSTRING_CASE(F)
		GAME_KEYTOSTRING_CASE(G)
		GAME_KEYTOSTRING_CASE(H)
		GAME_KEYTOSTRING_CASE(I)
		GAME_KEYTOSTRING_CASE(J)
		GAME_KEYTOSTRING_CASE(K)
		GAME_KEYTOSTRING_CASE(L)
		GAME_KEYTOSTRING_CASE(M)
		GAME_KEYTOSTRING_CASE(N)
		GAME_KEYTOSTRING_CASE(O)
		GAME_KEYTOSTRING_CASE(P)
		GAME_KEYTOSTRING_CASE(Q)
		GAME_KEYTOSTRING_CASE(R)
		GAME_KEYTOSTRING_CASE(S)
		GAME_KEYTOSTRING_CASE(T)
		GAME_KEYTOSTRING_CASE(U)
		GAME_KEYTOSTRING_CASE(V)
		GAME_KEYTOSTRING_CASE(W)
		GAME_KEYTOSTRING_CASE(X)
		GAME_KEYTOSTRING_CASE(Y)
		GAME_KEYTOSTRING_CASE(Z)
		GAME_KEYTOSTRING_CASE(Num0)
		GAME_KEYTOSTRING_CASE(Num1)
		GAME_KEYTOSTRING_CASE(Num2)
		GAME_KEYTOSTRING_CASE(Num3)
		GAME_KEYTOSTRING_CASE(Num4)
		GAME_KEYTOSTRING_CASE(Num5)
		GAME_KEYTOSTRING_CASE(Num6)
		GAME_KEYTOSTRING_CASE(Num7)
		GAME_KEYTOSTRING_CASE(Num8)
		GAME_KEYTOSTRING_CASE(Num9)
		GAME_KEYTOSTRING_CASE(Escape)
		GAME_KEYTOSTRING_CASE(LControl)
		GAME_KEYTOSTRING_CASE(LShift)
		GAME_KEYTOSTRING_CASE(LAlt)
		GAME_KEYTOSTRING_CASE(LSystem)
		GAME_KEYTOSTRING_CASE(RControl)
		GAME_KEYTOSTRING_CASE(RShift)
		GAME_KEYTOSTRING_CASE(RAlt)
		GAME_KEYTOSTRING_CASE(RSystem)
		GAME_KEYTOSTRING_CASE(Menu)
		GAME_KEYTOSTRING_CASE(LBracket)
		GAME_KEYTOSTRING_CASE(RBracket)
		GAME_KEYTOSTRING_CASE(SemiColon)
		GAME_KEYTOSTRING_CASE(Comma)
		GAME_KEYTOSTRING_CASE(Period)
		GAME_KEYTOSTRING_CASE(Quote)
		GAME_KEYTOSTRING_CASE(Slash)
		GAME_KEYTOSTRING_CASE(BackSlash)
		GAME_KEYTOSTRING_CASE(Tilde)
		GAME_KEYTOSTRING_CASE(Equal)
		GAME_KEYTOSTRING_CASE(Dash)
		GAME_KEYTOSTRING_CASE(Space)
		GAME_KEYTOSTRING_CASE(Return)
		GAME_KEYTOSTRING_CASE(BackSpace)
		GAME_KEYTOSTRING_CASE(Tab)
		GAME_KEYTOSTRING_CASE(PageUp)
		GAME_KEYTOSTRING_CASE(PageDown)
		GAME_KEYTOSTRING_CASE(End)
		GAME_KEYTOSTRING_CASE(Home)
		GAME_KEYTOSTRING_CASE(Insert)
		GAME_KEYTOSTRING_CASE(Delete)
		GAME_KEYTOSTRING_CASE(Add)
		GAME_KEYTOSTRING_CASE(Subtract)
		GAME_KEYTOSTRING_CASE(Multiply)
		GAME_KEYTOSTRING_CASE(Divide)
		GAME_KEYTOSTRING_CASE(Left)
		GAME_KEYTOSTRING_CASE(Right)
		GAME_KEYTOSTRING_CASE(Up)
		GAME_KEYTOSTRING_CASE(Down)
		GAME_KEYTOSTRING_CASE(Numpad0)
		GAME_KEYTOSTRING_CASE(Numpad1)
		GAME_KEYTOSTRING_CASE(Numpad2)
		GAME_KEYTOSTRING_CASE(Numpad3)
		GAME_KEYTOSTRING_CASE(Numpad4)
		GAME_KEYTOSTRING_CASE(Numpad5)
		GAME_KEYTOSTRING_CASE(Numpad6)
		GAME_KEYTOSTRING_CASE(Numpad7)
		GAME_KEYTOSTRING_CASE(Numpad8)
		GAME_KEYTOSTRING_CASE(Numpad9)
		GAME_KEYTOSTRING_CASE(F1)
		GAME_KEYTOSTRING_CASE(F2)
		GAME_KEYTOSTRING_CASE(F3)
		GAME_KEYTOSTRING_CASE(F4)
		GAME_KEYTOSTRING_CASE(F5)
		GAME_KEYTOSTRING_CASE(F6)
		GAME_KEYTOSTRING_CASE(F7)
		GAME_KEYTOSTRING_CASE(F8)
		GAME_KEYTOSTRING_CASE(F9)
		GAME_KEYTOSTRING_CASE(F10)
		GAME_KEYTOSTRING_CASE(F11)
		GAME_KEYTOSTRING_CASE(F12)
		GAME_KEYTOSTRING_CASE(F13)
		GAME_KEYTOSTRING_CASE(F14)
		GAME_KEYTOSTRING_CASE(F15)
		GAME_KEYTOSTRING_CASE(Pause)
	}

	return "";
}

void centerOrigin(sf::Sprite& sprite)
{
	sf::FloatRect bounds = sprite.getLocalBounds();
	sprite.setOrigin(std::floor(bounds.width / 2.f), std::floor(bounds.height / 2.f));
}

void centerOrigin(sf::Text& text)
{
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(std::floor(bounds.width / 2.f), std::floor(bounds.height / 2.f));
}

void centerOrigin(Animation& animation)
{
	sf::FloatRect bounds = animation.getLocalBounds();
	animation.setOrigin(std::floor(bounds.width / 2.f), std::floor(bounds.height / 2.f));
}

sf::Vector2i getCenter(sf::IntRect bounds)
{
	return sf::Vector2i(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
}

float toDegree(float radian)
{
	return 180.f / 3.141592653589793238462643383f * radian;
}

float toRadian(float degree)
{
	return 3.141592653589793238462643383f / 180.f * degree;
}

int randomInt(int exclusiveMax)
{
	std::uniform_int_distribution<> distr(0, exclusiveMax - 1);
	return distr(RandomEngine);
}

float length(sf::Vector2f vector)
{
	return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f unitVector(sf::Vector2f vector)
{
	assert(vector != sf::Vector2f(0.f, 0.f));
	return vector / length(vector);
}