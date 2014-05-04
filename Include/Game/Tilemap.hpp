#ifndef GAME_TILEMAP_HPP
#define GAME_TILEMAP_HPP

#include <Game/SceneNode.hpp>
#include <Game/Tile.hpp>
#include <Game/ResourceIdentifiers.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <map>
#include <vector>
#include <memory>
#include <utility>


class Tilemap : public SceneNode
{
	public:
		typedef std::shared_ptr<Tile> 			TilePtr;
		typedef std::unique_ptr<sf::IntRect> 	BoundsPtr;


	public:
										Tilemap(const TextureHolder& textures);

		virtual sf::FloatRect			getBoundingRect() const;

		void							addTile(Tile::ID, Tile::Type type);
		TilePtr 						getTile(Tile::ID id);
		TilePtr 						getTile(sf::Vector2f position);
		void 							getNeighbours(Tile::ID id, std::vector<TilePtr>& neighbours);
		void 							getNeighbours(sf::Vector2f position, std::vector<TilePtr>& neighbours);

		unsigned int 					getNumberRooms();
		sf::Vector2i 					getRoomCenter(unsigned int index);


	private:
		virtual void					drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void 					updateCurrent(sf::Time dt, CommandQueue& commands);

		bool 							validateTile(Tile::ID id);	

		void							createRoom(sf::IntRect bounds);
		void 							createTunnelH(int x1, int x2, int y);
		void 							createTunnelV(int y1, int y2, int x);
		void							generateMap();


	private:
		const sf::Texture 				mTileset;
		sf::Vector2u					mSize;		
		sf::FloatRect					mBounds;
		sf::VertexArray					mImage;
		std::map<Tile::ID, TilePtr>		mMap;	
		std::vector<BoundsPtr> 			mRooms;
};

#endif // GAME_TILEMAP_HPP