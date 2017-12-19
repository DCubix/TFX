R"(#version 330 core
out vec4 fragColor;

in DATA {
	vec3 Position;
	vec2 UV;
	vec4 Color;
} F_IN;

uniform sampler2D TEX_Diffuse;

const float smoothing = 1.0 / 16.0;

const float outlineDistance = 0.5; // Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
const vec4 outlineColor = vec4(1.0);

void main() {
	float distance = texture2D(TEX_Diffuse, F_IN.UV).r;
	float outlineFactor = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
	vec4 color = mix(outlineColor, F_IN.Color, outlineFactor);
	float alpha = smoothstep(outlineDistance - smoothing, outlineDistance + smoothing, distance);
    fragColor = vec4(F_IN.Color.rgb, F_IN.Color.a * alpha);
}
)"