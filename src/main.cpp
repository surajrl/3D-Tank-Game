// Includes
#include <GL/glew.h>
#include <GL/glut.h>
#include "Shader.h"
#include "Vector.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Texture.h"
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#define PI 3.14159265358979323846

// ------------------------------- FUNCTION PROTOTYPES ------------------------------- //

bool initGL(int argc, char** argv);
void initShader();
void initTexture(std::string filename, GLuint& textureID);
bool loadMap(std::string filename);
void display(void);
void Timer(int value);

// Keyboard Interaction
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void handleKeys();

// Mouse Interaction
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

// Collision Detection
bool AABBintersectAABB(Mesh& mesh, Vector3f max, Vector3f min);
bool AABBintersectPoint(Mesh& mesh, Vector3f point);
bool AABBintersectLine(Mesh& mesh, Vector2f line);

// 2D Text
void render2dText(std::string text, float r, float g, float b, float x, float y);

// ------------------------------- GLOBAL VARIABLES ------------------------------- //

// Screen Size
int screenWidth = 720;
int screenHeight = 720;

GLuint textureCube;             // Texture for Cube
GLuint textureCoin;             // Texture for Coin
GLuint textureBall;             // Texture for Ball
GLuint textureTank;             // Texture for Tank

GLuint shaderProgramID;

GLuint vertexPositionAttribute;         // Vertex Position Attribute Location
GLuint vertexNormalAttribute;           // Vertex Normal Attribute Location

GLuint vertexTexCoordAttribute;         // Vertex Texture Coordinate Attribute Location
GLuint TextureMapUniformLocation;       // Texture Map Uniform Location

Matrix4x4 ModelViewMatrix;              // Model View Matrix
GLuint ModelViewMatrixUniformLocation;  // Model View Matrix Uniform Location

Matrix4x4 ProjectionMatrix;             // Projection Matrix
GLuint ProjectionMatrixUniformLocation; // Projection Matrix Uniform Location

// Maze Map
std::string mapFile = "levels/level1.txt";
std::vector<std::vector<int>> map;
float timeLimit = 60.0;

// Game State
bool gameOver = false;
bool restart = false;
float timeRemaining = timeLimit;


// Timing
float deltaTime;
float currentTime;
float lastTime;

// Physics
Vector3f gravity = Vector3f(0.0, -0.0001, 0.0);
const float groundY = 14.5;

// Array of Key States
bool keyStates[256];

// Camera
Vector3f cameraPosition;
Vector3f cameraTarget;
Vector3f cameraUp = Vector3f(0.0, 1.0, 0.0);

float distanceFromTank = 20.0;
float cameraTiltDegrees;
float cameraTiltRadians;
float cameraPanDegrees;
float cameraPanRadians;

bool thirdPersonCamera = true;
bool firstPersonCamera = false;
bool freeThirdPersonCamera = false;

// Mouse Interaction
int previousMousePositionX = 0.0;
int previousMousePositionY = 0.0;

int currentButton = 0;
int currentState = 0;

bool reset;

// Coins
float coinRotation;
int coinsRemaining = 0;
std::vector<Vector3f> collisionPoints;

// Tank
Vector3f tankPosition;
Vector3f tankVelocity;
Vector3f tankForce;
float tankMass = 48000.0;

float tankRotationDegrees;
float tankRotationRadians;
float turretRotationDegrees;
float turretRotationRadians;

bool tankFalling = false;
bool tankSet = false;

// Ball
Vector3f ballPosition;
Vector3f ballVelocity;
Vector3f ballForce;
const float ballMass = 1.0;

float ballRotationDegrees;

bool launchBall = false;

// Lighting - Phong Reflection Model
GLuint LightPositionUniformLocation;
GLuint AmbientUniformLocation;
GLuint SpecularUniformLocation;
GLuint SpecularPowerUniformLocation;

Vector3f lightPosition;
bool lightSet = false;

