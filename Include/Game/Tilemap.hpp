#ifndef GAME_TILEMAP_HPP
#define GAME_TILEMAP_HPP

#include <Game/SceneNode.hpp>
#include <Game/Tile.hpp>
#include <Game/ResourceIdentifiers.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <map>
#include <memory>
#include <utility>


class Tilemap : public SceneNode
{
	public:
		typedef std::shared_ptr<Tile> TilePtr;


	public:
									Tilemap(const TextureHolder& textures);

		virtual sf::FloatRect		getBoundingRect() const;

		void						addTile(Tile::ID, Tile::Type type);
		TilePtr 					getTile(Tile::ID id);
		TilePtr 					getTile(sf::Vector2f position);
		void 						getNeighbours(Tile::ID id, std::vector<TilePtr>& neighbours);
		void 						getNeighbours(sf::Vector2f position, std::vector<TilePtr>& neighbours);


	private:
		virtual void				drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void 				updateCurrent(sf::Time dt, CommandQueue& commands);


	private:
		const sf::Texture 			mTileset;
		sf::Vector2u				mSize;		
		sf::FloatRect				mBounds;
		sf::VertexArray				mImage;
		std::map<Tile::ID, TilePtr>	mMap;	
};

#endif // GAME_TILEMAP_HPP