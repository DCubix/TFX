R"(#version 330 core
layout (location = 0) in vec2 v_position;

out vec2 uv;

void main() {
	gl_Position = vec4(v_position * 2.0 - 1.0, 0.0, 1.0);
	uv = v_position;
}
)"