#version 130


uniform mat3 invViewMatrix;

attribute vec2 corner;

out vec2 fragPos;


void main()
{
    gl_Position = vec4(corner, 0, 1);
    fragPos = (invViewMatrix * vec3(corner, 1)).xy;
}
