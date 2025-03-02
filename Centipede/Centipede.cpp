/*Insharah Irfan Nazir
23i-0615
CS-D*/
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <time.h>
#include <filesystem>

using namespace std;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. To resize the window, change the function call in the window section.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; //30 rows
const int gameColumns = resolutionY / boxPixelsY; //30 columns

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;

void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite, sf::Sprite& deathSprite, sf::Clock& switchClock, int& deathState, int switchTime, sf::Sound& deathSound);
void moveBullet(float bullet[], sf::Clock& bulletClock);
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite);
void movePlayer(float player[], float speed);
void handleBullet(float player[], float bullet[], float mushrooms[][3], int mushroomsCount, float speed, int& score, sf::Clock& bulletClock, sf::Sound& fireSound, float poisonedMushroom[][3]);
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite, float mushrooms[][3], int mushroomsCount);
void drawCentipede(sf::RenderWindow& window, float centipede[][4], int centipedeLength, sf::Sprite& centipedeHeadSprite, sf::Sprite& centipedeBodySprite);
void moveCentipede(float centipede[][4], int centipedeLength, float centipedeSpeed);
void mushroomCentipedeCollision(float centipede[][4], int centipedeLength, float mushrooms[][3], int mushroomsCount, float centipedeSpeed);
void drawPoisonedMushroom(sf::RenderWindow& window, sf::Sprite& poisonedMushroomSprite, int& hits, float poisonedMushroom[][3]);
void shootCentipede(float bullet[], float centipede[][4], int centipedeLength, int& score, int& hits, sf::RenderWindow& window, sf::Sprite& poisonedMushroomSprite, float poisonedMushroom[][3]);
void poisonMushroomPlayerCollision(float player[], float poisonedMushroom[][3], int& score);
void centipedePlayerCollision(float player[], float centipede[][4], int centipedeLength, int& score);
void splitCentipede(float centipede[][4], int centipedeLength, int index);

