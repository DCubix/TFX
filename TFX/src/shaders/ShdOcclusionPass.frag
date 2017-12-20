R"(#version 330 core
layout (location = 0) out float o_occlusion;

in DATA {
	vec3 Position;
	vec2 UV;
	vec4 Color;
} F_IN;

uniform sampler2D TEX_Diffuse;

void main() {
	vec4 col = texture(TEX_Diffuse, F_IN.UV);
	col.rgb = vec3(0.0);
	o_occlusion = col.a;
}
)"