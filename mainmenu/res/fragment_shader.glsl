#version 140

in vec2 textureCoords;

out vec4 outputColor;

uniform sampler2D textureSampler;

void
main(void) {
	outputColor = texture(textureSampler, vec2(textureCoords.x, textureCoords.y));
// 	outputColor = vec4(1, 0, 1, 1);
}
