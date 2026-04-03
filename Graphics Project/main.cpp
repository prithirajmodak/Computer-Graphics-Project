#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <cstdlib>



int displayMode = 0;  // Tracks current display mode (0 to 3)


void toggleDisplay();


// Window dimensions
const int windowWidth1 = 800;
const int windowHeight1 = 600;



// Fixed car dimensions
const float carWidth1 = 35;
const float carHeight1 = 60;

// Road and lane dimensions
const float laneWidth1 = 200;
const float roadHeight1 = windowHeight1;
const float lanePositions1[4] = {
    windowWidth1 / 2 - 1.5f * laneWidth1,  // Leftmost lane
    windowWidth1 / 2 - 0.5f * laneWidth1, // Left center lane
    windowWidth1 / 2 + 0.5f * laneWidth1, // Right center lane
    windowWidth1 / 2 + 1.5f * laneWidth1  // Rightmost lane
};

float playerCarX1 = lanePositions1[1]; // Start in the left center lane
float playerCarY1 = 50;


// Modes
enum Mode { DAY, NIGHT, RAIN };
Mode currentMode = static_cast<Mode>(-1);  // No initial mode

// Menu variables
bool menuVisible = true;
int menuIndex = 0;
const char* menuOptions[3] = { "Level 1 (EASY)", "Level 2 (MEDIUM)", "Level 3 (HARD)" };

void drawText(float x, float y, const char* text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}




//methods for level1


// Opponent cars
struct OpponentCar {
    float x, y;
};

std::vector<OpponentCar> opponentCars;
int score = 0;
bool gameOver = false;

struct Obstacle {
    float x, y;
    int type;
};
std::vector<Obstacle> obstacles;


void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircle(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; ++i) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
    }
    glEnd();
}

void drawCar1(float x, float y, float bodyR, float bodyG, float bodyB) {
    // Car body
    drawRectangle(x, y, carWidth1, carHeight1, bodyR, bodyG, bodyB);

    // Windows
    float windowHeight1 = carHeight1 / 3;
    float windowWidth1 = carWidth1 - 10;
    drawRectangle(x + 5, y + carHeight1 - windowHeight1 - 10, windowWidth1, windowHeight1, 0.5f, 0.8f, 1.0f);

    // Wheels
    float wheelRadius = 10;
    drawCircle(x + 10, y + 5, wheelRadius, 0, 0, 0);         // Front left wheel
    drawCircle(x + carWidth1 - 10, y + 5, wheelRadius, 0, 0, 0); // Front right wheel

    // Headlights
    float lightRadius = 5;
    drawCircle(x + 5, y + carHeight1 - 5, lightRadius, 1.0f, 1.0f, 0.0f);        // Left headlight
    drawCircle(x + carWidth1 - 5, y + carHeight1 - 5, lightRadius, 1.0f, 1.0f, 0.0f); // Right headlight
}

void drawPlayerCar1() {
    drawCar1(playerCarX1, playerCarY1, 0.0f, 1.0f, 0.0f); // Green car for player
}

void drawOpponentCars1() {
    for (auto &car : opponentCars) {
        drawCar1(car.x, car.y, 1.0f, 0.0f, 0.0f); // Red cars for opponents
    }
}



float skyColorR = 0.5f, skyColorG = 0.7f, skyColorB = 1.0f; // Initial sky color (daylight blue)
float grassColorR = 0.0f, grassColorG = 0.6f, grassColorB = 0.0f; // Initial grass color (green)
float roadColorR = 0.15f, roadColorG = 0.15f, roadColorB = 0.15f; // Initial road color (gray)
float colorChangeSpeed = 0.01f; // Speed of color change

void updateColors1() {
    // Gradually change colors with distinct phases
    skyColorR = 0.5f + 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0001f); // Oscillates between 0.5 and 1.0
    skyColorG = 0.7f + 0.3f * cos(glutGet(GLUT_ELAPSED_TIME) * 0.0001f); // Oscillates between 0.4 and 1.0
    skyColorB = 1.0f - 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0001f); // Oscillates between 0.5 and 1.0

    grassColorR = 0.2f + 0.3f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0002f); // Oscillates between 0.2 and 0.5
    grassColorG = 0.6f + 0.2f * cos(glutGet(GLUT_ELAPSED_TIME) * 0.0002f); // Oscillates between 0.4 and 0.8
    grassColorB = 0.0f + 0.3f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0002f); // Oscillates between 0.0 and 0.3

    roadColorR = 0.1f + 0.1f * cos(glutGet(GLUT_ELAPSED_TIME) * 0.0003f); // Oscillates between 0.1 and 0.2
    roadColorG = 0.1f + 0.1f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0003f); // Oscillates between 0.1 and 0.2
    roadColorB = 0.15f + 0.1f * cos(glutGet(GLUT_ELAPSED_TIME) * 0.0003f); // Oscillates between 0.15 and 0.25
}



void drawRoad1() {
    //updateColors1();
    glClearColor(skyColorR, skyColorG, skyColorB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the main road with gradient
    glBegin(GL_QUADS);
    glColor3f(roadColorR, roadColorG, roadColorB); // Dark gray bottom
    glVertex2f(windowWidth1 / 2 - laneWidth1 * 2, 0);
    glVertex2f(windowWidth1 / 2 + laneWidth1 * 2, 0);
    glColor3f(roadColorR * 1.2f, roadColorG * 1.2f, roadColorB * 1.2f); // Lighter gray top
    glVertex2f(windowWidth1 / 2 + laneWidth1 * 2, roadHeight1);
    glVertex2f(windowWidth1 / 2 - laneWidth1 * 2, roadHeight1);
    glEnd();



    // Add tiles to the footpaths
    int tileWidth = 15;  // Width of each tile
    int tileHeight = 15; // Height of each tile
    for (int y = 0; y < roadHeight1; y += tileHeight) {
        for (int x = 0; x < 30; x += tileWidth) {
            float tileColor = (x / tileWidth + y / tileHeight) % 2 == 0 ? 0.7f : 0.9f; // Checkered pattern
            drawRectangle(windowWidth1 / 2 - laneWidth1 * 2 - 30 + x, y, tileWidth, tileHeight, tileColor, tileColor, tileColor); // Left tiles
            drawRectangle(windowWidth1 / 2 + laneWidth1 * 2 + x, y, tileWidth, tileHeight, tileColor, tileColor, tileColor); // Right tiles
        }
    }

    // Add decorative flower pots along the footpaths
    for (int y = 70; y < roadHeight1; y += 200) {
        // Left flower pot
        drawRectangle(windowWidth1 / 2 - laneWidth1 * 2 - 40, y, 12, 20, 0.4f, 0.2f, 0.0f); // Pot base
        drawRectangle(windowWidth1 / 2 - laneWidth1 * 2 - 42, y + 20, 16, 12, 0.0f, 0.8f, 0.0f); // Plant
        // Right flower pot
        drawRectangle(windowWidth1 / 2 + laneWidth1 * 2 + 30, y, 12, 20, 0.4f, 0.2f, 0.0f); // Pot base
        drawRectangle(windowWidth1 / 2 + laneWidth1 * 2 + 28, y + 20, 16, 12, 0.0f, 0.8f, 0.0f); // Plant
    }

    // Add zebra crossings at regular intervals
    glColor3f(1.0f, 1.0f, 1.0f);
    for (float y = 150; y < roadHeight1; y += 300) {
        for (float x = windowWidth1 / 2 - laneWidth1 * 2 + 40; x < windowWidth1 / 2 + laneWidth1 * 2 - 40; x += 50) {
            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + 25, y);
            glVertex2f(x + 25, y + 12);
            glVertex2f(x, y + 12);
            glEnd();
        }
    }

    // Add lane dividers
    for (int i = 1; i <= 3; ++i) {
        float x = windowWidth1 / 2 - 2 * laneWidth1 + i * laneWidth1;
        for (float y = 0; y < roadHeight1; y += 50) {
            glColor3f(1, 1, 1); // Dashed lines
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x, y + 30);
            glEnd();

            glColor3f(1.0f, 0.8f, 0.0f); // Reflective markers
            drawRectangle(x - 2, y + 15, 4, 4, 1.0f, 0.8f, 0.0f);
        }

    }



    // Add directional arrows in the center lanes
    glColor3f(1, 1, 1);
    for (float y = 200; y < roadHeight1; y += 300) {
        float x = windowWidth1 / 2;
        glBegin(GL_TRIANGLES); // Arrowhead
        glVertex2f(x, y + 30);
        glVertex2f(x - 15, y);
        glVertex2f(x + 15, y);
        glEnd();
        glBegin(GL_LINES); // Arrow shaft
        glVertex2f(x, y - 30);
        glVertex2f(x, y);
        glEnd();
    }
}






