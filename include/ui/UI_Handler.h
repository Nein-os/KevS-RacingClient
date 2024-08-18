#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include "imgui.h"
#include "datatypes/doom_settings.h"
#include "datatypes/general_settings.h"

class IKevS_DataCollector;

class UI_Handler
{
public:
	static void RenderWholeUI(ImFont *font, IKevS_DataCollector *data, GeneralSettings*, DoomSettings*);

};

#endif // UI_HANDLER_H