R"(#version 330 core
out vec4 fragColor;

uniform sampler2D TEX_Screen;
uniform vec3 COL_Tint = vec3(1.0);

in vec2 uv;

void main() {
	fragColor = texture(TEX_Screen, uv) * vec4(COL_Tint, 1.0);
}
)"