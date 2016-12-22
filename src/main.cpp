#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include <GL/glew.h>

#include "Fractal.hpp"
#include "Camera.hpp"


/* Returns mouse position in window coordinates [-1,1]x[-1,1] */
glm::vec2 getMouseCoords (sf::Window const& window);

void manageNavigation (Fractal& fractal, sf::Window const& window, sf::Event const& event, glm::vec2& mousePos);

bool checkRequirements (sf::Window const& window);

int main()
{
    const float fps = 50.f;
    sf::ContextSettings openGLContext(0, 0, 0, //no depth, no stencil, no antialiasing
                                      3, 0, //openGL 3.0 requested
                                      sf::ContextSettings::Default);
    sf::RenderWindow windowMandel(sf::VideoMode(600, 600), "Fractals: Mandelbrot",
                                  sf::Style::Default,
                                  openGLContext);
    windowMandel.setPosition(sf::Vector2i(0,0));
    windowMandel.display();

    sf::RenderWindow windowJulia(sf::VideoMode(600, 600), "Fractals: Julia",
                                 sf::Style::Titlebar | sf::Style::Resize,
                                 openGLContext);
    windowJulia.setPosition(sf::Vector2i(windowMandel.getPosition().x + windowMandel.getSize().x + 10,
                                         windowMandel.getPosition().y));
    windowJulia.display();

    /* Checking if the requested OpenGL version is available */
    std::cout << "openGL version: " << windowMandel.getSettings().majorVersion << "." << windowMandel.getSettings().minorVersion << std::endl << std::endl;
    if (!checkRequirements(windowMandel))
        return EXIT_FAILURE;
    glewInit();


    sf::Font font;
    if (!font.loadFromFile("rc/font.ttf")) {
        std::cerr << "Warning: unable to load rc/font.ttf." << std::endl;
    }
    sf::Text text("<empty>", font, 16);

    Fractal mandel(Fractal::Type::Mandelbrot, "rc/palette.png");
    mandel.getCamera().setScreenSize(windowMandel.getSize().x, windowMandel.getSize().y);

    Fractal julia(Fractal::Type::Julia, "rc/palette.png");
    julia.getCamera().setScreenSize(windowMandel.getSize().x, windowMandel.getSize().y);

    glm::vec2 mouseMandelPos = getMouseCoords(windowMandel);
    glm::vec2 mouseJuliaPos = getMouseCoords(windowJulia);
    sf::Clock clock;
    while (windowMandel.isOpen()) {
        if (clock.getElapsedTime() < sf::seconds(1.f / fps)) {
            sf::sleep(sf::seconds(1.f/fps - clock.getElapsedTime().asSeconds()));
            clock.restart();
        }

        if (mandel.needToRedraw()) {
            text.setString(mandel.getInfoString());
            windowMandel.clear();
            windowMandel.setActive(true);
            mandel.draw();
            windowMandel.draw(text);
            windowMandel.display();
        }
        if (julia.needToRedraw()) {
            text.setString(julia.getInfoString());
            windowJulia.clear();
            windowJulia.setActive(true);
            julia.draw();
            windowJulia.draw(text);
            windowJulia.display();
        }

        sf::Event event;
        while (windowMandel.pollEvent(event)) {
            manageNavigation(mandel, windowMandel, event, mouseMandelPos);

            switch (event.type) {
                case sf::Event::Closed:
                    windowMandel.close();
                break;
                case sf::Event::Resized:
                    windowMandel.setView(sf::View(sf::FloatRect(0.f, 0.f, event.size.width, event.size.height)));
                break;
                break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        julia.setSeed(mandel.getCamera().windowToWorld(mouseMandelPos));
                    }
                break;
                case sf::Event::MouseMoved:
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                        julia.setSeed(mandel.getCamera().windowToWorld(mouseMandelPos));
                    }
                default:
                    break;
            }
        }

        while (windowJulia.pollEvent(event)) {
            manageNavigation(julia, windowJulia, event, mouseJuliaPos);

            switch (event.type) {
                case sf::Event::Resized:
                    windowJulia.setView(sf::View(sf::FloatRect(0.f, 0.f, event.size.width, event.size.height)));
                break;
                default:
                    break;
            }
        }
    }

    return EXIT_SUCCESS;
}

glm::vec2 getMouseCoords (sf::Window const& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    glm::vec2 pos(mousePos.x, window.getSize().y - mousePos.y);
    pos.x /= window.getSize().x;
    pos.y /= window.getSize().y;
    return pos * 2.f - 1.f;
}


void manageNavigation (Fractal& fractal, sf::Window const& window, sf::Event const& event, glm::vec2& mousePos)
{
    switch (event.type) {
        case sf::Event::Resized:
            fractal.getCamera().setScreenSize(event.size.width, event.size.height);
        break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::A) {
                fractal.changeMaxIter(1);
            } else if (event.key.code == sf::Keyboard::E) {
                fractal.changeMaxIter(-1);
            } else if (event.key.code == sf::Keyboard::R) {
                fractal.getCamera().setOrigin(glm::vec2(0,0));
                fractal.getCamera().setZoomLevel(0.5f);
            } else if (event.key.code == sf::Keyboard::S) {
                fractal.saveToFile("fractal.png");
            }
        break;
        case sf::Event::MouseWheelScrolled:
        {
            float zoomFactor = 0.1f*event.mouseWheelScroll.delta;
            glm::vec2 pos = fractal.getCamera().windowToWorld(getMouseCoords(window));
            fractal.getCamera().zoom(pos, zoomFactor);
        }
        break;
        case sf::Event::MouseMoved:
        {
            glm::vec2 newMousePos = getMouseCoords(window);

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                fractal.getCamera().relativeMovement(mousePos - newMousePos);
            }

            mousePos = newMousePos;
        }
        break;
        default:
            break;
    }
}

bool checkRequirements(sf::Window const& window)
{
    if (window.getSettings().majorVersion < 3) {
        std::cerr << "This program requires at least OpenGL 3.0" << std::endl << std::endl;
        return false;
    }
    if (!sf::Shader::isAvailable()) {
        std::cerr << "This program requires support for OpenGL shaders." << std::endl << std::endl;
        return false;
    }

    return true;
}