void drawObstacle1(float x, float y, int type) {
    switch (type) {
        case 0: // Traffic cone
            drawRectangle(x + carWidth1 / 4, y, carWidth1 / 2, carHeight1 / 8, 1.0f, 0.5f, 0.0f); // Orange base
            drawRectangle(x + carWidth1 / 4 + 5, y + carHeight1 / 8, carWidth1 / 2 - 10, carHeight1 / 8, 1.0f, 0.6f, 0.0f); // Lighter orange middle
            drawCircle(x + carWidth1 / 2, y + carHeight1 / 4 + 10, carWidth1 / 8, 1.0f, 0.7f, 0.0f); // Circular tip
            break;
        case 1: // Barrel
            drawRectangle(x, y, carWidth1 / 2, carHeight1 / 2, 0.8f, 0.4f, 0.0f); // Brown barrel
            drawRectangle(x, y + carHeight1 / 8, carWidth1 / 2, carHeight1 / 16, 1.0f, 1.0f, 0.0f); // Yellow stripe
            break;
        case 2: // Roadblock
            drawRectangle(x, y, carWidth1, carHeight1 / 4, 0.5f, 0.5f, 0.5f); // Gray base
            drawRectangle(x, y + carHeight1 / 8, carWidth1, carHeight1 / 16, 1.0f, 0.0f, 0.0f); // Red stripe
            break;
    }
}

void drawObstacles1() {
    for (const auto &obs : obstacles) {
        drawObstacle1(obs.x, obs.y, obs.type);
    }
}






// Function to update obstacles
void updateObstacles1() {
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        it->y -= 6;

        // Check collision with player car
        if (it->y < playerCarY1 + carHeight1 && it->y + carHeight1 / 2 > playerCarY1 &&
            it->x < playerCarX1 + carWidth1 && it->x + carWidth1 > playerCarX1) {
            gameOver = true;
            return;
        }

        // Remove obstacles that go off-screen
        if (it->y + carHeight1 / 2 < 0) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }
}

void spawnObstacle1() {
    int lane = rand() % 4; // Random lane
    float x = lanePositions1[lane];
    int type = rand() % 3; // Random type (0, 1, 2)
    obstacles.push_back({x, windowHeight1, type});
}

void spawnOpponentCar1() {
    // Track occupied lanes (0, 1, 2, 3)
    bool occupied[4] = {false, false, false, false};
    int occupiedCount = 0;

    // Check which lanes are occupied by existing opponent cars
    for (const auto &car : opponentCars) {
        for (int i = 0; i < 4; ++i) {  // Iterate through all 4 lanes
            if (fabs(car.x - lanePositions1[i]) < laneWidth1 / 2) {
                occupied[i] = true; // Mark the lane as occupied
                break;
            }
        }
    }

    // Count occupied lanes
    for (int i = 0; i < 4; ++i) {
        if (occupied[i]) occupiedCount++;
    }

    // Only spawn a new car if fewer than 4 lanes are occupied
    if (occupiedCount < 4) {
        int lane;
        do {
            lane = rand() % 4; // Randomly select one of the 4 lanes (0, 1, 2, 3)
        } while (occupied[lane]); // Ensure the lane isn't occupied

        float x = lanePositions1[lane]; // Get the X position of the chosen lane
        opponentCars.push_back({x, windowHeight1}); // Spawn the car at the top
    }
}


void updateOpponentCars1() {
    for (auto it = opponentCars.begin(); it != opponentCars.end();) {
        it->y -= 6;

        // Check collision
        if (it->y < playerCarY1 + carHeight1 && it->y + carHeight1 > playerCarY1 &&
            it->x < playerCarX1 + carWidth1 && it->x + carWidth1 > playerCarX1) {
            gameOver = true;
            return;
        }

        // Remove cars that go out of screen
        if (it->y + carHeight1 < 0) {
            it = opponentCars.erase(it);
            score++;
        } else {
            ++it;
        }
    }
}
int obstacleSpawnCounter1 = 100;

// Modify update function to include obstacles
void update1(int value) {
    if (!gameOver) {
        updateOpponentCars1();
        updateObstacles1();

        // Spawn opponent cars
        if (rand() % 100 < 2) {
            spawnOpponentCar1();
        }

        // Decrease the spawn counter and spawn obstacles less frequently
        if (obstacleSpawnCounter1 <= 0) {
            spawnObstacle1();
            obstacleSpawnCounter1 = 100;  // Reset to a higher value (longer delay)
        } else {
            obstacleSpawnCounter1--;
        }

        glutPostRedisplay();
        glutTimerFunc(16, update1, 0);
    }
}



void handleKeypress1(int key, int x, int y) {
    if (!gameOver) {
        int currentLane = -1;

        // Identify the current lane
        for (int i = 0; i < 4; ++i) {
            if (fabs(playerCarX1 - lanePositions1[i]) < laneWidth1 / 2) {
                currentLane = i;
                break;
            }
        }

        // Move the player car between four lanes
if (key == GLUT_KEY_LEFT && currentLane > 0) {
    currentLane--;  // Move left to the previous lane
    playerCarX1 = lanePositions1[currentLane]; // Update player position
}
else if (key == GLUT_KEY_RIGHT && currentLane < 3) {
    currentLane++;  // Move right to the next lane
    playerCarX1 = lanePositions1[currentLane]; // Update player position
}


        glutPostRedisplay();
    }
}

void resetGame1() {
    playerCarX1 = lanePositions1[1];
    opponentCars.clear();
    obstacles.clear(); // Clear obstacles
    score = 0;
    gameOver = false;

    glutTimerFunc(16, update1, 0);
}



void display1() {

    glutSpecialFunc(handleKeypress1);
    drawRoad1();
    drawPlayerCar1();
    drawOpponentCars1();
    drawObstacles1();

    if (gameOver) {
glColor3f(0.0f, 1.0f, 1.0f);  // Bright Cyan

        glRasterPos2f(windowWidth1 / 2 - 100, windowHeight1 / 1.78);
        const char *message = "Game Over! Press 'R' to Restart or 'X' to Exit.";
        for (const char *c = message; *c != '\0'; ++c) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

        }
    }

    glColor3f(1, 1, 1);
    glRasterPos2f(10, windowHeight1 - 30);
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score);
    for (const char *c = scoreStr; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glutSwapBuffers();


    }

//Level 1 ends
//############################

//level 2 methods


void update2(int value);

// Window dimensions
// Window dimensions (Full HD 1920x1080)
const int windowWidth2 = 800;
const int windowHeight2 = 600;

// Fixed car dimensions (adjusted for better scaling)
//const float carWidth = 50;
//const float carHeight = 80;

// Road and lane dimensions
const float laneWidth2 = 200; // Adjusted for better road division
const float roadHeight2 = windowHeight2;  // Full height of the screen
const float roadWidth2 = windowWidth2 * 0.6;  // Road covers ~60% of screen width



// Define the 4 positions (lanes) for cars
const float lanePositions2[4] = {
    (windowWidth2 - roadWidth2) / 2 - 0.5f * laneWidth2,  // Leftmost lane
    (windowWidth2 - roadWidth2) / 2 + 0.45f * laneWidth2,  // Left center lane
    (windowWidth2 + roadWidth2) / 2 - 0.7f * laneWidth2,  // Right center lane
    (windowWidth2 + roadWidth2) / 2 + 0.5f * laneWidth2   // Rightmost lane
};

// Variables for player car position
float playerCarX2 = lanePositions2[1]; // Start in the left center lane
  // Horizontal center

float playerCarY2 = 50;

// Opponent cars
struct OpponentCar2 {
    float x, y;
};

