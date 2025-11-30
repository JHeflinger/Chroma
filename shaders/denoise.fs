#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D tmp;
uniform sampler2D curr;
uniform sampler2D normals;
uniform sampler2D positions;
uniform sampler2D albedo;
uniform int step;
uniform int width;
uniform int height;
uniform float sigma_color;
uniform float sigma_normal;
uniform float sigma_depth;

out vec4 finalColor;

float kernel[25] = float[](
    1/256.0,  4/256.0,  6/256.0,  4/256.0, 1/256.0,
    4/256.0, 16/256.0, 24/256.0, 16/256.0, 4/256.0,
    6/256.0, 24/256.0, 36/256.0, 24/256.0, 6/256.0,
    4/256.0, 16/256.0, 24/256.0, 16/256.0, 4/256.0,
    1/256.0,  4/256.0,  6/256.0,  4/256.0, 1/256.0
);

void main() {
	vec3 sum = vec3(0.0);
	float sumW = 0.0;
	vec3 c_p = vec3(texture2D(curr, fragTexCoord));
	vec3 n_p = vec3(texture2D(normals, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y)));
	vec3 p_p = vec3(texture2D(positions, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y)));
	int index = 0;
	float w_color = 0.0;
	float w_normal = 0.0;
	float w_depth = 0.0;
	for (int ky = -2; ky <= 2; ky++) {
		for (int kx = -2; kx <= 2; kx++, index++) {
			float sx = fragTexCoord.x + (float(kx * step) / float(width));
			float sy = fragTexCoord.y + (float(ky * step) / float(height));
			//if (sx < 0 || sx >= 1.0 || sy < 0 || sy >= 1.0) continue;
			vec3 c_q = vec3(texture2D(curr, vec2(sx, sy)));
			vec3 n_q = vec3(texture2D(normals, vec2(sx, 1.0 - sy)));
			vec3 p_q = vec3(texture2D(positions, vec2(sx, 1.0 - sy)));
			w_color = exp(-dot(c_q - c_p, c_q - c_p) / (sigma_color*sigma_color));
			w_normal = exp(-(1.0 - dot(n_p, n_q)) / sigma_normal);
			w_depth = exp(-dot(p_q - p_p, p_q - p_p) / (sigma_depth*sigma_depth));
			float w = kernel[index] * w_color * w_normal * w_depth;
			sum += c_q*w;
			sumW += w;
		}
	}
	if (sumW < 0.000001) sumW = 0.01;
	finalColor = vec4(sum / sumW, 1.0);
}