int main()
{

	srand(time(0)); //seed the random function

	///////////////////////////////////////////////////////////////////////
	//								WINDOW						         //
	///////////////////////////////////////////////////////////////////////

	// Declaring RenderWindow.
	sf::RenderWindow window(sf::VideoMode(resolutionX, resolutionY), "Centipede", sf::Style::Close | sf::Style::Titlebar);

	// Used to resize your window if it's too big or too small. Use according to your needs.
	int windowSize = 800; //640 is recommended for 1366x768 (768p) displays.
	window.setSize(sf::Vector2u(windowSize, windowSize));

	// Used to position your window on every launch. Use according to your needs.
	window.setPosition(sf::Vector2i(100, 0));

	///////////////////////////////////////////////////////////////////////
	//								SOUNDS						         //
	///////////////////////////////////////////////////////////////////////
	
	//initialize the background music
	sf::Music bgMusic;
	bgMusic.openFromFile("GameElements\\field_of_hopes.ogg");
	bgMusic.play();
	bgMusic.setVolume(50);
	bgMusic.setLoop(true); //loop when finished

	//load the sound buffer for the bullet
	sf::SoundBuffer fireBuffer;
	fireBuffer.loadFromFile("GameElements\\fire1.wav");
	sf::Sound fireSound;
	fireSound.setBuffer(fireBuffer);
	fireSound.setVolume(25); 

	//initialize the death sound 
	sf::SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("GameElements\\death.wav");
	sf::Sound deathSound;
	deathSound.setBuffer(deathBuffer);
	deathSound.setVolume(25);

	///////////////////////////////////////////////////////////////////////
	//								BACKGROUND						     //
	///////////////////////////////////////////////////////////////////////
	// Initializing Background.
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	//backgroundTexture.loadFromFile("background.png");
	backgroundTexture.loadFromFile("GameElements\\spacebackground.png");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.20)); // Reduces Opacity to 25%

	///////////////////////////////////////////////////////////////////////
	//								SPRITES						         //
	///////////////////////////////////////////////////////////////////////
	
	//initalize player and player sprites
	float player[3] = {};
	player[x] = (gameColumns / 2) * boxPixelsX; //center
	player[y] = resolutionY - boxPixelsY; //bottom
	player[exists] = 1;
	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	playerTexture.loadFromFile("GameElements\\player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	
	// initialize death sprite 
	float death[3] = {};
	death[x] = player[x];
	death[y] = player[y];
	death[exists] = 1;
	sf::Texture deathTexture;
	sf::Sprite deathSprite;
	deathTexture.loadFromFile("GameElements\\death.png");
	deathSprite.setTexture(deathTexture);
	deathSprite.setColor(sf::Color::Red); // red and black to match the centipede
	deathSprite.setTextureRect(sf::IntRect(0, 32, 2*boxPixelsX, boxPixelsY));
	// for the death sprite, we need to cycle through the png four times so initialize a clock for that
	int switchTime = 1000; // milliseconds to switch are 1000 aka one second
	sf::Clock switchClock; 
	int deathState = 0; // 0: initial state (player), 1-3: switch, 4: vanish and reset
	
	//initializing bullet and bullet sprites
	float bullet[3] = {};
	bullet[x] = player[x];
	bullet[y] = player[y] - boxPixelsY;
	bullet[exists] = true;
	sf::Clock bulletClock;
	sf::Texture bulletTexture;
	sf::Sprite bulletSprite;
	bulletTexture.loadFromFile("GameElements\\bullet.png");
	bulletSprite.setTexture(bulletTexture);
	bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	//initializing mushroom and mushroom sprites
	const int maxMushrooms = 50;
	int mushroomsCount = rand() % 11 + 20; // between 20-30
	float mushrooms[maxMushrooms][3]; // array for mushroom position with x and y coordinates + shots taken
	sf::Texture mushroomTexture;
	sf::Sprite mushroomSprite;
	mushroomTexture.loadFromFile("GameElements\\purplemushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY)); 
	for (int i = 0; i < mushroomsCount; i++)
	{
	    do
	    {
		    mushrooms[i][0] = rand() % resolutionX; // x coordinate of mushroom
		    //y coordinate can be anywhere but first row and player area as seen on the original game
		    do
		    {
				int minY = boxPixelsY; //minimum y coordinate excluding the first row
				int maxY = resolutionY - 6*boxPixelsY; // maximum y coordinate excluding the last 5 rows aka player area
				mushrooms[i][1] = minY + rand() % (maxY - minY + 1); // random y coordinate within the specified range aka 32 - 800
		    }
		    while (mushrooms[i][1] < 0); //so that there are if there is a negative, the count stays within the range of 20-30
	    }
	    while (gameGrid[int(mushrooms[i][0] / boxPixelsX)][int(mushrooms[i][1] / boxPixelsY)] != 0); // randomly select a position in the game grid until an empty one is filled
	    mushrooms[i][2] = 2; // shots taken by mushroom are initially 2 so we can decrement it to 0
	}
	//draw mushrooms based on their shots taken
	for (int i = 0; i < mushroomsCount; i++)
	{
	    // shot once = different mushroom
	    if (mushrooms[i][2] == 2)	        
            mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	    else if (mushrooms[i][2] == 1)	        
	    mushroomSprite.setTextureRect(sf::IntRect(32, 0, boxPixelsX, boxPixelsY));
			
	    mushroomSprite.setPosition(mushrooms[i][0], mushrooms[i][1]);
	    window.draw(mushroomSprite);
	}

	//initialize the poisoned mushroom and its sprite
	float poisonedMushroom[1][3] = {}; // array for mushroom position with x and y coordinates + shots taken
	for (int i = 0; i < 1; i++)
	{
		poisonedMushroom[i][0] = rand() % resolutionX; // any x coordinate
		// y coordinate should be within player area i.e. bottom 5 rows thus the y coordinate can be from row 5 (800) to the last row(928)
		poisonedMushroom[i][1] = rand() % ((resolutionY - boxPixelsY - (resolutionY - 5 * boxPixelsY)) + 1) + (resolutionY - 5 * boxPixelsY); //rand % 129 + 800 means from 800 to 928
		poisonedMushroom[i][2] = 2; //shots taken set to 2 so we can decrement it
	}
	sf::Texture poisonedMushroomTexture;
	sf::Sprite poisonedMushroomSprite;
	poisonedMushroomTexture.loadFromFile("GameElements\\purplemushroom.png");
	poisonedMushroomSprite.setTexture(poisonedMushroomTexture);
	poisonedMushroomSprite.setTextureRect(sf::IntRect(0, 32, boxPixelsX, boxPixelsY));

	// initializing centipede and centipede sprites

	const int maxCentipedeLength = 20;
	int centipedeLength = 12;
    float centipede[maxCentipedeLength][4]; // 11 segements array for centipede segments with x and y coordinates + existence + direction
    
	//int centipedeX = rand() % resolutionX; // random x coordinate
	int centipedeX = resolutionX; //start from the top right
	int centipedeY = 0;
	for (int i = centipedeLength - 1; i >= 0; i--) //initialize to - 1 so that the centipede starts from the very edge of the screen instead of leaving one segment blank
	{
		centipede[i][0] = centipedeX;
		centipede[i][1] = centipedeY * boxPixelsY;
		centipede[i][2] = true; // existence initiallized to true
		centipede[i][3] = 1; // assuming the last dimension specifies when the centipede is moving left, we initialize that to true, assuming left = 1 and right = 0
		centipedeX -= boxPixelsX; 	// move an equal distance to the left each time for a new segment of the body
	}
	float centipedeSpeed = 2*1.0 / static_cast<float>(boxPixelsX); // speed of the centipede = 2xplayer's speed
	
	sf::Texture centipedeHeadTexture;
	sf::Texture centipedeBodyTexture;
	sf::Sprite centipedeHeadSprite;
	sf::Sprite centipedeBodySprite;
	centipedeBodyTexture.loadFromFile("GameElements\\c_body_left_walk.png");
    centipedeHeadTexture.loadFromFile("GameElements\\c_head_left_walk.png");
	centipedeBodySprite.setTexture(centipedeBodyTexture);
	centipedeBodySprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	centipedeHeadSprite.setTexture(centipedeHeadTexture);
	centipedeHeadSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
    
	//tint the centipede red and black so it looks "evil"
	centipedeBodySprite.setColor(sf::Color::Red); // red and black
	centipedeHeadSprite.setColor(sf::Color::Red); // red and black
	

	///////////////////////////////////////////////////////////////////////
	//								TEXT						         //
	///////////////////////////////////////////////////////////////////////

    //outputting the score
 	sf::Font font;
    if (!font.loadFromFile("GameElements\\ARCADECLASSIC.TTF"))
    {
        //font loading failure
        cerr << "Failed to load font!" << endl;
        return EXIT_FAILURE;
    }
    sf::Text text;
    text.setFont(font);
	text.setCharacterSize(30);
	text.setFillColor(sf::Color::White);
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 0); //set position to the top right i.e. origin of the screen

	

	///////////////////////////////////////////////////////////////////////
	//							VARIABLES								 //
	///////////////////////////////////////////////////////////////////////
	float speed = 1.0 / static_cast<float>(boxPixelsX) ; //player speed = one row/column at a time
		
	int score = 0; //score initialized to 0
	
	int hits = 0; //hits taken by centipede initialized to 0


	///////////////////////////////////////////////////////////////////////
	//								GAME						         //
	///////////////////////////////////////////////////////////////////////

	while(window.isOpen()) 
	{

		window.draw(backgroundSprite);
		
		if (player[exists])
			movePlayer(player, speed);
		
		if (bullet[exists] == true)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				moveBullet(bullet, bulletClock);
			drawBullet(window, bullet, bulletSprite);
		}

		drawPlayer(window, player, playerSprite, deathSprite, switchClock, deathState, switchTime, deathSound);
		
		drawMushrooms(window, mushroomSprite, mushrooms, mushroomsCount);
			
		handleBullet(player, bullet, mushrooms, mushroomsCount, speed, score, bulletClock, fireSound, poisonedMushroom);
				
		drawCentipede(window, centipede, centipedeLength, centipedeHeadSprite, centipedeBodySprite);
		
		moveCentipede(centipede, centipedeLength, centipedeSpeed);
		
		mushroomCentipedeCollision(centipede, centipedeLength, mushrooms, mushroomsCount, centipedeSpeed);

		shootCentipede(bullet, centipede, centipedeLength, score, hits, window, poisonedMushroomSprite, poisonedMushroom);
		
		drawPoisonedMushroom(window, poisonedMushroomSprite, hits, poisonedMushroom);
		
		poisonMushroomPlayerCollision(player, poisonedMushroom, score);
		
		centipedePlayerCollision(player, centipede, centipedeLength, score);

		// update the text string with the current score
        text.setString("Score " + to_string(score));

		sf::Event e;
		while (window.pollEvent(e)) 
		{
			if (e.type == sf::Event::Closed)
			{
				return 0;
			}
			if (e.type == sf::Event::MouseButtonPressed)
			{
				if (e.mouseButton.button == sf::Mouse::Right)
				{
					std::cout << "the right button was pressed" << std::endl;
					std::cout << "mouse x: " << e.mouseButton.x << std::endl;
					std::cout << "mouse y: " << e.mouseButton.y << std::endl;
				}
				else if (e.mouseButton.button == sf::Mouse::Left)
				{
					std::cout << "the left button was pressed" << std::endl;
					std::cout << "mouse x: " << e.mouseButton.x << std::endl;
					std::cout << "mouse y: " << e.mouseButton.y << std::endl;
				}
			}
		}	

		window.draw(text); //for the score output
		window.display();
		window.clear();
	}
}

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//						FUNCTION DEFINITIONS							  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

