#name _BOREK_FONT_2D_SHADER

#vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCord;

uniform mat4 u_ProjectionView;

layout(location = 0) out vec4 e_Color;
layout(location = 1) out vec2 e_TexCord;

void main()
{
        e_Color = a_Color;
        e_TexCord = a_TexCord;
        gl_Position = u_ProjectionView * vec4(a_Position, 1.0);
}

#fragment
#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_FontTexture;

layout(location = 0) in vec4 e_Color;
layout(location = 1) in vec2 e_TexCord;
layout(location = 2) in float e_TextureId;


float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

const float pxRange = 2.0;

float screenPxRange() {
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_FontTexture, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(e_TexCord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}
void main()
{
        vec3 msd = texture(u_FontTexture, e_TexCord).rgb;
        float sd = median(msd.r, msd.g, msd.b);
        float screenPxDistance = screenPxRange()*(sd - 0.5);
        float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
        color = e_Color;
        color.a = opacity;
        //color = texture(u_FontTexture, e_TexCord);
}
