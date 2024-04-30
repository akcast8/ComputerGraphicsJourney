#version 330 core

uniform samplerCube skybox;
uniform vec2 iResolution;
uniform float iTime;
uniform int iFrame;
in vec3 vtx_model_position;
in vec3 vtx_pos; // [-1, 1]
in vec2 vtx_uv; // [0, 1]
out vec4 frag_color;

#define NUM_STAR 100.

// return random vec2 between 0 and 1
vec2 hash2d(float t)
{
    t += 1.;
    float x = fract(sin(t * 674.3) * 453.2);
    float y = fract(sin((t + x) * 714.3) * 263.2);

    return vec2(x, y);
}

vec3 renderParticle(vec2 uv, vec2 pos, float brightness, vec3 color)
{
    float d = length(uv - pos);
    return brightness / d * color;
}

vec3 renderStars(vec2 uv)
{
    vec3 fragColor = vec3(0.0);

    float t = iTime;
    for(float i = 0.; i < NUM_STAR; i++)
    {
        vec2 pos = hash2d(i) * 2. - 1.; // map to [-1, 1]
        float brightness = .0015;
        brightness *= sin(1.5 * t + i) * .5 + .5; // flicker
        vec3 color = vec3(0.15, 0.71, 0.92);

        fragColor += renderParticle(uv, pos, brightness, color);
    }

    return fragColor;
}

void main()
{
    // skybox is surounding the NDC cube, so we can use the vertex position as the direction
    vec3 outputColor = renderStars(vtx_model_position.xy);
    vec3 color = texture(skybox, vtx_model_position).rgb;
    frag_color = vec4(color, 1.0);
    frag_color = frag_color + vec4(outputColor, 1.0);
}