// Mesh Object
Mesh meshCube;
Mesh meshCoin;
Mesh meshBall;
Mesh meshChassis;
Mesh meshBackWheel;
Mesh meshFrontWheel;
Mesh meshTurret;

float aabbOffset = 3.0;

// ------------------------------- MAIN PROGRAM ENTRY ------------------------------- //
int main(int argc, char** argv)
{
    // Initialise OpenGL
    if (!initGL(argc, argv))
        return -1;

    // Initialise Key States to false
    for (int i = 0; i < 256; i++)
        keyStates[i] = false;

    // Load Map from File
    if (!loadMap(mapFile))
        return -1;

    // Initialise OpenGL Shader
    initShader();

    // Intialise Mesh Geometry
    meshCube.loadOBJ("models/cube.obj");
    meshCoin.loadOBJ("models/coin.obj");
    meshBall.loadOBJ("models/ball.obj");
    meshChassis.loadOBJ("models/chassis.obj");
    meshBackWheel.loadOBJ("models/back_wheel.obj");
    meshFrontWheel.loadOBJ("models/front_wheel.obj");
    meshTurret.loadOBJ("models/turret.obj");

    // Initialise Textures
    initTexture("models/cube.bmp", textureCube);
    initTexture("models/coin.bmp", textureCoin);
    initTexture("models/ball.bmp", textureBall);
    initTexture("models/tank.bmp", textureTank);
    
    // Sets Tank Position, Light Position and Counts Coins
    for (int z = 0; z < map.size(); z++) {
        for (int x = 0; x < map[z].size(); x++) {
            if (map[z][x] == 1 && !tankSet)
            {
                tankPosition = Vector3f(x * 2 * 15, groundY, z * 2 * 15);
                tankSet = true;
            }
            if (map[z][x] == 2)
            {
                coinsRemaining++;
            }
            if (z != 0 && x != 0 && !lightSet)
            {
                lightPosition = Vector3f(x * 2 * 15, groundY, z * 2 * 15);
                lightSet = true;
            }
        }
    }

    // Enter Main Loop
    glutMainLoop();

    // Delete Shader Program
    glDeleteProgram(shaderProgramID);

    return 0;
}

// ------------------------------- FUNCTION TO INITIALISE OPENGL ------------------------------- //
bool initGL(int argc, char** argv)
{
    // Initialise GLUT
    glutInit(&argc, argv);

    // Set Display Mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

    // Set Window Size
    glutInitWindowSize(screenWidth, screenHeight);

    // Set Window Position
    glutInitWindowPosition(200, 200);

    // Create Window
    glutCreateWindow("Tank Game");

    // Initialise GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    // Set Display Function
    glutDisplayFunc(display);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // Set Keyboard Interaction Functions
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);

    // Set Mouse Interaction Functions
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(motion);
    glutMotionFunc(motion);
    
    // Start Timer Function After 100 milliseconds
    glutTimerFunc(100, Timer, 0);
    
    return true;
}

// ------------ FUNCTION TO LOAD MAP FROM TEXTFILE ------------ //
bool loadMap(std::string filename)
{
    std::ifstream inputFile(filename);
    std::string tempString;
    int tempInt;

    std::vector<std::vector<int>> tempMap;

    if (!inputFile.is_open())
        return 0;

    while (std::getline(inputFile, tempString))
    {
        std::istringstream iss(tempString);
        std::vector<int> rows;

        while (iss >> tempInt)
        {
            rows.push_back(tempInt);
        }
        tempMap.push_back(rows);
    }

    map = tempMap;

    return 1;
}


