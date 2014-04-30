#ifndef GAME_CATEGORY_HPP
#define GAME_CATEGORY_HPP


// Entity/scene node category, used to dispatch commands
namespace Category
{
	enum Type
	{
		None				= 0,
		SceneLayer			= 1 << 0,
		PlayerCharacter		= 1 << 1,
		AlliedCharacter		= 1 << 2,
		EnemyCharacter		= 1 << 3,
		ParticleSystem		= 1 << 4,
		SoundEffect			= 1 << 5,

		Character = PlayerCharacter | AlliedCharacter | EnemyCharacter,
	};
}

#endif // GAME_CATEGORY_HPP
