#include "compositor.h"

Composite g_composite = { 0 };

void LoadComposite(const char* filepath) {
	CleanComposite();
	char buffer[1024] = { 0 };
	g_composite.sColor = 4.0f;
	g_composite.sNormal = 128.0f;
	g_composite.sDepth = 1.0f;
	g_composite.colors = LoadTexture(filepath);
	g_composite.denoises = 1;

	SimpleFile* file;
	sprintf(buffer, "%s.normals", filepath);
	file = ReadFile(buffer);
	Image img = (Image){
		.data = file->data,
		.width = g_composite.colors.width,
		.height = g_composite.colors.height,
		.format = PIXELFORMAT_UNCOMPRESSED_R32G32B32,
		.mipmaps = 1
	};
	g_composite.normals = LoadTextureFromImage(img);
	FreeFile(file);

	sprintf(buffer, "%s.positions", filepath);
	file = ReadFile(buffer);
	img.data = file->data;
	g_composite.positions = LoadTextureFromImage(img);
	FreeFile(file);	

	sprintf(buffer, "%s.albedo", filepath);
	file = ReadFile(buffer);
	img.data = file->data;
	g_composite.albedo = LoadTextureFromImage(img);
	FreeFile(file);

	g_composite.shaders.denoise = LoadShader(NULL, "shaders/denoise.fs");
	g_composite.shaders.demodulate = LoadShader(NULL, "shaders/demodulate.fs");
	g_composite.shaders.modulate = LoadShader(NULL, "shaders/modulate.fs");
	g_composite.target = LoadRenderTexture(g_composite.colors.width, g_composite.colors.height);
	g_composite.temp = LoadRenderTexture(g_composite.colors.width, g_composite.colors.height);	
}

void CleanComposite() {
	if (IsTextureValid(g_composite.normals)) UnloadTexture(g_composite.normals);
	if (IsTextureValid(g_composite.positions)) UnloadTexture(g_composite.positions);
	if (IsTextureValid(g_composite.albedo)) UnloadTexture(g_composite.albedo);
	if (IsTextureValid(g_composite.colors)) UnloadTexture(g_composite.colors);
	if (IsShaderValid(g_composite.shaders.denoise)) UnloadShader(g_composite.shaders.denoise);
	if (IsShaderValid(g_composite.shaders.demodulate)) UnloadShader(g_composite.shaders.demodulate);
	if (IsShaderValid(g_composite.shaders.modulate)) UnloadShader(g_composite.shaders.modulate);
	if (IsRenderTextureValid(g_composite.target)) UnloadRenderTexture(g_composite.target);
	if (IsRenderTextureValid(g_composite.temp)) UnloadRenderTexture(g_composite.temp);
}

void RenderComposite() {
	// set up images
	BeginTextureMode(g_composite.target);
	BeginShaderMode(g_composite.shaders.demodulate);
	SetShaderValueTexture(
		g_composite.shaders.demodulate,
		GetShaderLocation(g_composite.shaders.demodulate, "albedo"),
		g_composite.albedo);
	SetShaderValueTexture(
		g_composite.shaders.demodulate,
		GetShaderLocation(g_composite.shaders.demodulate, "image"),
		g_composite.colors);
	DrawTexture(g_composite.colors, 0, 0, WHITE);
	EndTextureMode();
	BeginTextureMode(g_composite.temp);
	DrawTexture(g_composite.colors, 0, 0, WHITE);
	EndShaderMode();
	EndTextureMode();

	// pointers to ref
	RenderTexture2D curr = g_composite.target;
	RenderTexture2D tmp = g_composite.temp;

	// shade
	int step = 1;
	for (uint32_t i = 0; i < g_composite.denoises; i++) {
		BeginTextureMode(tmp);	
		BeginShaderMode(g_composite.shaders.denoise);
		SetShaderValueTexture(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "tmp"),
			tmp.texture);
		SetShaderValueTexture(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "curr"),
			curr.texture);
		SetShaderValueTexture(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "normals"),
			g_composite.normals);
		SetShaderValueTexture(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "positions"),
			g_composite.positions);
		SetShaderValueTexture(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "albedo"),
			g_composite.albedo);
		SetShaderValue(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "step"),
			&step, SHADER_UNIFORM_INT);
		SetShaderValue(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "width"),
			&g_composite.colors.width, SHADER_UNIFORM_INT);
		SetShaderValue(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "height"),
			&g_composite.colors.height, SHADER_UNIFORM_INT);
		SetShaderValue(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "sigma_color"),
			&g_composite.sColor, SHADER_UNIFORM_FLOAT);
		SetShaderValue(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "sigma_depth"),
			&g_composite.sDepth, SHADER_UNIFORM_FLOAT);
		SetShaderValue(
			g_composite.shaders.denoise,
			GetShaderLocation(g_composite.shaders.denoise, "sigma_normal"),
			&g_composite.sNormal, SHADER_UNIFORM_FLOAT);
		DrawTexturePro(
			tmp.texture,
			(Rectangle){ 0, 0, g_composite.target.texture.width, -g_composite.target.texture.height },
			(Rectangle){ 0, 0, g_composite.target.texture.width, g_composite.target.texture.height },
			(Vector2){ 0, 0 }, 0.0f, WHITE);
		EndShaderMode();
		EndTextureMode();
		RenderTexture2D t = tmp;
		tmp = curr;
		curr = t;
		step *= 2;
	}

	BeginTextureMode(g_composite.target);
	BeginShaderMode(g_composite.shaders.modulate);
	SetShaderValueTexture(
		g_composite.shaders.modulate,
		GetShaderLocation(g_composite.shaders.modulate, "image"),
		curr.texture);
	SetShaderValueTexture(
		g_composite.shaders.modulate,
		GetShaderLocation(g_composite.shaders.modulate, "albedo"),
		g_composite.albedo);
	DrawTexturePro(
		curr.texture,
		(Rectangle){ 0, 0, g_composite.target.texture.width, -g_composite.target.texture.height },
		(Rectangle){ 0, 0, g_composite.target.texture.width, g_composite.target.texture.height },
		(Vector2){ 0, 0 }, 0.0f, WHITE);
	EndShaderMode();
	EndTextureMode();
}

void DrawComposite(int x, int y, int w, int h) {
	float psuedo_w;
	float psuedo_h;
	if (h < w) {
		psuedo_w = g_composite.colors.width;
		psuedo_h = ((float)h)/((float)w) * g_composite.colors.height;
	} else {
		psuedo_h = g_composite.colors.height;
		psuedo_w = ((float)w)/((float)h) * g_composite.colors.width;
	}
	
    DrawTexturePro(
        g_composite.target.texture,
        (Rectangle){
            (g_composite.colors.width / 2.0f) - (psuedo_w/2.0f),
            (g_composite.colors.height / 2.0f) - (psuedo_h/2.0f),
            psuedo_w,
            -psuedo_h },
        (Rectangle){ x, y, w, h},
        (Vector2){ 0, 0 },
        0.0f,
        WHITE);
}

Composite* CompositeRef() {
	return &g_composite;
}
