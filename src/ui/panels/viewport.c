#include "viewport.h"
#include "data/input.h"
#include <easylogger.h>
#include <rlgl.h>

RenderTexture2D g_viewport_target;

void DrawViewportPanel(float width, float height) {
    DrawTexturePro(
        g_viewport_target.texture,
        (Rectangle){ 0, 0, g_viewport_target.texture.width, -g_viewport_target.texture.height },
        (Rectangle){ 0, 0, g_viewport_target.texture.width, g_viewport_target.texture.height },
        (Vector2){ 0, 0 }, 0, WHITE);
}

void UpdateViewportPanel(float width, float height) {
    BeginTextureMode(g_viewport_target);
    rlSetBlendFactorsSeparate(RL_SRC_ALPHA, RL_ONE_MINUS_SRC_ALPHA, RL_ONE, RL_ONE, RL_FUNC_ADD, RL_MAX);
    BeginBlendMode(BLEND_CUSTOM_SEPARATE);
    EndBlendMode();
    EndTextureMode();
}

void ConfigureViewportPanel(Panel* panel) {
    SetupPanel(panel, "Viewport");
    panel->draw = DrawViewportPanel;
    panel->update = UpdateViewportPanel;
    g_viewport_target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}