// a function to draw the player and the death of the player when it dies in the explosion cycle, then respawn the player after its death
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite, sf::Sprite& deathSprite, sf::Clock& switchClock, int& deathState, int switchTime, sf::Sound& deathSound)
{
	if (player[exists] == 1)
	{
		playerSprite.setPosition(player[x], player[y]);
		window.draw(playerSprite);
	}

	//player dies: cycle through the explosion in the death png
	if (player[exists] == 0) //only if player doesnt exist
	{
		if (deathState == 0) 
		{
			deathSound.play(); //play the death sound only once, when the explosion begins
			// initial state: display the first texture rectangle
			deathSprite.setTextureRect(sf::IntRect(0, 32, 2 * boxPixelsX, boxPixelsY));
			deathSprite.setPosition(player[x], player[y]);
			window.draw(deathSprite);
		} 
		else if (deathState == 1) 
		{
			deathSprite.setTextureRect(sf::IntRect(64, 32, 2 * boxPixelsX, boxPixelsY));

			deathSprite.setPosition(player[x], player[y]);
			window.draw(deathSprite);
		} 
		else if (deathState == 2) 
		{
			deathSprite.setTextureRect(sf::IntRect(128, 32, 2 * boxPixelsX, boxPixelsY));
			deathSprite.setPosition(player[x], player[y]);
			window.draw(deathSprite);
		} 
		else if (deathState == 3) 
		{
			deathSprite.setTextureRect(sf::IntRect(192, 32, 2 * boxPixelsX, boxPixelsY));
			deathSprite.setPosition(player[x], player[y]);
			window.draw(deathSprite);
		}
		else if (deathState == 4)
		{
			if (switchClock.getElapsedTime().asMilliseconds() >= switchTime) 
			{
				switchClock.restart();
				deathState = 0;
	
				// reset
				player[x] = (gameColumns / 2) * boxPixelsX;
				player[y] = resolutionY - boxPixelsY;
				player[exists] = 1;
			}	
		}

		if (deathState < 4)
		{
			//this will check if its time to switch to the next rectangle
			if (switchClock.getElapsedTime().asMilliseconds() >= switchTime)
			{
				switchClock.restart();
				deathState++;
			}
		}
	}
}

