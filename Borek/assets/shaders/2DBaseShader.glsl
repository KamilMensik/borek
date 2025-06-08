#name _BOREK_BASE_2D_SHADER

#vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCord;
layout(location = 3) in float a_TextureId;

uniform mat4 u_ProjectionView;

out vec4 e_Color;
out vec2 e_TexCord;
out float e_TextureId;

void main()
{
        e_Color = a_Color;
        e_TexCord = a_TexCord;
        e_TextureId = a_TextureId;
        gl_Position = u_ProjectionView * vec4(a_Position, 1.0);
}

#fragment
#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Textures[32];

in vec4 e_Color;
in vec2 e_TexCord;
in float e_TextureId;

void main()
{
        color = texture(u_Textures[uint(e_TextureId)], e_TexCord) * e_Color;
}
