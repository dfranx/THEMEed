#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <SDL2/SDL.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include <string>
#include <sstream>
#include <algorithm>

#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_sdl.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <ImGuiColorTextEdit/TextEditor.h>
#include <inih/INIReader.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

// SDL defines main
#undef main

struct CustomColors {
	ImVec4 ComputePass;
	ImVec4 ErrorMessage;
	ImVec4 WarningMessage;
	ImVec4 InfoMessage;
};

const std::string EditorColorNames[] = {
	"Default",
	"Keyword",
	"Number",
	"String",
	"CharLiteral",
	"Punctuation",
	"Preprocessor",
	"Identifier",
	"KnownIdentifier",
	"PreprocIdentifier",
	"Comment",
	"MultiLineComment",
	"Background",
	"Cursor",
	"Selection",
	"ErrorMarker",
	"Breakpoint",
	"BreakpointOutline",
	"CurrentLineIndicator",
	"CurrentLineIndicatorOutline",
	"LineNumber",
	"CurrentLineFill",
	"CurrentLineFillInactive",
	"CurrentLineEdge",
	"ErrorMessage",
	"BreakpointDisabled",
	"UserFunction",
	"UserType",
	"UniformVariable",
	"GlobalVariable",
	"LocalVariable",
	"FunctionArgument"
};

std::ostream& operator<<(std::ostream& out, const ImVec4& vec)
{
	out << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
	return out;
}
std::string buildIndent(const char* name)
{
	return std::string(std::max<int>(0, 28-strlen(name)), ' ');
}

void buildStyle(std::string& styleContent, const std::string& name, int version, const ImGuiStyle& style, const TextEditor::Palette& editor, const CustomColors& customs)
{
	styleContent = "";

	std::stringstream ss;

	ss << "[general]" << std::endl;
	ss << "name=" << name << std::endl;
	ss << "version=" << version << std::endl;
	ss << "editor=Custom" << std::endl << std::endl;

	ss << "[style]" << std::endl;
	ss << "Alpha=" << style.Alpha << std::endl;
	ss << "WindowPaddingX=" << style.WindowPadding.x << std::endl;
	ss << "WindowPaddingY=" << style.WindowPadding.y << std::endl;
	ss << "WindowRounding=" << style.WindowRounding << std::endl;
	ss << "WindowBorderSize=" << style.WindowBorderSize << std::endl;
	ss << "WindowMinSizeX=" << style.WindowMinSize.x << std::endl;
	ss << "WindowMinSizeY=" << style.WindowMinSize.y << std::endl;
	ss << "WindowTitleAlignX=" << style.WindowTitleAlign.x << std::endl;
	ss << "WindowTitleAlignY=" << style.WindowTitleAlign.y << std::endl;
	ss << "ChildRounding=" << style.ChildRounding << std::endl;
	ss << "ChildBorderSize=" << style.ChildBorderSize << std::endl;
	ss << "PopupRounding=" << style.PopupRounding << std::endl;
	ss << "PopupBorderSize=" << style.PopupBorderSize << std::endl;
	ss << "FramePaddingX=" << style.FramePadding.x << std::endl;
	ss << "FramePaddingY=" << style.FramePadding.y << std::endl;
	ss << "FrameRounding=" << style.FrameRounding << std::endl;
	ss << "FrameBorderSize=" << style.FrameBorderSize << std::endl;
	ss << "ItemSpacingX=" << style.ItemSpacing.x << std::endl;
	ss << "ItemSpacingY=" << style.ItemSpacing.y << std::endl;
	ss << "ItemInnerSpacingX=" << style.ItemInnerSpacing.x << std::endl;
	ss << "ItemInnerSpacingY=" << style.ItemInnerSpacing.y << std::endl;
	ss << "TouchExtraPaddingX=" << style.TouchExtraPadding.x << std::endl;
	ss << "TouchExtraPaddingY=" << style.TouchExtraPadding.y << std::endl;
	ss << "IndentSpacing=" << style.IndentSpacing << std::endl;
	ss << "ColumnsMinSpacing=" << style.ColumnsMinSpacing << std::endl;
	ss << "ScrollbarSize=" << style.ScrollbarSize << std::endl;
	ss << "ScrollbarRounding=" << style.ScrollbarRounding << std::endl;
	ss << "GrabMinSize=" << style.GrabMinSize << std::endl;
	ss << "GrabRounding=" << style.GrabRounding << std::endl;
	ss << "TabRounding=" << style.TabRounding << std::endl;
	ss << "TabBorderSize=" << style.TabBorderSize << std::endl;
	ss << "ButtonTextAlignX=" << style.ButtonTextAlign.x << std::endl;
	ss << "ButtonTextAlignY=" << style.ButtonTextAlign.y << std::endl;
	ss << "DisplayWindowPaddingX=" << style.DisplayWindowPadding.x << std::endl;
	ss << "DisplayWindowPaddingY=" << style.DisplayWindowPadding.y << std::endl;
	ss << "DisplaySafeAreaPaddingX=" << style.DisplaySafeAreaPadding.x << std::endl;
	ss << "DisplaySafeAreaPaddingY=" << style.DisplaySafeAreaPadding.y << std::endl;
	ss << "MouseCursorScale=" << style.MouseCursorScale << std::endl;
	ss << "AntiAliasedLines=" << style.AntiAliasedLines << std::endl;
	ss << "AntiAliasedFill=" << style.AntiAliasedFill << std::endl;
	ss << "CurveTessellationTol=" << style.CurveTessellationTol << std::endl << std::endl;

	ss << "[colors]" << std::endl;
	for (int i = 0; i < ImGuiCol_COUNT; i++) {
		const char* name = ImGui::GetStyleColorName(i);
		ss << name << buildIndent(name) << "= " << style.Colors[i] << std::endl;
	}
	ss << "ComputePass" << buildIndent("ComputePass") << "= " << customs.ComputePass << std::endl;
	ss << "InfoMessage" << buildIndent("InfoMessage") << "= " << customs.ComputePass << std::endl;
	ss << "WarningMessage" << buildIndent("WarningMessage") << "= " << customs.WarningMessage << std::endl;
	ss << "ErrorMessage" << buildIndent("ErrorMessage") << "= " << customs.ErrorMessage << std::endl << std::endl;
	
	ss << "[editor]" << std::endl;
	for (int i = 0; i < (int)TextEditor::PaletteIndex::Max; i++)
	{
		const char* name = EditorColorNames[i].c_str();
		ImVec4 actualColor = ImGui::ColorConvertU32ToFloat4(editor[i]);
		ss << name << buildIndent(name) << "= " << actualColor << std::endl;
	}
	ss << std::endl;

	styleContent = ss.str();
}