void moveBullet(float bullet[], sf::Clock& bulletClock) 
{
	if (bulletClock.getElapsedTime().asMilliseconds() < 20)
		return;

	bulletClock.restart();
	bullet[y] -= 10;	
	if (bullet[y] < -32)
		bullet[exists] = false;
}

void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite) 
{
	bulletSprite.setPosition(bullet[x], bullet[y]);
	window.draw(bulletSprite);
}

// a function to move the player based on keyboard press
void movePlayer(float player[], float speed) 
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && player[x] > 0) 
        player[x] -= speed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && player[x] < resolutionX - boxPixelsX)
        player[x] += speed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && player[y] > resolutionY - 5 * boxPixelsY)
        player[y] -= speed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && player[y] < resolutionY - boxPixelsY)
        player[y] += speed;
}

// a function to handle the firing of the bullet and the shooting of the mushrooms so that the score can be incremented and the mushroom can disappear upon being shot at
void handleBullet(float player[], float bullet[], float mushrooms[][3], int mushroomsCount, float speed, int& score, sf::Clock& bulletClock, sf::Sound& fireSound, float poisonedMushroom[][3])
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !bullet[exists]) 
    {
        bullet[x] = player[x] + boxPixelsX / 2;
        bullet[y] = player[y] - boxPixelsY;
        bullet[exists] = true;
        // play the fire sound when the bullet is shot
        fireSound.play();
    }
    
    if (bullet[exists]) 
    {	
		moveBullet(bullet, bulletClock);
    
		// check for collisions with (normal) mushrooms and update score
		for (int i = 0; i < mushroomsCount; i++) 
		{
			if (mushrooms[i][2] > 0 && bullet[x] < mushrooms[i][0] + boxPixelsX && bullet[x] + boxPixelsX > mushrooms[i][0] && bullet[y] < mushrooms[i][1] + boxPixelsY && bullet[y] + boxPixelsY > mushrooms[i][1]) //check the corners against each other 
			{
				// mushroom hit by bullet
				mushrooms[i][2]--; // dec by 1 every time it is hit
				bullet[exists] = false; // bullet disappears on hit
	
			if (mushrooms[i][2] == 0) //2 shots taken
				score++; // mushroom shot twice, remove it and increase score	    		
			}
		}
    
		//check for collisions with the posioned mushrooms
		for (int i = 0; i < 1; i++)
		{
    		if (poisonedMushroom[i][2] > 0 && bullet[x] < poisonedMushroom[i][0] + boxPixelsX && bullet[x] + boxPixelsX > poisonedMushroom[i][0] && bullet[y] < poisonedMushroom[i][1] + boxPixelsY && bullet[y] + boxPixelsY > poisonedMushroom[i][1]) //corner check 
			{
				// mushroom hit by bullet
				poisonedMushroom[i][2]--; // dec by 1 every time it is hit
				bullet[exists] = false; // bullet disappears on hit
    		}
		}
    }
}