std::vector<OpponentCar2> opponentCars2;
int score2 = 0;
bool gameOver2 = false;

struct Obstacle2 {
    float x, y;
    int type;
};
std::vector<Obstacle2> obstacles2;


void drawRectangle2(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircle2(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; ++i) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
    }
    glEnd();
}


void drawRoundedRectangle2(float x, float y, float width, float height, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);

    // Draw rectangle with rounded corners
    for (int i = 0; i < 4; i++) {
        float angle1 = (i * 90) * (M_PI / 180.0);
        float angle2 = ((i + 1) * 90) * (M_PI / 180.0);

        float cx = x + (i % 2) * (width - 2 * radius) + radius;
        float cy = y + (i / 2) * (height - 2 * radius) + radius;

        for (float angle = angle1; angle <= angle2; angle += 0.1f) {
            glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
        }
    }

    glEnd();
}



// Adjusted car width and height based on new lane width
// Adjusted car width and height (3 times larger than the previous size)
const float carWidth2 = laneWidth2 * 0.2f;  // 30% of the lane width
const float carHeight2 = carWidth2 * 1.9f;  // Height of the car is 1.5x the width

void drawCar2(float x, float y, float bodyR, float bodyG, float bodyB) {
    // Car body
    drawRectangle2(x, y, carWidth2, carHeight2, bodyR, bodyG, bodyB);

    // Windows
    float windowHeight2 = carHeight2 / 3;
    float windowWidth2 = carWidth2 - 10;
    drawRectangle2(x + 5, y + carHeight2 - windowHeight2 - 10, windowWidth2, windowHeight2, 0.5f, 0.8f, 1.0f);

    // Wheels
    float wheelRadius2 = carWidth2 * 0.3f;  // Adjust wheel radius to be proportionate to car width
    drawCircle2(x + 10, y + 5, wheelRadius2, 0, 0, 0);         // Front left wheel
    drawCircle2(x + carWidth2 - 10, y + 5, wheelRadius2, 0, 0, 0); // Front right wheel

    // Headlights
    float lightRadius2 = carWidth2 * 0.15f;  // Adjust light radius to be proportionate to car width
    drawCircle2(x + 5, y + carHeight2 - 5, lightRadius2, 1.0f, 1.0f, 0.0f);        // Left headlight
    drawCircle2(x + carWidth2 - 5, y + carHeight2 - 5, lightRadius2, 1.0f, 1.0f, 0.0f); // Right headlight
}
void drawPlayerCar2() {
    drawCar2(playerCarX2, playerCarY2, 0.0f, 1.0f, 0.0f); // Green car for player
}

void drawOpponentCars2() {
    for (auto &car2 : opponentCars2) {
        drawCar2(car2.x, car2.y, 1.0f, 0.0f, 0.0f); // Red cars for opponents
    }
}



float skyColorR2 = 0.5f, skyColorG2 = 0.7f, skyColorB2 = 1.0f; // Initial sky color (daylight blue)
float grassColorR2 = 0.0f, grassColorG2 = 0.6f, grassColorB2 = 0.0f; // Initial grass color (green)
float roadColorR2 = 0.15f, roadColorG2 = 0.15f, roadColorB2 = 0.15f; // Initial road color (gray)
float colorChangeSpeed2 = 0.01f; // Speed of color change

void updateColors2() {
    // Gradually change colors with distinct phases
    skyColorR2 = 0.5f + 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0001f); // Oscillates between 0.5 and 1.0
    skyColorG2 = 0.7f + 0.3f * cos(glutGet(GLUT_ELAPSED_TIME) * 0.0001f); // Oscillates between 0.4 and 1.0
    skyColorB2 = 1.0f - 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0001f); // Oscillates between 0.5 and 1.0

    grassColorR2 = 0.2f + 0.3f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0002f); // Oscillates between 0.2 and 0.5
    grassColorG2 = 0.6f + 0.2f * cos(glutGet(GLUT_ELAPSED_TIME) * 0.0002f); // Oscillates between 0.4 and 0.8
    grassColorB2 = 0.0f + 0.3f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0002f); // Oscillates between 0.0 and 0.3

    roadColorR2 = 0.1f + 0.1f * cos(glutGet(GLUT_ELAPSED_TIME) * 0.0003f); // Oscillates between 0.1 and 0.2
    roadColorG2 = 0.1f + 0.1f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.0003f); // Oscillates between 0.1 and 0.2
    roadColorB2 = 0.15f + 0.1f * cos(glutGet(GLUT_ELAPSED_TIME) * 0.0003f); // Oscillates between 0.15 and 0.25
}


void drawTree2(float x, float y) {
    // Trunk
    drawRectangle2(x + 10, y, 20, 40, 0.6f, 0.3f, 0.0f); // Brown trunk

    // Leaves (Multiple overlapping circles for a fuller effect)
    drawCircle2(x + 20, y + 50, 25, 0.0f, 0.6f, 0.0f); // Bottom foliage
    drawCircle2(x, y + 60, 20, 0.0f, 0.8f, 0.0f); // Left foliage
    drawCircle2(x + 40, y + 60, 20, 0.0f, 0.8f, 0.0f); // Right foliage
    drawCircle2(x + 20, y + 80, 30, 0.0f, 1.0f, 0.0f); // Top foliage
}


void drawCircle2(float x, float y) {
    // House body (Main rectangle)
    drawRectangle2(x, y, 80, 60, 0.8f, 0.5f, 0.3f); // Brown house body

    // Roof (Triangle with overhang effect)
    glColor3f(0.9f, 0.0f, 0.0f); // Red roof
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 10, y + 60); // Left corner (overhang)
    glVertex2f(x + 90, y + 60); // Right corner (overhang)
    glVertex2f(x + 40, y + 90); // Top peak
    glEnd();

    // Chimney (Small rectangle on roof)
    drawRectangle2(x + 50, y + 70, 15, 25, 0.6f, 0.3f, 0.2f); // Brown chimney

    // Door
    drawRectangle2(x + 30, y, 20, 30, 0.5f, 0.3f, 0.2f); // Dark brown door

    // Windows (Two small blue windows)
    drawRectangle2(x + 10, y + 35, 15, 15, 0.0f, 0.7f, 1.0f); // Left window (blue)
    drawRectangle2(x + 55, y + 35, 15, 15, 0.0f, 0.7f, 1.0f); // Right window (blue)
}

void drawBush2(float x, float y) {
    // Three circles to form a bush
    drawCircle2(x, y, 20, 0.0f, 0.5f, 0.0f);
    drawCircle2(x + 15, y + 5, 18, 0.0f, 0.6f, 0.0f);
    drawCircle2(x - 15, y + 5, 18, 0.0f, 0.6f, 0.0f);
}


void drawRoad2() {
    // Draw green areas
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);

    // Left green area
    glVertex2f(0, 0);
    glVertex2f((windowWidth2 - roadWidth2) / 2, 0);
    glVertex2f((windowWidth2 - roadWidth2) / 2, windowWidth2);
    glVertex2f(0, windowWidth2);

    // Right green area
    glVertex2f((windowWidth2 + roadWidth2) / 2, 0);
    glVertex2f(windowWidth2, 0);
    glVertex2f(windowWidth2, windowWidth2);
    glVertex2f((windowWidth2 + roadWidth2) / 2, windowWidth2);

    glEnd();

    // Draw the road (main road)
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f((windowWidth2 - roadWidth2) / 2, 0);
    glVertex2f((windowWidth2 + roadWidth2) / 2, 0);
    glVertex2f((windowWidth2 + roadWidth2) / 2, windowWidth2);
    glVertex2f((windowWidth2 - roadWidth2) / 2, windowWidth2);
    glEnd();

    // Lane markings
    glColor3f(1, 1, 1);
    glLineWidth(4);
    glBegin(GL_LINES);
    for (float y = 0; y < windowWidth2; y += 80) {
        glVertex2f(windowWidth2 / 2, y);
        glVertex2f(windowWidth2 / 2, y + 40);
    }
    glEnd();

    // Static objects in green area
    float houseSpacing2 = 200;
    float treeSpacing2 = 100;
    float bushSpacing2 = 80;

    // Left side (trees moved closer to the road)
    for (float y = windowWidth2 - 150; y > 0; y -= houseSpacing2) {
        drawCircle2((windowWidth2 - roadWidth2) / 2 -180 , y);
    }
    for (float y = windowWidth2 - 100; y > 0; y -= treeSpacing2) {
        drawTree2((windowWidth2 - roadWidth2) / 2 - 90, y);  // Trees closer to road
    }
    for (float y = windowWidth2 - 50; y > 0; y -= bushSpacing2) {
        drawBush2(80, y);
    }

    // Right side (trees moved closer to the road)
    for (float y = windowWidth2 - 150; y > 0; y -= houseSpacing2) {
        drawCircle2(windowWidth2 - 150, y);
    }
    for (float y = windowWidth2 - 100; y > 0; y -= treeSpacing2) {
        drawTree2((windowWidth2 + roadWidth2) / 2 + 50, y);  // Trees closer to road
    }
    for (float y = windowWidth2 - 50; y > 0; y -= bushSpacing2) {
        drawBush2(windowWidth2 - 50, y);
    }
}


















