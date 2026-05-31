#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <cmath>

// No global variables or constants - everything will be in main()
// No STL containers - using arrays instead
// No classes, structs, or enums - using primitive types and arrays
// No sstream or fstream

int main()
{
    // Game State Constants
    const int INTRO = 0;
    const int LEVEL1 = 1;
    const int LEVEL2 = 2;
    const int GAME_OVER = 3;
    const int VICTORY = 4;

    // Game constants
    const float SUCTION_RADIUS = 170.0f;
    const float SUCTION_SECTION_ANGLE = 90.0f;
    const float SUCTION_VERTICAL_OFFSET = -20.0f;
    const int screen_x = 1136 - 64;
    const int screen_y = 896;
    const int cell_size = 64;
    const int height = 14;
    const int width = 18;
    const float ENEMY_SPAWN_DELAY = 1.0f;
    const float PLAYER_VERTICAL_TRAVERSAL_RATIO = 0.5f;
    const float PLAYER_HORIZONTAL_TRAVERSAL_RATIO = 0.9f;
    const float WALL_COLLISION_PADDING_X = -2.0f;
    const float WALL_COLLISION_PADDING_Y = 5.0f;
    const float SLOPE_COLLISION_PADDING = 5.0f;
    const float EDGE_CHECK_PADDING = 1.0f;
    const float INVINCIBILITY_TIME = 2.0f;
    const float FIRE_FRAME_TIME = 0.08f;
    const int FIRE_FRAME_COUNT = 5;
    const float jumpStrength = -20.0f;
    const float gravity = 1.0f;

    // Game variables
    int currentGameState = INTRO;
    int score = 0;
    int playerLives = 5;
    int currentLevel = 1;
    bool level1Completed = false;
    bool isTimerRunning = false;
    bool isMovingDown = false;
    float downSpeed = 5.0f;

    // Timer
    sf::Clock gameTimer;
    sf::Time finalGameTime;

    // Enemy spawning variables for level 2
    int enemiesSpawnedLevel2 = 0;
    int currentEnemyTypeIndex = 0;
    sf::Clock enemySpawnClock;

    // Level 2 enemy sequence
    int level2SequenceTypes[4] = {2, 3, 1, 0};
    int level2SequenceCounts[4] = {3, 4, 9, 4};

    // Create window
    sf::RenderWindow window(sf::VideoMode(screen_x, screen_y), "Tumble-POP Enhanced", sf::Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // Load fonts
    sf::Font uiFont;
    if (!uiFont.loadFromFile("Data/arial.ttf"))
        return -1;

    // Load textures
    sf::Texture introTexture;
    if (!introTexture.loadFromFile("Data/Tumble_Pop.png"))
        return -1;
    sf::Sprite introSprite;
    introSprite.setTexture(introTexture);
    float scaleX = (float)screen_x / introTexture.getSize().x;
    float scaleY = (float)screen_y / introTexture.getSize().y;
    introSprite.setScale(scaleX, scaleY);

    sf::Texture bgTex;
    if (!bgTex.loadFromFile("Data/bg.png"))
        return -1;
    sf::Sprite bgSprite;
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0, 0);

    sf::Texture blockTexture;
    if (!blockTexture.loadFromFile("Data/block1.png"))
        return -1;
    sf::Sprite blockSprite;
    blockSprite.setTexture(blockTexture);

    // Load player texture
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("Animations/All3.png"))
        return -1;

    // Player animation frames
    sf::IntRect playerFrames[5];
    playerFrames[0] = sf::IntRect(0, 0, 32, 43);
    playerFrames[1] = sf::IntRect(32, 0, 34, 43);
    playerFrames[2] = sf::IntRect(66, 0, 33, 43);
    playerFrames[3] = sf::IntRect(99, 0, 34, 43);
    playerFrames[4] = sf::IntRect(133, 0, 34, 43);

    sf::IntRect playerStaticFrame = sf::IntRect(133, 0, 34, 43);

    // Load enemy textures
    sf::Texture ghostTexture;
    if (!ghostTexture.loadFromFile("Animations/Ghost.png"))
        return -1;

    sf::Texture skeletonTexture;
    if (!skeletonTexture.loadFromFile("Animations/Skeleton.png"))
        return -1;

    sf::Texture invisibleTexture;
    if (!invisibleTexture.loadFromFile("Animations/Invisible.png"))
        return -1;

    sf::Texture chelnovTexture;
    if (!chelnovTexture.loadFromFile("Animations/Chelnov.png"))
        return -1;

    // Enemy animation frames
    sf::IntRect ghostFrames[3];
    ghostFrames[0] = sf::IntRect(1008, 0, 54, 49);
    ghostFrames[1] = sf::IntRect(1062, 0, 53, 49);
    ghostFrames[2] = sf::IntRect(1115, 0, 47, 49);

    sf::IntRect skeletonFrames[4];
    skeletonFrames[0] = sf::IntRect(199, 29, 39, 46);
    skeletonFrames[1] = sf::IntRect(238, 29, 35, 46);
    skeletonFrames[2] = sf::IntRect(273, 29, 32, 46);
    skeletonFrames[3] = sf::IntRect(305, 29, 35, 46);

    sf::IntRect invisibleManFrames[4];
    invisibleManFrames[0] = sf::IntRect(795, 12, 36, 51);
    invisibleManFrames[1] = sf::IntRect(831, 12, 36, 51);
    invisibleManFrames[2] = sf::IntRect(868, 12, 32, 51);
    invisibleManFrames[3] = sf::IntRect(900, 12, 38, 51);

    sf::IntRect chelnovFrames[6];
    chelnovFrames[0] = sf::IntRect(87, 0, 33, 44);
    chelnovFrames[1] = sf::IntRect(120, 0, 29, 44);
    chelnovFrames[2] = sf::IntRect(149, 0, 34, 44);
    chelnovFrames[3] = sf::IntRect(183, 0, 31, 44);
    chelnovFrames[4] = sf::IntRect(214, 0, 34, 44);
    chelnovFrames[5] = sf::IntRect(248, 0, 34, 44);

    // Load music
    sf::Music lvlMusic;
    if (!lvlMusic.openFromFile("Data/mus.ogg"))
        return -1;
    lvlMusic.setVolume(10);
    lvlMusic.setLoop(true);

    // Load fire textures
    sf::Texture fireTextures[5];
    sf::Sprite fireSprites[5];
    for (int i = 0; i < FIRE_FRAME_COUNT; i++)
    {
        char filename[50];
        sprintf(filename, "Animations/Fire2/%d.png", i + 1);
        if (!fireTextures[i].loadFromFile(filename))
            return -1;
        fireSprites[i].setTexture(fireTextures[i]);
        fireSprites[i].setScale(2.0, 2.0);
    }

    // Player variables
    float player_x = 500;
    float player_y = 150;
    float speed = 5;
    bool onGround = false;
    float velocityY = 0;
    float terminal_Velocity = 20;
    bool ignorePlatformCollision = false;
    int PlayerHeight = 43;
    int PlayerWidth = 33;

    sf::Sprite PlayerSprite;
    PlayerSprite.setTexture(playerTexture);
    PlayerSprite.setScale(2, 2);
    PlayerSprite.setTextureRect(playerStaticFrame);
    PlayerSprite.setPosition(player_x - cell_size + 25, player_y);

    // Player animation variables
    float playerAnimationTime = 0.0f;
    int currentPlayerFrame = 0;
    float playerFrameTime = 0.1f;
    bool playerAnimationPlaying = false;
    bool playerIsMoving = false;
    sf::Clock playerAnimationClock;

    // Fire animation variables
    sf::Clock fireAnimationClock;
    float fireAnimationTime = 0.0f;
    int currentFireFrame = 0;
    bool isFiring = false;
    int fireDirection = 1; // 0=LEFT, 1=RIGHT, 2=UP, 3=DOWN

    // Level data
    char lvl[14][18] = {
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '.', '.', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '.', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '#', '#', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '#', '#', '#', '#', '.', '.', '#', '#', '#', '#', '.', '.', '#', '#', '#', '#', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '#', '#', '#', '#', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '.', '.', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '.', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '#', '#', '#', '#', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '#', '#', '#', '#', '.', '.', '#', '#', '#', '#', '.', '.', '#', '#', '#', '#', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '#', '#', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '.', '.', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '.', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}};

    char lvl2[14][18] = {
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '#', '#', '#', '#', 's', '.', '.', '#', '#', '#', '#', '#', '#', '#', '.', '.', '#'},
        {'#', '.', '.', '.', 'S', '?', 's', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '#', '.', '.', '.', 'S', '?', 's', '.', '.', '#', '#', '#', '.', '.', '#', '#', '#'},
        {'#', '.', '.', '.', '.', '.', 'S', '?', 's', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '#', '#', '.', '.', '.', '.', 'S', '?', 's', '.', '.', '#', '#', '#', '#', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', 'S', '?', 's', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '#', '#', '#', '#', '.', '.', '.', '.', 'S', '?', 's', '.', '.', '#', '#', '#', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', 'S', '?', 's', '.', '.', '.', '.', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '.', '.', '.', '.', 'S', '?', '#', '#', '.', '.', '#'},
        {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}};

    char (*currentLevelData)[18] = lvl;

    // Enemy arrays (max 100 enemies)
    const int MAX_ENEMIES = 100;
    int enemyCount = 0;
    float enemyX[MAX_ENEMIES];
    float enemyY[MAX_ENEMIES];
    float enemySpeed[MAX_ENEMIES];
    int enemyDirection[MAX_ENEMIES];
    bool enemyAlive[MAX_ENEMIES];
    bool enemyIgnorePlatformCollision[MAX_ENEMIES];
    float enemyMoveTimer[MAX_ENEMIES];
    float enemyDirectionChangeTime[MAX_ENEMIES];
    int enemyType[MAX_ENEMIES];
    float enemyVelocityY[MAX_ENEMIES];
    bool enemyOnGround[MAX_ENEMIES];
    float enemyActionTimer[MAX_ENEMIES];
    float enemyActionCooldown[MAX_ENEMIES];
    bool enemyIsInvisible[MAX_ENEMIES];
    bool enemyIsShooting[MAX_ENEMIES];
    float enemyShootTimer[MAX_ENEMIES];
    float enemyAnimationTimer[MAX_ENEMIES];
    int enemyCurrentFrame[MAX_ENEMIES];
    float enemyCurrentTime[MAX_ENEMIES];
    sf::Sprite enemySprites[MAX_ENEMIES];

    // Projectile arrays (max 100 projectiles)
    const int MAX_PROJECTILES = 100;
    int projectileCount = 0;
    float projectileX[MAX_PROJECTILES];
    float projectileY[MAX_PROJECTILES];
    int projectileDirection[MAX_PROJECTILES];
    bool projectileActive[MAX_PROJECTILES];
    float projectileSpeed[MAX_PROJECTILES];
    sf::RectangleShape projectileShapes[MAX_PROJECTILES];

    // UI Text
    sf::Text scoreText;
    scoreText.setFont(uiFont);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setPosition(screen_x - 150, 10);

    // Clocks
    sf::Clock introClock;
    sf::Clock invincibilityClock;
    bool isInvincible = false;

    // Suction timer
    sf::Clock suctionClock;
    float suctionTimer = 0.0f;
    const float suctionCooldown = 0.3f;

    // Helper functions declarations
    float getSlopeHeightAtX(int row, int col, float xInCell, char lvl[][18]);
    bool isOnSlope(float x, float y, char lvl[][18], int cell_size, float &slopeY, char &slopeType);
    bool checkCharacterOnSlope(float x, float y, float charWidth, float charHeight, char lvl[][18], int cell_size, float &groundY, char &slopeType);
    void moveEnemyHorizontally(float &enemyX, float &enemyY, int enemyDirection, bool enemyOnGround, char lvl[][18], int cell_size, float dx, int screen_x, int screen_y, int height, int width);
    void move_player_horizontally(char lvl[][18], float &player_x, float &player_y, int Pwidth, int Pheight, float dx, int cell_size, bool &onGround, int screen_x, int screen_y, int height, int width);
    bool checkSlopeCollision(float x, float y, char lvl[][18], int cell_size, float &groundY);
    void player_gravity(char lvl[][18], float &velocityY, bool &onGround, const float &gravity, float &terminal_Velocity, float &player_x, float &player_y, const int cell_size, int &Pheight, int &Pwidth, bool &ignorePlatformCollision, bool isMovingDown, float downSpeed, int height, int width, int screen_y);
    void applyEnemyGravity(float &enemyY, float &enemyVelocityY, bool &enemyOnGround, bool &enemyIgnorePlatformCollision, char lvl[][18], int cell_size, float gravity, float terminalVelocity, int height, int width, int screen_y);
    void display_level(sf::RenderWindow & window, char lvl[][18], sf::Texture &bgTex, sf::Sprite &bgSprite, sf::Texture &blockTexture, sf::Sprite &blockSprite, const int cell_size);
    void getRandomBlockPosition(char lvl[][18], int cell_size, int enemyWidth, int enemyHeight, int screen_x, int height, int width, float &outX, float &outY);
    void updateGhost(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection, bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer, float &enemyDirectionChangeTime, float &enemyVelocityY, bool &enemyOnGround, float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible, bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer, int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite, sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount, float deltaTime, char lvl[][18], int cell_size, int screen_x, int screen_y, int height, int width);
    void updateSkeleton(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection, bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer, float &enemyDirectionChangeTime, float &enemyVelocityY, bool &enemyOnGround, float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible, bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer, int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite, sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount, float deltaTime, char lvl[][18], int cell_size, int screen_x, int screen_y, int height, int width);
    void initEnemy(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection, bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer, float &enemyDirectionChangeTime, int &enemyType, float &enemyVelocityY, bool &enemyOnGround, float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible, bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer, int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite, sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount, char lvl[][18], int cell_size, int enemyTypeValue, int screen_x, int screen_y, int height, int width);
    void checkEnemyProximityDamage(float *enemyX, float *enemyY, bool *enemyAlive, int enemyCount, float playerX, float playerY, int playerWidth, int playerHeight, bool &isInvincible, sf::Clock &invincibilityClock, int &playerLives, int &currentGameState, sf::Music &lvlMusic, sf::Clock &gameTimer, sf::Time &finalGameTime, bool &isTimerRunning);
    void resetGame(int &score, int &playerLives, int &currentLevel, bool &level1Completed, int &enemiesSpawnedLevel2, int &currentEnemyTypeIndex, sf::Clock &gameTimer, bool &isTimerRunning, int &currentGameState);
    void updateInvisibleMan(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection, bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer, float &enemyDirectionChangeTime, float &enemyVelocityY, bool &enemyOnGround, float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible, bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer, int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite, sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount, float deltaTime, char lvl[][18], int cell_size, int screen_x, int screen_y, int height, int width);
    void updateChelnov(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection, bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer, float &enemyDirectionChangeTime, float &enemyVelocityY, bool &enemyOnGround, float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible, bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer, int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite, sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount, float deltaTime, char lvl[][18], int cell_size, float *projectileX, float *projectileY, int *projectileDirection, bool *projectileActive, float *projectileSpeed, sf::RectangleShape *projectileShapes, int &projectileCount, int MAX_PROJECTILES, int screen_x, int screen_y, int height, int width);
    void drawEnemy(sf::RenderWindow & window, sf::Sprite & enemySprite, float drawX, float drawY, int enemyType, bool enemyIsInvisible, bool enemyAlive);
    bool checkEnemySuction(float enemyX, float enemyY, bool enemyAlive, int enemyType, bool enemyIsShooting, float playerX, float playerY, float radius, char directionKey, float SUCTION_RADIUS, float SUCTION_SECTION_ANGLE, float SUCTION_VERTICAL_OFFSET);
    bool checkEnemyCollision(float enemyX, float enemyY, float playerX, float playerY, int playerWidth, int playerHeight);
    bool checkProjectileCollision(float projX, float projY, float playerX, float playerY, int playerWidth, int playerHeight);
    void drawUI(sf::RenderWindow & window, sf::Font & uiFont, sf::Text & scoreText, int playerLives, int currentLevel, int score, int screen_x, bool isTimerRunning, int currentGameState, sf::Clock &gameTimer, sf::Time &finalGameTime);
    void drawIntroScreen(sf::RenderWindow & window, sf::Texture & introTexture, sf::Sprite & introSprite, sf::Clock & introClock, sf::Font & uiFont, int screen_x, int screen_y);
    void drawGameOverScreen(sf::RenderWindow & window, sf::Texture & introTexture, sf::Sprite & introSprite, sf::Font & uiFont, int score, sf::Time &finalGameTime, int screen_x, int screen_y);
    void drawVictoryScreen(sf::RenderWindow & window, sf::Texture & introTexture, sf::Sprite & introSprite, sf::Font & uiFont, int score, sf::Time &finalGameTime, int screen_x, int screen_y);
    bool checkLevelCompletion(bool *enemyAlive, int enemyCount, int currentLevel, int currentEnemyTypeIndex, int *level2SequenceTypes, int *level2SequenceCounts, int level2SequenceSize);

    // Main game loop
    sf::Event ev;
    while (window.isOpen())
    {
        float deltaTime = 1.0f / 60.0f;

        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();

            if (ev.type == sf::Event::KeyPressed)
            {
                if (currentGameState == INTRO)
                {
                    if (introClock.getElapsedTime().asSeconds() > 2.0f)
                    {
                        currentGameState = LEVEL1;
                        gameTimer.restart();
                        isTimerRunning = true;
                        lvlMusic.play();

                        // Initialize enemies for level 1
                        enemyCount = 12;
                        for (int i = 0; i < enemyCount; i++)
                        {
                            int enemyTypeValue = (i < 8) ? 0 : 1;
                            sf::Texture *enemyTex;
                            sf::IntRect *frames;
                            int framesCount;

                            switch (enemyTypeValue)
                            {
                            case 0:
                                enemyTex = &ghostTexture;
                                frames = ghostFrames;
                                framesCount = 3;
                                break;
                            case 1:
                                enemyTex = &skeletonTexture;
                                frames = skeletonFrames;
                                framesCount = 4;
                                break;
                            default:
                                enemyTex = &ghostTexture;
                                frames = ghostFrames;
                                framesCount = 3;
                            }

                            initEnemy(
                                enemyX[i], enemyY[i], enemySpeed[i], enemyDirection[i],
                                enemyAlive[i], enemyIgnorePlatformCollision[i], enemyMoveTimer[i],
                                enemyDirectionChangeTime[i], enemyType[i], enemyVelocityY[i],
                                enemyOnGround[i], enemyActionTimer[i], enemyActionCooldown[i],
                                enemyIsInvisible[i], enemyIsShooting[i], enemyShootTimer[i],
                                enemyAnimationTimer[i], enemyCurrentFrame[i], enemyCurrentTime[i],
                                enemySprites[i], *enemyTex, frames, framesCount,
                                currentLevelData, cell_size, enemyTypeValue,
                                screen_x, screen_y, height, width);
                        }
                    }
                }
                else if (currentGameState == GAME_OVER || currentGameState == VICTORY)
                {
                    if (ev.key.code == sf::Keyboard::R)
                    {
                        resetGame(score, playerLives, currentLevel, level1Completed,
                                  enemiesSpawnedLevel2, currentEnemyTypeIndex, gameTimer,
                                  isTimerRunning, currentGameState);
                        player_x = 500;
                        player_y = 150;
                        velocityY = 0;
                        onGround = false;
                        currentLevelData = lvl;
                        PlayerSprite.setTextureRect(playerStaticFrame);
                        enemyCount = 0;
                        projectileCount = 0;
                    }
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        // Handle different game states
        switch (currentGameState)
        {
        case INTRO:
            drawIntroScreen(window, introTexture, introSprite, introClock, uiFont, screen_x, screen_y);
            continue;

        case LEVEL1:
        case LEVEL2:
            break;

        case GAME_OVER:
            drawGameOverScreen(window, introTexture, introSprite, uiFont, score, finalGameTime, screen_x, screen_y);
            continue;

        case VICTORY:
            drawVictoryScreen(window, introTexture, introSprite, uiFont, score, finalGameTime, screen_x, screen_y);
            continue;
        }

        // Update player animation
        playerAnimationTime += deltaTime;

        int dir = 0;
        bool playerFacingRight = true;
        playerIsMoving = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            dir = -1;
            PlayerSprite.setScale(-2, 2);
            PlayerSprite.setOrigin(PlayerSprite.getTextureRect().width, 0);
            playerFacingRight = false;
            playerIsMoving = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            dir = 1;
            PlayerSprite.setScale(2, 2);
            PlayerSprite.setOrigin(0, 0);
            playerFacingRight = true;
            playerIsMoving = true;
        }

        // Update player animation based on movement
        if (playerIsMoving)
        {
            if (!playerAnimationPlaying)
            {
                playerAnimationPlaying = true;
                currentPlayerFrame = 0;
                playerAnimationTime = 0.0f;
            }

            playerAnimationTime += deltaTime;
            if (playerAnimationTime >= playerFrameTime)
            {
                playerAnimationTime -= playerFrameTime;
                currentPlayerFrame++;
                if (currentPlayerFrame >= 5)
                    currentPlayerFrame = 0;
                PlayerSprite.setTextureRect(playerFrames[currentPlayerFrame]);
            }
        }
        else
        {
            playerAnimationPlaying = false;
            PlayerSprite.setTextureRect(playerStaticFrame);
        }

        // Move player horizontally
        if (dir != 0)
        {
            float dx = speed * dir;
            move_player_horizontally(currentLevelData, player_x, player_y, PlayerWidth, PlayerHeight,
                                     dx, cell_size, onGround, screen_x, screen_y, height, width);

            if (player_x < 0)
                player_x = 0;
            if (player_x + PlayerWidth > screen_x)
                player_x = screen_x - PlayerWidth;
        }

        // Jump
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) &&
            onGround)
        {
            velocityY = jumpStrength;
            onGround = false;
        }

        // Down movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            isMovingDown = true;

            bool canMoveDown = true;
            float checkBelowY = player_y + PlayerHeight + 5;
            int checkRow = (int)(checkBelowY) / cell_size;

            float checkPoints[3] = {player_x + 10, player_x + PlayerWidth / 2, player_x + PlayerWidth - 10};

            for (int i = 0; i < 3; i++)
            {
                int col = (int)(checkPoints[i] + cell_size - 25) / cell_size;
                if (col >= 0 && col < width && checkRow >= 0 && checkRow < height)
                {
                    if (currentLevelData[checkRow][col] == '#')
                    {
                        canMoveDown = false;
                        break;
                    }

                    float slopeY;
                    char slopeType;
                    if (isOnSlope(checkPoints[i], checkBelowY, currentLevelData, cell_size, slopeY, slopeType))
                    {
                        if (checkBelowY >= slopeY - 5)
                        {
                            canMoveDown = false;
                            break;
                        }
                    }
                }
            }

            if (canMoveDown && !onGround)
            {
                player_y += downSpeed;
            }
        }
        else
        {
            isMovingDown = false;
        }

        // Drop through platforms
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && onGround && velocityY == 0)
        {
            int bottomRow = (int)(player_y + PlayerHeight + 1) / cell_size;
            bool platformBelow = false;
            bool slopeBelow = false;

            if (bottomRow < height - 1)
            {
                float checkPoints[5] = {
                    player_x + 5,
                    player_x + PlayerWidth * 0.25f,
                    player_x + PlayerWidth * 0.5f,
                    player_x + PlayerWidth * 0.75f,
                    player_x + PlayerWidth - 5};

                for (int i = 0; i < 5; i++)
                {
                    int col = (int)(checkPoints[i] + cell_size - 25) / cell_size;
                    if (col >= 0 && col < width && bottomRow >= 0 && bottomRow < height)
                    {
                        char cellType = currentLevelData[bottomRow][col];
                        if (cellType == '#')
                        {
                            platformBelow = true;
                            break;
                        }
                        else if (cellType == 's' || cellType == 'S' || cellType == '?')
                        {
                            slopeBelow = true;
                        }
                    }
                }

                if (slopeBelow)
                {
                    bool solidGroundFurtherBelow = false;
                    for (int checkRow = bottomRow + 1; checkRow < height; checkRow++)
                    {
                        for (int i = 0; i < 5; i++)
                        {
                            int col = (int)(checkPoints[i] + cell_size - 25) / cell_size;
                            if (col >= 0 && col < width && checkRow >= 0 && checkRow < height)
                            {
                                if (currentLevelData[checkRow][col] == '#')
                                {
                                    solidGroundFurtherBelow = true;
                                    break;
                                }
                            }
                        }
                        if (solidGroundFurtherBelow)
                            break;
                    }

                    if (solidGroundFurtherBelow)
                    {
                        ignorePlatformCollision = true;
                        onGround = false;
                        velocityY = 2;
                    }
                }
                else if (platformBelow)
                {
                    ignorePlatformCollision = true;
                    onGround = false;
                    velocityY = 2;
                }
            }
        }

        // Handle suction/fire
        char currentSuctionKey = ' ';
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            fireDirection = 0; // LEFT
            currentSuctionKey = 'A';
            if (!isFiring)
            {
                isFiring = true;
                fireAnimationClock.restart();
                currentFireFrame = 0;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            fireDirection = 1; // RIGHT
            currentSuctionKey = 'D';
            if (!isFiring)
            {
                isFiring = true;
                fireAnimationClock.restart();
                currentFireFrame = 0;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            fireDirection = 2; // UP
            currentSuctionKey = 'W';
            if (!isFiring)
            {
                isFiring = true;
                fireAnimationClock.restart();
                currentFireFrame = 0;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            fireDirection = 3; // DOWN
            currentSuctionKey = 'S';
            if (!isFiring)
            {
                isFiring = true;
                fireAnimationClock.restart();
                currentFireFrame = 0;
            }
        }
        else
        {
            isFiring = false;
        }

        // Update fire animation
        if (isFiring)
        {
            fireAnimationTime += fireAnimationClock.restart().asSeconds();

            if (fireAnimationTime >= FIRE_FRAME_TIME)
            {
                fireAnimationTime = 0.0f;
                currentFireFrame++;
                if (currentFireFrame >= FIRE_FRAME_COUNT)
                    currentFireFrame = 0;
            }

            float playerVisualX = player_x - cell_size + 25;
            float playerVisualY = player_y;

            sf::Sprite &currentFireSprite = fireSprites[currentFireFrame];
            float spriteWidth = fireTextures[currentFireFrame].getSize().x * 2.0f;
            float spriteHeight = fireTextures[currentFireFrame].getSize().y * 2.0f;
            float horizontalGap = 10.0f;
            float verticalGap = 10.0f;

            switch (fireDirection)
            {
            case 0: // LEFT
            {
                float fireX = playerVisualX - spriteWidth + horizontalGap - 20;
                float fireY = playerVisualY + PlayerHeight / 2 - spriteHeight / 2 + 10 + SUCTION_VERTICAL_OFFSET;
                currentFireSprite.setPosition(fireX, fireY);
                currentFireSprite.setScale(-2.0, 2.0);
                currentFireSprite.setOrigin(fireTextures[currentFireFrame].getSize().x, 0);
                currentFireSprite.setRotation(0);
                break;
            }
            case 1: // RIGHT
            {
                float fireX = playerVisualX + PlayerWidth - horizontalGap + 20;
                float fireY = playerVisualY + PlayerHeight / 2 - spriteHeight / 2 + 10 + SUCTION_VERTICAL_OFFSET;
                currentFireSprite.setPosition(fireX, fireY);
                currentFireSprite.setScale(2.0, 2.0);
                currentFireSprite.setOrigin(0, 0);
                currentFireSprite.setRotation(0);
                break;
            }
            case 2: // UP
            {
                float fireX = playerVisualX + PlayerWidth / 2 - spriteHeight / 2;
                float fireY = playerVisualY - spriteWidth + verticalGap + 50 + SUCTION_VERTICAL_OFFSET;
                currentFireSprite.setPosition(fireX, fireY);
                currentFireSprite.setScale(2.0, 2.0);
                currentFireSprite.setOrigin(0, 0);
                currentFireSprite.setRotation(-90);
                break;
            }
            case 3: // DOWN
            {
                float fireX = playerVisualX + PlayerWidth / 2 - spriteHeight / 2;
                float fireY = playerVisualY + PlayerHeight - verticalGap + 10 + SUCTION_VERTICAL_OFFSET;
                currentFireSprite.setPosition(fireX, fireY);
                currentFireSprite.setScale(2.0, 2.0);
                currentFireSprite.setOrigin(0, fireTextures[currentFireFrame].getSize().y);
                currentFireSprite.setRotation(90);
                break;
            }
            }
        }

        // Update enemies
        for (int i = 0; i < enemyCount; i++)
        {
            if (!enemyAlive[i])
                continue;

            sf::Texture *enemyTex;
            sf::IntRect *frames;
            int framesCount;

            switch (enemyType[i])
            {
            case 0: // Ghost
                enemyTex = &ghostTexture;
                frames = ghostFrames;
                framesCount = 3;
                updateGhost(
                    enemyX[i], enemyY[i], enemySpeed[i], enemyDirection[i],
                    enemyAlive[i], enemyIgnorePlatformCollision[i], enemyMoveTimer[i],
                    enemyDirectionChangeTime[i], enemyVelocityY[i], enemyOnGround[i],
                    enemyActionTimer[i], enemyActionCooldown[i], enemyIsInvisible[i],
                    enemyIsShooting[i], enemyShootTimer[i], enemyAnimationTimer[i],
                    enemyCurrentFrame[i], enemyCurrentTime[i], enemySprites[i],
                    ghostTexture, ghostFrames, 3, deltaTime,
                    currentLevelData, cell_size, screen_x, screen_y, height, width);
                break;

            case 1: // Skeleton
                enemyTex = &skeletonTexture;
                frames = skeletonFrames;
                framesCount = 4;
                updateSkeleton(
                    enemyX[i], enemyY[i], enemySpeed[i], enemyDirection[i],
                    enemyAlive[i], enemyIgnorePlatformCollision[i], enemyMoveTimer[i],
                    enemyDirectionChangeTime[i], enemyVelocityY[i], enemyOnGround[i],
                    enemyActionTimer[i], enemyActionCooldown[i], enemyIsInvisible[i],
                    enemyIsShooting[i], enemyShootTimer[i], enemyAnimationTimer[i],
                    enemyCurrentFrame[i], enemyCurrentTime[i], enemySprites[i],
                    *enemyTex, frames, framesCount, deltaTime,
                    currentLevelData, cell_size, screen_x, screen_y, height, width);
                break;

            case 2: // Invisible Man
                enemyTex = &invisibleTexture;
                frames = invisibleManFrames;
                framesCount = 4;
                updateInvisibleMan(
                    enemyX[i], enemyY[i], enemySpeed[i], enemyDirection[i],
                    enemyAlive[i], enemyIgnorePlatformCollision[i], enemyMoveTimer[i],
                    enemyDirectionChangeTime[i], enemyVelocityY[i], enemyOnGround[i],
                    enemyActionTimer[i], enemyActionCooldown[i], enemyIsInvisible[i],
                    enemyIsShooting[i], enemyShootTimer[i], enemyAnimationTimer[i],
                    enemyCurrentFrame[i], enemyCurrentTime[i], enemySprites[i],
                    *enemyTex, frames, framesCount, deltaTime,
                    currentLevelData, cell_size, screen_x, screen_y, height, width);
                break;

            case 3: // Chelnov
                enemyTex = &chelnovTexture;
                frames = chelnovFrames;
                framesCount = 6;
                updateChelnov(
                    enemyX[i], enemyY[i], enemySpeed[i], enemyDirection[i],
                    enemyAlive[i], enemyIgnorePlatformCollision[i], enemyMoveTimer[i],
                    enemyDirectionChangeTime[i], enemyVelocityY[i], enemyOnGround[i],
                    enemyActionTimer[i], enemyActionCooldown[i], enemyIsInvisible[i],
                    enemyIsShooting[i], enemyShootTimer[i], enemyAnimationTimer[i],
                    enemyCurrentFrame[i], enemyCurrentTime[i], enemySprites[i],
                    *enemyTex, frames, framesCount, deltaTime,
                    currentLevelData, cell_size,
                    projectileX, projectileY, projectileDirection, projectileActive,
                    projectileSpeed, projectileShapes, projectileCount, MAX_PROJECTILES,
                    screen_x, screen_y, height, width);
                break;
            }
        }

        // Spawn enemies for level 2
        if (currentGameState == LEVEL2 && currentEnemyTypeIndex < 4)
        {
            if (enemySpawnClock.getElapsedTime().asSeconds() >= ENEMY_SPAWN_DELAY)
            {
                int enemiesToSpawn = level2SequenceCounts[currentEnemyTypeIndex] - enemiesSpawnedLevel2;
                if (enemiesToSpawn > 3)
                    enemiesToSpawn = 3;

                for (int i = 0; i < enemiesToSpawn; i++)
                {
                    if (enemyCount >= MAX_ENEMIES)
                        break;

                    int enemyTypeValue = level2SequenceTypes[currentEnemyTypeIndex];
                    sf::Texture *enemyTex;
                    sf::IntRect *frames;
                    int framesCount;

                    switch (enemyTypeValue)
                    {
                    case 0:
                        enemyTex = &ghostTexture;
                        frames = ghostFrames;
                        framesCount = 3;
                        break;
                    case 1:
                        enemyTex = &skeletonTexture;
                        frames = skeletonFrames;
                        framesCount = 4;
                        break;
                    case 2:
                        enemyTex = &invisibleTexture;
                        frames = invisibleManFrames;
                        framesCount = 4;
                        break;
                    case 3:
                        enemyTex = &chelnovTexture;
                        frames = chelnovFrames;
                        framesCount = 6;
                        break;
                    default:
                        enemyTex = &ghostTexture;
                        frames = ghostFrames;
                        framesCount = 3;
                    }

                    initEnemy(
                        enemyX[enemyCount], enemyY[enemyCount], enemySpeed[enemyCount], enemyDirection[enemyCount],
                        enemyAlive[enemyCount], enemyIgnorePlatformCollision[enemyCount], enemyMoveTimer[enemyCount],
                        enemyDirectionChangeTime[enemyCount], enemyType[enemyCount], enemyVelocityY[enemyCount],
                        enemyOnGround[enemyCount], enemyActionTimer[enemyCount], enemyActionCooldown[enemyCount],
                        enemyIsInvisible[enemyCount], enemyIsShooting[enemyCount], enemyShootTimer[enemyCount],
                        enemyAnimationTimer[enemyCount], enemyCurrentFrame[enemyCount], enemyCurrentTime[enemyCount],
                        enemySprites[enemyCount], *enemyTex, frames, framesCount,
                        currentLevelData, cell_size, enemyTypeValue,
                        screen_x, screen_y, height, width);

                    enemyCount++;
                    enemiesSpawnedLevel2++;
                }

                enemySpawnClock.restart();

                if (enemiesSpawnedLevel2 >= level2SequenceCounts[currentEnemyTypeIndex])
                {
                    currentEnemyTypeIndex++;
                    enemiesSpawnedLevel2 = 0;
                }
            }
        }

        // Update projectiles
        for (int i = 0; i < projectileCount; i++)
        {
            if (!projectileActive[i])
                continue;

            projectileX[i] += projectileSpeed[i] * projectileDirection[i];

            if (!isInvincible && checkProjectileCollision(projectileX[i], projectileY[i],
                                                          player_x - cell_size + 25, player_y, PlayerWidth, PlayerHeight))
            {
                projectileActive[i] = false;
                playerLives--;
                isInvincible = true;
                invincibilityClock.restart();

                if (playerLives <= 0)
                {
                    currentGameState = GAME_OVER;
                    finalGameTime = gameTimer.getElapsedTime();
                    isTimerRunning = false;
                    lvlMusic.stop();
                }
            }

            if (projectileX[i] < 0 || projectileX[i] > screen_x ||
                projectileY[i] < 0 || projectileY[i] > screen_y)
            {
                projectileActive[i] = false;
            }

            projectileShapes[i].setPosition(projectileX[i], projectileY[i]);
        }

        // Remove inactive projectiles
        int newProjectileCount = 0;
        for (int i = 0; i < projectileCount; i++)
        {
            if (projectileActive[i])
            {
                projectileX[newProjectileCount] = projectileX[i];
                projectileY[newProjectileCount] = projectileY[i];
                projectileDirection[newProjectileCount] = projectileDirection[i];
                projectileActive[newProjectileCount] = projectileActive[i];
                projectileSpeed[newProjectileCount] = projectileSpeed[i];
                projectileShapes[newProjectileCount] = projectileShapes[i];
                newProjectileCount++;
            }
        }
        projectileCount = newProjectileCount;

        // Check enemy collisions
        if (!isInvincible)
        {
            for (int i = 0; i < enemyCount; i++)
            {
                if (enemyAlive[i] && checkEnemyCollision(enemyX[i], enemyY[i],
                                                         player_x - cell_size + 25, player_y, PlayerWidth, PlayerHeight))
                {
                    playerLives--;
                    isInvincible = true;
                    invincibilityClock.restart();

                    if (playerLives <= 0)
                    {
                        currentGameState = GAME_OVER;
                        finalGameTime = gameTimer.getElapsedTime();
                        isTimerRunning = false;
                        lvlMusic.stop();
                    }
                    break;
                }
            }
        }

        // Update invincibility
        if (isInvincible && invincibilityClock.getElapsedTime().asSeconds() >= INVINCIBILITY_TIME)
        {
            isInvincible = false;
        }

        // Handle suction
        suctionTimer += deltaTime;
        if (suctionTimer >= suctionCooldown && currentSuctionKey != ' ')
        {
            bool enemySucked = false;
            for (int i = 0; i < enemyCount; i++)
            {
                if (enemyAlive[i] && checkEnemySuction(enemyX[i], enemyY[i], enemyAlive[i],
                                                       enemyType[i], enemyIsShooting[i], player_x - cell_size + 25, player_y,
                                                       SUCTION_RADIUS, currentSuctionKey, SUCTION_RADIUS, SUCTION_SECTION_ANGLE, SUCTION_VERTICAL_OFFSET))
                {
                    enemyAlive[i] = false;

                    switch (enemyType[i])
                    {
                    case 0:
                        score += 10;
                        break;
                    case 1:
                        score += 20;
                        break;
                    case 2:
                        score += 30;
                        break;
                    case 3:
                        score += 40;
                        break;
                    }

                    suctionTimer = 0.0f;
                    enemySucked = true;
                    break;
                }
            }
        }

        // Apply gravity to player
        player_gravity(currentLevelData, velocityY, onGround, gravity, terminal_Velocity,
                       player_x, player_y, cell_size, PlayerHeight, PlayerWidth, ignorePlatformCollision,
                       isMovingDown, downSpeed, height, width, screen_y);

        // Check proximity damage
        checkEnemyProximityDamage(enemyX, enemyY, enemyAlive, enemyCount,
                                  player_x - cell_size + 25, player_y, PlayerWidth, PlayerHeight,
                                  isInvincible, invincibilityClock, playerLives, currentGameState,
                                  lvlMusic, gameTimer, finalGameTime, isTimerRunning);

        // Check boundaries
        if (player_y < 0)
        {
            player_y = 0;
            velocityY = 0;
            ignorePlatformCollision = false;
        }

        // Check level completion
        if (checkLevelCompletion(enemyAlive, enemyCount, currentLevel,
                                 currentEnemyTypeIndex, level2SequenceTypes, level2SequenceCounts, 4))
        {
            if (currentGameState == LEVEL1)
            {
                currentLevel = 2;
                currentGameState = LEVEL2;
                level1Completed = true;
                currentLevelData = lvl2;
                enemyCount = 0;
                projectileCount = 0;
                player_x = 500;
                player_y = 150;
                velocityY = 0;
                onGround = false;
                ignorePlatformCollision = false;
                currentEnemyTypeIndex = 0;
                enemiesSpawnedLevel2 = 0;
                enemySpawnClock.restart();
            }
            else if (currentGameState == LEVEL2)
            {
                currentGameState = VICTORY;
                finalGameTime = gameTimer.getElapsedTime();
                isTimerRunning = false;
                lvlMusic.stop();
            }
        }

        // Draw everything
        window.clear();

        if (currentGameState == LEVEL1 || currentGameState == LEVEL2)
        {
            display_level(window, currentLevelData, bgTex, bgSprite, blockTexture, blockSprite, cell_size);

            for (int i = 0; i < enemyCount; i++)
            {
                drawEnemy(window, enemySprites[i], enemyX[i], enemyY[i],
                          enemyType[i], enemyIsInvisible[i], enemyAlive[i]);
            }

            for (int i = 0; i < projectileCount; i++)
            {
                if (projectileActive[i])
                {
                    window.draw(projectileShapes[i]);
                }
            }

            PlayerSprite.setPosition(player_x - cell_size + 25, player_y - 40);
            if (isInvincible)
            {
                if (static_cast<int>(invincibilityClock.getElapsedTime().asSeconds() * 10) % 2 == 0)
                {
                    PlayerSprite.setColor(sf::Color(255, 255, 255, 128));
                }
                else
                {
                    PlayerSprite.setColor(sf::Color::White);
                }
            }
            else
            {
                PlayerSprite.setColor(sf::Color::White);
            }
            window.draw(PlayerSprite);
            PlayerSprite.setColor(sf::Color::White);

            if (isFiring)
            {
                window.draw(fireSprites[currentFireFrame]);
            }

            drawUI(window, uiFont, scoreText, playerLives, currentLevel, score,
                   screen_x, isTimerRunning, currentGameState, gameTimer, finalGameTime);
        }

        window.display();
    }

    lvlMusic.stop();
    return 0;
}