// a function to draw the mushrooms and to change the texture when the mushroom is shot
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite, float mushrooms[][3], int mushroomsCount)
{
    for (int i = 0; i < mushroomsCount; i++)
    {
        if (mushrooms[i][2] > 0)
        {
			//change the mushroom if shot to the next mushroom in the png
			if (mushrooms[i][2] == 2)
				mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY)); 
		               		
			else if (mushrooms[i][2] == 1)
				mushroomSprite.setTextureRect(sf::IntRect(32, 0, boxPixelsX, boxPixelsY)); 
		                	
			//draw the mushroom at its position
			mushroomSprite.setPosition(mushrooms[i][x], mushrooms[i][y]);
			window.draw(mushroomSprite);
        }
        
        
    }
}

// a function to control the centipede's movement after colliding with a mushroom so it turns and descends faster
void mushroomCentipedeCollision(float centipede[][4], int centipedeLength, float mushrooms[][3], int mushroomsCount, float centipedeSpeed)
{
    for (int i = 0; i < centipedeLength; i++)
    {
        if (centipede[i][2]) // centipede segment exists
        {
            int centipedeX = centipede[i][x] / boxPixelsX; // calculate centipede segment coordinates within the game grid by dividing by 32 because each cell is 32x32
            int centipedeY = centipede[i][y] / boxPixelsY;

            // check for collisions with mushrooms
            for (int j = 0; j < mushroomsCount; j++)
            {
                int mushroomX = mushrooms[j][x] / boxPixelsX; // calculate mushroom coordinates within the game grid
                int mushroomY = mushrooms[j][y] / boxPixelsY;
				
                if (centipedeX == mushroomX + boxPixelsX && centipedeY == mushroomY && centipede[i][3] == 1 && mushrooms[j][2] > 0)
                {
					centipedeX -= boxPixelsX/2; //so it turns half a column instead from 32 (boxPixels)
					centipede[i][y] += boxPixelsY;
                }
                
                if (centipedeX == mushroomX && centipedeY == mushroomY && centipede[i][3] == 1 && mushrooms[j][2] > 0)
                {
					centipedeX += boxPixelsX/2;
					centipede[i][y] += boxPixelsY;
					centipede[i][3] = 0; //change the direction so it can move right now
				}
				
				if (centipedeX == mushroomX - boxPixelsX && centipedeY == mushroomY && centipede[i][3] == 0 && mushrooms[j][2] > 0)
				{
					centipedeX += boxPixelsX/2;
					centipede[i][y] += boxPixelsY;
				}
				
				if (centipedeX == mushroomX && centipedeY == mushroomY && centipede[i][3] == 0 && mushrooms[j][2] > 0)
				{
					centipedeX += boxPixelsX/2;
					centipede[i][y] += boxPixelsY;
					centipede[i][3] = 1; //change the direction so it can move left now
				}

            }
        }
    }
}