void drawObstacle2(float x, float y, int type) {
    switch (type) {
        case 0: // Traffic cone
            drawRectangle2(x + carWidth2 / 4, y, carWidth2 / 2, carHeight2 / 8, 1.0f, 0.5f, 0.0f); // Orange base
            drawRectangle2(x + carWidth2 / 4 + 5, y + carHeight2 / 8, carWidth2 / 2 - 10, carHeight2 / 8, 1.0f, 0.6f, 0.0f); // Lighter orange middle
            drawCircle2(x + carWidth2 / 2, y + carHeight2 / 4 + 10, carWidth2 / 8, 1.0f, 0.7f, 0.0f); // Circular tip
            break;
        case 1: // Barrel
            drawRectangle2(x, y, carWidth2 / 2, carHeight2 / 2, 0.8f, 0.4f, 0.0f); // Brown barrel
            drawRectangle2(x, y + carHeight2 / 8, carWidth2 / 2, carHeight2 / 16, 1.0f, 1.0f, 0.0f); // Yellow stripe
            break;
        case 2: // Roadblock
            drawRectangle2(x, y, carWidth2, carHeight2 / 4, 0.5f, 0.5f, 0.5f); // Gray base
            drawRectangle2(x, y + carHeight2 / 8, carWidth2, carHeight2 / 16, 1.0f, 0.0f, 0.0f); // Red stripe
            break;
    }
}

void drawObstacles2() {
    for (const auto &obs : obstacles2) {
        drawObstacle2(obs.x, obs.y, obs.type);
    }
}






// Function to update obstacles
void updateObstacles2() {
    for (auto it = obstacles2.begin(); it != obstacles2.end();) {
        it->y -= 6;

        // Check collision with player car
        if (it->y < playerCarY2 + carHeight2 && it->y + carHeight2 / 2 > playerCarY2 &&
            it->x < playerCarX2 + carWidth2 && it->x + carWidth2 > playerCarX2) {
            gameOver2 = true;
            return;
        }

        // Remove obstacles that go off-screen
        if (it->y + carHeight2 / 2 < 0) {
            it = obstacles2.erase(it);
        } else {
            ++it;
        }
    }
}

void spawnObstacle2() {
    // Track occupied lanes (only lanes 1 and 2)
    bool occupied[2] = {false, false}; // Only track lanes 1 and 2
    int occupiedCount = 0;

    for (const auto &obstacle : obstacles2) {
        for (int i = 1; i <= 2; ++i) { // Check only lanes 1 and 2
            if (fabs(obstacle.x - lanePositions2[i]) < laneWidth2 / 2) {
                occupied[i - 1] = true; // Map lane[1] to occupied[0], lane[2] to occupied[1]
                break;
            }
        }
    }

    // Count occupied lanes in lane[1] and lane[2]
    for (int i = 0; i < 2; ++i) {
        if (occupied[i]) occupiedCount++;
    }

    // Only spawn if fewer than 2 lanes are occupied
    if (occupiedCount < 2) {
        int lane;
        do {
            lane = rand() % 2 + 1; // Randomly select lane[1] or lane[2]
        } while (occupied[lane - 1]);

        float x = lanePositions2[lane];
        int type = rand() % 3; // Random type (0, 1, 2)
        obstacles2.push_back({x, windowWidth2, type});
    }
}


void spawnOpponentCar2() {
    // Track occupied lanes for lanes 1 and 2
    bool occupied[2] = {false, false}; // Track lane 1 and lane 2
    int occupiedCount = 0;

    // Check which lanes are occupied by existing opponent cars
    for (const auto &car2 : opponentCars2) {
        for (int i = 1; i <= 2; ++i) {  // Check only lanes 1 and 2
            if (fabs(car2.x - lanePositions2[i]) < laneWidth2 / 2) {
                occupied[i - 1] = true;  // Adjust for zero-based indexing in occupied
                break;
            }
        }
    }

    // Count occupied lanes
    for (int i = 0; i < 2; ++i) {
        if (occupied[i]) occupiedCount++;
    }

    // Only spawn if fewer than 2 lanes are occupied
    if (occupiedCount < 2) {
        int lane;
        do {
            // Randomly select lane 1 or lane 2 (indexes 1 and 2)
            lane = 1 + rand() % 2;  // Randomly select either lane 1 or lane 2
        } while (occupied[lane - 1]);  // Ensure the selected lane is not occupied

        float x = lanePositions2[lane]; // Select the lane's x position
        opponentCars2.push_back({x, windowWidth2}); // Spawn the car at the top
    }
}

void updateopponentCars2() {
    for (auto it = opponentCars2.begin(); it != opponentCars2.end();) {
        it->y -= 10;

        // Check collision
        if (it->y < playerCarY2 + carHeight2 && it->y + carHeight2 > playerCarY2 &&
            it->x < playerCarX2 + carWidth2 && it->x + carWidth2 > playerCarX2) {
            gameOver2 = true;
            return;
        }

        // Remove cars that go out of screen
        if (it->y + carHeight2 < 0) {
            it = opponentCars2.erase(it);
            score2++;
        } else {
            ++it;
        }
    }
}
int obstacleSpawnCounter2 = 100;

// Modify update function to include obstacles
int carSpawnCounter2 = 50; // Initialize a spawn delay for cars (you can adjust this value)

void update2(int value) {
    if (!gameOver2) {
        updateopponentCars2();
        // updateObstacles2();

        // Control the frequency of opponent car spawning
        if (carSpawnCounter2 <= 0) {
            spawnOpponentCar2();  // Spawn an opponent car
            carSpawnCounter2 = 100; // Reset to a higher value (longer delay) to control the spawn frequency
        } else {
            carSpawnCounter2--;  // Decrease the counter
        }

        // Decrease the spawn counter for obstacles and spawn them less frequently
        if (obstacleSpawnCounter2 <= 0) {
            spawnObstacle2();
            obstacleSpawnCounter2 = 100;  // Reset to a higher value for obstacles
        } else {
            obstacleSpawnCounter2--;
        }

        glutPostRedisplay();
        glutTimerFunc(16, update2, 0);
    }
}



void handleKeypress2(int key, int x, int y) {
    if (!gameOver2) {
        int currentLane = -1;

        // Identify the current lane
        for (int i = 0; i < 4; ++i) {
            if (fabs(playerCarX2 - lanePositions2[i]) < laneWidth2 / 2) {
                currentLane = i;
                break;
            }
        }

        // Allow movement only between center-left (lane 1) and center-right (lane 2)
        if (key == GLUT_KEY_LEFT && currentLane == 2) {
            playerCarX2 = lanePositions2[1]; // Move to center-left lane
        }
        else if (key == GLUT_KEY_RIGHT && currentLane == 1) {
            playerCarX2 = lanePositions2[2]; // Move to center-right lane
        }

        glutPostRedisplay();
    }
}

void resetGame2() {
    playerCarX2 = lanePositions2[1];
    opponentCars2.clear();
    obstacles2.clear(); // Clear obstacles2
    score2 = 0;
    gameOver2 = false;

    glutTimerFunc(16, update2, 0);
}