// Helper function implementations
float getSlopeHeightAtX(int row, int col, float xInCell, char lvl[][18])
{
    if (row < 0 || row >= 14 || col < 0 || col >= 18)
        return -1;

    char cellType = lvl[row][col];

    if (cellType == 'S')
    {
        float ratio = xInCell / 64.0f;
        return 64.0f * ratio;
    }
    else if (cellType == 's')
    {
        float ratio = xInCell / 64.0f;
        return 64.0f * (1.0f - ratio);
    }
    else if (cellType == '?')
    {
        return 64.0f;
    }

    return -1;
}

bool isOnSlope(float x, float y, char lvl[][18], int cell_size, float &slopeY, char &slopeType)
{
    int col = (int)(x + cell_size - 25) / cell_size;
    int row = (int)(y) / cell_size;

    if (row < 0 || row >= 14 || col < 0 || col >= 18)
        return false;

    slopeType = lvl[row][col];

    if (slopeType == 'S' || slopeType == 's' || slopeType == '?')
    {
        float cellBaseX = col * cell_size - cell_size + 25;
        float xInCell = x - cellBaseX;

        if (xInCell >= -5 && xInCell <= cell_size + 5)
        {
            xInCell = (xInCell < 0) ? 0 : ((xInCell > cell_size) ? cell_size : xInCell);
            float slopeHeight = getSlopeHeightAtX(row, col, xInCell, lvl);

            if (slopeHeight >= 0)
            {
                float cellBottomY = (row + 1) * cell_size;
                slopeY = cellBottomY - slopeHeight;
                return true;
            }
        }
    }

    return false;
}

