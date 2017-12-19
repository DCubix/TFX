R"(#version 330 core
out vec4 fragColor;
in vec2 uv;

uniform sampler2D TEX_Occlusion;
uniform float TEX_ShadowSize;

const float PI = 3.141592654;
const float THRESHOLD = 0.75;

void main() {
	float res = TEX_ShadowSize;
	float sdist = 1.0;
	for (float y = 0.0; y < res; y += 1.0) {
		vec2 norm = vec2(uv.x, y / res) * 2.0 - 1.0;
		float theta = PI * 1.5 + norm.x * PI; 
		float r = (1.0 + norm.y) * 0.5;

		vec2 coord = vec2(-r * sin(theta), -r * cos(theta)) / 0.5 + 0.5;
		float caster = texture2D(TEX_Occlusion, coord).a;

		float dst = y / res;
		if (caster > THRESHOLD) {
			sdist = min(sdist, dst);
			break;
  		}
	}
	fragColor = vec4(vec3(sdist), 1.0);
}
)"