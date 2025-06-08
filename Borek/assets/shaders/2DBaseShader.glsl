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

uniform sampler2D[32] u_Textures;

in vec4 e_Color;
in vec2 e_TexCord;
in float e_TextureId;

void main()
{
        vec4 texColor = e_Color;

        switch(int(e_TextureId)) {
                case  0: texColor *= texture(u_Textures[ 0], e_TexCord); break;
                case  1: texColor *= texture(u_Textures[ 1], e_TexCord); break;
                case  2: texColor *= texture(u_Textures[ 2], e_TexCord); break;
                case  3: texColor *= texture(u_Textures[ 3], e_TexCord); break;
                case  4: texColor *= texture(u_Textures[ 4], e_TexCord); break;
                case  5: texColor *= texture(u_Textures[ 5], e_TexCord); break;
                case  6: texColor *= texture(u_Textures[ 6], e_TexCord); break;
                case  7: texColor *= texture(u_Textures[ 7], e_TexCord); break;
                case  8: texColor *= texture(u_Textures[ 8], e_TexCord); break;
                case  9: texColor *= texture(u_Textures[ 9], e_TexCord); break;
                case 10: texColor *= texture(u_Textures[10], e_TexCord); break;
                case 11: texColor *= texture(u_Textures[11], e_TexCord); break;
                case 12: texColor *= texture(u_Textures[12], e_TexCord); break;
                case 13: texColor *= texture(u_Textures[13], e_TexCord); break;
                case 14: texColor *= texture(u_Textures[14], e_TexCord); break;
                case 15: texColor *= texture(u_Textures[15], e_TexCord); break;
                case 16: texColor *= texture(u_Textures[16], e_TexCord); break;
                case 17: texColor *= texture(u_Textures[17], e_TexCord); break;
                case 18: texColor *= texture(u_Textures[18], e_TexCord); break;
                case 19: texColor *= texture(u_Textures[19], e_TexCord); break;
                case 20: texColor *= texture(u_Textures[20], e_TexCord); break;
                case 21: texColor *= texture(u_Textures[21], e_TexCord); break;
                case 22: texColor *= texture(u_Textures[22], e_TexCord); break;
                case 23: texColor *= texture(u_Textures[23], e_TexCord); break;
                case 24: texColor *= texture(u_Textures[24], e_TexCord); break;
                case 25: texColor *= texture(u_Textures[25], e_TexCord); break;
                case 26: texColor *= texture(u_Textures[26], e_TexCord); break;
                case 27: texColor *= texture(u_Textures[27], e_TexCord); break;
                case 28: texColor *= texture(u_Textures[28], e_TexCord); break;
                case 29: texColor *= texture(u_Textures[29], e_TexCord); break;
                case 30: texColor *= texture(u_Textures[30], e_TexCord); break;
                case 31: texColor *= texture(u_Textures[31], e_TexCord); break;
        }

        color = texColor;
}