bool checkCharacterOnSlope(float x, float y, float charWidth, float charHeight,
                           char lvl[][18], int cell_size, float &groundY, char &slopeType)
{
    float checkPoints[5] = {x, x + charWidth * 0.25f, x + charWidth * 0.5f,
                            x + charWidth * 0.75f, x + charWidth};

    bool foundSlope = false;
    groundY = 1000000.0f;
    slopeType = ' ';

    for (int i = 0; i < 5; i++)
    {
        float slopeY;
        char currentSlopeType;
        if (isOnSlope(checkPoints[i], y + charHeight, lvl, cell_size, slopeY, currentSlopeType))
        {
            foundSlope = true;
            if (slopeY < groundY)
            {
                groundY = slopeY;
                slopeType = currentSlopeType;
            }
        }
    }

    return foundSlope;
}

void moveEnemyHorizontally(float &enemyX, float &enemyY, int &enemyDirection, bool enemyOnGround,
                           char lvl[][18], int cell_size, float dx, int screen_x, int screen_y, int height, int width)
{
    if (dx == 0)
        return;

    float newX = enemyX + dx;

    if (newX < 0)
    {
        enemyX = 0;
        enemyDirection = 1; // Change direction
        return;
    }

    if (newX + 80 > screen_x)
    {
        enemyX = screen_x - 80;
        enemyDirection = -1; // Change direction
        return;
    }

    int middleRow = (int)(enemyY + 80 * 0.5) / cell_size;
    int bottomRow = (int)(enemyY + 75) / cell_size;

    middleRow = (middleRow < 0) ? 0 : ((middleRow >= height) ? height - 1 : middleRow);
    bottomRow = (bottomRow < 0) ? 0 : ((bottomRow >= height) ? height - 1 : bottomRow);

    if (dx > 0)
    {
        float visualRightX = newX + 60;
        int rightCol = (int)(visualRightX + cell_size - 25) / cell_size;
        rightCol = (rightCol >= width) ? width - 1 : rightCol;

        bool collision = false;
        for (int row = middleRow; row <= bottomRow && !collision; row++)
        {
            if (rightCol >= 0 && rightCol < width && lvl[row][rightCol] == '#')
            {
                collision = true;
            }
        }

        if (collision)
        {
            enemyX = rightCol * cell_size - cell_size + 25 - 60;
            enemyDirection = -1; // Change direction
            return;
        }
    }
    else
    {
        int leftCol = (int)(newX + cell_size - 100) / cell_size;
        leftCol = (leftCol < 0) ? 0 : leftCol;

        bool collision = false;
        for (int row = middleRow; row <= bottomRow && !collision; row++)
        {
            if (leftCol >= 0 && leftCol < width && lvl[row][leftCol] == '#')
            {
                collision = true;
            }
        }

        if (collision)
        {
            enemyX = (leftCol + 1) * cell_size - cell_size + 25;
            enemyDirection = 1; // Change direction
            return;
        }
    }

    enemyX = newX;

    if (enemyOnGround)
    {
        float checkPoints[3] = {enemyX + 10, enemyX + 40, enemyX + 70};
        float lowestSlopeY = enemyY + 80;
        bool foundSlope = false;
        char foundSlopeType = ' ';

        for (int i = 0; i < 3; i++)
        {
            float slopeY;
            char slopeType;
            if (isOnSlope(checkPoints[i], enemyY + 80, lvl, cell_size, slopeY, slopeType))
            {
                foundSlope = true;
                foundSlopeType = slopeType;
                if (slopeY < lowestSlopeY)
                    lowestSlopeY = slopeY;
            }
        }

        if (foundSlope)
        {
            enemyY = lowestSlopeY - 80;

            float absDx = (dx < 0) ? -dx : dx;
            if (foundSlopeType == 's')
            {
                if (dx > 0)
                    enemyY += absDx * 0.5f;
                else if (dx < 0)
                    enemyY -= absDx * 0.5f;
            }
            else if (foundSlopeType == 'S')
            {
                if (dx > 0)
                    enemyY -= absDx * 0.5f;
                else if (dx < 0)
                    enemyY += absDx * 0.5f;
            }
        }
    }
}

