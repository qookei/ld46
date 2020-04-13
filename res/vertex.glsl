#version 150 core

in vec3 vert_pos;
in vec4 color;
in vec2 tex_pos;

out vec4 frag_color;
out vec2 frag_uv;

uniform mat4 ortho;
uniform mat4 model;

void main() {
	gl_Position = ortho * model * vec4(vert_pos.xy, 0.0, 1.0);
	frag_color = color;
	frag_uv = tex_pos;
}
