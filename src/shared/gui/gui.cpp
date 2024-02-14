#include "gui.hpp"
#include "../openhack.hpp"
#include <imgui.h>

namespace openhack::gui {
    std::vector<Font> fonts;
    Font *currentFont = nullptr;

    void setFont(Font &font) {
        currentFont = &font;
        ImGui::GetIO().FontDefault = font.normal;
    }

    void setFont(const std::string &name) {
        for (auto &font: fonts) {
            if (font.name == name) {
                setFont(font);
                break;
            }
        }
    }

    Font &getFont() {
        return *currentFont;
    }

    void init() {
        utils::resetWindowHandle();

        // Load all fonts from "OPENHACK_DIRECTORY/fonts/"
        auto fontDir = utils::getModFontsDirectory();

        // Check if the directory exists
        if (!std::filesystem::exists(fontDir)) {
            L_ERROR("Font directory does not exist: {}", fontDir);
            return;
        }

        ImGuiIO &io = ImGui::GetIO();

        io.IniFilename = nullptr;

        // Scan for fonts
        ImFontConfig font_cfg;
        font_cfg.OversampleH = 2;
        font_cfg.OversampleV = 2;

        fonts.clear();
        for (const auto &entry: std::filesystem::directory_iterator(fontDir)) {
            if (entry.is_regular_file()) {
                auto path = entry.path().string();
                auto ext = path.substr(path.find_last_of('.') + 1);
                if (ext == "ttf") {
                    auto fontDefault = io.Fonts->AddFontFromFileTTF(path.c_str(), 16.0f, &font_cfg);
                    auto fontTitle = io.Fonts->AddFontFromFileTTF(path.c_str(), 20.0f, &font_cfg);
                    if (fontDefault && fontTitle) {
                        auto name = entry.path().stem().string();
                        name = name.substr(0, name.find_last_of('.'));
                        auto font = Font(name, fontDefault, fontTitle);
                        fonts.push_back(font);
                    } else {
                        L_ERROR("Failed to load font: {}", path);
                    }
                }
            }
        }

        // Set default font
        if (!fonts.empty()) {
            auto first = fonts.front();
            auto selectedFont = config::get<std::string>("menu.font", first.name);
            setFont(selectedFont);
        }

        // Set theme
        auto theme = config::get<Themes>("menu.theme");
        setTheme(theme);

        // Set colors if they aren't set
        if (!config::has("menu.color.background"))
            loadPalette();

        L_INFO("Initialized ImGui");
    }

    void width(float width) {
        if (width > 0) {
            ImGui::PushItemWidth(width);
        } else {
            ImGui::PopItemWidth();
        }
    }

    bool checkbox(const char *label, const char *valueKey) {
        bool value = config::get<bool>(valueKey, false);
        bool result = gui::checkbox(label, &value);
        if (result)
            config::set(valueKey, value);
        return result;
    }

    bool combo(const char *label, const char *current, const char *const items[], int itemsCount) {
        int value = config::get<int>(current, 0);
        bool result = gui::combo(label, &value, items, itemsCount);
        if (result)
            config::set(current, value);
        return result;
    }

    bool combo(const char *label, const char *current, const char *items) {
        int value = config::get<int>(current, 0);
        bool result = gui::combo(label, &value, items);
        if (result)
            config::set(current, value);
        return result;
    }

    bool inputFloat(const char *label, const char *valueKey, float min, float max, const char *format) {
        auto value = config::get<float>(valueKey, 0.0f);
        bool result = gui::inputFloat(label, &value, min, max, format);
        if (result)
            config::set(valueKey, value);
        return result;
    }

    bool colorEdit(const char *label, const char *colorKey) {
        auto value = config::get<Color>(colorKey, Color());
        bool result = gui::colorEdit(label, &value);
        if (result)
            config::set(colorKey, value);
        return result;
    }
}