void handleNormalKeypress2(unsigned char key, int x, int y) {
    if (gameOver2 && (key == 'r' || key == 'R')) {
        resetGame2();
        glutPostRedisplay();
    }

    if (key == 'x' || key == 'X') {
        exit(0);
    }

}

void display2() {
    glutSpecialFunc(handleKeypress2);
    drawRoad2();
    drawPlayerCar2();
    drawOpponentCars2();
    drawObstacles2();

    if (gameOver2) {
        glColor3f(1, 0, 0);
        glRasterPos2f(windowWidth2 / 2 - 50, windowWidth2 / 2);
        const char *message = "Game Over! Press 'R' to Restart.";
        for (const char *c = message; *c != '\0'; ++c) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    glColor3f(1, 1, 1);
    glRasterPos2f(8, windowWidth2 - 230);
    char score2Str[20];
    sprintf(score2Str, "Score: %d", score2);
    for (const char *c = score2Str; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glutSwapBuffers();
}
////level 2 ends here
//####################

// level 3 methods




// Window dimensions
// Window dimensions (Full HD 1920x1080)
const int windowWidth3 = 800;
const int windowHeight3 = 600;

// Fixed car dimensions (adjusted for better scaling)
//const float carWidth = 50;
//const float carHeight = 80;

// Road and lane dimensions
const float laneWidth3 = 200; // Adjusted for better road division
const float roadHeight3 = windowHeight3;  // Full height of the screen
const float roadWidth3 = windowWidth3 * 0.5;  // Road covers ~60% of screen width



// Define the 4 positions (lanes) for cars
const float lanePositions3[4] = {
    (windowWidth3 - roadWidth3) / 2 - 0.5f * laneWidth3,  // Leftmost lane
    (windowWidth3 - roadWidth3) / 2 + 0.5f * laneWidth3,  // Left center lane
    (windowWidth3 + roadWidth3) / 2 - 0.5f * laneWidth3,  // Right center lane
    (windowWidth3 + roadWidth3) / 2 + 0.5f * laneWidth3   // Rightmost lane
};

// Variables for player car position
float playerCarX3 = lanePositions3[1]; // Start in the left center lane
  // Horizontal center

float playerCarY3 = 50;

// Opponent cars
struct OpponentCar3 {
    float x, y;
};

std::vector<OpponentCar3> opponentCars3;
int score3 = 0;
bool gameOver3 = false;

struct Obstacle3 {
    float x, y;
    int type;
};
std::vector<Obstacle3> obstacles3;


void drawRectangle3(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircle3(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; ++i) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
    }
    glEnd();
}


void drawRoundedRectangle3(float x, float y, float width, float height, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);

    // Draw rectangle with rounded corners
    for (int i = 0; i < 4; i++) {
        float angle1 = (i * 90) * (M_PI / 180.0);
        float angle2 = ((i + 1) * 90) * (M_PI / 180.0);

        float cx = x + (i % 2) * (width - 2 * radius) + radius;
        float cy = y + (i / 2) * (height - 2 * radius) + radius;

        for (float angle = angle1; angle <= angle2; angle += 0.1f) {
            glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
        }
    }

    glEnd();
}



// Adjusted car width and height based on new lane width
// Adjusted car width and height (3 times larger than the previous size)
const float carWidth3 = laneWidth3 * 0.2f;  // 30% of the lane width
const float carHeight3 = carWidth3 * 1.9f;  // Height of the car is 1.5x the width

void drawCar3(float x, float y, float bodyR, float bodyG, float bodyB) {
    // Car body
    drawRectangle3(x, y, carWidth3, carHeight3, bodyR, bodyG, bodyB);

    // Windows
    float windowHeight3 = carHeight3 / 3;
    float windowWidth3 = carWidth3 - 10;
    drawRectangle3(x + 5, y + carHeight3 - windowHeight3 - 10, windowWidth3, windowHeight3, 0.5f, 0.8f, 1.0f);

    // Wheels
    float wheelRadius3 = carWidth3 * 0.3f;  // Adjust wheel radius to be proportionate to car width
    drawCircle3(x + 10, y + 5, wheelRadius3, 0, 0, 0);         // Front left wheel
    drawCircle3(x + carWidth3 - 10, y + 5, wheelRadius3, 0, 0, 0); // Front right wheel

    // Headlights
    float lightRadius3 = carWidth3 * 0.15f;  // Adjust light radius to be proportionate to car width
    drawCircle3(x + 5, y + carHeight3 - 5, lightRadius3, 1.0f, 1.0f, 0.0f);        // Left headlight
    drawCircle3(x + carWidth3 - 5, y + carHeight3 - 5, lightRadius3, 1.0f, 1.0f, 0.0f); // Right headlight
}
void drawPlayerCar3() {
    drawCar3(playerCarX3, playerCarY3, 0.0f, 1.0f, 0.0f); // Green car for player
}

void drawopponentCars3() {
    for (auto &car2 : opponentCars3) {
        drawCar3(car2.x, car2.y, 1.0f, 0.0f, 0.0f); // Red cars for opponents
    }
}






void drawTree3(float x, float y) {
    // Trunk
    drawRectangle3(x + 10, y, 20, 40, 0.6f, 0.3f, 0.0f); // Brown trunk

    // Leaves (Multiple overlapping circles for a fuller effect)
    drawCircle3(x + 20, y + 50, 25, 0.0f, 0.6f, 0.0f); // Bottom foliage
    drawCircle3(x, y + 60, 20, 0.0f, 0.8f, 0.0f); // Left foliage
    drawCircle3(x + 40, y + 60, 20, 0.0f, 0.8f, 0.0f); // Right foliage
    drawCircle3(x + 20, y + 80, 30, 0.0f, 1.0f, 0.0f); // Top foliage
}





void drawCircle3(float x, float y) {

    // House body (Main rectangle)
    drawRectangle3(x, y, 80, 60, 0.8f, 0.5f, 0.3f); // Brown house body

    // Snow-covered roof (White overhang effect)
    glColor3f(1.0f, 1.0f, 1.0f); // White snow
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 10, y + 60); // Left corner (overhang)
    glVertex2f(x + 90, y + 60); // Right corner (overhang)
    glVertex2f(x + 40, y + 90); // Top peak
    glEnd();

    // Snow pile effect at the base of the house
    glColor3f(1.0f, 1.0f, 1.0f); // Snow color
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y); // Left side of the snow pile
    glVertex2f(x + 90, y); // Right side of the snow pile
    glVertex2f(x + 90, y - 10); // Right bottom corner of the snow pile
    glVertex2f(x - 10, y - 10); // Left bottom corner of the snow pile
    glEnd();

    // Chimney (Small rectangle on roof with snow on top)
    drawRectangle3(x + 50, y + 70, 15, 25, 0.6f, 0.3f, 0.2f); // Brown chimney
    glColor3f(1.0f, 1.0f, 1.0f); // Snow on chimney
    glBegin(GL_QUADS);
    glVertex2f(x + 50, y + 95); // Left top corner of snow
    glVertex2f(x + 65, y + 95); // Right top corner of snow
    glVertex2f(x + 65, y + 105); // Right bottom corner of snow
    glVertex2f(x + 50, y + 105); // Left bottom corner of snow
    glEnd();

    // Door with a snowy bottom edge
    drawRectangle3(x + 30, y, 20, 30, 0.5f, 0.3f, 0.2f); // Dark brown door
    glColor3f(1.0f, 1.0f, 1.0f); // Snow on door
    glBegin(GL_QUADS);
    glVertex2f(x + 30, y - 5); // Left side of the snow
    glVertex2f(x + 50, y - 5); // Right side of the snow
    glVertex2f(x + 50, y); // Right bottom corner of the snow
    glVertex2f(x + 30, y); // Left bottom corner of the snow
    glEnd();

    // Windows (Snow and frost effect on windows)
    drawRectangle3(x + 10, y + 35, 15, 15, 0.0f, 0.7f, 1.0f); // Left window (blue)
    drawRectangle3(x + 55, y + 35, 15, 15, 0.0f, 0.7f, 1.0f); // Right window (blue)

    // Frost effect (Light snowflakes around windows)
    glColor3f(0.9f, 0.9f, 0.9f); // Light frost color
    glBegin(GL_POINTS);
    glVertex2f(x + 15, y + 45); // Frost on the left window
    glVertex2f(x + 25, y + 45); // Frost on the left window
    glVertex2f(x + 60, y + 45); // Frost on the right window
    glVertex2f(x + 70, y + 45); // Frost on the right window
    glEnd();
}