// ------------------------------- FUNCTION TO INITIALISE SHADERS ------------------------------- //
void initShader()
{
    // Create Shader
    shaderProgramID = Shader::LoadFromFile("shaders/shader.vert", "shaders/shader.frag");

    // Attribute Locations
    vertexPositionAttribute = glGetAttribLocation(shaderProgramID, "aVertexPosition");
    vertexNormalAttribute = glGetAttribLocation(shaderProgramID, "aVertexNormal");
    vertexTexCoordAttribute = glGetAttribLocation(shaderProgramID, "aVertexTexCoord");
    
    // Uniform Locations
    ModelViewMatrixUniformLocation = glGetUniformLocation(shaderProgramID, "ModelViewMatrix_uniform");
    ProjectionMatrixUniformLocation = glGetUniformLocation(shaderProgramID, "ProjectionMatrix_uniform");

    TextureMapUniformLocation = glGetUniformLocation(shaderProgramID, "TextureMap_uniform");

    LightPositionUniformLocation = glGetUniformLocation(shaderProgramID, "LightPosition_uniform");

    AmbientUniformLocation = glGetUniformLocation(shaderProgramID, "Ambient_uniform");
    SpecularUniformLocation = glGetUniformLocation(shaderProgramID, "Specular_uniform");
    SpecularPowerUniformLocation = glGetUniformLocation(shaderProgramID, "SpecularPower_uniform");
}

// ------------------------------- FUNCTION TO INITIALISE TEXTURES ------------------------------- //
void initTexture(std::string filename, GLuint& textureID)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Get Texture Data
    int width, height;
    char* data;
    Texture::LoadBMP(filename, width, height, data);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        data
    );

    // Cleanup Data as Copied to GPU
    delete[] data;
}

// Collision Detection Algorithms
bool AABBintersectAABB(Mesh& mesh, Vector3f max, Vector3f min)
{
    return
        (mesh.transformedMin.x <= max.x && mesh.transformedMax.x >= min.x) &&
        (mesh.transformedMin.y <= max.y && mesh.transformedMax.y >= min.y) &&
        (mesh.transformedMin.z <= max.z && mesh.transformedMax.z >= min.z);
}

bool AABBintersectPoint(Mesh& mesh, Vector3f point)
{
    return
        (point.x >= mesh.transformedMin.x && point.x <= mesh.transformedMax.x) &&
        (point.y >= mesh.transformedMin.y && point.y <= mesh.transformedMax.y) &&
        (point.z >= mesh.transformedMin.z && point.z <= mesh.transformedMax.z);
}
bool AABBintersectLine(Mesh& mesh, Vector2f line)
{
    return
        (line.x >= mesh.transformedMin.x && line.x <= mesh.transformedMax.x) &&
        (line.y >= mesh.transformedMin.z && line.y <= mesh.transformedMax.z);
}