void move_player_horizontally(char lvl[][18], float &player_x, float &player_y,
                              int Pwidth, int Pheight, float dx, int cell_size, bool &onGround,
                              int screen_x, int screen_y, int height, int width)
{
    if (dx == 0)
        return;

    float newX = player_x + dx;
    float effectiveWidth = Pwidth * 0.9f;
    float effectiveHeight = Pheight * 0.5f;
    float widthOffset = (Pwidth - effectiveWidth) / 2.0f;
    float heightOffset = (Pheight - effectiveHeight) / 2.0f;

    if (dx > 0)
    {
        float rightEdge = newX + effectiveWidth + widthOffset;
        if (rightEdge > screen_x)
        {
            player_x = screen_x - Pwidth;
            return;
        }
    }
    else if (dx < 0)
    {
        float leftEdge = newX + widthOffset;
        if (leftEdge < 0)
        {
            player_x = 0;
            return;
        }
    }

    float checkHeightPoints[3] = {
        player_y + heightOffset + 5.0f,
        player_y + Pheight / 2,
        player_y + Pheight - heightOffset - 5.0f};

    bool wallCollision = false;
    int collisionCol = 0; // Store the collision column

    if (dx > 0)
    {
        float rightEdge = newX + effectiveWidth + widthOffset;
        collisionCol = (int)(rightEdge + cell_size - 25) / cell_size;

        for (int h = 0; h < 3 && !wallCollision; h++)
        {
            int row = (int)(checkHeightPoints[h]) / cell_size;
            if (collisionCol >= 0 && collisionCol < width && row >= 0 && row < height)
            {
                if (lvl[row][collisionCol] == '#')
                    wallCollision = true;
            }
        }

        if (wallCollision)
        {
            player_x = collisionCol * cell_size - cell_size + 25 - effectiveWidth - widthOffset - (-2.0f);
            return;
        }
    }
    else
    {
        float leftEdge = newX + widthOffset - 30;

        for (int h = 0; h < 3 && !wallCollision; h++)
        {
            collisionCol = (int)(leftEdge - (-2.0f)) / cell_size;
            int row = (int)(checkHeightPoints[h]) / cell_size;
            if (collisionCol >= 0 && collisionCol < width && row >= 0 && row < height)
            {
                if (lvl[row][collisionCol] == '#')
                    wallCollision = true;
            }
        }

        if (wallCollision)
        {
            player_x = (collisionCol + 1) * cell_size - cell_size + 25 + 30;
            return;
        }
    }

    player_x = newX;
    if (player_x < 0)
        player_x = 0;
    if (player_x + Pwidth > screen_x)
        player_x = screen_x - Pwidth;

    float slopeGroundY;
    char slopeType;
    bool onSlopeNow = checkCharacterOnSlope(player_x + widthOffset, player_y + heightOffset,
                                            effectiveWidth, effectiveHeight,
                                            lvl, cell_size, slopeGroundY, slopeType);

    if (onSlopeNow)
    {
        float playerBottom = player_y + Pheight;
        float slopeSurfaceY = slopeGroundY - 5.0f;

        if (playerBottom >= slopeSurfaceY - 10 && playerBottom <= slopeSurfaceY + 10)
        {
            player_y = slopeSurfaceY - Pheight;

            if (dx != 0)
            {
                float slopeRatio = 0.5f;
                float absDx = (dx < 0) ? -dx : dx;
                if (slopeType == 's')
                {
                    if (dx > 0)
                        player_y += absDx * slopeRatio;
                    else if (dx < 0)
                        player_y -= absDx * slopeRatio;
                }
                else if (slopeType == 'S')
                {
                    if (dx > 0)
                        player_y -= absDx * slopeRatio;
                    else if (dx < 0)
                        player_y += absDx * slopeRatio;
                }
            }

            onGround = true;
        }
    }
}

