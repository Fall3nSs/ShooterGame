#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>

double pi = 2 * acos(0.0);

int main() 
{
	srand(time(0));

	sf::ContextSettings settings;
	settings.antiAliasingLevel = 16;
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktop, "Game Window", sf::Style::Default, sf::State::Fullscreen);
	window.setVerticalSyncEnabled(true);

	sf::Texture texture("Sprites/AK-47.png");
	sf::Texture bulletT("Sprites/bullet.png");
	texture.setSmooth(true);
	sf::Sprite Player(texture);
	Player.setPosition({ 250,170 });
	Player.setScale({ .2f,.2f });
	Player.setOrigin({Player.getTexture().getSize().x/2.f,Player.getTexture().getSize().y/2.f});
	int playerSpeed = 300;

	sf::SoundBuffer bulletBuffer("SFX/shoot.wav");
	sf::Sound bulletSound(bulletBuffer);
	bool firstBullet = true;
	std::vector<sf::RectangleShape> bullets;
	std::vector<sf::Vector2f> bulletVelocities;
	sf::Clock bulletClock;
	std::vector<sf::FloatRect> bb_bullets;

	sf::Texture txtr_enemy("Sprites/enemy.png");
	txtr_enemy.setSmooth(true);
	std::vector<sf::Sprite> enemies;
	std::vector<sf::FloatRect> bb_enemies;
	sf::Clock clock_enemy;
	bool firstEnemy = true;
	float enemySpeed = 320;

	sf::SoundBuffer buffer_collision("SFX/collision.wav");
	sf::Sound sound_collision(buffer_collision);

	sf::Music music_bg("SFX/music_bg.ogg");
	music_bg.setLooping(true);
	music_bg.setVolume(50.f);
	music_bg.play();

	sf::Clock deltaClock;

	sf::Font font("Fonts/Comic Sans MS.ttf");
	sf::Text score(font);
	int scoreText = 0;
	score.setCharacterSize(48);
	score.setFillColor(sf::Color::White);
	score.setStyle(sf::Text::Bold);
	score.setPosition({ window.getSize().x / 2.f,10 });
	score.setString(std::to_string(scoreText));

	//GAME LOOP
	while (window.isOpen() == true) {
		sf::Time dt = deltaClock.restart();
		while (const std::optional event = window.pollEvent())
		{
			if (event->is < sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				window.close();
			}
		}

		//Rotation
		int dx = sf::Mouse::getPosition(window).x - Player.getPosition().x;
		int dy = sf::Mouse::getPosition(window).y - Player.getPosition().y;
		float degree = atan2(dy, dx) * 180.f / pi;
		Player.setRotation(sf::degrees(degree));

		//Movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		{
			Player.move({ -playerSpeed * dt.asSeconds(),0 });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		{
			Player.move({ playerSpeed * dt.asSeconds(),0 });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		{
			Player.move({ 0,-playerSpeed * dt.asSeconds() });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			Player.move({ 0,playerSpeed * dt.asSeconds() });
		}

		//Shoot.1
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			if (firstBullet == true || bulletClock.getElapsedTime().asSeconds() >= .15f)
			{
				sf::Vector2f gunTip(
					Player.getPosition().x + cos(degree * pi / 180) * 80,
					Player.getPosition().y + sin(degree * pi / 180) * 70
				);
				firstBullet = false;
				float angle = Player.getRotation().asRadians();
				int speed_bullet = 600;
				sf::Vector2f bulletVel = { speed_bullet * cos(angle),speed_bullet * sin(angle) };
				sf::RectangleShape bullet({ 30,15 });
				bullet.setOrigin({ bullet.getSize().x / 2.f,bullet.getSize().y / 2.f });
				bullet.setTexture(&bulletT, false);
				bullet.setPosition(gunTip);
				bullet.rotate(sf::degrees(degree));
				sf::FloatRect bb_bullet = bullet.getGlobalBounds();
				bb_bullets.push_back(bb_bullet);
				bulletVelocities.push_back(bulletVel);
				bullets.push_back(bullet);
				bulletSound.play();
				bulletClock.restart();
			}

		}
		//Enemy Spawn
		if (clock_enemy.getElapsedTime().asSeconds() >= 2)
		{
			firstEnemy = false;
			float randX = rand() % window.getSize().x;
			float randY = rand() % window.getSize().y;
			sf::Vector2f randCord = { randX,randY };
			sf::Sprite enemy(txtr_enemy);
			sf::FloatRect bb_enemy = enemy.getGlobalBounds();
			enemy.setPosition(randCord);
			enemy.setScale({ .2f,.2f });
			enemies.push_back(enemy);
			bb_enemies.push_back(bb_enemy);
			clock_enemy.restart();
		}

		

		//window.handleEvents(onFocusLost, onFocusGain);
		window.clear(sf::Color::Black);
		window.draw(Player);
		//Shoot.2
		for (int i = bullets.size()-1; i >= 0;i--)
		{
			window.draw(bullets[i]);
			bullets[i].move(bulletVelocities[i] * dt.asSeconds());
			bb_bullets[i] = bullets[i].getGlobalBounds();
			if (bullets[i].getPosition().x < 0 || bullets[i].getPosition().x > window.getSize().x || bullets[i].getPosition().y < 0 || bullets[i].getPosition().y > window.getSize().y)
			{
				bullets.erase(bullets.begin() + i);
				bulletVelocities.erase(bulletVelocities.begin() + i);
				bb_bullets.erase(bb_bullets.begin() + i);
			}

		}
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			//enemy rotate
			enemies[i].setOrigin({ enemies[i].getTexture().getSize().x / 2.f,enemies[i].getTexture().getSize().y / 2.f});
			int dxEnemy = Player.getPosition().x - enemies[i].getPosition().x;
			int dyEnemy = Player.getPosition().y - enemies[i].getPosition().y;
			float degree = atan2(dyEnemy, dxEnemy) * 180.f / pi;//Radyan olarak hesaplandığı için kalan işlemi yapıyoruz
			enemies[i].setRotation(sf::degrees(degree + 90));

			//enemy move
			sf::Vector2f enemyVel = { enemySpeed * cos(degree + 90),enemySpeed * sin(degree + 90) };
			enemies[i].move(enemyVel * dt.asSeconds());

			window.draw(enemies[i]);
			bb_enemies[i] = enemies[i].getGlobalBounds();

		}

		//collision
		for (int i = bb_bullets.size() - 1; i >= 0; i--)
		{
			for (int j = bb_enemies.size() - 1; j >= 0; j--)
			{
				if (const std::optional intersection = bb_bullets[i].findIntersection(bb_enemies[j]))
				{
					bullets.erase(bullets.begin() + i);
					bulletVelocities.erase(bulletVelocities.begin() + i);
					bb_bullets.erase(bb_bullets.begin() + i);
					enemies.erase(enemies.begin() + j);
					bb_enemies.erase(bb_enemies.begin() + j);
					sound_collision.play();
					scoreText++;
					score.setString(std::to_string(scoreText));
					break;
				}
			}
		}

		window.draw(score);
		window.display();

	}

}
