#ifndef GAME_COMMANDQUEUE_HPP
#define GAME_COMMANDQUEUE_HPP

#include <Game/Command.hpp>

#include <queue>


class CommandQueue
{
	public:
		void						push(const Command& command);
		Command						pop();
		bool						isEmpty() const;

		
	private:
		std::queue<Command>			mQueue;
};

#endif // GAME_COMMANDQUEUE_HPP
