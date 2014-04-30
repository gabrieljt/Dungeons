#ifndef GAME_STATESTACK_HPP
#define GAME_STATESTACK_HPP

#include <Game/State.hpp>
#include <Game/StateIdentifiers.hpp>
#include <Game/ResourceIdentifiers.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <vector>
#include <utility>
#include <functional>
#include <map>


namespace sf
{
	class Event;
	class RenderWindow;
}

class StateStack : private sf::NonCopyable
{
	public:
		enum Action
		{
			Push,
			Pop,
			Clear,
		};


	public:		
		explicit			StateStack(State::Context context);

		template <typename T>
		void				registerState(States::ID stateID);

		void				update(sf::Time dt);
		void				draw();
		void				handleEvent(const sf::Event& event);

		void				pushState(States::ID stateID);
		void				popState();
		void				clearStates();

		bool				isEmpty() const;


	private:
		State::Ptr			createState(States::ID stateID);
		void				applyPendingChanges();


	private:
		struct PendingChange
		{
			explicit			PendingChange(Action action, States::ID stateID = States::None);

			Action				action;
			States::ID			stateID;
		};


	private:
		std::vector<State::Ptr>								mStack;
		std::vector<PendingChange>							mPendingList;

		State::Context										mContext;
		std::map<States::ID, std::function<State::Ptr()>>	mFactories;
};


template <typename T>
void StateStack::registerState(States::ID stateID)
{
	mFactories[stateID] = [this] ()
	{
		return State::Ptr(new T(*this, mContext));
	};
}

#endif // GAME_STATESTACK_HPP
