#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include "imgui.h"

class IKevS_DataCollector;

class UI_Handler
{
public:
	static void RenderWholeUI(ImFont *font, IKevS_DataCollector *data);

};

#endif // UI_HANDLER_H