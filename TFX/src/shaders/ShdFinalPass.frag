R"(#version 330 core
out vec4 fragColor;

uniform sampler2D TEX_Lighting;
uniform sampler2D TEX_Diffuse;

in vec2 uv;

void main() {
	fragColor = texture(TEX_Lighting, uv) * texture(TEX_Diffuse, uv);
}
)"