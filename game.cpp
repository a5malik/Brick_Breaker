#include "game.hpp"
#include "stringhelper.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

#define OFFSET 450
#define MAX_BALL_VEL 500



const float Game::PlayerSpeed = 650.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(640, 480), "SFML Application", sf::Style::Close)
	, mTexture()
	, paddle(sf::Vector2f(PADDLE_LENGTH, 10))
	, ball(BALL_RADIUS)
	, mFont()
	, start(false)
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	wall.push_back(sf::RectangleShape(sf::Vector2f(WALL_WIDTH, HEIGHT)));
	wall.push_back(sf::RectangleShape(sf::Vector2f(WALL_WIDTH, HEIGHT)));
	wall.push_back(sf::RectangleShape(sf::Vector2f(WIDTH, WALL_WIDTH)));
	wall.push_back(sf::RectangleShape(sf::Vector2f(WIDTH, WALL_WIDTH)));

	//wall[0].setPosition(WALL_WIDTH / 2, HEIGHT / 2);
	//wall[1].setPosition(WIDTH - WALL_WIDTH / 2, HEIGHT / 2);
	//wall[2].setPosition(WIDTH / 2, WALL_WIDTH / 2);
	wall[0].setPosition(0, 0);
	wall[1].setPosition(WIDTH - WALL_WIDTH, 0);
	wall[2].setPosition(0,0);
	wall[3].setPosition(0, HEIGHT - WALL_WIDTH);
	wall[3].setFillColor(sf::Color::Black);

	if (!mTexture.loadFromFile("bevel_rectangle.jpg"))
	{
		// Handle loading error
	}
	
	for (int i = 0; i < 5; i++)
	{
		brick br;
		br.body = sf::RectangleShape(sf::Vector2f(BRICK_LENGTH, BRICK_WIDTH));
		br.alive = true;
		br.body.setPosition(15 + i*((BRICK_LENGTH)+BRICK_LENGTH), HEIGHT / 3);
		br.body.setTexture(&mTexture);
		bricks.push_back(br);
	}
	for (int i = 0; i < 4; i++)
	{
		brick br;
		br.body = sf::RectangleShape(sf::Vector2f(BRICK_LENGTH, BRICK_WIDTH));
		br.alive = true;
		br.body.setPosition(15 + BRICK_LENGTH + i*((BRICK_LENGTH)+BRICK_LENGTH), HEIGHT / 3 - 1.5*BRICK_WIDTH);
		br.body.setTexture(&mTexture);
		bricks.push_back(br);
	}
	 
		
		//mPlayer.setTexture(mTexture);
	ball.setPosition(100.f, OFFSET - 5 - 10);
	paddle.setPosition(100.f, OFFSET);
	ball_vel = sf::Vector2f(0, -500);

	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			if(!update(TimePerFrame))
				return;
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			exit(0);
			break;
		}
	}
}

bool Game::update(sf::Time elapsedTime)
{
	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp)
		movement.y -= PlayerSpeed;
	if (mIsMovingDown)
		movement.y += PlayerSpeed;
	if (mIsMovingLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight)
		movement.x += PlayerSpeed;

	paddle.move(movement * elapsedTime.asSeconds());
	if (!start)
	{
		ball.setPosition(paddle.getPosition().x + PADDLE_LENGTH/2, paddle.getPosition().y-5-2*BALL_RADIUS);
		return true;
	}
	ball.move(ball_vel*elapsedTime.asSeconds());
	sf::FloatRect ball_rect = ball.getGlobalBounds();
	if (ball_rect.intersects(wall[0].getGlobalBounds()) ||
		ball_rect.intersects(wall[1].getGlobalBounds()))
		ball_vel = sf::Vector2f(-1 * ball_vel.x, ball_vel.y);
	
	if (ball_rect.intersects(wall[2].getGlobalBounds()) )
		ball_vel = sf::Vector2f(ball_vel.x, -1*ball_vel.y);
	if (ball_rect.intersects(wall[3].getGlobalBounds()))
		return false;
	if (paddle.getGlobalBounds().intersects(ball.getGlobalBounds()))
	{
		ball_vel.y = 2 * movement.y - ball_vel.y;
		ball_vel.x = 2 * movement.x + ball_vel.x;
		if (ball_vel.x > MAX_BALL_VEL)
			ball_vel.x = MAX_BALL_VEL;
		else if (ball_vel.x < -1 * MAX_BALL_VEL)
			ball_vel.x = -1*MAX_BALL_VEL;
		if (ball_vel.y > MAX_BALL_VEL)
			ball_vel.y = MAX_BALL_VEL;
		else if (ball_vel.y < -1 * MAX_BALL_VEL)
			ball_vel.y = -1 * MAX_BALL_VEL;

	}
	for (auto &br : bricks)
	{
		if (br.alive && ball.getGlobalBounds().intersects(br.body.getGlobalBounds()))
		{

			sf::Vector2f ball_pos = ball.getPosition();
			sf::Vector2f brick_pos = br.body.getPosition();
			sf::FloatRect brick_rect = br.body.getGlobalBounds();
			if (brick_rect.contains(ball_pos.x - BALL_RADIUS, ball_pos.y)
				^ brick_rect.contains(ball_pos.x + BALL_RADIUS, ball_pos.y))
				//ball_pos.x - BALL_RADIUS < brick_pos.x + BRICK_LENGTH / 2 || ball_pos.x + BALL_RADIUS > brick_pos.x - BRICK_LENGTH / 2)
			{
				ball_vel.x *= -1;
				br.alive = false;
			}
			if (brick_rect.contains(ball_pos.x, ball_pos.y - BALL_RADIUS)
				^ brick_rect.contains(ball_pos.x, ball_pos.y + BALL_RADIUS))
				//ball_pos.y + BALL_RADIUS > brick_pos.y - BRICK_WIDTH / 2 || ball_pos.y - BALL_RADIUS < brick_pos.y + BRICK_WIDTH / 2)
			{
				ball_vel.y *= -1;
				br.alive = false;
			}
		}
	}
	return true;
}

void Game::render()
{
	mWindow.clear();
	mWindow.draw(wall[3]); mWindow.draw(wall[0]); mWindow.draw(wall[1]); mWindow.draw(wall[2]);
	mWindow.draw(paddle);
	mWindow.draw(ball);
	for (auto &br : bricks)
		if (br.alive)
			mWindow.draw(br.body);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::W)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::S)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::A)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::D)
		mIsMovingRight = isPressed;
	else if (key == sf::Keyboard::Space)
		start = true;
}