bool checkSlopeCollision(float x, float y, char lvl[][18], int cell_size, float &groundY)
{
    float slopeY;
    char slopeType;

    if (isOnSlope(x, y, lvl, cell_size, slopeY, slopeType))
    {
        if (y >= slopeY - 5)
        {
            groundY = slopeY;
            return true;
        }
    }
    return false;
}

void player_gravity(char lvl[][18], float &velocityY, bool &onGround,
                    const float &gravity, float &terminal_Velocity,
                    float &player_x, float &player_y,
                    const int cell_size, int &Pheight, int &Pwidth, bool &ignorePlatformCollision,
                    bool isMovingDown, float downSpeed, int height, int width, int screen_y)
{
    float effectiveWidth = Pwidth * 0.9f;
    float effectiveHeight = Pheight * 0.5f;
    float widthOffset = (Pwidth - effectiveWidth) / 2.0f;
    float heightOffset = (Pheight - effectiveHeight) / 2.0f;

    if (isMovingDown)
    {
        velocityY = (velocityY + gravity < 5.0f) ? velocityY + gravity : 5.0f;
    }
    else if (!onGround && !ignorePlatformCollision)
    {
        velocityY += gravity;
        if (velocityY > terminal_Velocity)
            velocityY = terminal_Velocity;
    }

    float newY = player_y + velocityY;

    float slopeGroundY;
    char slopeType;
    bool onAnySlope = checkCharacterOnSlope(player_x + widthOffset, newY + heightOffset,
                                            effectiveWidth, effectiveHeight,
                                            lvl, cell_size, slopeGroundY, slopeType);

    if (onAnySlope && velocityY >= 0 && !ignorePlatformCollision)
    {
        float playerBottom = newY + Pheight;
        float verticalDiff = playerBottom - (slopeGroundY - 5.0f);

        if (verticalDiff >= -10)
        {
            player_y = (slopeGroundY - 5.0f) - Pheight - 1;
            velocityY = 0;
            onGround = true;
            ignorePlatformCollision = false;
            return;
        }
    }

    if (velocityY > 0 && !ignorePlatformCollision)
    {
        int bottomRow = (int)(newY + Pheight) / cell_size;
        bool collisionBottom = false;

        float bottomCheckPoints[] = {
            player_x + widthOffset,
            player_x + widthOffset + effectiveWidth * 0.25f,
            player_x + widthOffset + effectiveWidth * 0.5f,
            player_x + widthOffset + effectiveWidth * 0.75f,
            player_x + widthOffset + effectiveWidth};

        for (int i = 0; i < 5; i++)
        {
            int col = (int)(bottomCheckPoints[i] + cell_size - 25) / cell_size;
            if (col >= 0 && col < width && bottomRow >= 0 && bottomRow < height)
            {
                if (lvl[bottomRow][col] == '#')
                {
                    collisionBottom = true;
                    break;
                }
            }
        }

        if (collisionBottom)
        {
            player_y = bottomRow * cell_size - Pheight;
            velocityY = 0;
            onGround = true;
            ignorePlatformCollision = false;
            return;
        }
    }

    player_y = newY;

    if (ignorePlatformCollision && velocityY > 0)
    {
        int currentRow = (int)(player_y + Pheight) / cell_size;
        int previousRow = (int)(player_y - velocityY + Pheight) / cell_size;

        if (currentRow > previousRow)
        {
            bool solidGroundAtCurrentRow = false;
            float checkPoints[] = {
                player_x + widthOffset,
                player_x + widthOffset + effectiveWidth * 0.25f,
                player_x + widthOffset + effectiveWidth * 0.5f,
                player_x + widthOffset + effectiveWidth * 0.75f,
                player_x + widthOffset + effectiveWidth};

            for (int i = 0; i < 5; i++)
            {
                int col = (int)(checkPoints[i] + cell_size - 25) / cell_size;
                if (col >= 0 && col < width && currentRow >= 0 && currentRow < height)
                {
                    if (lvl[currentRow][col] == '#')
                    {
                        solidGroundAtCurrentRow = true;
                        break;
                    }
                }
            }

            if (solidGroundAtCurrentRow)
            {
                player_y = currentRow * cell_size - Pheight;
                velocityY = 0;
                onGround = true;
                ignorePlatformCollision = false;
                return;
            }
        }
    }

    if (velocityY == 0 && !ignorePlatformCollision)
    {
        bool foundGround = false;

        if (onAnySlope)
        {
            float playerBottom = player_y + Pheight;
            float verticalDiff = playerBottom - (slopeGroundY - 5.0f);

            if (verticalDiff >= -3 && verticalDiff <= 3)
            {
                foundGround = true;
                if ((verticalDiff < 0 ? -verticalDiff : verticalDiff) > 1)
                {
                    player_y = (slopeGroundY - 5.0f) - Pheight - 1;
                }
            }
        }

        if (!foundGround)
        {
            int checkRow = (int)(player_y + Pheight + 1.0f) / cell_size;
            float checkPoints[5] = {
                player_x + widthOffset,
                player_x + widthOffset + effectiveWidth * 0.25f,
                player_x + widthOffset + effectiveWidth * 0.5f,
                player_x + widthOffset + effectiveWidth * 0.75f,
                player_x + widthOffset + effectiveWidth};

            for (int i = 0; i < 5; i++)
            {
                int col = (int)(checkPoints[i] + cell_size - 25) / cell_size;
                if (col >= 0 && col < width && checkRow >= 0 && checkRow < height)
                {
                    if (lvl[checkRow][col] == '#')
                    {
                        foundGround = true;
                        break;
                    }
                }
            }
        }

        onGround = foundGround;
    }
    else if (velocityY != 0)
    {
        onGround = false;
    }

    if (player_y < 0)
    {
        player_y = 0;
        velocityY = 0;
        onGround = false;
        ignorePlatformCollision = false;
    }

    if (player_y + Pheight > (height - 1) * cell_size)
    {
        player_y = (height - 2) * cell_size - Pheight;
        velocityY = 0;
        onGround = true;
        ignorePlatformCollision = false;
    }

    if (player_y + Pheight > screen_y)
    {
        player_y = screen_y - Pheight;
        velocityY = 0;
        onGround = true;
        ignorePlatformCollision = false;
    }
}

void applyEnemyGravity(float &enemyX, float &enemyY, float &enemyVelocityY, bool &enemyOnGround,
                       bool &enemyIgnorePlatformCollision, char lvl[][18], int cell_size,
                       float gravity, float terminalVelocity, int height, int width, int screen_y)
{
    if (!enemyOnGround)
    {
        enemyVelocityY += gravity;
        if (enemyVelocityY > terminalVelocity)
            enemyVelocityY = terminalVelocity;
    }

    float newY = enemyY + enemyVelocityY;
    float groundY = enemyY + 80;
    bool foundGround = false;
    char slopeType;

    float checkPoints[3] = {enemyX + 10, enemyX + 40, enemyX + 70};

    for (int i = 0; i < 3; i++)
    {
        float slopeY;
        char currentSlopeType;
        if (isOnSlope(checkPoints[i], newY + 80, lvl, cell_size, slopeY, currentSlopeType))
        {
            foundGround = true;
            slopeType = currentSlopeType;
            if (slopeY < groundY)
                groundY = slopeY;
        }
    }

    if (foundGround && enemyVelocityY >= 0 && !enemyIgnorePlatformCollision)
    {
        if (newY + 80 >= groundY - 10)
        {
            enemyY = groundY - 80;
            enemyVelocityY = 0;
            enemyOnGround = true;
            enemyIgnorePlatformCollision = false;
            return;
        }
    }

    if (enemyVelocityY > 0 && !enemyIgnorePlatformCollision)
    {
        int bottomRow = (int)(newY + 80) / cell_size;
        bool collisionBottom = false;

        float bottomCheckPoints[3] = {enemyX + 10, enemyX + 40, enemyX + 70};

        for (int i = 0; i < 3; i++)
        {
            int col = (int)(bottomCheckPoints[i] + cell_size - 25) / cell_size;
            if (col >= 0 && col < width && bottomRow >= 0 && bottomRow < height)
            {
                if (lvl[bottomRow][col] == '#')
                {
                    collisionBottom = true;
                    break;
                }
            }
        }

        if (collisionBottom)
        {
            enemyY = bottomRow * cell_size - 80;
            enemyVelocityY = 0;
            enemyOnGround = true;
            enemyIgnorePlatformCollision = false;
            return;
        }
    }

    enemyY = newY;

    if (enemyVelocityY == 0 && !enemyIgnorePlatformCollision)
    {
        enemyOnGround = false;
        int bottomRow = (int)(enemyY + 81) / cell_size;
        float groundCheckPoints[3] = {enemyX + 10, enemyX + 40, enemyX + 70};

        for (int i = 0; i < 3; i++)
        {
            int col = (int)(groundCheckPoints[i] + cell_size - 25) / cell_size;
            if (col >= 0 && col < width && bottomRow >= 0 && bottomRow < height)
            {
                if (lvl[bottomRow][col] == '#')
                {
                    enemyOnGround = true;
                    break;
                }
            }
        }

        if (!enemyOnGround)
        {
            float dummyY;
            char dummyChar;
            for (int i = 0; i < 3; i++)
            {
                if (isOnSlope(groundCheckPoints[i], enemyY + 81, lvl, cell_size, dummyY, dummyChar))
                {
                    enemyOnGround = true;
                    break;
                }
            }
        }
    }

    if (enemyIgnorePlatformCollision && enemyVelocityY >= 0)
    {
        int bottomRow = (int)(enemyY + 80) / cell_size;
        bool stillInPlatform = false;
        float platformCheckPoints[3] = {enemyX + 10, enemyX + 40, enemyX + 70};

        for (int i = 0; i < 3; i++)
        {
            int col = (int)(platformCheckPoints[i] + cell_size - 25) / cell_size;
            if (col >= 0 && col < width && bottomRow >= 0 && bottomRow < height)
            {
                if (lvl[bottomRow][col] == '#')
                {
                    stillInPlatform = true;
                    break;
                }
            }
        }

        if (!stillInPlatform)
            enemyIgnorePlatformCollision = false;
    }

    if (enemyY < 0)
    {
        enemyY = 0;
        enemyVelocityY = 0;
        enemyIgnorePlatformCollision = false;
    }

    if (enemyY + 80 > (height - 1) * cell_size)
    {
        enemyY = (height - 2) * cell_size - 80;
        enemyVelocityY = 0;
        enemyOnGround = true;
        enemyIgnorePlatformCollision = false;
        return;
    }

    if (enemyY + 80 > screen_y)
    {
        enemyY = screen_y - 80;
        enemyVelocityY = 0;
        enemyOnGround = true;
        enemyIgnorePlatformCollision = false;
    }
}