ImVec4 parseColor(const std::string& str)
{
	float res[4] = { 0, 0, 0, 0 };
	int cur = 0;

	std::stringstream ss(str);
	while (ss.good() && cur <= 3) {
		std::string substr;
		std::getline(ss, substr, ',');
		res[cur] = std::stof(substr);

		cur++;
	}

	return ImVec4(res[0], res[1], res[2], res[3]);
}
ImVec4 loadColor(const INIReader& ini, const std::string& clrName)
{
	std::string clr = ini.Get("colors", clrName, "0");
	if (clr == "0") {
		// default colors
		if (clrName == "ComputePass")
			return ImVec4(1, 0, 0, 1);
		else if (clrName == "OutputError")
			return ImVec4(1.0f, 0.17f, 0.13f, 1.0f);
		else if (clrName == "OutputWarning")
			return ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
		else if (clrName == "OutputMessage")
			return ImVec4(0.106f, 0.631f, 0.886f, 1.0f);
	}
	else
		return parseColor(clr);
}
std::string loadTheme(const std::string& filename, char* themeName, int& version, ImGuiStyle& style, TextEditor::Palette& editor, CustomColors& customs)
{
	INIReader ini(filename);
	if (ini.ParseError() != 0)
		return "";

	std::string name = ini.Get("general", "name", "NULL");
	std::string editorTheme = ini.Get("general", "editor", "Dark");

	version = ini.GetInteger("general", "version", 1);
	strcpy(themeName, name.substr(0, std::min<size_t>(64, name.size())).c_str());

	ImGuiStyle defaultStyle = ImGui::GetStyle();

	for (int i = 0; i < ImGuiCol_COUNT; i++) {
		const char* name = ImGui::GetStyleColorName(i);
		std::string clr = ini.Get("colors", name, "0");

		if (clr == "0")
			style.Colors[(ImGuiCol_)i] = defaultStyle.Colors[(ImGuiCol_)i];
		else
			style.Colors[(ImGuiCol_)i] = parseColor(clr);
	}
	customs.ComputePass = loadColor(ini, "ComputePass");
	customs.ErrorMessage = loadColor(ini, "OutputError");
	customs.WarningMessage = loadColor(ini, "OutputWarning");
	customs.InfoMessage = loadColor(ini, "OutputMessage");

	style.Alpha = ini.GetReal("style", "Alpha", defaultStyle.Alpha);
	style.WindowPadding.x = ini.GetReal("style", "WindowPaddingX", defaultStyle.WindowPadding.x);
	style.WindowPadding.y = ini.GetReal("style", "WindowPaddingY", defaultStyle.WindowPadding.y);
	style.WindowRounding = ini.GetReal("style", "WindowRounding", defaultStyle.WindowRounding);
	style.WindowBorderSize = ini.GetReal("style", "WindowBorderSize", defaultStyle.WindowBorderSize);
	style.WindowMinSize.x = ini.GetReal("style", "WindowMinSizeX", defaultStyle.WindowMinSize.x);
	style.WindowMinSize.y = ini.GetReal("style", "WindowMinSizeY", defaultStyle.WindowMinSize.y);
	style.WindowTitleAlign.x = ini.GetReal("style", "WindowTitleAlignX", defaultStyle.WindowTitleAlign.x);
	style.WindowTitleAlign.y = ini.GetReal("style", "WindowTitleAlignY", defaultStyle.WindowTitleAlign.y);
	style.ChildRounding = ini.GetReal("style", "ChildRounding", defaultStyle.ChildRounding);
	style.ChildBorderSize = ini.GetReal("style", "ChildBorderSize", defaultStyle.ChildBorderSize);
	style.PopupRounding = ini.GetReal("style", "PopupRounding", defaultStyle.PopupRounding);
	style.PopupBorderSize = ini.GetReal("style", "PopupBorderSize", defaultStyle.PopupBorderSize);
	style.FramePadding.x = ini.GetReal("style", "FramePaddingX", defaultStyle.FramePadding.x);
	style.FramePadding.y = ini.GetReal("style", "FramePaddingY", defaultStyle.FramePadding.y);
	style.FrameRounding = ini.GetReal("style", "FrameRounding", defaultStyle.FrameRounding);
	style.FrameBorderSize = ini.GetReal("style", "FrameBorderSize", defaultStyle.FrameBorderSize);
	style.ItemSpacing.x = ini.GetReal("style", "ItemSpacingX", defaultStyle.ItemSpacing.x);
	style.ItemSpacing.y = ini.GetReal("style", "ItemSpacingY", defaultStyle.ItemSpacing.y);
	style.ItemInnerSpacing.x = ini.GetReal("style", "ItemInnerSpacingX", defaultStyle.ItemInnerSpacing.x);
	style.ItemInnerSpacing.y = ini.GetReal("style", "ItemInnerSpacingY", defaultStyle.ItemInnerSpacing.y);
	style.TouchExtraPadding.x = ini.GetReal("style", "TouchExtraPaddingX", defaultStyle.TouchExtraPadding.x);
	style.TouchExtraPadding.y = ini.GetReal("style", "TouchExtraPaddingY", defaultStyle.TouchExtraPadding.y);
	style.IndentSpacing = ini.GetReal("style", "IndentSpacing", defaultStyle.IndentSpacing);
	style.ColumnsMinSpacing = ini.GetReal("style", "ColumnsMinSpacing", defaultStyle.ColumnsMinSpacing);
	style.ScrollbarSize = ini.GetReal("style", "ScrollbarSize", defaultStyle.ScrollbarSize);
	style.ScrollbarRounding = ini.GetReal("style", "ScrollbarRounding", defaultStyle.ScrollbarRounding);
	style.GrabMinSize = ini.GetReal("style", "GrabMinSize", defaultStyle.GrabMinSize);
	style.GrabRounding = ini.GetReal("style", "GrabRounding", defaultStyle.GrabRounding);
	style.TabRounding = ini.GetReal("style", "TabRounding", defaultStyle.TabRounding);
	style.TabBorderSize = ini.GetReal("style", "TabBorderSize", defaultStyle.TabBorderSize);
	style.ButtonTextAlign.x = ini.GetReal("style", "ButtonTextAlignX", defaultStyle.ButtonTextAlign.x);
	style.ButtonTextAlign.y = ini.GetReal("style", "ButtonTextAlignY", defaultStyle.ButtonTextAlign.y);
	style.DisplayWindowPadding.x = ini.GetReal("style", "DisplayWindowPaddingX", defaultStyle.DisplayWindowPadding.x);
	style.DisplayWindowPadding.y = ini.GetReal("style", "DisplayWindowPaddingY", defaultStyle.DisplayWindowPadding.y);
	style.DisplaySafeAreaPadding.x = ini.GetReal("style", "DisplaySafeAreaPaddingX", defaultStyle.DisplaySafeAreaPadding.x);
	style.DisplaySafeAreaPadding.y = ini.GetReal("style", "DisplaySafeAreaPaddingY", defaultStyle.DisplaySafeAreaPadding.y);
	style.MouseCursorScale = ini.GetReal("style", "MouseCursorScale", defaultStyle.MouseCursorScale);
	style.AntiAliasedLines = ini.GetBoolean("style", "AntiAliasedLines", defaultStyle.AntiAliasedLines);
	style.AntiAliasedFill = ini.GetBoolean("style", "AntiAliasedFill", defaultStyle.AntiAliasedFill);
	style.CurveTessellationTol = ini.GetReal("style", "CurveTessellationTol", defaultStyle.CurveTessellationTol);

	if (editorTheme == "Custom") {
		editor = TextEditor::GetDarkPalette();
		for (int i = 0; i < (int)TextEditor::PaletteIndex::Max; i++) {
			std::string clr = ini.Get("editor", EditorColorNames[i], "0");
			if (clr == "0") {}
			else {
				ImVec4 c = parseColor(clr);
				uint32_t r = c.x * 255;
				uint32_t g = c.y * 255;
				uint32_t b = c.z * 255;
				uint32_t a = c.w * 255;
				editor[i] = (a << 24) | (b << 16) | (g << 8) | r;
			}
		}
	}
	else if (editorTheme == "Light" || editorTheme == "Dark") {
		if (editorTheme == "Light") {
			editor = TextEditor::GetLightPalette();
			editor[(int)TextEditor::PaletteIndex::Background] = 0x00000000;
		} else if (editorTheme == "Dark") {
			editor = TextEditor::GetDarkPalette();
			editor[(int)TextEditor::PaletteIndex::Background] = 0x00000000;
		}
	}

	return name;
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

	// init sdl2
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
		printf("Failed to initialize SDL2\n");
		return 0;
	}
	else
		printf("Initialized SDL2\n");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // double buffering

	// open window
	SDL_Window* wnd = SDL_CreateWindow("THEMEed", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_SetWindowMinimumSize(wnd, 200, 200);
	SDL_MaximizeWindow(wnd);

	// get GL context
	SDL_GLContext glContext = SDL_GL_CreateContext(wnd);
	SDL_GL_MakeCurrent(wnd, glContext);
	SDL_GL_SetSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	// init glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return 0;
	}
	else
		printf("Initialized GLEW\n");


	char themeName[64] = { 0 };
	int themeVersion = 1;


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	ImGui::StyleColorsLight();
	ImGuiStyle editorStyle = ImGui::GetStyle();
	ImGuiStyle outputStyle = ImGui::GetStyle();
	TextEditor::Palette defaultTextStyle = TextEditor::GetLightPalette();
	defaultTextStyle[(int)TextEditor::PaletteIndex::Background] = 0x00000000;
	TextEditor::Palette outputTextStyle = defaultTextStyle;

	CustomColors customColors;
	customColors.ComputePass = ImVec4(1, 0, 0, 1);
	customColors.ErrorMessage = ImVec4(1.0f, 0.17f, 0.13f, 1.0f);
	customColors.WarningMessage = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
	customColors.InfoMessage = ImVec4(0.106f, 0.631f, 0.886f, 1.0f);

	// stuff for preview
	TextEditor previewEditor;
	previewEditor.SetText(R"(cbuffer cbPerFrame : register(b0)
{
	float3 lightPos;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

SamplerState smp : register(s0);

Texture2D posTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D diffuseTex : register(t2);

/*
 *	This is a totally cool function that serves it's purpose.
 */
// Hmmm.. a single line comment..
float myFunction(float n, float t)
{
	return saturate(dot(n,t));
}

float4 main(PSInput pin) : SV_TARGET
{
	pin.UV.y = 1-pin.UV.y;
	
	float4 pos = posTex.Sample(smp,pin.UV);   
	clip((pos.w != 0) - 1);
	
	float4 n = normalTex.Sample(smp, pin.UV);
	float3 normal = normalize(n.xyz);
	float3 toLight = normalize(lightPos - pos.xyz);
 
	float diffuse = myFunction(normal, toLight);
	
	float4 ret = diffuse * diffuseTex.Sample(smp, pin.UV);
	ret.a = 1.0f;
	return ret;
})");
	previewEditor.SetShowWhitespaces(true);
	previewEditor.SetHighlightLine(true);
	previewEditor.SetShowLineNumbers(true);
	previewEditor.SetHorizontalScroll(false);
	previewEditor.SetColorizerEnable(true);
	previewEditor.SetScrollbarMarkers(true);
	previewEditor.SetReadOnly(true);
	previewEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
	previewEditor.SetCurrentLineIndicator(24);
	previewEditor.SetCursorPosition(TextEditor::Coordinates(14, 0));
	previewEditor.AddBreakpoint(34);
	previewEditor.AddBreakpoint(35, "n.x > 0.1f");
	previewEditor.AddBreakpoint(36, "", false);
	previewEditor.SetErrorMarkers({ { 31, "This is just for previewing" } });
	previewEditor.SetUIScale(1.0f);
	previewEditor.SetUIFontSize(18.0f);
	previewEditor.SetEditorFontSize(20.0f);
	previewEditor.ClearAutocompleteEntries();
	previewEditor.AddAutocompleteFunction("main", 27, 43, std::vector<std::string>(), { "pos", "n", "normal", "toLight", "diffuse", "ret" });
	previewEditor.AddAutocompleteFunction("myFunction", 22, 25, { "n", "t" }, std::vector<std::string>());
	previewEditor.AddAutocompleteGlobal("smp");
	previewEditor.AddAutocompleteGlobal("posTex");
	previewEditor.AddAutocompleteGlobal("normalTex");
	previewEditor.AddAutocompleteGlobal("diffuseTex");
	previewEditor.AddAutocompleteUniform("lightPos");
	previewEditor.AddAutocompleteUserType("PSInput");
	previewEditor.AddAutocompleteUserType("cbPerFrame");
	previewEditor.SetPalette(outputTextStyle);

	TextEditor outputEditor;
	outputEditor.SetColorizerEnable(false);
	outputEditor.SetSidebarVisible(false);
	outputEditor.SetFunctionTooltips(false);
	outputEditor.SetPalette(defaultTextStyle);
	outputEditor.SetScrollbarMarkers(false);

	std::string currentStyleContent = "";
	buildStyle(currentStyleContent, themeName, themeVersion, outputStyle, outputTextStyle, customColors);
	outputEditor.SetText(currentStyleContent);
	outputEditor.OnContentUpdate = [&](TextEditor* editor) {
		currentStyleContent = editor->GetText();

		std::ofstream oFile("temp.ini");
		oFile << currentStyleContent << std::endl;
		oFile.close();

		loadTheme("temp.ini", themeName, themeVersion, outputStyle, outputTextStyle, customColors);
		previewEditor.SetPalette(outputTextStyle);
	};

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(wnd, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");

	io.Fonts->AddFontDefault();
	ImFont* textEditorFont = io.Fonts->AddFontFromFileTTF("data/inconsolata.ttf", 20.0f);
	ImFont* previewFont = io.Fonts->AddFontFromFileTTF("data/NotoSans.ttf", 18.0f);

	// timer for time delta
	SDL_Event event;
	bool run = true;
	bool testWindow1 = true, testWindow2 = true;
	while (run) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				run = false;
			ImGui_ImplSDL2_ProcessEvent(&event);
		}

		if (!run) break;

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(wnd);
		ImGui::NewFrame();

		ImGuiViewport* viewport = ImGui::GetMainViewport();

		/* EDITOR */
		ImGui::GetStyle() = editorStyle;

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Load from file"))
					igfd::ImGuiFileDialog::Instance()->OpenModal("LoadThemeDlg", "Open SHADERed theme file", "INI file (*.ini){.ini},.*", ".");
				if (ImGui::MenuItem("Save to file"))
					igfd::ImGuiFileDialog::Instance()->OpenModal("SaveThemeDlg", "Save SHADERed theme file", "INI file (*.ini){.ini},.*", ".");

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f + ImGui::GetFrameHeight()), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x / 2.0f - 10.0f, viewport->Size.y - 10.0f - ImGui::GetFrameHeight()));
		if (ImGui::Begin("Editor", 0, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
			if (ImGui::BeginTabBar("BrowseOnlineTabBar")) {
				if (ImGui::BeginTabItem("Editor")) {
					bool updateData = false;

					if (ImGui::Button("Set to light theme")) {
						ImGui::StyleColorsLight();
						editorStyle = ImGui::GetStyle();
						outputStyle = ImGui::GetStyle();
						defaultTextStyle = TextEditor::GetLightPalette();
						defaultTextStyle[(int)TextEditor::PaletteIndex::Background] = 0x00000000;
						outputTextStyle = defaultTextStyle;
						previewEditor.SetPalette(outputTextStyle);
						outputEditor.SetPalette(defaultTextStyle);

						customColors.ComputePass = ImVec4(1, 0, 0, 1);
						customColors.ErrorMessage = ImVec4(1.0f, 0.17f, 0.13f, 1.0f);
						customColors.WarningMessage = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
						customColors.InfoMessage = ImVec4(0.106f, 0.631f, 0.886f, 1.0f);

						updateData = true;
					}
					ImGui::SameLine();
					if (ImGui::Button("Set to dark theme")) {
						ImGui::StyleColorsDark();
						editorStyle = ImGui::GetStyle();
						outputStyle = ImGui::GetStyle();
						defaultTextStyle = TextEditor::GetDarkPalette();
						defaultTextStyle[(int)TextEditor::PaletteIndex::Background] = 0x00000000;
						outputTextStyle = defaultTextStyle;
						previewEditor.SetPalette(outputTextStyle);
						outputEditor.SetPalette(defaultTextStyle);

						customColors.ComputePass = ImVec4(1, 0, 0, 1);
						customColors.ErrorMessage = ImVec4(1.0f, 0.17f, 0.13f, 1.0f);
						customColors.WarningMessage = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
						customColors.InfoMessage = ImVec4(0.106f, 0.631f, 0.886f, 1.0f);

						updateData = true;
					}

					if (ImGui::CollapsingHeader("Information")) {
						updateData |= ImGui::InputText("Theme name", themeName, sizeof(themeName));
						updateData |= ImGui::InputInt("Version", &themeVersion);

						if (themeVersion <= 0)
							themeVersion = 1;
					}

					if (ImGui::CollapsingHeader("Variables")) {
						ImGui::Text("Main");
						updateData |= ImGui::SliderFloat2("WindowPadding", (float*)&outputStyle.WindowPadding, 0.0f, 20.0f, "%.0f");
						updateData |= ImGui::SliderFloat2("FramePadding", (float*)&outputStyle.FramePadding, 0.0f, 20.0f, "%.0f");
						updateData |= ImGui::SliderFloat2("CellPadding", (float*)&outputStyle.CellPadding, 0.0f, 20.0f, "%.0f");
						updateData |= ImGui::SliderFloat2("ItemSpacing", (float*)&outputStyle.ItemSpacing, 0.0f, 20.0f, "%.0f");
						updateData |= ImGui::SliderFloat2("ItemInnerSpacing", (float*)&outputStyle.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
						updateData |= ImGui::SliderFloat2("TouchExtraPadding", (float*)&outputStyle.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
						updateData |= ImGui::SliderFloat("IndentSpacing", &outputStyle.IndentSpacing, 0.0f, 30.0f, "%.0f");
						updateData |= ImGui::SliderFloat("ScrollbarSize", &outputStyle.ScrollbarSize, 1.0f, 20.0f, "%.0f");
						updateData |= ImGui::SliderFloat("GrabMinSize", &outputStyle.GrabMinSize, 1.0f, 20.0f, "%.0f");
						ImGui::Text("Borders");
						updateData |= ImGui::SliderFloat("WindowBorderSize", &outputStyle.WindowBorderSize, 0.0f, 1.0f, "%.0f");
						updateData |= ImGui::SliderFloat("ChildBorderSize", &outputStyle.ChildBorderSize, 0.0f, 1.0f, "%.0f");
						updateData |= ImGui::SliderFloat("PopupBorderSize", &outputStyle.PopupBorderSize, 0.0f, 1.0f, "%.0f");
						updateData |= ImGui::SliderFloat("FrameBorderSize", &outputStyle.FrameBorderSize, 0.0f, 1.0f, "%.0f");
						updateData |= ImGui::SliderFloat("TabBorderSize", &outputStyle.TabBorderSize, 0.0f, 1.0f, "%.0f");
						ImGui::Text("Rounding");
						updateData |= ImGui::SliderFloat("WindowRounding", &outputStyle.WindowRounding, 0.0f, 12.0f, "%.0f");
						updateData |= ImGui::SliderFloat("ChildRounding", &outputStyle.ChildRounding, 0.0f, 12.0f, "%.0f");
						updateData |= ImGui::SliderFloat("FrameRounding", &outputStyle.FrameRounding, 0.0f, 12.0f, "%.0f");
						updateData |= ImGui::SliderFloat("PopupRounding", &outputStyle.PopupRounding, 0.0f, 12.0f, "%.0f");
						updateData |= ImGui::SliderFloat("ScrollbarRounding", &outputStyle.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
						updateData |= ImGui::SliderFloat("GrabRounding", &outputStyle.GrabRounding, 0.0f, 12.0f, "%.0f");
						updateData |= ImGui::SliderFloat("TabRounding", &outputStyle.TabRounding, 0.0f, 12.0f, "%.0f");
						ImGui::Text("Alignment");
						updateData |= ImGui::SliderFloat2("WindowTitleAlign", (float*)&outputStyle.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
						int window_menu_button_position = outputStyle.WindowMenuButtonPosition + 1;
						if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0")) {
							outputStyle.WindowMenuButtonPosition = window_menu_button_position - 1;
							updateData = true;
						}
						updateData |= ImGui::Combo("ColorButtonPosition", (int*)&outputStyle.ColorButtonPosition, "Left\0Right\0");
						updateData |= ImGui::SliderFloat2("ButtonTextAlign", (float*)&outputStyle.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
						updateData |= ImGui::SliderFloat2("SelectableTextAlign", (float*)&outputStyle.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
						ImGui::Text("Safe Area Padding");
						updateData |= ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&outputStyle.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
					}

					if (ImGui::CollapsingHeader("Rendering"))
					{
						updateData |= ImGui::Checkbox("Anti-aliased lines", &outputStyle.AntiAliasedLines);
						updateData |= ImGui::Checkbox("Anti-aliased lines use texture", &outputStyle.AntiAliasedLinesUseTex);
						updateData |= ImGui::Checkbox("Anti-aliased fill", &outputStyle.AntiAliasedFill);
						ImGui::PushItemWidth(100);
						updateData |= ImGui::DragFloat("Curve Tessellation Tolerance", &outputStyle.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
						if (outputStyle.CurveTessellationTol < 0.10f) outputStyle.CurveTessellationTol = 0.10f;
						updateData |= ImGui::DragFloat("Circle segment Max Error", &outputStyle.CircleSegmentMaxError, 0.01f, 0.10f, 10.0f, "%.2f");
						updateData |= ImGui::DragFloat("Global Alpha", &outputStyle.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
						ImGui::PopItemWidth();
					}

					if (ImGui::CollapsingHeader("UI colors"))
					{
						static ImGuiTextFilter filter;
						filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

						ImGui::BeginChild("##ui_colors", ImVec2(0, 1000), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
						ImGui::PushItemWidth(-160);
						for (int i = 0; i < ImGuiCol_COUNT; i++)
						{
							const char* name = ImGui::GetStyleColorName(i);
							if (!filter.PassFilter(name))
								continue;
							ImGui::PushID(i);
							updateData |= ImGui::ColorEdit4("##color", (float*)&outputStyle.Colors[i], ImGuiColorEditFlags_AlphaBar);
							ImGui::SameLine(0.0f, editorStyle.ItemInnerSpacing.x);
							ImGui::TextUnformatted(name);
							ImGui::PopID();
						}

						// custom colors
						if (filter.PassFilter("ComputePass")) {
							updateData |= ImGui::ColorEdit4("##computpass_color", (float*)&customColors.ComputePass, ImGuiColorEditFlags_AlphaBar);
							ImGui::SameLine(0.0f, editorStyle.ItemInnerSpacing.x);
							ImGui::TextUnformatted("ComputePass");
						}
						if (filter.PassFilter("ErrorMessage")) {
							updateData |= ImGui::ColorEdit4("##error_msg_color", (float*)&customColors.ErrorMessage, ImGuiColorEditFlags_AlphaBar);
							ImGui::SameLine(0.0f, editorStyle.ItemInnerSpacing.x);
							ImGui::TextUnformatted("ErrorMessage");
						}
						if (filter.PassFilter("WarningMessage")) {
							updateData |= ImGui::ColorEdit4("##warning_msg_color", (float*)&customColors.WarningMessage, ImGuiColorEditFlags_AlphaBar);
							ImGui::SameLine(0.0f, editorStyle.ItemInnerSpacing.x);
							ImGui::TextUnformatted("WarningMessage");
						}
						if (filter.PassFilter("InfoMessage")) {
							updateData |= ImGui::ColorEdit4("##info_msg_color", (float*)&customColors.InfoMessage, ImGuiColorEditFlags_AlphaBar);
							ImGui::SameLine(0.0f, editorStyle.ItemInnerSpacing.x);
							ImGui::TextUnformatted("InfoMessage");
						}


						ImGui::PopItemWidth();
						ImGui::EndChild();
					}

					if (ImGui::CollapsingHeader("TextEditor colors"))
					{
						static ImGuiTextFilter filter;
						filter.Draw("Filter editor colors", ImGui::GetFontSize() * 16);

						ImGui::BeginChild("##text_ui_colors", ImVec2(0, 770), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
						ImGui::PushItemWidth(-160);
						
						bool needsColorRefresh = false;

						for (int i = 0; i < (int)TextEditor::PaletteIndex::Max; i++)
						{
							const char* name = EditorColorNames[i].c_str();
							if (!filter.PassFilter(name))
								continue;
							ImGui::PushID(i);
							ImVec4 actualColor = ImGui::ColorConvertU32ToFloat4(outputTextStyle[i]);
							bool changed = ImGui::ColorEdit4("##editor_color", (float*)&actualColor, ImGuiColorEditFlags_AlphaBar);
							needsColorRefresh |= changed;
							outputTextStyle[i] = ImGui::ColorConvertFloat4ToU32(actualColor);
							ImGui::SameLine(0.0f, editorStyle.ItemInnerSpacing.x);
							ImGui::TextUnformatted(name);
							ImGui::PopID();
						}

						if (needsColorRefresh) {
							updateData = true;
							previewEditor.SetPalette(outputTextStyle);
						}

						ImGui::PopItemWidth();
						ImGui::EndChild();
					}




					if (updateData) {
						buildStyle(currentStyleContent, themeName, themeVersion, outputStyle, outputTextStyle, customColors);
						outputEditor.SetText(currentStyleContent);
						updateData = false;
					}

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Output")) {
					ImGui::PushFont(textEditorFont);
					outputEditor.Render("StyleTextEditor");
					ImGui::PopFont();

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::End();

		if (igfd::ImGuiFileDialog::Instance()->FileDialog("LoadThemeDlg")) {
			if (igfd::ImGuiFileDialog::Instance()->IsOk) {
				std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilepathName();
				loadTheme(filePathName, themeName, themeVersion, outputStyle, outputTextStyle, customColors);
				previewEditor.SetPalette(outputTextStyle);
			}

			igfd::ImGuiFileDialog::Instance()->CloseDialog("LoadThemeDlg");
		}
		if (igfd::ImGuiFileDialog::Instance()->FileDialog("SaveThemeDlg")) {
			if (igfd::ImGuiFileDialog::Instance()->IsOk) {
				std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilepathName();

				std::ofstream oFile(filePathName);
				oFile << currentStyleContent << std::endl;
				oFile.close();
			}

			igfd::ImGuiFileDialog::Instance()->CloseDialog("SaveThemeDlg");
		}


		/* PREVIEW */
		ImGui::GetStyle() = outputStyle;
		ImGui::SetNextWindowPos(ImVec2(viewport->Size.x / 2.0f + 5.0f, 5.0f + ImGui::GetFrameHeight()), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x / 2.0f - 10.0f, viewport->Size.y - 10.0f - ImGui::GetFrameHeight()));
		ImGui::PushFont(previewFont);
		if (ImGui::Begin("Preview", 0, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
			ImGui::Text("Theme preview");


			ImGui::PushFont(textEditorFont);
			previewEditor.Render("TextEditor", ImVec2(0.0f, 600.0f), true);
			ImGui::PopFont();
			ImGui::NewLine();



			if (ImGui::Button("Show test window #1"))
				testWindow1 = true;
			ImGui::SameLine();
			if (ImGui::Button("Show test window #2"))
				testWindow2 = true;



			ImGui::Text("Here's how:");
			ImGui::Indent(60.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, customColors.ComputePass);
			ImGui::Text("ComputePass");
			ImGui::PopStyleColor();
			ImGui::Text("ShaderPass");
			ImGui::Unindent(60.0f);
			ImGui::Text("will look.");

			ImGui::Columns(2);
			static int currentListItem = 0, currentComboItem = 0;
			static bool checkboxState = true;
			static char textboxBuffer[256] = "Hello\0";
			static int radioState = 0;
			static const char* listBoxItems[] = {
				"Item #1", "Item #2",
				"Item #3", "Item #4",
				"Item #5", "Item #6",
				"Item #7", "Item #8",
			};
			ImGui::ListBox("Listbox", &currentListItem, listBoxItems, 8);

			ImGui::NextColumn();
			ImGui::Combo("Combo", &currentComboItem, "Item #1\0Item #2\0Item #3\0Item #4\0");
			ImGui::Checkbox("Checkbox", &checkboxState);
			ImGui::RadioButton("RadioButton 1", &radioState, 0);
			ImGui::RadioButton("RadioButton 2", &radioState, 1);
			ImGui::InputText("Textbox", textboxBuffer, sizeof(textboxBuffer));

			ImGui::Columns(1);




			if (ImGui::BeginTable("##msg_table", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollFreezeTopRow | ImGuiTableFlags_ScrollY, ImVec2(0, 100))) {
				ImGui::TableSetupColumn("Shader Pass", ImGuiTableColumnFlags_WidthFixed, 120.0f);
				ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 120.0f);
				ImGui::TableSetupColumn("Line", ImGuiTableColumnFlags_WidthFixed, 120.0f);
				ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableAutoHeaders();

			ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Selectable("ShaderPass", false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick);

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("PS");

				ImGui::TableSetColumnIndex(2);
				ImGui::Text("31");

				ImGui::TableSetColumnIndex(3);
				ImGui::TextColored(customColors.InfoMessage, "Info message's content goes here");

			ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Selectable("ShaderPass", false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick);

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("PS");

				ImGui::TableSetColumnIndex(2);
				ImGui::Text("31");

				ImGui::TableSetColumnIndex(3);
				ImGui::TextColored(customColors.WarningMessage, "Warning message's content goes here");

			ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Selectable("ShaderPass", false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick);

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("PS");

				ImGui::TableSetColumnIndex(2);
				ImGui::Text("31");

				ImGui::TableSetColumnIndex(3);
				ImGui::TextColored(customColors.ErrorMessage, "Error message's content goes here");


				ImGui::EndTable();
			}
		}
		ImGui::End();

		// test window 1
		if (testWindow1) {
			if (viewport->Size.y != 0.0f)
				ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 390.0f, viewport->Size.y / 2.0f - 50.0f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(250.0f, 100.0f), ImGuiCond_Appearing);
			if (ImGui::Begin("Test window #1", &testWindow1))
				ImGui::Text("Hello from the test window!");
			ImGui::End();
		}

		// test window 2
		if (testWindow2) {
			if (viewport->Size.y != 0.0f)
				ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 370.0f, viewport->Size.y / 2.0f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(250.0f, 100.0f), ImGuiCond_Appearing);
			if (ImGui::Begin("Test window #2", &testWindow2, ImGuiWindowFlags_MenuBar)) {
				if (ImGui::BeginMenuBar()) {
					if (ImGui::BeginMenu("Menu #1")) {
						ImGui::MenuItem("Item #1");
						ImGui::MenuItem("Item #2");
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Menu #2")) {
						ImGui::MenuItem("Item #1");
						ImGui::MenuItem("Item #2");
						if (ImGui::BeginMenu("Submenu #1")) {
							ImGui::MenuItem("Item #1");
							ImGui::MenuItem("Item #2");
							ImGui::EndMenu();
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}
				ImGui::Text("Hello from another test window!");
			}
			ImGui::End();
		}

		ImGui::PopFont();

		// Rendering
		ImGui::Render();


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(wnd);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(wnd);
	SDL_Quit();

	return 0;
}