
#define printusers() printf("Program by Team C\n");

#define _CRT_SECURE_NO_WARNINGS
#define RESOLUTION 1280
#define TARGET_FPS 30                // controls spin update rate
#define TIME_BETWEEN_UPDATES 0.015   // seconds between motion updates
#define PRINT_FPS_INTERVAL 10.0f


#ifdef __APPLE__
#include <fcntl.h>
#include <unistd.h>
#endif

#define GL3W
#ifdef GLEW
#define GLEW_STATIC
#include "glew/glew.h"
#endif
#ifdef GL3W
#include "gl3w/gl3w.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
//#define _USE_MATH_DEFINES
//#include <math.h>
#define PI 3.14159f
#include "ShaderManager.h"
#include "GLHelper.h"
#include "RenderEngine.h"
#include "WorldState.h"

class Program4
{
public:
	Program4()
	{
		getWindowContext();
        render.init(state);
        resize(App->getSize().x, App->getSize().y);

        render.buildRenderBuffers(App->getSize().x, App->getSize().y);
        previousPos = glm::vec2(0);
		buttonDown[0]=false;
		buttonDown[1]=false;
		buttonDown[2]=false;
		
		sf::Clock c;
		float lastFrame = c.restart().asSeconds();
		float lastPrint = lastFrame;
		float targetFrameTime = 1.0f/(float)TARGET_FPS;
        App->setMouseCursorVisible(false);
        
		while (state.isRunning())
		{			
			App->setActive();
			float currentTime = c.getElapsedTime().asSeconds();
			float sinceLastFrame = currentTime - lastFrame;
			float sleepTime = targetFrameTime - sinceLastFrame;
			if(sleepTime > 0)
				sf::sleep(sf::seconds(sleepTime));
			
			currentTime = c.getElapsedTime().asSeconds();
			lastFrame = currentTime;
			float sinceLastPrint = currentTime - lastPrint;
            
			handleEvents(state, render);
			state.timeStep(currentTime);
            
			if(sinceLastPrint > PRINT_FPS_INTERVAL) {
				lastPrint = currentTime;
				state.printFPS();
			}
            
			render.display(state);
			App->display();
		}
	}
	
private:
	sf::Window * App;
	RenderEngine render;
    WorldState state;
	sf::Clock timer;
	float lastUpdate;
	float motionTime;
	glm::ivec2 previousPos;
	bool buttonDown[3];


    void resize(size_t x, size_t y)
    {
        
        state.currentRes[0] = x;
        state.currentRes[1] = y;
    
    }
    
    
	void handleEvents(WorldState & state, RenderEngine & render)
	{
		sf::Event event;
		
		while (App->pollEvent(event))

        {
			if (event.type == sf::Event::Closed)
				state.setRunning(false);
            
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				state.setRunning(false);

            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'z'))
                state.toggleShadingMode();
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'x'))
                state.toggleSwirlEnable();
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'c'))
                state.toggleLightRotate();
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'v'))
                state.toggleDiscoMode();
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'p'))
                state.togglePause();
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'n'))
                state.speedUp();
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'm'))
                state.speedDown();
            
            if(event.type == sf::Event::MouseMoved)
            {
                state.rotateCamera(event.mouseMove.x, event.mouseMove.y);
                if (event.mouseMove.x!=512 || event.mouseMove.y!=512) {
                    sf::Mouse::setPosition(sf::Vector2i(512,512), *App);
                }
            }
		}
        
        if (!state.getPause()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)||sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
                state.moveLeft();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)||sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
                state.moveRight();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)||sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
                state.moveUp();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)||sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
                state.moveDown();
            }
        }
	}
    
	void getWindowContext()
	{
		sf::err().rdbuf(NULL); //hide errors
		
#ifdef __APPLE__
		int nullFD = open("/dev/null", O_WRONLY);
		int oldFD = dup(2); // Duplicate the old file descriptor, so it can be restored
		dup2(nullFD, 2); // Redirect
#endif
		
		sf::VideoMode mode(RESOLUTION, RESOLUTION, 32);
#ifdef __linux__
		sf::ContextSettings settings(32, 0, 0, 3, 3);
#else
		sf::ContextSettings settings(32, 0, 0, 3, 3, sf::ContextSettings::Core);
#endif
		App = new sf::Window(mode, "SFML application", sf::Style::Default, settings);
		
#ifdef __APPLE__
		dup2(oldFD, 2); // Redirect back
		close(oldFD); // Not needed anymore
#endif
	}
};

int main()
{
	printusers();
	Program4 prog;
	
    return EXIT_SUCCESS;
}
