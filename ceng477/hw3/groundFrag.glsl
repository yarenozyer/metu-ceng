#version 330 core

in vec4 pos; 
in vec3 nor; 
in vec4 initialPos;
out vec4 fragColor; 


vec3 offset = vec3(-100.0, 0.0, -100.0); 
float scale = 0.2; // scale of checkerboard
vec3 color1 = vec3(0.0, 0.1, 0.3); // color 1 of the checkerboard pattern
vec3 color2 = vec3(0.0, 0.5, 0.9); // color 2 of the checkerboard pattern

void main()
{
    // Calculate the checkerboard pattern based on the fragment's world position
    bool x = (int((initialPos.x + offset.x) * scale)) % 2 != 0;
    bool y = (int((initialPos.y + offset.y) * scale)) % 2 != 0;
    bool z = (int((initialPos.z + offset.z) * scale)) % 2 != 0;
    bool xorXY = x != y;

    // Determine the fragment color based on the checkerboard pattern
    vec3 finalColor = xorXY != z ? color1 : color2;

    // Set the output color of the fragment
    fragColor = vec4(finalColor, 1.0);
}
