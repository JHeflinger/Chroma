#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include "core/file.h"
#include <raylib.h>
#include <easyobjects.h>
#include <stdlib.h>

typedef struct {
	Shader denoise;
	Shader demodulate;
	Shader modulate;
} CompositeShaders;

typedef struct {
	RenderTexture2D target;
	RenderTexture2D temp;
	Texture2D colors;
	Texture2D normals;
	Texture2D positions;
	Texture2D albedo;
	float sColor;
	float sNormal;
	float sDepth;
	uint32_t denoises;
	CompositeShaders shaders;
} Composite;

void LoadComposite(const char* filepath);

void CleanComposite();

void RenderComposite();

void DrawComposite(int x, int y, int w, int h);

Composite* CompositeRef();

#endif