void drawMultipleHouses() {
    // Leftmost houses
    drawCircle3(50, 100);  // First house (left)
    drawCircle3(50, 150);  // Second house (left)
    drawCircle3(50, 200);  // Third house (left)

    // Rightmost houses
    drawCircle3(50, 650);  // First house (right)
    drawCircle3(50, 700);  // Second house (right)
}



/*void drawBush2(float x, float y) {
    // Three circles to form a bush
    drawCircle3(x, y, 20, 0.0f, 0.5f, 0.0f);
    drawCircle3(x + 15, y + 5, 18, 0.0f, 0.6f, 0.0f);
    drawCircle3(x - 15, y + 5, 18, 0.0f, 0.6f, 0.0f);
}*/




struct Snowflake {
    float x, y;
    float speed;
};

const int numSnowflakes = 300;
Snowflake snowflakes[numSnowflakes];

void initSnowflakes() {
    for (int i = 0; i < numSnowflakes; i++) {
        snowflakes[i].x = rand() % windowWidth3;
        snowflakes[i].y = rand() % windowWidth3;
        snowflakes[i].speed = 1 + (rand() % 3); // Different speeds for variation
    }
}

void drawSnowflakes() {
    glColor3f(1.0, 1.0, 1.0); // White snowflakes
    glPointSize(3);
    glBegin(GL_POINTS);
    for (int i = 0; i < numSnowflakes; i++) {
        glVertex2f(snowflakes[i].x, snowflakes[i].y);
    }
    glEnd();
}

void updateSnowflakes() {
    for (int i = 0; i < numSnowflakes; i++) {
        snowflakes[i].y -= snowflakes[i].speed;
        if (snowflakes[i].y < 0) {
            snowflakes[i].y = windowWidth3;
            snowflakes[i].x = rand() % windowWidth3;
        }
    }
}



void drawSnowPile(float x, float y) {
    glColor3f(1.0, 1.0, 1.0); // White snow
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x - 30, y - 40);
    glVertex2f(x + 30, y - 40);
    glEnd();
}


void drawFrozenLake(float x, float y) {
    glColor3f(0.6, 0.8, 1.0); // Light blue ice color
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 30) {
        float angle = i * 3.14159 / 180;
        glVertex2f(x + cos(angle) * 50, y + sin(angle) * 30);
    }
    glEnd();
}

void drawIcePatch(float x, float y) {
    glColor4f(0.7, 0.9, 1.0, 0.6); // Light blue with transparency
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + 10, y + 8);
    glVertex2f(x + 40, y + 20);
    glVertex2f(x + 50, y + 30);
    glVertex2f(x, y + 50);
    glVertex2f(x - 10, y + 10);
    glEnd();
}


void drawIcicles(float x, float y) {
    glColor3f(0.7, 0.9, 1.0); // Light blue ice color
    glBegin(GL_TRIANGLES);

    for (int i = 0; i < 5; i++) { // Create multiple icicles in a row
        glVertex2f(x + i * 15, y);
        glVertex2f(x + i * 15 - 5, y - 20);
        glVertex2f(x + i * 15 + 5, y - 20);
    }

    glEnd();
}


void drawSignboard(float x, float y) {
    // Draw post
    glColor3f(0.6, 0.3, 0.1); // Brown color
    glBegin(GL_QUADS);
    glVertex2f(x - 5, y - 20);
    glVertex2f(x + 5, y - 20);
    glVertex2f(x + 5, y);
    glVertex2f(x - 5, y);
    glEnd();

    // Draw board
    glColor3f(0.9, 0.9, 1.0); // Light icy color
    glBegin(GL_QUADS);
    glVertex2f(x - 25, y);
    glVertex2f(x + 25, y);
    glVertex2f(x + 25, y + 15);
    glVertex2f(x - 25, y + 15);
    glEnd();
}


void drawSnowman(float x, float y) {
    glColor3f(1.0, 1.0, 1.0); // White snow

    // Draw body
    drawCircle3(x, y);       // Bottom
    drawCircle3(x, y + 20);  // Middle
    drawCircle3(x, y + 35);  // Head

    // Eyes
    glColor3f(0, 0, 0);
    glPointSize(3);
    glBegin(GL_POINTS);
    glVertex2f(x - 3, y + 40);
    glVertex2f(x + 3, y + 40);
    glEnd();
}


void drawCrackedIce(float x, float y) {
    glColor3f(0.7, 0.8, 1.0); // Light blue ice cracks
    glLineWidth(2);
    glBegin(GL_LINES);

    glVertex2f(x, y);
    glVertex2f(x + 20, y + 10);

    glVertex2f(x + 20, y + 10);
    glVertex2f(x + 40, y);

    glVertex2f(x + 40, y);
    glVertex2f(x + 60, y + 15);

    glEnd();
}

/*void drawSnowyMountains() {
    glColor3f(0.8, 0.8, 0.9); // Light grayish-white for mountains

    glBegin(GL_TRIANGLES);
    // First mountain
    glVertex2f(50, 200);
    glVertex2f(150, 350);
    glVertex2f(250, 200);

    // Second mountain
    glVertex2f(200, 180);
    glVertex2f(350, 400);
    glVertex2f(500, 180);

    // Third mountain
    glVertex2f(400, 150);
    glVertex2f(600, 350);
    glVertex2f(800, 150);
    glEnd();

    // Add snowy peaks
    glColor3f(1.0, 1.0, 1.0); // White for snowcaps
    glBegin(GL_TRIANGLES);

    glVertex2f(150, 350);
    glVertex2f(130, 300);
    glVertex2f(170, 300);

    glVertex2f(350, 400);
    glVertex2f(330, 350);
    glVertex2f(370, 350);

    glVertex2f(600, 350);
    glVertex2f(580, 300);
    glVertex2f(620, 300);

    glEnd();
}*/




void drawRoad3() {
    // Draw snowy areas (background)
    glColor3f(0.9, 0.9, 1.0); // Light blueish white for snow
    glBegin(GL_QUADS);

    // Left snow area
    glVertex2f(0, 0);
    glVertex2f((windowWidth3 - roadWidth3) / 2 - 30, 0);
    glVertex2f((windowWidth3 - roadWidth3) / 2 - 30, windowWidth3);
    glVertex2f(0, windowWidth3);

    // Right snow area
    glVertex2f((windowWidth3 + roadWidth3) / 2 + 30, 0);
    glVertex2f(windowWidth3, 0);
    glVertex2f(windowWidth3, windowWidth3);
    glVertex2f((windowWidth3 + roadWidth3) / 2 + 30, windowWidth3);

    glEnd();

    // Draw the road (slightly icy look)
    glColor3f(0.6, 0.6, 0.7); // Darker icy shade
    glBegin(GL_QUADS);
    glVertex2f((windowWidth3 - roadWidth3) / 2.37, 0);
    glVertex2f((windowWidth3 + roadWidth3) / 1.9, 0);
    glVertex2f((windowWidth3 + roadWidth3) / 1.9, windowWidth3);
    glVertex2f((windowWidth3 - roadWidth3) / 2.37, windowWidth3);
    glEnd();

    // Lane markings (dashed, slightly faded for snow effect)
    glColor3f(0.8, 0.8, 0.8);
    glLineWidth(4);
    glBegin(GL_LINES);
    for (float y = 0; y < windowWidth3; y += 80) {
        glVertex2f(windowWidth3 / 2, y);
        glVertex2f(windowWidth3 / 2, y + 40);
    }
    glEnd();

    // Snow piles on the sides
    float snowSpacing = 110;
    for (float y = windowWidth3 - 100; y > 0; y -= snowSpacing) {
        drawSnowPile((windowWidth3 - roadWidth3) / 2 - 100, y);
        drawSnowPile((windowWidth3 + roadWidth3) / 2 + 80, y);
    }

    // Ice patches on the road
    float icePatchSpacing = 200;
    for (float y = windowWidth3 - 100; y > 0; y -= icePatchSpacing) {
        drawIcePatch(windowWidth3 / 2 - 50, y);
    }

    // Frozen lakes on the far sides
    drawFrozenLake(100, windowWidth3 / 3);
    drawFrozenLake(windowWidth3 - 90, 2 * windowWidth3 / 3);

    // Icicles hanging from top
    drawIcicles(50, windowWidth3 - 20);
    drawIcicles(windowWidth3 - 100, windowWidth3 - 20);

    // Snowy mountains in the background
    //drawSnowyMountains();

    // Frozen signboards
    drawSignboard(120, windowWidth3 - 80);
    drawSignboard(windowWidth3 - 130, windowWidth3 - 80);

    // Snowmen on ice
    drawSnowman(80, windowWidth3 / 2);
    drawSnowman(windowWidth3 - 100, windowWidth3 / 2);

    // Cracked ice effect
    drawCrackedIce(100, windowWidth3 / 3);
    drawCrackedIce(windowWidth3 - 150, 2 * windowWidth3 / 3);
}





















