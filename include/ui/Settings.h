#ifndef SETTINGS_H
#define SETTINGS_H

#include "imgui.h"
#include "datatypes/doom_settings.h"
#include "datatypes/general_settings.h"

class IKevS_DataCollector;

class Settings 
{
public:
	static void RenderUI(ImFont *font, GeneralSettings *gen_settings, DoomSettings *doom_settings, bool con_status);
};

#endif // SETTINGS_H