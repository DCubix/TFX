R"(#version 330 core
out vec4 fragColor;

uniform sampler2D TEX_Diffuse;

uniform vec3 LIT_ambient;

in DATA {
	vec3 Position;
	vec2 UV;
	vec4 Color;
} F_IN;

void main() {
	fragColor = texture(TEX_Diffuse, F_IN.UV) * F_IN.Color * vec4(LIT_ambient, 1.0);
}
)"