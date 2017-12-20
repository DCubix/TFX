R"(#version 330 core
layout (location = 0) in vec2 v_position;

uniform mat4 MAT_Projection;
uniform mat4 MAT_Model;

out vec2 uv;

void main() {
	gl_Position = MAT_Projection * MAT_Model * vec4(v_position * 2.0 - 1.0, 0.0, 1.0);
	uv = v_position;
}
)"