void drawObstacle3(float x, float y, int type) {
    switch (type) {
        case 0: // Traffic cone
            drawRectangle3(x + carWidth3 / 4, y, carWidth3 / 2, carHeight3 / 8, 1.0f, 0.5f, 0.0f); // Orange base
            drawRectangle3(x + carWidth3 / 4 + 5, y + carHeight3 / 8, carWidth3 / 2 - 10, carHeight3 / 8, 1.0f, 0.6f, 0.0f); // Lighter orange middle
            drawCircle3(x + carWidth3 / 2, y + carHeight3 / 4 + 10, carWidth3 / 8, 1.0f, 0.7f, 0.0f); // Circular tip
            break;
        case 1: // Barrel
            drawRectangle3(x, y, carWidth3 / 2, carHeight3 / 2, 0.8f, 0.4f, 0.0f); // Brown barrel
            drawRectangle3(x, y + carHeight3 / 8, carWidth3 / 2, carHeight3 / 16, 1.0f, 1.0f, 0.0f); // Yellow stripe
            break;
        case 2: // Roadblock
            drawRectangle3(x, y, carWidth3, carHeight3 / 4, 0.5f, 0.5f, 0.5f); // Gray base
            drawRectangle3(x, y + carHeight3 / 8, carWidth3, carHeight3 / 16, 1.0f, 0.0f, 0.0f); // Red stripe
            break;
    }
}

void drawobstacles3() {
    for (const auto &obs : obstacles3) {
        drawObstacle3(obs.x, obs.y, obs.type);
    }
}






// Function to update obstacles
void updateobstacles3() {
    for (auto it = obstacles3.begin(); it != obstacles3.end();) {
        it->y -= 8;

        // Check collision with player car
        if (it->y < playerCarY3 + carHeight3 && it->y + carHeight3 / 2 > playerCarY3 &&
            it->x < playerCarX3 + carWidth3 && it->x + carWidth3 > playerCarX3) {
            gameOver3 = true;
            return;
        }

        // Remove obstacles that go off-screen
        if (it->y + carHeight3 / 2 < 0) {
            it = obstacles3.erase(it);
        } else {
            ++it;
        }
    }
}

void spawnObstacle3() {
    // Track occupied lanes (only lanes 1 and 2)
    bool occupied[2] = {false, false}; // Only track lanes 1 and 2
    int occupiedCount = 0;

    for (const auto &obstacle : obstacles3) {
        for (int i = 1; i <= 2; ++i) { // Check only lanes 1 and 2
            if (fabs(obstacle.x - lanePositions3[i]) < laneWidth3 / 2) {
                occupied[i - 1] = true; // Map lane[1] to occupied[0], lane[2] to occupied[1]
                break;
            }
        }
    }

    // Count occupied lanes in lane[1] and lane[2]
    for (int i = 0; i < 2; ++i) {
        if (occupied[i]) occupiedCount++;
    }

    // Only spawn if fewer than 2 lanes are occupied
    if (occupiedCount < 2) {
        int lane;
        do {
            lane = rand() % 2 + 1; // Randomly select lane[1] or lane[2]
        } while (occupied[lane - 1]);

        float x = lanePositions3[lane];
        int type = rand() % 3; // Random type (0, 1, 2)
        obstacles3.push_back({x, windowWidth3, type});
    }
}


void spawnOpponentCar3() {
    // Track occupied lanes for lanes 1 and 2
    bool occupied[2] = {false, false}; // Track lane 1 and lane 2
    int occupiedCount = 0;

    // Check which lanes are occupied by existing opponent cars
    for (const auto &car2 : opponentCars3) {
        for (int i = 1; i <= 2; ++i) {  // Check only lanes 1 and 2
            if (fabs(car2.x - lanePositions3[i]) < laneWidth3 / 2) {
                occupied[i - 1] = true;  // Adjust for zero-based indexing in occupied
                break;
            }
        }
    }

    // Count occupied lanes
    for (int i = 0; i < 2; ++i) {
        if (occupied[i]) occupiedCount++;
    }

    // Only spawn if fewer than 2 lanes are occupied
    if (occupiedCount < 2) {
        int lane;
        do {
            // Randomly select lane 1 or lane 2 (indexes 1 and 2)
            lane = 1 + rand() % 2;  // Randomly select either lane 1 or lane 2
        } while (occupied[lane - 1]);  // Ensure the selected lane is not occupied

        float x = lanePositions3[lane]; // Select the lane's x position
        opponentCars3.push_back({x, windowWidth3}); // Spawn the car at the top
    }
}

void updateopponentCars3() {
    for (auto it = opponentCars3.begin(); it != opponentCars3.end();) {
        it->y -= 10;

        // Check collision
        if (it->y < playerCarY3 + carHeight3 && it->y + carHeight3 > playerCarY3 &&
            it->x < playerCarX3 + carWidth3 && it->x + carWidth3 > playerCarX3) {
            gameOver3 = true;
            return;
        }

        // Remove cars that go out of screen
        if (it->y + carHeight3 < 0) {
            it = opponentCars3.erase(it);
            score2++;
        } else {
            ++it;
        }
    }
}
int obstacleSpawnCounter3 = 100;

// Modify update function to include obstacles
int carSpawnCounter3 = 50; // Initialize a spawn delay for cars (you can adjust this value)

void update3(int value) {
    if (!gameOver3) {
        updateopponentCars3();
        updateobstacles3();

        // Control the frequency of opponent car spawning
        if (carSpawnCounter3 <= 0) {
            spawnOpponentCar3();  // Spawn an opponent car
            carSpawnCounter3 = 50; // Reset to a higher value (longer delay) to control the spawn frequency
        } else {
            carSpawnCounter3--;  // Decrease the counter
        }

        // Decrease the spawn counter for obstacles and spawn them less frequently
        if (obstacleSpawnCounter3 <= 0) {
            spawnObstacle3();
            obstacleSpawnCounter3 = 200;  // Reset to a higher value for obstacles
        } else {
            obstacleSpawnCounter3--;
        }

        glutPostRedisplay();
        glutTimerFunc(16, update3, 0);
    }
}



void handleKeypress3(int key, int x, int y) {
    if (!gameOver3) {
        int currentLane = -1;

        // Identify the current lane
        for (int i = 0; i < 4; ++i) {
            if (fabs(playerCarX3 - lanePositions3[i]) < laneWidth3 / 2) {
                currentLane = i;
                break;
            }
        }

        // Allow movement only between center-left (lane 1) and center-right (lane 2)
        if (key == GLUT_KEY_LEFT && currentLane == 2) {
            playerCarX3 = lanePositions3[1]; // Move to center-left lane
        }
        else if (key == GLUT_KEY_RIGHT && currentLane == 1) {
            playerCarX3 = lanePositions3[2]; // Move to center-right lane
        }

        glutPostRedisplay();
    }
}

void resetGame3() {
    playerCarX3 = lanePositions3[1];
    opponentCars3.clear();
    obstacles3.clear(); // Clear obstacles3
    score3 = 0;
    gameOver3 = false;

    glutTimerFunc(16, update3, 0);
}

void handleNormalKeypress3(unsigned char key, int x, int y) {
    if (gameOver3 && (key == 'r' || key == 'R')) {
        resetGame3();
        glutPostRedisplay();
    }

    if (key == 'x' || key == 'X') {
        exit(0);
    }

}

