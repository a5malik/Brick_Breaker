#ifndef BOOK_GAME_HPP
#define BOOK_GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>

using std::vector;

#define WIDTH 640
#define HEIGHT  480
#define WALL_WIDTH 10
#define BRICK_LENGTH WIDTH/10
#define BRICK_WIDTH HEIGHT/10
#define PADDLE_LENGTH 120
#define BALL_RADIUS  6

struct brick
{
	sf::RectangleShape body;
	bool alive;
};

class Game : private sf::NonCopyable
{
public:
	Game();
	void					run();


private:
	void					processEvents();
	bool					update(sf::Time elapsedTime);
	void					render();

	void					updateStatistics(sf::Time elapsedTime);
	void					handlePlayerInput(sf::Keyboard::Key key, bool isPressed);


private:
	static const float		PlayerSpeed;
	static const sf::Time	TimePerFrame;

	sf::RenderWindow		mWindow;
	sf::Texture				mTexture;
	sf::RectangleShape		paddle;
	sf::CircleShape         ball;
	vector<sf::RectangleShape> wall;
	vector<brick> bricks;
	sf::Font				mFont;
	sf::Text				mStatisticsText;
	sf::Time				mStatisticsUpdateTime;
	sf::Vector2f            ball_vel;
	std::size_t				mStatisticsNumFrames;
	bool                    start;
	bool					mIsMovingUp;
	bool					mIsMovingDown;
	bool					mIsMovingRight;
	bool					mIsMovingLeft;
};

#endif // BOOK_GAME_HPP