R"(#version 330 core
out vec4 fragColor;

uniform sampler2D TEX_Position;
uniform sampler2D TEX_Normal;
uniform sampler2D TEX_Diffuse;

uniform sampler2D TEX_Shadow;
uniform sampler2D TEX_Mask;
uniform float TEX_ShadowSize = 1.0;
uniform bool TEX_ShadowEnabled = false;

uniform vec2 sresolution;

uniform vec3 LIT_position;
uniform float LIT_radius;
uniform float LIT_intensity = 1.0;
uniform vec3 LIT_color = vec3(1.0);

const float PI = 3.141592654;

in vec2 uv;

float sample(vec2 coord, float r) {
	return step(r, texture(TEX_Shadow, coord).r);
}

void main() {
	vec2 screenUV = gl_FragCoord.xy / sresolution;
	vec3 P = texture(TEX_Position, screenUV).xyz;
	vec3 N = texture(TEX_Normal, screenUV).xyz;

	vec3 Lpos = LIT_position;
	vec3 lightVec = (Lpos - P);
	vec3 L = normalize(lightVec);
	float dist = length(lightVec);

	if (dist <= LIT_radius) {
		vec4 diff = texture(TEX_Diffuse, screenUV);
		float att = smoothstep(LIT_radius, 0.0, dist);
		float shadow = 1.0;
		
		if (TEX_ShadowEnabled) {
			float mask = 1.0 - texture(TEX_Mask, screenUV).r;
			vec2 norm = uv * 2.0 - 1.0;
			float theta = atan(norm.y, norm.x);
			float r = length(norm);	
			float coord = (theta + PI) / (2.0 * PI);
	
			//the tex coord to sample our 1D lookup texture	
			vec2 tc = vec2(coord, 0.0);
	
			//the center tex coord, which gives us hard shadows
			float center = sample(tc, r);        
	
			float blur = (1.0 / TEX_ShadowSize); 
	
			//now we use a simple gaussian blur
			float sum = 0.0;

			sum += sample(vec2(tc.x - 4.0 * blur, tc.y), r) * 0.05;
			sum += sample(vec2(tc.x - 3.0 * blur, tc.y), r) * 0.09;
			sum += sample(vec2(tc.x - 2.0 * blur, tc.y), r) * 0.12;
			sum += sample(vec2(tc.x - 1.0 * blur, tc.y), r) * 0.15;
	
			sum += center * 0.16;
	
			sum += sample(vec2(tc.x + 1.0 * blur, tc.y), r) * 0.15;
			sum += sample(vec2(tc.x + 2.0 * blur, tc.y), r) * 0.12;
			sum += sample(vec2(tc.x + 3.0 * blur, tc.y), r) * 0.09;
			sum += sample(vec2(tc.x + 4.0 * blur, tc.y), r) * 0.05;

			if (mask > 0.0) {
				shadow *= sum;
			}
		}
		float nl = max(dot(N, L), 0.0);
		float fac = nl * shadow * att;

		vec3 lighting = fac * LIT_color * LIT_intensity;

		fragColor = diff * vec4(lighting, 1.0);
	} else {
		discard;
	}
}
)"