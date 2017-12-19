R"(#version 330 core
out vec4 fragColor;

uniform sampler2D TEX_Position;
uniform sampler2D TEX_Normal;
uniform sampler2D TEX_Diffuse;

uniform vec3 LIT_position;
uniform float LIT_radius;
uniform float LIT_intensity = 1.0;
uniform vec3 LIT_color = vec3(1.0);

const float PI = 3.141592654;

in vec2 uv;

void main() {
	vec3 P = texture(TEX_Position, uv).xyz;
	vec3 N = texture(TEX_Normal, uv).xyz;

	vec3 lightVec = (LIT_position - P);
	vec3 L = normalize(lightVec);
	float dist = length(lightVec);

	if (dist <= LIT_radius) {
		float att = smoothstep(LIT_radius, 0, dist);
		float nl = max(dot(N, L), 0.0);
		float fac = nl * att;

		vec3 lighting = fac * LIT_color * LIT_intensity;

		fragColor = vec4(lighting, 1.0) * texture(TEX_Diffuse, uv);
	} else {
		fragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
)"