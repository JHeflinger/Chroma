#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D image;
uniform sampler2D albedo;

out vec4 finalColor;

void main() {
	finalColor = texture2D(image, fragTexCoord) / max(texture2D(albedo, fragTexCoord), vec4(0.1, 0.1, 0.1, 1.0));
}