// a function to draw the poisoned mushroom when the centipede is hit
void drawPoisonedMushroom(sf::RenderWindow& window, sf::Sprite& poisonedMushroomSprite, int& hits, float poisonedMushroom[][3])
{
    for (int i = 0; i < 1 ; i++)
    {
		if (poisonedMushroom[i][2] > 0 && hits > 0)
		{
			//change the mushroom if shot to the next mushroom in the png
			if (poisonedMushroom[i][2] == 2)
				poisonedMushroomSprite.setTextureRect(sf::IntRect(0, 32, boxPixelsX, boxPixelsY)); 
		               		
			else if (poisonedMushroom[i][2] == 1)
				poisonedMushroomSprite.setTextureRect(sf::IntRect(32, 32, boxPixelsX, boxPixelsY)); 
			             	
			//draw the mushroom at its position
			poisonedMushroomSprite.setPosition(poisonedMushroom[i][x], poisonedMushroom[i][y]);
			window.draw(poisonedMushroomSprite);
		}
	}
}

// a function to detect the poisoned mushroom and player collision so that player can "die" upon being hit
void poisonMushroomPlayerCollision(float player[], float poisonedMushroom[][3], int& score)
{
	int playerX = player[x] / boxPixelsX; //calculate player coordinates
	int playerY = player[y] / boxPixelsY;

	for (int i = 0; i < 1; i++)
	{
		int mushroomX = poisonedMushroom[i][x] / boxPixelsX; //do the same for the mushrooms 
		int mushroomY = poisonedMushroom[i][y] / boxPixelsY;

		if (poisonedMushroom[i][2] > 0  && playerX == mushroomX && playerY == mushroomY)
		{
			// reposition player's coordinates after colliding with a mushroom
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				player[x] = poisonedMushroom[i][x] + boxPixelsX;
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				player[x] = poisonedMushroom[i][x] - boxPixelsX;
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				player[y] = poisonedMushroom[i][y] + boxPixelsY;
    		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			player[y] = poisonedMushroom[i][y] - boxPixelsY;
			player[exists] = 0; //player dies upon touch
			score = 0; //score resets after death
		}
	}
}

// a function to detect centipede and player collision so that player can "die" upon being hit
void centipedePlayerCollision(float player[], float centipede[][4], int centipedeLength, int& score)
{
	int playerX = player[x] / boxPixelsX; //calculate player coordinates
	int playerY = player[y] / boxPixelsY;

	for (int i = 0; i < centipedeLength; i++)
	{
	    int centipedeX = centipede[i][x] / boxPixelsX; // calculate centipede segment coordinates
	    int centipedeY = centipede[i][y] / boxPixelsY;	
	    //check for collision
	    if (centipede[i][2] > 0 && centipedeX == playerX && centipedeY == playerY)
	    {
            player[exists] = 0; //player dies upon touch
            score = 0; //score resets after death
	    } 
	}
}

// a function to draw the centipede with the head at the front
void drawCentipede(sf::RenderWindow& window, float centipede[][4], int centipedeLength, sf::Sprite& centipedeHeadSprite, sf::Sprite& centipedeBodySprite)
{
	for (int i = 0; i < centipedeLength; i++)
	{
		if (centipede[i][exists]) //draw only if segment exists
		{
			if (i == 0 || !centipede[i - 1][exists]) //head at 0th index and the index next to every deleted portion (segment head)
			{
				// draw the head at the front
				centipedeHeadSprite.setPosition(centipede[i][x], centipede[i][y]);
				window.draw(centipedeHeadSprite);
			}
			else //body
			{
				// draw the body segments
				centipedeBodySprite.setPosition(centipede[i][x], centipede[i][y]);
				window.draw(centipedeBodySprite);
			}

		}

	}
}