// ------------------------------- DISPLAY LOOP ------------------------------- //
void display(void)
{
    // Handle Keys
    handleKeys();

    // Set Viewport
    glViewport(0, 0, screenWidth, screenHeight);

    // Clear the Screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    // Set Background Colour
    glClearColor(0.086, 0.603, 0.854, 1.0);

    // Use Shader
    glUseProgram(shaderProgramID);

    // Convert Degrees to Radians
    tankRotationRadians = (tankRotationDegrees * PI) / 180;
    turretRotationRadians = (turretRotationDegrees * PI) / 180;
    cameraPanRadians = (cameraPanDegrees * PI) / 180;
    cameraTiltRadians = (cameraTiltDegrees * PI) / 180;

    // Calculate Delta Time
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Projection Matrix - Perspective Projection
    ProjectionMatrix.perspective(90, 1.0, 0.1, 1000.0);
    glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, false, ProjectionMatrix.getPtr());

    // Set Lighting of Scene
    glUniform3f(LightPositionUniformLocation, lightPosition.x, lightPosition.y, lightPosition.z);

    // Restart the Game
    if (restart && gameOver)
    {      
        // Reset Map
        loadMap(mapFile);

        // Reset Time and Coins Count
        timeRemaining = timeLimit;
        coinsRemaining = 0;

        // Reset Tank Position and Count Coins
        tankSet = false;
        for (int z = 0; z < map.size(); z++) {
            for (int x = 0; x < map[z].size(); x++) {
                if (map[z][x] == 1 && !tankSet)
                {
                    tankPosition = Vector3f(x * 2 * 15, groundY, z * 2 * 15);
                    tankSet = true;
                }
                if (map[z][x] == 2)
                    coinsRemaining++;
            }
        }

        restart = false;
        gameOver = false;
    }
    
    // Calculate Camera Position and Camera Target
    if (thirdPersonCamera)
    {
        // Update Camera Position
        cameraPosition.x = tankPosition.x - (10 * sin(turretRotationRadians));
        cameraPosition.y = tankPosition.y + 10;
        cameraPosition.z = tankPosition.z - (10 * cos(turretRotationRadians));

        // Update Camera Target
        cameraTarget.x = tankPosition.x;
        cameraTarget.y = tankPosition.y;
        cameraTarget.z = tankPosition.z;
    }
    if (firstPersonCamera)
    {
        // Update Camera Position
        cameraPosition.x = tankPosition.x;
        cameraPosition.y = tankPosition.y + 3.5;
        cameraPosition.z = tankPosition.z;

        // Update Camera Target
        cameraTarget.x = cameraPosition.x + sin(turretRotationRadians);
        cameraTarget.y = cameraPosition.y;
        cameraTarget.z = cameraPosition.z + cos(turretRotationRadians);
    }
    if (freeThirdPersonCamera)
    {
        // Update Camera Position
        cameraPosition.x = tankPosition.x - (distanceFromTank * sin(cameraPanRadians) * cos(cameraTiltRadians));
        cameraPosition.y = tankPosition.y + (distanceFromTank * sin(cameraTiltRadians));
        cameraPosition.z = tankPosition.z - (distanceFromTank * cos(cameraPanRadians) * cos(cameraTiltRadians));

        // Update Camera Target
        cameraTarget.x = tankPosition.x;
        cameraTarget.y = tankPosition.y;
        cameraTarget.z = tankPosition.z;
    }
    
    // Calculate Tank Position
    if (!gameOver)
    {
        // Update Total Tank Force
        tankForce.x += tankMass * gravity.x;
        tankForce.y += tankMass * gravity.y;
        tankForce.x += tankMass * gravity.x;

        // Update Tank Velocity
        tankVelocity.x += deltaTime * (tankForce.x / tankMass);
        tankVelocity.y += deltaTime * (tankForce.y / tankMass);
        tankVelocity.z += deltaTime * (tankForce.z / tankMass);

        // Update Tank Position
        tankPosition.x += deltaTime * tankVelocity.x * sin(tankRotationRadians);
        tankPosition.y += deltaTime * tankVelocity.y;
        tankPosition.z += deltaTime * tankVelocity.z * cos(tankRotationRadians);
    }

    // Draw Maze
    for (int z = 0; z < map.size(); z++) {
        for (int x = 0; x < map[z].size(); x++) {
            if (map[z][x] == 1 || map[z][x] == 2)
            {
                // Set Material Properties of Cube
                glUniform4f(AmbientUniformLocation, 0.1, 0.1, 0.1, 1.0);
                glUniform4f(SpecularUniformLocation, 0.0, 0.0, 0.0, 1.0);
                glUniform1f(SpecularPowerUniformLocation, 10);

                // Set Textures of Cube
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureCube);
                glUniform1i(TextureMapUniformLocation, 0);

                // Set Model View Matrix of Cube
                ModelViewMatrix.toIdentity();
                ModelViewMatrix.lookAt(
                    cameraPosition,
                    cameraTarget,
                    cameraUp
                );
                ModelViewMatrix.scale(15.0, 15.0, 15.0);
                ModelViewMatrix.translate(x * 2.0, 0.0, z * 2.0);
                glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, false, ModelViewMatrix.getPtr());

                // Draw Cube
                meshCube.draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexCoordAttribute);
            }
            if (map[z][x] == 2)
            {
                // Set Material Properties of Coin
                glUniform4f(AmbientUniformLocation, 0.24725, 0.1995, 0.0745, 1.0);
                glUniform4f(SpecularUniformLocation, 0.628281, 0.555802, 0.366065, 1.0);
                glUniform1f(SpecularPowerUniformLocation, 51.2);
           
                // Set Textures of Coin
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureCoin);
                glUniform1i(TextureMapUniformLocation, 0);

                // Set Model View Matrix of Coin
                ModelViewMatrix.toIdentity();
                ModelViewMatrix.lookAt(
                    cameraPosition,
                    cameraTarget,
                    cameraUp
                );
                ModelViewMatrix.scale(3.0, 3.0, 3.0);
                ModelViewMatrix.translate(x * 10.0, 7.0, z * 10.0);
                ModelViewMatrix.rotate(coinRotation, 0.0, 1.0, 0.0);
                glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, false, ModelViewMatrix.getPtr());

                // Check Tank Intersects with Coin
                if (AABBintersectAABB(
                    meshChassis,
                    Vector3f(meshCoin.max.x + x * 30.0, meshCoin.min.y + 18.0, meshCoin.max.z + z * 30.0),
                    Vector3f(meshCoin.min.x + x * 30.0, meshCoin.min.y + 18.0, meshCoin.min.z + z * 30.0))
                    )
                {
                    map[z][x] = 1;
                    coinsRemaining--;
                }
                
                // Check Ball Intersects with Coin
                if (AABBintersectAABB(
                    meshBall,
                    Vector3f(meshCoin.max.x + x * 30.0, meshCoin.min.y + 18.0, meshCoin.max.z + z * 30.0),
                    Vector3f(meshCoin.min.x + x * 30.0, meshCoin.min.y + 18.0, meshCoin.min.z + z * 30.0))
                    )
                {
                    map[z][x] = 1;
                    coinsRemaining--;
                    launchBall = false;
                }

                // Draw Coin 
                meshCoin.draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexCoordAttribute);
            }
        }
    } 

    // Detect Tank On Top of Cube
    for (int z = 0; z < map.size(); z++) {
        for (int x = 0; x < map[z].size(); x++) {
            if (map[z][x] != 0)
            {
                // Tank Inside of Cube
                if (tankPosition.x > ((x * 30) - 15) && tankPosition.x < ((x * 30) + 15) &&
                    tankPosition.z > ((z * 30) - 15) && tankPosition.z < ((z * 30) + 15))
                {
                    // Tank On Cube Surface
                    if (tankPosition.y < groundY)
                    {
                        tankPosition.y = groundY;
                        tankForce.y = 0.0;
                        tankVelocity.y = 0.0;
                        tankFalling = false;
                    }
                }

                // Tank Outside of Cube
                else if (tankPosition.y < groundY)
                    tankFalling = true;
               
            }
        }
    }

    // Set Material Properties of Tank
    glUniform4f(AmbientUniformLocation, 0.135, 0.2225, 0.1575, 0.95);
    glUniform4f(SpecularUniformLocation, 0.316228, 0.316228, 0.316228, 0.95);
    glUniform1f(SpecularPowerUniformLocation, 12.8);

    // Set Textures of Tank
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureTank);
    glUniform1i(TextureMapUniformLocation, 0);

    // Set Model View Matrix of Tank
    ModelViewMatrix.toIdentity();
    ModelViewMatrix.lookAt(
        cameraPosition,
        cameraTarget,
        cameraUp
    );
    ModelViewMatrix.translate(tankPosition.x, tankPosition.y, tankPosition.z);
    ModelViewMatrix.rotate(tankRotationDegrees, 0.0, 1.0, 0.0);
    glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, false, ModelViewMatrix.getPtr());

    // Update AABB of Tank after Transformations
    meshChassis.transformAABB(tankPosition, Vector3f(1.0, 1.0, 1.0));

    // Draw Tank
    meshChassis.draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexCoordAttribute);
    meshBackWheel.draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexCoordAttribute);
    meshFrontWheel.draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexCoordAttribute);

    // Set Model View Matrix of Turret
    ModelViewMatrix.toIdentity();
    ModelViewMatrix.lookAt(
        cameraPosition,
        cameraTarget,
        cameraUp
    );
    ModelViewMatrix.translate(tankPosition.x, tankPosition.y, tankPosition.z);
    ModelViewMatrix.rotate(turretRotationDegrees, 0.0, 1.0, 0.0);
    glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, false, ModelViewMatrix.getPtr());

    // Draw Turret
    meshTurret.draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexCoordAttribute);
    
    // Launch Ball
    if (launchBall)
    {
        // Update Ball Force
        ballForce.x += ballMass * gravity.x;
        ballForce.y += ballMass * gravity.y;
        ballForce.z += ballMass * gravity.z;

        // Update Ball Velocity
        ballVelocity.x += deltaTime * (ballForce.x / ballMass);
        ballVelocity.y += deltaTime * (ballForce.y / ballMass);
        ballVelocity.z += deltaTime * (ballForce.z / ballMass);

        // Update Ball Position
        ballPosition.x += deltaTime * ballVelocity.x * sin(turretRotationRadians);
        ballPosition.y += deltaTime * ballVelocity.y;
        ballPosition.z += deltaTime * ballVelocity.z * cos(turretRotationRadians);

        // Detect Ball On Top of Cube
        for (int z = 0; z < map.size(); z++) {
            for (int x = 0; x < map[z].size(); x++) {
                if (map[z][x] != 0)
                {
                    // Ball Inside of Cube
                    if (ballPosition.x > ((x * 30) - 15) && ballPosition.x < ((x * 30) + 15) &&
                        ballPosition.z > ((z * 30) - 15) && ballPosition.z < ((z * 30) + 15))
                    {
                        // Ball On Cube Surface
                        if (ballPosition.y < 15.5)
                        {
                            ballPosition.y = 15.5;
                            ballForce.y = 0.0;
                            ballVelocity.y = 0.0;
                        }      
                    }
                    
                    // Ball Outside of Cube and Below a Cube
                    else if (ballPosition.y < -30)
                        launchBall = false;
                }
            }
        }

        // Set Material Properties of Ball
        glUniform4f(AmbientUniformLocation, 0.02, 0.02, 0.02, 1.0);
        glUniform4f(SpecularUniformLocation, 0.4, 0.4, 0.4, 1.0);
        glUniform1f(SpecularPowerUniformLocation, 10.0);
        
        // Set Textures of Ball
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureBall);
        glUniform1i(TextureMapUniformLocation, 0);

        // Set Model View Matrix of Ball
        ModelViewMatrix.toIdentity();
        ModelViewMatrix.lookAt(
            cameraPosition,
            cameraTarget,
            cameraUp
        );       
        ModelViewMatrix.scale(0.5, 0.5, 0.5);
        ModelViewMatrix.translate(ballPosition.x * 2, ballPosition.y * 2, ballPosition.z * 2);       
        glUniformMatrix4fv(ModelViewMatrixUniformLocation, 1, false, ModelViewMatrix.getPtr());

        // Update AABB of Ball after Transformations
        meshBall.transformAABB(ballPosition * 2, Vector3f(0.5, 0.5, 0.5));

        // Draw Ball       
        meshBall.draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexCoordAttribute);        
    }
       
    // 2D Text
    char timeRemainingString[100];
    sprintf_s(timeRemainingString, "Time: %.2f", timeRemaining);

    char coinsRemainingString[100];
    sprintf_s(coinsRemainingString, "Coins Remaining: %d", coinsRemaining);

    render2dText(timeRemainingString, 1.0, 1.0, 1.0, -0.98, 0.92);
    render2dText(coinsRemainingString, 1.0, 1.0, 1.0, -0.98, 0.86);

    if (thirdPersonCamera)
        render2dText("Camera: Third Person Camera", 1.0, 1.0, 1.0, -0.98, 0.74);
    else if (firstPersonCamera)
        render2dText("Camera: First Person Camera", 1.0, 1.0, 1.0, -0.98, 0.74);
    else if (freeThirdPersonCamera)
        render2dText("Camera: Free Third Person Camera", 1.0, 1.0, 1.0, -0.98, 0.74);

    render2dText("Press 1 to change to First Person Camera", 1.0, 1.0, 1.0, -0.98, -0.50);
    render2dText("Press 2 to change to Free Third Person Camera", 1.0, 1.0, 1.0, -0.98, -0.56);
    render2dText("Press 3 to change to Thrid Person Camera", 1.0, 1.0, 1.0, -0.98, -0.62);
   

    // Check Player has Won or Lost
    if (coinsRemaining == 0 && timeRemaining > 0)
    {
        char wonMessage[100];
        sprintf_s(wonMessage, "You collected all the coins in %.2f seconds", 60.0 - timeRemaining);
        render2dText("YOU WON!", 0.0, 1.0, 0.0, -0.15, 0.5);
        render2dText(wonMessage, 0.0, 1.0, 0.0, -0.46, 0.44);
        render2dText("Press R to restart or ESC to exit the game", 1.0, 1.0, 1.0, -0.45, 0.32);
        gameOver = true;
        
    }
    if (timeRemaining == 0)
    {
        char gameOverMessage[100];
        sprintf_s(gameOverMessage, "You run out of time and had %d coins left", coinsRemaining);
        render2dText("GAME OVER", 1.0, 0.0, 0.0, -0.15, 0.5);
        render2dText(gameOverMessage, 1.0, 0.0, 0.0, -0.4, 0.44);
        render2dText("Press R to restart or ESC to exit the game", 1.0, 1.0, 1.0, -0.41, 0.32);
        gameOver = true;
    }
    if (tankPosition.y < -groundY)
    {
        render2dText("GAME OVER", 1.0, 0.0, 0.0, -0.15, 0.2);
        render2dText("Press R to restart or ESC to exit the game", 1.0, 1.0, 1.0, -0.45, 0.14);
        gameOver = true;
    }

    tankForce = Vector3f(0.0, 0.0, 0.0);
    ballForce = Vector3f(0.0, 0.0, 0.0);

    // Swap Buffers and Post Redisplay
    glutSwapBuffers();
    glutPostRedisplay();
}


