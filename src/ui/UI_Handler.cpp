#include "ui/UI_Handler.h"
#include "ui/DoomWheel.h"
#include "ui/Settings.h"
#include "ui/DBClient.h"

//#define IMSPINNER_DEMO
//#define TESTING_IR
//#define IMGUI_DEMO

#include "imgui.h"
#include <math.h>
#include "imspinner.h"
#include "irsdk_client.h"
#include "defines.h"
#include "connection/data_collector.h"

struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	bool                ScrollToBottom;

	void    Clear() { Buf.clear(); }

	void    AddLog(const char* fmt, ...)// IM_PRINTFARGS(2)
	{
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* p_opened = NULL)
	{
		ImGui::Begin(title, p_opened);
		ImGui::TextUnformatted(Buf.begin());
		if (ScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;
		ImGui::End();
	}
};

void UI_Handler::RenderWholeUI(ImFont *font, IKevS_DataCollector *data, GeneralSettings *gen_settings, DoomSettings *doom_settings)
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("KevS-Test", nullptr, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	
	DoomWheel::RenderUI(font, data, doom_settings);
	Settings::RenderUI(font, gen_settings, doom_settings, data->is_connected());
	if (gen_settings->b_db_client_show)
		DBClient::RenderUI(font);
	ImGui::End();
}