void display_level(sf::RenderWindow &window, char lvl[][18], sf::Texture &bgTex, sf::Sprite &bgSprite,
                   sf::Texture &blockTexture, sf::Sprite &blockSprite, const int cell_size)
{
    window.draw(bgSprite);
    for (int i = 0; i < 14; i++)
    {
        for (int j = 0; j < 18; j++)
        {
            if (lvl[i][j] == '#')
            {
                blockSprite.setPosition(j * cell_size - cell_size + 25, i * cell_size);
                window.draw(blockSprite);
            }
            else if (lvl[i][j] == 'S')
            {
                sf::ConvexShape triangle;
                triangle.setPointCount(3);
                float baseX = (j + 1) * cell_size - cell_size + 25;
                float baseY = (i + 1) * cell_size;
                triangle.setPoint(0, sf::Vector2f(0, 0));
                triangle.setPoint(1, sf::Vector2f(-cell_size, -cell_size));
                triangle.setPoint(2, sf::Vector2f(0, -cell_size));
                triangle.setPosition(baseX, baseY);
                triangle.setTexture(&blockTexture);
                triangle.setTextureRect(sf::IntRect(0, 0, cell_size, cell_size));
                triangle.setOutlineColor(sf::Color(101, 51, 15));
                triangle.setOutlineThickness(1);
                window.draw(triangle);
            }
            else if (lvl[i][j] == 's')
            {
                sf::ConvexShape triangle;
                triangle.setPointCount(3);
                float baseX = j * cell_size - cell_size + 25;
                float baseY = i * cell_size;
                triangle.setPoint(0, sf::Vector2f(0, 0));
                triangle.setPoint(1, sf::Vector2f(cell_size, cell_size));
                triangle.setPoint(2, sf::Vector2f(0, cell_size));
                triangle.setPosition(baseX, baseY);
                triangle.setTexture(&blockTexture);
                triangle.setTextureRect(sf::IntRect(0, 0, cell_size, cell_size));
                triangle.setOutlineColor(sf::Color(101, 51, 15));
                triangle.setOutlineThickness(1);
                window.draw(triangle);
            }
            else if (lvl[i][j] == '?')
            {
                blockSprite.setPosition(j * cell_size - cell_size + 25, i * cell_size);
                window.draw(blockSprite);
            }
        }
    }
}

void getRandomBlockPosition(char lvl[][18], int cell_size, int enemyWidth, int enemyHeight,
                            int screen_x, int height, int width, float &outX, float &outY)
{
    // Count valid positions
    int validCount = 0;
    float tempPositions[100][2]; // Store up to 100 positions

    for (int i = 4; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            if (lvl[i][j] == '#')
            {
                float blockTop = i * cell_size;
                float blockLeft = j * cell_size - cell_size + 25;

                float enemyY = blockTop - enemyHeight;
                float enemyX = blockLeft + 5;

                if (enemyY >= 0 && enemyX >= 0 && enemyX + enemyWidth <= screen_x)
                {
                    bool hasSpace = true;
                    if (i > 0)
                    {
                        if (lvl[i - 1][j] == '#')
                        {
                            hasSpace = false;
                        }
                    }

                    if (hasSpace && validCount < 100)
                    {
                        tempPositions[validCount][0] = enemyX;
                        tempPositions[validCount][1] = enemyY;
                        validCount++;
                    }
                }
            }
        }
    }

    if (validCount > 0)
    {
        int randomIndex = rand() % validCount;
        outX = tempPositions[randomIndex][0];
        outY = tempPositions[randomIndex][1];
    }
    else
    {
        outX = 200 + (rand() % 600);
        outY = 300;
    }
}

void updateGhost(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection,
                 bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer,
                 float &enemyDirectionChangeTime, float &enemyVelocityY, bool &enemyOnGround,
                 float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible,
                 bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer,
                 int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite,
                 sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount,
                 float deltaTime, char lvl[][18], int cell_size, int screen_x, int screen_y,
                 int height, int width)
{
    if (!enemyAlive)
        return;

    enemyMoveTimer += deltaTime;
    enemyCurrentTime += deltaTime;

    applyEnemyGravity(enemyX, enemyY, enemyVelocityY, enemyOnGround, enemyIgnorePlatformCollision,
                      lvl, cell_size, 1.0f, 20.0f, height, width, screen_y);

    bool shouldChangeDirection = false;

    if (enemyOnGround && enemyDirection != 0)
    {
        float checkDistance = 10.0f;
        float checkX = enemyX + 10 + (enemyDirection * checkDistance);
        float checkAheadPoints[3] = {checkX - 20, checkX, checkX + 40};
        bool groundAhead = false;

        for (int i = 0; i < 3; i++)
        {
            int col = (int)(checkAheadPoints[i] + cell_size) / cell_size;
            int rowBelow = (int)(enemyY + 40) / cell_size + 1;

            if (col >= 0 && col < width && rowBelow >= 0 && rowBelow < height)
            {
                if (lvl[rowBelow][col] == '#')
                    groundAhead = true;

                float dummyY;
                char dummyChar;
                if (isOnSlope(checkAheadPoints[i], enemyY + 81 + cell_size, lvl, cell_size, dummyY, dummyChar))
                {
                    groundAhead = true;
                }
            }
        }

        if (!groundAhead)
            shouldChangeDirection = true;
    }

    if (enemyMoveTimer >= enemyDirectionChangeTime || shouldChangeDirection)
    {
        int action = rand() % 100;

        if (shouldChangeDirection)
        {
            enemyDirection = -enemyDirection;
            enemyDirectionChangeTime = 1.0f + (rand() % 100) / 100.0f;
        }
        else if (action < 20)
        {
            enemyDirection = 0;
            enemyDirectionChangeTime = 0.3f + (rand() % 100) / 100.0f;
        }
        else
        {
            enemyDirection = (rand() % 2 == 0) ? -1 : 1;
            enemyDirectionChangeTime = 1.0f + (rand() % 100) / 100.0f;
        }

        enemyMoveTimer = 0.0f;
    }

    if (enemyDirection != 0 && enemyOnGround)
    {
        // FIXED: Pass enemyDirection by reference
        moveEnemyHorizontally(enemyX, enemyY, enemyDirection, enemyOnGround, lvl, cell_size,
                              enemySpeed * enemyDirection * deltaTime * 60,
                              screen_x, screen_y, height, width);
    }

    // Update animation
    if (enemyCurrentTime >= 0.15f)
    {
        enemyCurrentTime -= 0.15f;
        enemyCurrentFrame++;
        if (enemyCurrentFrame >= enemyFramesCount)
            enemyCurrentFrame = 0;
        enemySprite.setTextureRect(enemyFrames[enemyCurrentFrame]);
    }

    if (enemyDirection > 0)
    {
        enemySprite.setScale(2.0f, 2.0f);
        enemySprite.setOrigin(0, 0);
    }
    else if (enemyDirection < 0)
    {
        enemySprite.setScale(-2.0f, 2.0f);
        if (enemyCurrentFrame < enemyFramesCount)
            enemySprite.setOrigin(enemyFrames[enemyCurrentFrame].width, 0);
    }

    enemySprite.setPosition(enemyX, enemyY);
}

void updateSkeleton(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection,
                    bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer,
                    float &enemyDirectionChangeTime, float &enemyVelocityY, bool &enemyOnGround,
                    float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible,
                    bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer,
                    int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite,
                    sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount,
                    float deltaTime, char lvl[][18], int cell_size, int screen_x, int screen_y,
                    int height, int width)
{
    if (!enemyAlive)
        return;

    enemyMoveTimer += deltaTime;
    enemyActionTimer += deltaTime;
    enemyCurrentTime += deltaTime;

    applyEnemyGravity(enemyX, enemyY, enemyVelocityY, enemyOnGround, enemyIgnorePlatformCollision,
                      lvl, cell_size, 1.0f, 20.0f, height, width, screen_y);

    if (enemyMoveTimer >= enemyDirectionChangeTime)
    {
        enemyDirection = (rand() % 2 == 0) ? -1 : 1;
        enemyMoveTimer = 0.0f;
        enemyDirectionChangeTime = 0.8f + (rand() % 120) / 100.0f;
    }

    if (enemyActionTimer >= enemyActionCooldown && enemyOnGround)
    {
        int decision = rand() % 100;
        int currentRow = (int)(enemyY + 80) / cell_size;

        if (decision < 50)
        {
            enemyVelocityY = -20.0f;
            enemyOnGround = false;
        }
        else if (decision < 70 && currentRow < height - 1)
        {
            enemyIgnorePlatformCollision = true;
            enemyVelocityY = 1.0f;
            enemyOnGround = false;
        }

        enemyActionTimer = 0.0f;
        enemyActionCooldown = 1.5f + (rand() % 150) / 100.0f;
    }

    if (enemyDirection != 0 && enemyOnGround && enemyVelocityY == 0)
    {
        // FIXED: Pass enemyDirection by reference
        moveEnemyHorizontally(enemyX, enemyY, enemyDirection, enemyOnGround, lvl, cell_size,
                              enemySpeed * enemyDirection * deltaTime * 60,
                              screen_x, screen_y, height, width);
    }

    // Update animation
    if (enemyCurrentTime >= 0.12f)
    {
        enemyCurrentTime -= 0.12f;
        enemyCurrentFrame++;
        if (enemyCurrentFrame >= enemyFramesCount)
            enemyCurrentFrame = 0;
        enemySprite.setTextureRect(enemyFrames[enemyCurrentFrame]);
    }

    if (enemyDirection > 0)
    {
        enemySprite.setScale(2.0f, 2.0f);
        enemySprite.setOrigin(0, 0);
    }
    else if (enemyDirection < 0)
    {
        enemySprite.setScale(-2.0f, 2.0f);
        enemySprite.setOrigin(enemyFrames[enemyCurrentFrame].width, 0);
    }

    enemySprite.setPosition(enemyX, enemyY);
}