// ------- FUNCTION FOR KEYBOARD INTERACTION ------- //
void keyDown(unsigned char key, int x, int y)
{
    // Quits Program when ESC is Pressed
    if (key == 27)
        exit(0);

    // Set Key Satus
    keyStates[key] = true;

    glutPostRedisplay();
}

// ------- FUNCTION FOR KEY UP EVENTS ------- //
void keyUp(unsigned char key, int x, int y)
{
    keyStates[key] = false;
  
}


// ---------------- FUNCTION FOR KEYBOARD BUFFERING ---------------- //
void handleKeys()
{
    // Move Forward
    if (keyStates['W'] || keyStates['w'])
    {
        tankForce.x = 20.0;
        tankForce.z = 20.0;
    }

    // Move Backwards
    if (keyStates['S'] || keyStates['s'])
    {
        tankForce.x = -20.0;
        tankForce.z = -20.0;
    }

    // Rotate Tank Left
    if (keyStates['A'] || keyStates['a'])
        tankRotationDegrees += 0.3;
    
    // Rotate Tank Right
    if (keyStates['D'] || keyStates['d'])
        tankRotationDegrees -= 0.3;

    // Launch Ball
    if (keyStates['E'] || keyStates['e'])
    {
        ballPosition.x = tankPosition.x;
        ballPosition.y = tankPosition.y + 3;
        ballPosition.z = tankPosition.z;

        ballForce.x = 0.0006;
        ballForce.z = 0.0006;
        launchBall = true;
    }

    // Change to Third Person Camera
    if (keyStates['3'])
    {
        thirdPersonCamera = true;
        firstPersonCamera = false;
        freeThirdPersonCamera = false;
    }

    // Change to First Person Camera
    if (keyStates['1'])
    {
        thirdPersonCamera = false;
        firstPersonCamera = true;
        freeThirdPersonCamera = false;
    }

    // Change to Free Camera
    if (keyStates['2'])
    {
        thirdPersonCamera = false;
        firstPersonCamera = false;
        freeThirdPersonCamera = true;
    }

    // Restart Game
    if (keyStates['R'] || keyStates['r'] && gameOver)
        restart = true;
}

