//! \file player.glsl
//tuto 9 material
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;
layout(location= 4) in uint material;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

out vec3 vertex_position;
out vec3 vertex_normal;
flat out uint vertex_material;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    
    vertex_position = vec3(mvMatrix * vec4(position, 1));
    vertex_normal = mat3(mvMatrix) * normal;

    vertex_material = material;
}

#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

in vec3 vertex_position;
in vec3 vertex_normal;
flat in uint vertex_material;
uniform vec3 light_dir;
uniform float emission_factor;

#define MAX_MATERIALS 16
uniform vec4 diffuses[MAX_MATERIALS];
uniform vec4 emissions[MAX_MATERIALS];
void main( )
{
    vec3 l = normalize(light_dir);
    vec3 n = normalize(vertex_normal);

    float light = max(0, dot(l, n)); // dot(l,n) => [-1,1]

    //cell shading
    if (light >= 0.9) light = 1.0;
    else if(light >= 0.5) light = 0.6;
    else if(light >= 0.2) light = 0.4;
    else if(light > 0) light = 0.1;

    vec4 dif = diffuses[vertex_material] * light;
    vec4 emit = emissions[vertex_material] * emission_factor;
    fragment_color = emit + dif;
}

#endif