void initEnemy(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection,
               bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer,
               float &enemyDirectionChangeTime, int &enemyType, float &enemyVelocityY,
               bool &enemyOnGround, float &enemyActionTimer, float &enemyActionCooldown,
               bool &enemyIsInvisible, bool &enemyIsShooting, float &enemyShootTimer,
               float &enemyAnimationTimer, int &enemyCurrentFrame, float &enemyCurrentTime,
               sf::Sprite &enemySprite, sf::Texture &enemyTexture, sf::IntRect *enemyFrames,
               int enemyFramesCount, char lvl[][18], int cell_size, int enemyTypeValue,
               int screen_x, int screen_y, int height, int width)
{

    getRandomBlockPosition(lvl, cell_size, 80, 80, screen_x, height, width, enemyX, enemyY);

    enemyType = enemyTypeValue;
    enemyAlive = true;

    enemyMoveTimer = 0.0f;
    enemyVelocityY = 0.0f;
    enemyOnGround = true;
    enemyIgnorePlatformCollision = false;
    enemyActionTimer = 0.0f;
    enemyIsInvisible = false;
    enemyIsShooting = false;
    enemyShootTimer = 0.0f;
    enemyAnimationTimer = 0.0f;
    enemyCurrentFrame = 0;
    enemyCurrentTime = 0.0f;

    enemySprite.setTexture(enemyTexture);

    switch (enemyTypeValue)
    {
    case 0: // Ghost
        enemySpeed = 1.5f;
        enemyDirection = (rand() % 2 == 0) ? -1 : 1;
        enemyDirectionChangeTime = 0.5f + (rand() % 100) / 100.0f;
        enemyActionCooldown = 1.0f + (rand() % 100) / 100.0f;
        enemySprite.setScale(2.0f, 2.0f);
        break;

    case 1: // Skeleton
        enemySpeed = 2.5f;
        enemyDirection = (rand() % 2 == 0) ? -1 : 1;
        enemyDirectionChangeTime = 0.5f + (rand() % 100) / 100.0f;
        enemyActionCooldown = 1.0f + (rand() % 100) / 100.0f;
        enemyY -= 15;
        enemySprite.setScale(2.0f, 2.0f);
        break;

    case 2: // Invisible Man
        enemySpeed = 2.0f;
        enemyDirection = (rand() % 2 == 0) ? -1 : 1;
        enemyDirectionChangeTime = 0.6f + (rand() % 90) / 100.0f;
        enemyActionCooldown = 2.0f + (rand() % 200) / 100.0f;
        enemySprite.setScale(2.0f, 2.0f);
        break;

    case 3: // Chelnov
        enemySpeed = 2.0f;
        enemyDirection = (rand() % 2 == 0) ? -1 : 1;
        enemyDirectionChangeTime = 1.0f + (rand() % 100) / 100.0f;
        enemyActionCooldown = 1.5f + (rand() % 150) / 100.0f;
        enemySprite.setScale(2.0f, 2.0f);
        break;
    }

    if (enemyFramesCount > 0)
    {
        enemySprite.setTextureRect(enemyFrames[0]);
    }

    enemySprite.setPosition(enemyX, enemyY);

    if (enemyDirection > 0)
    {
        enemySprite.setScale(2.0f, 2.0f);
        enemySprite.setOrigin(0, 0);
    }
    else
    {
        enemySprite.setScale(-2.0f, 2.0f);
        if (enemyFramesCount > 0)
        {
            enemySprite.setOrigin(enemyFrames[0].width, 0);
        }
    }
}

void checkEnemyProximityDamage(float *enemyX, float *enemyY, bool *enemyAlive, int enemyCount,
                               float playerX, float playerY, int playerWidth, int playerHeight,
                               bool &isInvincible, sf::Clock &invincibilityClock, int &playerLives,
                               int &currentGameState, sf::Music &lvlMusic, sf::Clock &gameTimer,
                               sf::Time &finalGameTime, bool &isTimerRunning)
{
    if (isInvincible)
        return;

    for (int i = 0; i < enemyCount; i++)
    {
        if (enemyAlive[i])
        {
            float enemyCenterX = enemyX[i] + 40;
            float enemyCenterY = enemyY[i] + 40;
            float playerCenterX = playerX + playerWidth / 2;
            float playerCenterY = playerY + playerHeight / 2;

            float dx = enemyCenterX - playerCenterX;
            float dy = enemyCenterY - playerCenterY;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 30.0f)
            {
                playerLives--;
                isInvincible = true;
                invincibilityClock.restart();

                if (playerLives <= 0)
                {
                    currentGameState = 3; // GAME_OVER
                    finalGameTime = gameTimer.getElapsedTime();
                    isTimerRunning = false;
                    lvlMusic.stop();
                }
                break;
            }
        }
    }
}

void resetGame(int &score, int &playerLives, int &currentLevel, bool &level1Completed,
               int &enemiesSpawnedLevel2, int &currentEnemyTypeIndex, sf::Clock &gameTimer,
               bool &isTimerRunning, int &currentGameState)
{
    score = 0;
    playerLives = 5;
    currentLevel = 1;
    level1Completed = false;
    enemiesSpawnedLevel2 = 0;
    currentEnemyTypeIndex = 0;
    gameTimer.restart();
    isTimerRunning = false;
    currentGameState = 0; // INTRO
}

void updateInvisibleMan(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection,
                        bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer,
                        float &enemyDirectionChangeTime, float &enemyVelocityY, bool &enemyOnGround,
                        float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible,
                        bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer,
                        int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite,
                        sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount,
                        float deltaTime, char lvl[][18], int cell_size, int screen_x, int screen_y,
                        int height, int width)
{
    if (!enemyAlive)
        return;

    enemyMoveTimer += deltaTime;
    enemyActionTimer += deltaTime;
    enemyCurrentTime += deltaTime;

    if (enemyActionTimer >= enemyActionCooldown)
    {
        int action = rand() % 100;
        if (action < 60)
        {
            enemyIsInvisible = !enemyIsInvisible;

            if (enemyIsInvisible)
            {
                enemyX = 200 + (rand() % 600);
                enemyY = 300;
                enemyVelocityY = 0;
                enemyOnGround = false;
            }
        }
        enemyActionTimer = 0.0f;
        enemyActionCooldown = 2.5f + (rand() % 250) / 100.0f;
    }

    if (enemyOnGround && enemyMoveTimer >= enemyDirectionChangeTime)
    {
        enemyDirection = (rand() % 2 == 0) ? -1 : 1;
        enemyMoveTimer = 0.0f;
        enemyDirectionChangeTime = 1.0f + (rand() % 150) / 100.0f;
    }

    applyEnemyGravity(enemyX, enemyY, enemyVelocityY, enemyOnGround, enemyIgnorePlatformCollision,
                      lvl, cell_size, 1.0f, 20.0f, height, width, screen_y);

    if (enemyDirection != 0 && enemyOnGround && enemyVelocityY == 0)
    {
        float moveSpeed = enemyIsInvisible ? enemySpeed * 0.5f : enemySpeed;
        moveEnemyHorizontally(enemyX, enemyY, enemyDirection, enemyOnGround, lvl, cell_size,
                              enemySpeed * enemyDirection, screen_x, screen_y, height, width);
    }

    // Update animation
    if (enemyCurrentTime >= 0.1f)
    {
        enemyCurrentTime -= 0.1f;
        enemyCurrentFrame++;
        if (enemyCurrentFrame >= enemyFramesCount)
            enemyCurrentFrame = 0;
        enemySprite.setTextureRect(enemyFrames[enemyCurrentFrame]);
    }

    if (enemyDirection > 0)
    {
        enemySprite.setScale(2.0f, 2.0f);
        enemySprite.setOrigin(0, 0);
    }
    else if (enemyDirection < 0)
    {
        enemySprite.setScale(-2.0f, 2.0f);
        enemySprite.setOrigin(enemyFrames[enemyCurrentFrame].width, 0);
    }

    enemySprite.setPosition(enemyX, enemyY);
}

void updateChelnov(float &enemyX, float &enemyY, float &enemySpeed, int &enemyDirection,
                   bool &enemyAlive, bool &enemyIgnorePlatformCollision, float &enemyMoveTimer,
                   float &enemyDirectionChangeTime, float &enemyVelocityY, bool &enemyOnGround,
                   float &enemyActionTimer, float &enemyActionCooldown, bool &enemyIsInvisible,
                   bool &enemyIsShooting, float &enemyShootTimer, float &enemyAnimationTimer,
                   int &enemyCurrentFrame, float &enemyCurrentTime, sf::Sprite &enemySprite,
                   sf::Texture &enemyTexture, sf::IntRect *enemyFrames, int enemyFramesCount,
                   float deltaTime, char lvl[][18], int cell_size,
                   float *projectileX, float *projectileY, int *projectileDirection,
                   bool *projectileActive, float *projectileSpeed, sf::RectangleShape *projectileShapes,
                   int &projectileCount, int MAX_PROJECTILES, int screen_x, int screen_y,
                   int height, int width)
{
    if (!enemyAlive)
        return;

    enemyMoveTimer += deltaTime;
    enemyShootTimer += deltaTime;
    enemyActionTimer += deltaTime;
    enemyCurrentTime += deltaTime;

    if (enemyShootTimer >= 4.0f && !enemyIsShooting)
    {
        enemyIsShooting = true;
        enemyShootTimer = 0.0f;

        if (projectileCount < MAX_PROJECTILES)
        {
            projectileX[projectileCount] = enemyX + 40;
            projectileY[projectileCount] = enemyY + 30;
            projectileDirection[projectileCount] = enemyDirection;
            projectileActive[projectileCount] = true;
            projectileSpeed[projectileCount] = 8.0f;
            projectileShapes[projectileCount].setSize(sf::Vector2f(10, 10));
            projectileShapes[projectileCount].setFillColor(sf::Color::Yellow);
            projectileCount++;
        }
    }

    if (enemyIsShooting && enemyShootTimer >= 1.0f)
    {
        enemyIsShooting = false;
        enemyShootTimer = 0.0f;
    }

    int currentRow = (int)(enemyY + 80) / cell_size;

    if (enemyActionTimer >= enemyActionCooldown && enemyOnGround)
    {
        int jumpType = rand() % 100;

        if (jumpType < 30)
        {
            enemyVelocityY = -20.0f;
            enemyOnGround = false;
        }
        else if (jumpType < 45 && currentRow < height - 1)
        {
            enemyIgnorePlatformCollision = true;
            enemyVelocityY = 1.0f;
            enemyOnGround = false;
        }

        enemyActionTimer = 0.0f;
        enemyActionCooldown = 2.5f + (rand() % 200) / 100.0f;
    }

    if (enemyMoveTimer >= enemyDirectionChangeTime)
    {
        enemyDirection = (rand() % 2 == 0) ? -1 : 1;
        enemyMoveTimer = 0.0f;
        enemyDirectionChangeTime = 2.0f + (rand() % 200) / 100.0f;
    }

    applyEnemyGravity(enemyX, enemyY, enemyVelocityY, enemyOnGround, enemyIgnorePlatformCollision,
                      lvl, cell_size, 1.0f, 20.0f, height, width, screen_y);

    if (enemyDirection != 0 && !enemyIsShooting && enemyOnGround && enemyVelocityY == 0)
    {
        moveEnemyHorizontally(enemyX, enemyY, enemyDirection, enemyOnGround, lvl, cell_size,
                              enemySpeed * enemyDirection, screen_x, screen_y, height, width);
    }

    // Update animation
    if (enemyCurrentTime >= 0.18f)
    {
        enemyCurrentTime -= 0.18f;
        enemyCurrentFrame++;
        if (enemyCurrentFrame >= enemyFramesCount)
            enemyCurrentFrame = 0;
        enemySprite.setTextureRect(enemyFrames[enemyCurrentFrame]);
    }

    if (enemyDirection > 0)
    {
        enemySprite.setScale(2.0f, 2.0f);
        enemySprite.setOrigin(0, 0);
    }
    else if (enemyDirection < 0)
    {
        enemySprite.setScale(-2.0f, 2.0f);
        enemySprite.setOrigin(enemyFrames[enemyCurrentFrame].width, 0);
    }

    enemySprite.setPosition(enemyX, enemyY);
}

