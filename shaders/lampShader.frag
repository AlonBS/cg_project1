#version 330 core

struct Material {
    vec3 position;
    vec3 color;

};


uniform Material lamp;

out vec4 color;

void main()
{    
	color = vec4(lamp.color, 1.0f);

}