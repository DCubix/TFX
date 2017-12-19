R"(#version 330 core
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec4 v_color;

uniform mat4 MAT_View = mat4(1.0);
uniform mat4 MAT_Projection;

out DATA {
	vec3 Position;
	vec2 UV;
	vec4 Color;
} V_OUT;

void main() {
	gl_Position = MAT_Projection * MAT_View * vec4(v_position, 1.0);
	V_OUT.Position = (MAT_View * vec4(v_position, 1.0)).xyz;
	V_OUT.UV = v_uv;
	V_OUT.Color = v_color;
}
)"