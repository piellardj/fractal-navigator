#include "Fractal.hpp"

#include "GLHelper.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <SFML/Graphics/RenderTexture.hpp>


Fractal::Fractal (Fractal::Type type, std::string const& palette):
            _type(type),
            _seed(glm::vec2(0.f, 0.f)),
            _maxIter(100),
            _cornersBufferID(-1),
            _camera(1, 1, glm::vec2(0,0), 0.5f),
            _needToRedraw(true)
{
    std::string vertex, fragment;
    vertex = "shaders/fractal.vert";
    if (type == Type::Mandelbrot)
        fragment = "shaders/mandelbrot.frag";
    else
        fragment = "shaders/julia.frag";

    if (!_shader.loadFromFile(vertex, fragment)) {
        throw std::runtime_error("Fractal: unable to load shader " + vertex + " or " + fragment);
    }

    if (!_palette.loadFromFile(palette)) {
        throw std::runtime_error("Fractal: unable to load " + palette);
    }
    _palette.setSmooth(true);

    /* Buffer allocation */
    std::vector<glm::vec2> corners(6);
    corners[0] = glm::vec2(-1,-1);
    corners[1] = glm::vec2(+1,+1);
    corners[2] = glm::vec2(-1,+1);
    corners[3] = glm::vec2(-1,-1);
    corners[4] = glm::vec2(+1,+1);
    corners[5] = glm::vec2(+1,-1);

    GLCHECK(glGenBuffers(1, &_cornersBufferID));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _cornersBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, corners.size()*sizeof(glm::vec2), corners.data(), GL_STATIC_DRAW));

    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

Fractal::~Fractal()
{
    if (_cornersBufferID != (GLuint)(-1)) {
        GLCHECK(glDeleteBuffers(1, &_cornersBufferID));
    }
}

bool Fractal::needToRedraw() const
{
    return _needToRedraw;
}

void Fractal::draw () const
{
    _needToRedraw = false;

    GLCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    _shader.setParameter("palette", _palette);
    sf::Shader::bind(&_shader);

    /* First retrieve locations */
    GLuint shaderHandle = -1;
    GLuint cornerALoc = -1;
    GLuint maxIterULoc = -1, matrixULoc = -1;

//    try {
        shaderHandle = getShaderHandle(_shader, false);

        maxIterULoc = getShaderUniformLoc(shaderHandle, "maxIter", false);
        matrixULoc = getShaderUniformLoc(shaderHandle, "invViewMatrix", false);


        cornerALoc = getShaderAttributeLoc(shaderHandle, "corner", false);
//    } catch (std::exception const& e) {
//        std::cerr << "Renderer2D.draw: " << e.what() << std::endl;
//        return;
//    }

    if (_type == Type::Julia) {
        GLuint seedULoc = -1;
        seedULoc = getShaderUniformLoc(shaderHandle, "c", false);
        GLCHECK(glUniform2f(seedULoc, _seed.x, _seed.y));
    }

    GLCHECK(glUniform1ui(maxIterULoc, getMaxIter()));
    GLCHECK(glUniformMatrix3fv(matrixULoc, 1, GL_FALSE, &_camera.getInvViewMatrix()[0][0]));

    /* Enabling corners coordinates buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _cornersBufferID));
    GLCHECK(glEnableVertexAttribArray(cornerALoc));
    GLCHECK(glVertexAttribPointer(cornerALoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

    /*Actual drawing */
    GLCHECK(glDisable(GL_CULL_FACE));
    GLCHECK(glDisable(GL_DEPTH_TEST));
    GLCHECK(glDisable(GL_BLEND));
    GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
    GLCHECK(glEnable(GL_BLEND));

    /* Don't forget to unbind buffers */
    GLCHECK(glDisableVertexAttribArray(cornerALoc));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    sf::Shader::bind(0);
}

void Fractal::saveToFile(std::string const& filepath) const
{
    float aspectRatio = getCamera().getViewSize().x / getCamera().getViewSize().y;
    float height = 3000.f, width = height * aspectRatio;

    sf::RenderTexture texture;
    texture.create(width, height);
    texture.setActive(true);
    glViewport(0, 0, width, height);
    draw();
    texture.display();
    sf::Image image = texture.getTexture().copyToImage();
    image.saveToFile(filepath);
}

Camera& Fractal::getCamera()
{
    _needToRedraw = true;
    return _camera;
}

Camera const& Fractal::getCamera() const
{
    return _camera;
}

void Fractal::changeMaxIter(int diff)
{
    int maxIter = static_cast<int>(_maxIter);
    maxIter += diff;

    if (maxIter < 2)
        maxIter = 2;

    setMaxIter(maxIter);
}

void Fractal::setMaxIter(unsigned int maxIter)
{
    _needToRedraw = (_maxIter != maxIter);
    _maxIter = maxIter;
}

unsigned int Fractal::getMaxIter() const
{
    return _maxIter;
}

void Fractal::setSeed(glm::vec2 seed)
{
    _needToRedraw = (_type == Type::Julia);
    _seed = seed;
}

std::string Fractal::getInfoString() const
{
    std::stringstream stream;
    stream << "max iteration: " << getMaxIter() << " (A/E)" << std::endl;
    stream << "zoom: " << getCamera().getZoomLevel() << std::endl << std::endl;

    if (_type == Type::Julia) {
        stream << "seed: " << _seed.x << " ; " << _seed.y << std::endl << std::endl;
    }

    stream << "origin: (" << getCamera().getOrigin().x << " ; " << getCamera().getOrigin().y << ")" << std::endl;
    stream << "width:  " << getCamera().getViewSize().x << std::endl;
    stream << "height: " << getCamera().getViewSize().y;

    return stream.str();
}