// ------- FUNCTION FOR MOUSE BUTTON INTERACTION ------- //
void mouse(int button, int state, int x, int y)
{
    currentButton = button;
    currentState = state;

    if (currentState == GLUT_UP)
    {
        reset = true;
    }

    // Wheel Scroll Up
    if (currentButton == 3)
        distanceFromTank = distanceFromTank - (0.2 * deltaTime);

    // Wheel Scroll Down
    if (currentButton == 4)
        distanceFromTank = distanceFromTank + (0.2 * deltaTime);

    // Distance From Tank Ranges
    if (distanceFromTank > 20)
        distanceFromTank = 20;
    if (distanceFromTank < 5)
        distanceFromTank = 5;

    glutPostRedisplay();
}

// --------------------- FUNCTION FOR MOUSE MOTION INTERACTION --------------------- //
void motion(int x, int y)
{
    // Motion
    float xMotion = (float)x - previousMousePositionX;
    float yMotion = (float)y - previousMousePositionY;

    if (reset)
    {
        xMotion = yMotion = 0.0;
        reset = false;
    }

    previousMousePositionX = (float)x;
    previousMousePositionY = (float)y;

    // Sets Turret Rotation
    if (currentButton == GLUT_RIGHT_BUTTON && currentState == GLUT_DOWN)
    {
        turretRotationDegrees -= (xMotion * 0.1 * deltaTime);
    }

    // Sets Turret Rotation
    if (currentButton == GLUT_LEFT_BUTTON && currentState == GLUT_DOWN)
    {
        cameraTiltDegrees += (yMotion * 0.1 * deltaTime);
        cameraPanDegrees -= (xMotion * 0.1 * deltaTime);
    }

    // Camera Tilt Ranges
    if (cameraTiltDegrees > 89)
        cameraTiltDegrees = 89;
    if (cameraTiltDegrees < 0)
        cameraTiltDegrees = 0;

    // Camera Pan Ranges
    if (cameraPanDegrees > 180 + tankRotationDegrees)
        cameraPanDegrees = 180 + tankRotationDegrees;
    if (cameraPanDegrees < -180 + tankRotationDegrees)
        cameraPanDegrees = -180 + tankRotationDegrees;

    glutPostRedisplay();
}

