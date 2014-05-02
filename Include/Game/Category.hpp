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
		WalkableTile		= 1 << 4,
		UnwalkableTile		= 1 << 5,
		ParticleSystem		= 1 << 6,
		SoundEffect			= 1 << 7,

		Character = PlayerCharacter | AlliedCharacter | EnemyCharacter,
		Tile = WalkableTile | UnwalkableTile,
	};
}

#endif // GAME_CATEGORY_HPP
