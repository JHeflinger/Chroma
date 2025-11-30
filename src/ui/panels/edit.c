#include "edit.h"
#include "data/assets.h"
#include "compositor/compositor.h"
#include <easymemory.h>
#include <float.h>
#include <limits.h>

void DrawEditPanel(float width, float height) {
	UIMoveCursor(0, 5);
	UIDrawText("Denoise passes");
    UIMoveCursor(110, -20);
    UIDragUInt(&(CompositeRef()->denoises), 0, UINT_MAX, 1, width - 130);
	UIMoveCursor(0, 5);
	UIDrawText("Color weight");
    UIMoveCursor(110, -20);
    UIDragFloat(&(CompositeRef()->sColor), 0.000001f, FLT_MAX, 0.01f, width - 130);
	UIMoveCursor(0, 5);
	UIDrawText("Normal weight");
    UIMoveCursor(110, -20);
    UIDragFloat(&(CompositeRef()->sNormal), 0.000001f, FLT_MAX, 0.5f, width - 130);
	UIMoveCursor(0, 5);
	UIDrawText("Depth weight");
    UIMoveCursor(110, -20);
    UIDragFloat(&(CompositeRef()->sDepth), 0.000001f, FLT_MAX, 0.05f, width - 130);
}

void ConfigureEditPanel(Panel* panel) {
    SetupPanel(panel, "Edit");
    panel->draw = DrawEditPanel;
}
