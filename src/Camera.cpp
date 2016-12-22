#include "Camera.hpp"


Camera::Camera (unsigned int screenWidth, unsigned screenHeight,
                glm::vec2 const& origin,
                float zoomLevel):
            _origin (origin),
            _zoomLevel (zoomLevel),
            _aspectRatio (static_cast<float>(screenWidth) / static_cast<float>(screenHeight))
{
    computeInvViewMatrix();
}

glm::vec2 const& Camera::getOrigin() const
{
    return _origin;
}

glm::vec2 Camera::getViewSize() const
{
    return glm::vec2(2*_aspectRatio,2) / _zoomLevel;
}

float Camera::getZoomLevel () const
{
    return _zoomLevel;
}

glm::mat3 const& Camera::getInvViewMatrix() const
{
    return _invViewMatrix;
}

void Camera::setScreenSize(unsigned int screenWidth, unsigned int screenHeight)
{
    _aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    computeInvViewMatrix();
}

void Camera::setOrigin (glm::vec2 const& newOrigin)
{
    _origin = newOrigin;
    computeInvViewMatrix();
}

void Camera::setZoomLevel (float newZoomLevel)
{
    if (newZoomLevel == 0.f)
        return;

    _zoomLevel = newZoomLevel;
    computeInvViewMatrix();
}

void Camera::zoom(glm::vec2 const& towards, float factor)
{
    _origin = _origin * (factor + 1.f) - towards * factor;
    _zoomLevel /= 1.f + factor;

   computeInvViewMatrix();
}

void Camera::relativeMovement(glm::vec2 const& movement)
{
    _origin += movement / _zoomLevel;
    computeInvViewMatrix();
}

glm::vec2 Camera::windowToWorld (glm::vec2 const& winPos)
{
    glm::vec2 worldPos(winPos.x, winPos.y);

    return worldPos / _zoomLevel + _origin;
}

void Camera::computeInvViewMatrix()
{
    _invViewMatrix[0][0] = _aspectRatio / _zoomLevel;
    _invViewMatrix[1][0] = 0.f;
    _invViewMatrix[2][0] = _origin.x;

    _invViewMatrix[0][1] = 0.f;
    _invViewMatrix[1][1] = 1.f / _zoomLevel;
    _invViewMatrix[2][1] = _origin.y;

    _invViewMatrix[0][2] = 0.f;
    _invViewMatrix[1][2] = 0.f;
    _invViewMatrix[2][2] = 0.f;
}
