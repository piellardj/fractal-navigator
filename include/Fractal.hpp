#ifndef FRACTAL_HPP_INCLUDED
#define FRACTAL_HPP_INCLUDED

#include <GL/glew.h>
#include "glm.hpp"
#include <SFML/OpenGL.hpp>

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include "Camera.hpp"


/* Class for drawing Mandelbrot or Julia fractals */
class Fractal
{
    public:
        enum class Type{Mandelbrot, Julia};

    public:
        Fractal(Fractal::Type type, std::string const& palette);
        ~Fractal();

        bool needToRedraw() const;

        /* Draws to the current active OpenGL context */
        void draw () const;

        void saveToFile (std::string const& filepath) const;

        Camera& getCamera();
        Camera const& getCamera() const;

        void changeMaxIter (int diff);
        void setMaxIter(unsigned int maxIter);
        unsigned int getMaxIter() const;

        void setSeed (glm::vec2 seed);

        std::string getInfoString() const;

    private:
        Type _type; //either mandelbrot or julia
        glm::vec2 _seed;
        unsigned int _maxIter;

        GLuint _cornersBufferID;
        sf::Texture _palette;

        Camera _camera;
        mutable bool _needToRedraw;
        mutable sf::Shader _shader;
};

#endif // FRACTAL_HPP_INCLUDED
