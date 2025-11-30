#include "core/editor.h"
#include "core/global.h"
#include <easylogger.h>

int main(int argc, char** argv) {
	if (argc != 2) EZ_FATAL("Invalid usage - must give filepath as argument.");
	Global()->filepath = argv[1];
	RunEditor();
    EZ_INFO("See you, Space Cowboy");
    return 0;
}