// -------------- FUNCTION FOR TIMER -------------- //
void Timer(int value)
{
    if (!gameOver)
    {
        // Calculates Time Remaining
        if (coinsRemaining > 0)
            timeRemaining -= 0.01;
        if (timeRemaining < 0)
            timeRemaining = 0;
    }

    // Speed of Coin Rotation
    coinRotation += 1.0;

    // Sets Velocity of Tank to Rest
    if (tankVelocity.x != 0)
        tankVelocity.x -= tankVelocity.x * (1.0 / 20.0);
    if (tankVelocity.z != 0)
        tankVelocity.z -= tankVelocity.z * (1.0 /20.0);


    
    // Sets Velocity of Tank to 0 By Higher Constant
    if (tankFalling)
    {
        if (tankVelocity.x > 0)
            tankVelocity.x -= tankVelocity.x * (1.0 / 2.0);
        if (tankVelocity.z > 0)
            tankVelocity.z -= tankVelocity.z * (1.0 / 2.0);
    }

    // Gets Time Elapsed Since Start of Program
    currentTime += 0.01;

    // Call Function Again After 10 milliseconds
    glutTimerFunc(10, Timer, 0);

    glutPostRedisplay();
}

// ----------------------- FUNCTION FOR RENDERING 2D TEXT ----------------------- //
void render2dText(std::string text, float r, float g, float b, float x, float y)
{
    glColor3f(r, g, b);
    glRasterPos2f(x, y); // Window coordinates
    for (unsigned int i = 0; i < text.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}