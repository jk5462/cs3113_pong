#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

float lastTicks = 0;

Entity player1, player2, ball;

int winningPlayer = 1;

float defaultColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float altColor[] = { 1.0f, 0.5f, 1.0f, 1.0f };

float playerVertices[] = { -0.05, -0.75, 0.05, -0.75, 0.05, 0.75, -0.05, -0.75, 0.05, 0.75, -0.05, 0.75 };
float ballVertices[] = { -0.2, -0.2, 0.2, -0.2, 0.2, 0.2, -0.2, -0.2, 0.2, 0.2, -0.2, 0.2 };

GLuint fontTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

	player1.width = 0.1;
	player1.height = 1.5;

	player2.width = 0.1;
	player2.height = 1.5;

	ball.width = 0.4;
	ball.height = 0.4;
    
	player1.position = glm::vec3(-4.9f, 0.0f, 0.0f);
	player1.movement = glm::vec3(0.0f, 0.0f, 0.0f);
	player1.speed = 4;

	player2.position = glm::vec3(4.9f, 0.0f, 0.0f);
	player2.movement = glm::vec3(0.0f, 0.0f, 0.0f);
	player2.speed = 4;

	ball.position = glm::vec3(0.0f, 0.0f, 0.0f);
	ball.movement = glm::vec3(1.0f, 1.0f, 0.0f);
	ball.speed = 3;
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

bool CheckTopBorderCollision(Entity* object) {
	return (object->position.y + (object->height / 2.0f)) > 3.75;
}

bool CheckBottomBorderCollision(Entity* object) {
	return (object->position.y - (object->height / 2.0f)) < -3.75;
}

bool CheckRightBorderCollision(Entity* object) {
	return (object->position.x + (object->width / 2.0f)) > 5;
}

bool CheckLeftBorderCollision(Entity* object) {
	return (object->position.x - (object->width / 2.0f)) < -5;
}

bool CheckEntityCollision(Entity* object1, Entity* object2) {
	float xdist = fabs(object2->position.x - object1->position.x) - ((object1->width + object2->width) / 2.0f);
	float ydist = fabs(object2->position.y - object1->position.y) - ((object1->height + object2->height) / 2.0f);

	return (xdist < 0 && ydist < 0);
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				// Some sort of action
				break;

			}
			break;
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_W])
	{
		if (CheckTopBorderCollision(&player1) == false) {
			player1.movement.y = 1;
		}
		else {
			player1.movement = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
	else if (keys[SDL_SCANCODE_S])
	{
		if (CheckBottomBorderCollision(&player1) == false) {
			player1.movement.y = -1;
		}
		else {
			player1.movement = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
	else {
		player1.movement = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	if (keys[SDL_SCANCODE_UP])
	{
		if (CheckTopBorderCollision(&player2) == false) {
			player2.movement.y = 1;
		}
		else {
			player2.movement = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
	else if (keys[SDL_SCANCODE_DOWN])
	{
		if (CheckBottomBorderCollision(&player2) == false) {
			player2.movement.y = -1;
		}
		else {
			player2.movement = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
	else {
		player2.movement = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	//Reset ball position
	if (keys[SDL_SCANCODE_R])
	{
		ball.position = glm::vec3(0.0f, 0.0f, 0.0f);
		ball.speed = 3;
	}
}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

	if (CheckTopBorderCollision(&ball)) {
		ball.movement.y *= -1;
	}

	else if (CheckBottomBorderCollision(&ball)) {
		ball.movement.y *= -1;
	}

	else if (CheckLeftBorderCollision(&ball)) {
		ball.position = glm::vec3(0.0f, 0.0f, 0.0f);
		ball.speed = 3;
		winningPlayer = 2;
	}

	else if (CheckRightBorderCollision(&ball)) {
		ball.position = glm::vec3(0.0f, 0.0f, 0.0f);
		ball.speed = 3;
		winningPlayer = 1;
	}

	//Ball gets faster every player entity-ball collision
	if (CheckEntityCollision(&ball, &player1)) {
		ball.speed += .2;
		ball.movement.x *= -1;
	}

	else if (CheckEntityCollision(&ball, &player2)) {
		ball.speed += .2;
		ball.movement.x *= -1;
	}

	ball.Update(deltaTime);
	player1.Update(deltaTime);
	player2.Update(deltaTime);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
	if (winningPlayer == 1) {
		player1.Render(&program, playerVertices, altColor);
		player2.Render(&program, playerVertices, defaultColor);
	}
	else {
		player1.Render(&program, playerVertices, defaultColor);
		player2.Render(&program, playerVertices, altColor);
	}

	ball.Render(&program, ballVertices, defaultColor);
 
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
