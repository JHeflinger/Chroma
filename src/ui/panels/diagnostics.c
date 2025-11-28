#include "diagnostics.h"
#include "data/assets.h"
#include <easymemory.h>

BOOL g_vsync_enabled = TRUE;
BOOL g_time_paused = TRUE;

float poop = 0.0f;

void DrawDevPanel(float width, float height) {
    UIDrawText("Application FPS: %d", (int)(1.0f / GetFrameTime()));
    UIDrawText("Frame time: %.6f ms", (1000.0f * GetFrameTime()));
    BOOL vsync = g_vsync_enabled;
	UICheckboxLabeled("VSYNC:", &vsync);
    if (vsync != g_vsync_enabled) {
        g_vsync_enabled = vsync;
        if (g_vsync_enabled)
            SetWindowState(FLAG_VSYNC_HINT);
        else
            ClearWindowState(FLAG_VSYNC_HINT);
    }
    if (EZ_ALLOCATED() > 1000000000) {
        UIDrawText("CPU Memory Usage: %.3f GB (%d bytes)", ((float)EZ_ALLOCATED()) / 1000000000, (int)EZ_ALLOCATED());
    } else if (EZ_ALLOCATED() > 1000000) {
        UIDrawText("CPU Memory Usage: %.3f MB (%d bytes)", ((float)EZ_ALLOCATED()) / 1000000, (int)EZ_ALLOCATED());
    } else if (EZ_ALLOCATED() > 1000) {
        UIDrawText("CPU Memory Usage: %.3f KB (%d bytes)", ((float)EZ_ALLOCATED()) / 1000, (int)EZ_ALLOCATED());
    } else {
        UIDrawText("CPU Memory Usage: %d bytes", (int)EZ_ALLOCATED());
    }
}

void ConfigureDiagnosticsPanel(Panel* panel) {
    SetupPanel(panel, "Diagnostics");
    panel->draw = DrawDevPanel;
}
