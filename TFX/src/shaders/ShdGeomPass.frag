R"(#version 330 core
out vec4 fragColor;

in DATA {
	vec3 Position;
	vec2 UV;
	vec4 Color;
} F_IN;

uniform sampler2D TEX_Diffuse;
uniform sampler2D TEX_Normal;
uniform int GBP_Mode;

void main() {
	switch (GBP_Mode) {
		case 0: fragColor = texture(TEX_Diffuse, F_IN.UV) * F_IN.Color; break;
		case 1: fragColor = vec4(F_IN.Position.xy, 1.0, 1.0); break;
		case 2: {
			float alpha = texture(TEX_Diffuse, F_IN.UV).a;
			vec3 N = texture(TEX_Normal, F_IN.UV).xyz;
			N.y = 1.0 - N.y;
			fragColor = vec4(N * 2.0 - 1.0, alpha);
		} break;
		case 3: {
			float a = texture(TEX_Diffuse, F_IN.UV).a;
			fragColor = vec4(a, a, a, 1.0);
		} break;
	}
}
)"