R"(#version 330 core
layout (location = 0) out vec3 o_position;
layout (location = 1) out vec4 o_normal;
layout (location = 2) out vec4 o_diffuse;

in DATA {
	vec3 Position;
	vec2 UV;
	vec4 Color;
} F_IN;

uniform sampler2D TEX_Diffuse;
uniform sampler2D TEX_Normal;

void main() {
	o_position = vec3(F_IN.Position.xy, 1.0);

	vec4 diff = texture(TEX_Diffuse, F_IN.UV);
	o_diffuse = diff * F_IN.Color;

	vec3 N = texture(TEX_Normal, F_IN.UV).xyz;
	N.y = 1.0 - N.y;
	o_normal = vec4(N * 2.0 - 1.0, diff.a);
}
)"