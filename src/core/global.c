#include "global.h"

GlobalData g_global = { 0 };

GlobalData* Global() {
	return &g_global;
}