void drawEnemy(sf::RenderWindow &window, sf::Sprite &enemySprite, float drawX, float drawY,
               int enemyType, bool enemyIsInvisible, bool enemyAlive)
{
    if (enemyAlive)
    {
        enemySprite.setPosition(drawX, drawY);

        if (enemyType == 2 && enemyIsInvisible)
        {
            sf::Color color = enemySprite.getColor();
            color.a = 80;
            enemySprite.setColor(color);
            window.draw(enemySprite);
            color.a = 255;
            enemySprite.setColor(color);
        }
        else
        {
            window.draw(enemySprite);
        }
    }
}

bool checkEnemySuction(float enemyX, float enemyY, bool enemyAlive, int enemyType,
                       bool enemyIsShooting, float playerX, float playerY, float radius,
                       char directionKey, float SUCTION_RADIUS, float SUCTION_SECTION_ANGLE,
                       float SUCTION_VERTICAL_OFFSET)
{
    if (!enemyAlive)
        return false;
    if (enemyType == 3 && enemyIsShooting)
        return false;

    float enemyCenterX = enemyX + 40;
    float enemyCenterY = enemyY + 40;
    float playerCenterX = playerX + 48;
    float playerCenterY = playerY + 51 + SUCTION_VERTICAL_OFFSET;

    float dx = enemyCenterX - playerCenterX;
    float dy = enemyCenterY - playerCenterY;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance <= SUCTION_RADIUS)
    {
        float angle = atan2(dy, dx) * 180.0f / 3.14159265f;
        if (angle < 0)
            angle += 360.0f;

        switch (directionKey)
        {
        case 'A': // LEFT
        {
            float leftAngle = 180.0f;
            float minAngle = leftAngle - SUCTION_SECTION_ANGLE / 2;
            float maxAngle = leftAngle + SUCTION_SECTION_ANGLE / 2;

            if (minAngle < 0)
            {
                minAngle += 360.0f;
                return (angle >= minAngle || angle <= maxAngle);
            }
            else if (maxAngle > 360)
            {
                maxAngle -= 360.0f;
                return (angle >= minAngle || angle <= maxAngle);
            }
            else
            {
                return (angle >= minAngle && angle <= maxAngle);
            }
        }

        case 'D': // RIGHT
        {
            float rightAngle = 0.0f;
            float minAngle = rightAngle - SUCTION_SECTION_ANGLE / 2;
            float maxAngle = rightAngle + SUCTION_SECTION_ANGLE / 2;

            if (minAngle < 0)
            {
                minAngle += 360.0f;
                return (angle >= minAngle || angle <= maxAngle);
            }
            else
            {
                return (angle >= minAngle && angle <= maxAngle);
            }
        }

        case 'W': // UP
        {
            float upAngle = 270.0f;
            float minAngle = upAngle - SUCTION_SECTION_ANGLE / 2;
            float maxAngle = upAngle + SUCTION_SECTION_ANGLE / 2;
            return (angle >= minAngle && angle <= maxAngle);
        }

        case 'S': // DOWN
        {
            float downAngle = 90.0f;
            float minAngle = downAngle - SUCTION_SECTION_ANGLE / 2;
            float maxAngle = downAngle + SUCTION_SECTION_ANGLE / 2;
            return (angle >= minAngle && angle <= maxAngle);
        }
        }
    }
    return false;
}

bool checkEnemyCollision(float enemyX, float enemyY, float playerX, float playerY,
                         int playerWidth, int playerHeight)
{
    float enemyRight = enemyX + 80;
    float enemyBottom = enemyY + 80;
    float playerRight = playerX + playerWidth;
    float playerBottom = playerY + playerHeight;

    return (enemyX < playerRight &&
            enemyRight > playerX &&
            enemyY < playerBottom &&
            enemyBottom > playerY);
}

bool checkProjectileCollision(float projX, float projY, float playerX, float playerY,
                              int playerWidth, int playerHeight)
{
    float projRight = projX + 10;
    float projBottom = projY + 10;
    float playerRight = playerX + playerWidth;
    float playerBottom = playerY + playerHeight;

    return (projX < playerRight &&
            projRight > playerX &&
            projY < playerBottom &&
            projBottom > playerY);
}

void drawUI(sf::RenderWindow &window, sf::Font &uiFont, sf::Text &scoreText,
            int playerLives, int currentLevel, int score, int screen_x,
            bool isTimerRunning, int currentGameState, sf::Clock &gameTimer, sf::Time &finalGameTime)
{
    for (int i = 0; i < playerLives; i++)
    {
        sf::CircleShape heart(15);
        heart.setFillColor(sf::Color::Red);
        heart.setPosition(10 + i * 35, 10);
        window.draw(heart);
    }

    for (int i = playerLives; i < 5; i++)
    {
        sf::CircleShape heart(15);
        heart.setFillColor(sf::Color::Transparent);
        heart.setOutlineColor(sf::Color::Red);
        heart.setOutlineThickness(2);
        heart.setPosition(10 + i * 35, 10);
        window.draw(heart);
    }

    sf::Text levelText;
    levelText.setFont(uiFont);
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::Cyan);
    levelText.setStyle(sf::Text::Bold);
    char levelStr[50];
    sprintf(levelStr, "Level: %d", currentLevel);
    levelText.setString(levelStr);
    levelText.setPosition(screen_x / 2 - 50, 10);
    window.draw(levelText);

    char scoreStr[50];
    sprintf(scoreStr, "Score: %d", score);
    scoreText.setString(scoreStr);
    scoreText.setPosition(screen_x - 150, 10);
    window.draw(scoreText);

    sf::Time elapsed;
    if (isTimerRunning && currentGameState != 3 && currentGameState != 4)
    {
        elapsed = gameTimer.getElapsedTime();
    }
    else
    {
        elapsed = finalGameTime;
    }

    int minutes = static_cast<int>(elapsed.asSeconds()) / 60;
    int seconds = static_cast<int>(elapsed.asSeconds()) % 60;

    sf::Text timerText;
    timerText.setFont(uiFont);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::Yellow);
    timerText.setStyle(sf::Text::Bold);

    char timeStr[50];
    sprintf(timeStr, "Time: %02d:%02d", minutes, seconds);
    timerText.setString(timeStr);
    timerText.setPosition(screen_x / 2 - 50, 50);
    window.draw(timerText);
}

void drawIntroScreen(sf::RenderWindow &window, sf::Texture &introTexture, sf::Sprite &introSprite,
                     sf::Clock &introClock, sf::Font &uiFont, int screen_x, int screen_y)
{
    window.clear();
    introSprite.setPosition(0, 0);
    window.draw(introSprite);

    if (introClock.getElapsedTime().asSeconds() > 2.0f)
    {
        sf::Text startText;
        startText.setFont(uiFont);
        startText.setCharacterSize(36);
        startText.setFillColor(sf::Color::Yellow);
        startText.setStyle(sf::Text::Bold);
        startText.setString("Press any key to start");
        startText.setPosition(screen_x / 2 - 200, screen_y - 100);
        window.draw(startText);
    }

    window.display();
}

void drawGameOverScreen(sf::RenderWindow &window, sf::Texture &introTexture, sf::Sprite &introSprite,
                        sf::Font &uiFont, int score, sf::Time &finalGameTime, int screen_x, int screen_y)
{
    window.clear();
    introSprite.setPosition(0, 0);
    window.draw(introSprite);

    sf::RectangleShape overlay(sf::Vector2f(screen_x, screen_y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    sf::Text gameOverText;
    gameOverText.setFont(uiFont);
    gameOverText.setCharacterSize(72);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);
    gameOverText.setString("GAME OVER");
    gameOverText.setPosition(screen_x / 2 - 200, 100);
    window.draw(gameOverText);

    sf::Text scoreText;
    scoreText.setFont(uiFont);
    scoreText.setCharacterSize(48);
    scoreText.setFillColor(sf::Color::Yellow);
    char scoreStr[50];
    sprintf(scoreStr, "Final Score: %d", score);
    scoreText.setString(scoreStr);
    scoreText.setPosition(screen_x / 2 - 150, 200);
    window.draw(scoreText);

    int totalSeconds = static_cast<int>(finalGameTime.asSeconds());
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    sf::Text timeText;
    timeText.setFont(uiFont);
    timeText.setCharacterSize(36);
    timeText.setFillColor(sf::Color::Cyan);
    char timeStr[50];
    sprintf(timeStr, "Total Time: %02d:%02d", minutes, seconds);
    timeText.setString(timeStr);
    timeText.setPosition(screen_x / 2 - 120, 300);
    window.draw(timeText);

    sf::Text restartText;
    restartText.setFont(uiFont);
    restartText.setCharacterSize(36);
    restartText.setFillColor(sf::Color::Green);
    restartText.setString("Press R to restart or ESC to quit");
    restartText.setPosition(screen_x / 2 - 250, 400);
    window.draw(restartText);

    window.display();
}

void drawVictoryScreen(sf::RenderWindow &window, sf::Texture &introTexture, sf::Sprite &introSprite,
                       sf::Font &uiFont, int score, sf::Time &finalGameTime, int screen_x, int screen_y)
{
    window.clear();
    introSprite.setPosition(0, 0);
    window.draw(introSprite);

    sf::RectangleShape overlay(sf::Vector2f(screen_x, screen_y));
    overlay.setFillColor(sf::Color(0, 50, 0, 150));
    window.draw(overlay);

    sf::Text victoryText;
    victoryText.setFont(uiFont);
    victoryText.setCharacterSize(72);
    victoryText.setFillColor(sf::Color::Green);
    victoryText.setStyle(sf::Text::Bold);
    victoryText.setString("VICTORY!");
    victoryText.setPosition(screen_x / 2 - 150, 100);
    window.draw(victoryText);

    sf::Text scoreText;
    scoreText.setFont(uiFont);
    scoreText.setCharacterSize(48);
    scoreText.setFillColor(sf::Color::Yellow);
    char scoreStr[50];
    sprintf(scoreStr, "Final Score: %d", score);
    scoreText.setString(scoreStr);
    scoreText.setPosition(screen_x / 2 - 150, 200);
    window.draw(scoreText);

    int totalSeconds = static_cast<int>(finalGameTime.asSeconds());
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    sf::Text timeText;
    timeText.setFont(uiFont);
    timeText.setCharacterSize(36);
    timeText.setFillColor(sf::Color::Cyan);
    char timeStr[50];
    sprintf(timeStr, "Total Time: %02d:%02d", minutes, seconds);
    timeText.setString(timeStr);
    timeText.setPosition(screen_x / 2 - 120, 300);
    window.draw(timeText);

    sf::Text restartText;
    restartText.setFont(uiFont);
    restartText.setCharacterSize(36);
    restartText.setFillColor(sf::Color::White);
    restartText.setString("Press R to restart or ESC to quit");
    restartText.setPosition(screen_x / 2 - 250, 400);
    window.draw(restartText);

    window.display();
}

bool checkLevelCompletion(bool *enemyAlive, int enemyCount, int currentLevel,
                          int currentEnemyTypeIndex, int *level2SequenceTypes,
                          int *level2SequenceCounts, int level2SequenceSize)
{
    if (currentLevel == 1)
    {
        int aliveCount = 0;
        for (int i = 0; i < enemyCount; i++)
        {
            if (enemyAlive[i])
                aliveCount++;
        }
        return aliveCount == 0;
    }
    else if (currentLevel == 2)
    {
        if (currentEnemyTypeIndex >= level2SequenceSize)
        {
            int aliveCount = 0;
            for (int i = 0; i < enemyCount; i++)
            {
                if (enemyAlive[i])
                    aliveCount++;
            }
            return aliveCount == 0;
        }
    }
    return false;
}