// a function to control the centipede's movement
void moveCentipede(float centipede[][4], int centipedeLength, float centipedeSpeed)
{
	bool lastRow; //flag to determine if centipede has reached the bottom

	for (int i = 0; i < centipedeLength; i++)
	{
		if (!centipede[i][exists])
			continue;

		lastRow = false;
		//starts moving left, hits the left edge
		if (centipede[i][x] != boxPixelsX && centipede[i][3] == 1) //move left until it needs to start turning (one column before)
			centipede[i][x] -= centipedeSpeed;

		if (centipede[i][x] == boxPixelsX && centipede[i][3] == 1) //start turning one row before: drop half a row down while still moving
		{
			centipede[i][x] -= boxPixelsX / 2; //so it turns half a column instead from 32 (boxPixels)
			centipede[i][y] += boxPixelsY / 2;
		}

		if (centipede[i][x] == 0 && centipede[i][3] == 1) //once dropped halfway, turn and move right; drop another half row so that in total, one full row is dropped
		{
			centipede[i][x] += boxPixelsX;
			centipede[i][y] += boxPixelsY / 2;
			centipede[i][3] = 0; //change the direction so it can move right now
		}

		//starts moving right, hits the right edge
		if (centipede[i][x] != resolutionX - 2 * boxPixelsX && centipede[i][3] == 0) //resolutionX - 2*boxPixelsX (896) is one column behind the edge, where we want to start turning
			centipede[i][x] += centipedeSpeed;

		if (centipede[i][x] == resolutionX - 2 * boxPixelsX && centipede[i][3] == 0)
		{
			centipede[i][x] += boxPixelsX / 2;
			centipede[i][y] += boxPixelsY / 2;
		}

		if (centipede[i][x] == resolutionX - boxPixelsX && centipede[i][3] == 0)
		{
			centipede[i][x] -= boxPixelsX;
			centipede[i][y] += boxPixelsY / 2;
			centipede[i][3] = 1; //change the direction so it can move left now
		}

		if (centipede[i][y] == resolutionY - boxPixelsY)
			lastRow = true;

		if (lastRow) //centipede at bottom of screen, start looping in player area
		{
			if (centipede[i][3] == 1) //moving left
			{
				centipede[i][x] += boxPixelsX / 2; //so it turns half a column instead from 32 (boxPixels)
				while (centipede[i][y] > resolutionY - 5 * boxPixelsY)
					centipede[i][y] -= boxPixelsY / 2;
				centipede[i][3] = 0; //change direction
			}
			else if (centipede[i][3] == 0) //moving right
			{
				centipede[i][x] -= boxPixelsX / 2; //so it turns half a column instead from 32 (boxPixels)
				while (centipede[i][y] > resolutionY - 5 * boxPixelsY)
					centipede[i][y] -= boxPixelsY / 2;
				centipede[i][3] = 1; //change direction
			}
		}
	}
}

// a function to control the centipede being hit by the bullet
void shootCentipede(float bullet[], float centipede[][4], int centipedeLength, int& score, int& hits, sf::RenderWindow& window, sf::Sprite& poisonedMushroomSprite, float poisonedMushroom[][3])
{
	int bulletX = bullet[x] / boxPixelsX; // bullet coordinates within grid
	int bulletY = bullet[y] / boxPixelsY;

	for (int i = 0; i < centipedeLength; i++)
	{
		if (!centipede[i][exists]) //sklip deleted segments
			continue;

		int centipedeX = centipede[i][x] / boxPixelsX; // centipede segment coordinates within grid
		int centipedeY = centipede[i][y] / boxPixelsY;

		// check for bullet and centipede collision
		if (bulletX == centipedeX && bulletY == centipedeY)
		{
			if (i == 0 || !centipede[i - 1][exists]) //bullet hits head
			{
				for (int j = 1; j < centipedeLength && centipede[j][exists]; j++) //iterate over that segment's portion
				{
					hits++;
					centipede[j][2] = false; // set all segments to false so that centipede disappears
				}
				centipede[i][2] = false;
				score += 20; // 20 points for the head
			}
			else //hit body
			{
				bullet[exists] = false;
				hits++;
				score += 10; //10 points for body
				splitCentipede(centipede, centipedeLength, i); //split the centipede
			}
		}
	}
}

// a function to split the centipede upon being hit
void splitCentipede(float centipede[][4], int centipedeLength, int index)
{

	if (index < 0 || index >= centipedeLength) //invalid index
		return;

	centipede[index][exists] = false; // mark hit segment as nonexistent

	bool newDirection = !centipede[index][3]; //toggle direction of new segment

	for (int i = index + 1; i < centipedeLength; i++)
	{
		if (!centipede[i][exists])
		{
			break; //stop if another split is encountered (segments divded by nonexistent parts)
		}
		centipede[i][3] = newDirection; //reverse direction of centipede
		//cout << "new coordinates: (" << centipede[i][x] << ", " << centipede[i][y] << ")" << endl;
	}
}





