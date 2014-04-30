#ifndef GAME_EMITTERNODE_HPP
#define GAME_EMITTERNODE_HPP

#include <Game/SceneNode.hpp>
#include <Game/Particle.hpp>


class ParticleNode;

class EmitterNode : public SceneNode
{
	public:
		explicit				EmitterNode(Particle::Type type);


	private:
		virtual void			updateCurrent(sf::Time dt, CommandQueue& commands);
		
		void					emitParticles(sf::Time dt);


	private:
		sf::Time				mAccumulatedTime;
		Particle::Type			mType;
		ParticleNode*			mParticleSystem;
};

#endif // GAME_EMITTERNODE_HPP