void display3() {
        glutSpecialFunc(handleKeypress3);

    drawRoad3();
    drawPlayerCar3();
    drawopponentCars3();
    initSnowflakes();  // Call this once at the beginning
    drawSnowflakes();  // Call inside draw function
    updateSnowflakes(); // Call inside update function
    drawMultipleHouses();
    drawobstacles3();

    if (gameOver3) {
        glColor3f(1, 0, 0);
        glRasterPos2f(windowWidth3 / 2 - 50, windowWidth3 / 2);
        const char *message = "Game Over! Press 'R' to Restart.";
        for (const char *c = message; *c != '\0'; ++c) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    glColor3f(0.5, 0.4, 1);
    glRasterPos2f(18, windowWidth3 - 230);
    char score2Str[20];
    sprintf(score2Str, "Score: %d", score2);
    for (const char *c = score2Str; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glutSwapBuffers();
}
















void level3()
{
    menuVisible=false;
    displayMode=3;
    toggleDisplay();
    display3();

}


void level2()
{
    menuVisible=false;
    displayMode=2;
    toggleDisplay();
    display2();

}









void level1()
{
    menuVisible=false;
    displayMode = 1;  // Tracks current display mode (0 to 3)

    toggleDisplay();
    display1();

}























void drawRainScene() {

}

void drawMenu() {

    // Example: Draw a semi-transparent background for the menu.
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(300, 440);
    glVertex2f(700, 440);
    glVertex2f(700, 240);
    glVertex2f(300, 240);
    glEnd();

glColor4f(0.031f, 0.024f, 0.337f, 0.91f);
    glBegin(GL_QUADS);
    glVertex2f(280, 510);
    glVertex2f(20, 510);
    glVertex2f(20, 90);
    glVertex2f(280, 90);
    glEnd();

    // Draw the game title.
    drawText(460,480, "TURBO RUSH", 0.0f, 0.0f, 0.5f);
    drawText(40,480, "SECTION: M", 0.0f, 1.0f, 1.0f);
    drawText(100,430, "   GROUP MEMBERS", 0.0f, 1.0f, 1.0f);
    drawText(40,390, "SERIAL          NAME                                    ID", 0.0f, 1.0f, 1.0f);
    drawText(40,350, "21       MONJURUL ISLAM NIMON     23-50004-1", 0.0f, 1.0f, 1.0f);
    drawText(40,310, "22       PRITHIRAJ MADOK                 23-50007-1", 0.0f, 1.0f, 1.0f);
    drawText(40,270, "28       NISHAN AHAMED                     23-50040-1", 0.0f, 1.0f, 1.0f);
    drawText(40,230, "27       MD.SADMAN SAKIB                23-50039-1  ", 0.0f, 1.0f, 1.0f);
    drawText(40,190, "26       ARIANA RAHMAN LAZUK        23-50019-1 ", 0.0f, 1.0f, 1.0f);
    drawText(100,130, "    Supervised By ", 0.0f, 1.0f, 1.0f);
    drawText(90,110, "    NOBORANJAN DEY", 0.0f, 1.0f, 1.0f);
    drawText(310,410, "                       (Press ''UP key'' or ''DOWN key'' to select the game mode)", 0.941f, 0.471f, 0.471f);
    drawText(310,390, "                                      (Press ''ENTER key'' to start the game)", 0.941f, 0.471f, 0.471f);
    drawText(410,160, "Rules after pressing ''ENTER'' key", 0.200f, 0.137f, 0.176f);
    drawText(410,120, " Enter ''r'' or ''R'' to reset the game", 0.200f, 0.137f, 0.176f);
    drawText(410,100, "  Enter ''x'' or''X'' to exit the game", 0.200f, 0.137f, 0.176f);

    // Draw the menu options.
    for (int i = 0; i < 3; ++i) {
        if (i == menuIndex) {
            drawText(450, 360 - i * 50, menuOptions[i], 1.0f, 1.0f, 0.0f);  // Highlight selected option.
        } else {
            drawText(450, 360 - i * 50, menuOptions[i], 1.0f, 1.0f, 1.0f);
        }
    }

}

void display() {
    // Clear the screen every frame.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If the menu is visible or if no mode has been chosen, draw the menu.
    if (menuVisible || currentMode == static_cast<Mode>(-1)) {
        drawMenu();  // Draw your menu background, title, and options.
    } else {
        // Depending on the mode, draw the correct scene.
        switch (currentMode) {
            case DAY:
                level1();  // For example, this can call level1() or the drawing functions for gameplay.
                break;
            case NIGHT:
                level2();
                break;
            case RAIN:
                level3();
                break;
        }
    }

    glutSwapBuffers();
}


void handleCombinedKeypress(unsigned char key, int x, int y) {
    // Toggle the menu regardless of game mode.
    if (key == 'm' || key == 'M') {
        menuVisible = !menuVisible;
        glutPostRedisplay();
        return;
    }

    // If the menu is visible, only process menu navigation.
    if (menuVisible) {
        if (key == 13) {  // Enter key
            currentMode = static_cast<Mode>(menuIndex);
            menuVisible = false;
            glutPostRedisplay();
        }
        return;
    }

    // Otherwise, if in game mode, process game keys.
    if (gameOver && (key == 'r' || key == 'R')) {
        resetGame1();
        resetGame2();
        resetGame3();
        glutPostRedisplay();
        return;
    }
    if (key == 'x' || key == 'X') {
        exit(0);
    }
}







void handleKeypress(unsigned char key, int x, int y) {
    if (key == 'm' || key == 'M') {
        menuVisible = !menuVisible;
    } else if (key == 13 && menuVisible) { // Enter key
        currentMode = static_cast<Mode>(menuIndex);
        menuVisible = true;

    }
}

void handleSpecialKeypress(int key, int x, int y) {
    if (menuVisible) {
        if (key == GLUT_KEY_UP) {
            menuIndex = (menuIndex + 2) % 3;
            glutPostRedisplay();
        } else if (key == GLUT_KEY_DOWN) {
            menuIndex = (menuIndex + 1) % 3;
            glutPostRedisplay();
        }
    } else {
        if (!gameOver) {
            int currentLane = -1;
            for (int i = 0; i < 4; ++i) {
                if (fabs(playerCarX1 - lanePositions1[i]) < laneWidth1 / 2) {
                    currentLane = i;
                    break;
                }
            }
            if (key == GLUT_KEY_LEFT && currentLane > 0) {
                currentLane--;
                playerCarX1 = lanePositions1[currentLane];
            } else if (key == GLUT_KEY_RIGHT && currentLane < 3) {
                currentLane++;
                playerCarX1 = lanePositions1[currentLane];
            }
            glutPostRedisplay();
        }
    }
}



void init() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, windowWidth1, 0, windowHeight1);
}


void toggleDisplay() {
   // displayMode = (displayMode + 1) % 4;  // Cycle through 4 display modes

    switch (displayMode) {
        case 0:
            glutDisplayFunc(display);  // First display function
            break;
        case 1:
            glutDisplayFunc(display1); // Second display function
            break;
        case 2:
            glutDisplayFunc(display2); // Third display function
            break;
       case 3:
           glutDisplayFunc(display3); // Fourth display function
            break;
    }

    glutPostRedisplay(); // Force a redraw
}




int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Initialize window size with default values first
    glutInitWindowSize(windowWidth1, windowHeight1);


    // Create the window
    glutCreateWindow("Mood Switcher");

    // Switch to fullscreen
    glutFullScreen();

    init();

    glutDisplayFunc(display);
    //glutIdleFunc(display);


    //glutKeyboardFunc(handleKeypress);
    //glutKeyboardFunc(handleKeypress1);
    glutSpecialFunc(handleSpecialKeypress);
    //glutKeyboardFunc(handleNormalKeypress);
    glutKeyboardFunc(handleCombinedKeypress);
    //glutKeyboardFunc(keyboardCallback);
    glutTimerFunc(16, update1, 0);
        glutTimerFunc(16, update2, 0);
        glutTimerFunc(16, update3, 0);


    glutMainLoop();
    return 0;
}

