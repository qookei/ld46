#version 150 core

in vec4 frag_color;
in vec2 frag_uv;

uniform sampler2D tex;

void main() {
	gl_FragColor = texture(tex, frag_uv) * frag_color;
}
