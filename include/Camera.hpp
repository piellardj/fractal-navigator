#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include "glm.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

/* Class for handling translation and scaling.
 * Allows to map window coordinates ([-1,1]x[-1,1]) to world coordinates.
 */
class Camera
{
    public:
        Camera (unsigned int screenWidth, unsigned screenHeight,
                glm::vec2 const& origin=glm::vec2(0.f,0.f),
                float zoomLevel=1.f);

        glm::vec2 const& getOrigin() const;
        glm::vec2 getViewSize() const;
        float getZoomLevel () const;
        
        /* Maps window coordinates ([-1,1]x[-1,1]) to world coordinates */
        glm::mat3 const& getInvViewMatrix() const;

        void setScreenSize(unsigned int screenWidth, unsigned int screenHeight);
        void setOrigin (glm::vec2 const& newOrigin);
        void setZoomLevel (float newZoomLevel);

        void zoom (glm::vec2 const& towards, float factor);
        void relativeMovement (glm::vec2 const& movement);

        glm::vec2 windowToWorld (glm::vec2 const& winPos);

    private:
        void computeInvViewMatrix();


    private:
        glm::vec2 _origin; //absolute coordinates
        float _zoomLevel;
        float _aspectRatio;

        glm::mat3 _invViewMatrix;
};

#endif // CAMERA_HPP_INCLUDED
