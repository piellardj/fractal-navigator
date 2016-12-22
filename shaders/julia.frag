#version 130


uniform sampler2D palette;

uniform uint maxIter;

uniform vec2 c;

in vec2 fragPos;

out vec4 fragColor;


vec2 complexProduct (const vec2 a, const vec2 b)
{
    return vec2(a.x * b.x - a.y * b.y,
                a.x * b.y + a.y * b.x);
}

void main()
{
    vec2 z = fragPos;
    
    uint currIter = 0u;
    while (currIter < maxIter) {
        if (length(z) > 2)
            break;
        
        z = complexProduct(z, z) + c;
        
        ++currIter;
    }
    
    float r = float(currIter) / float(maxIter);
    fragColor = texture(palette, vec2(r,0.5));
}
