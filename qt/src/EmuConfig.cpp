#include <cstdio>
#include <string>
#define TOML_LARGE_FILES 1
#define TOML_IMPLEMENTATION 1
#include <fstream>
#include "toml.hpp"
#include <filesystem>
namespace fs = std::filesystem;

#include "EmuConfig.hpp"
#include "EmuBinding.hpp"
#include <functional>

static const char *shortcut_names[] =
{
    "OpenROM",
    "EmuTurbo",
    "ToggleEmuTurbo",
    "PauseContinue",
    "SoftReset",
    "Reset",
    "Quit",
    "ToggleFullscreen",
    "Screenshot",
    "SaveSPC",
    "SaveState",
    "LoadState",
    "IncreaseSlot",
    "DecreaseSlot",
    "QuickSave000",
    "QuickSave001",
    "QuickSave002",
    "QuickSave003",
    "QuickSave004",
    "QuickSave005",
    "QuickSave006",
    "QuickSave007",
    "QuickSave008",
    "QuickSave009",
    "QuickLoad000",
    "QuickLoad001",
    "QuickLoad002",
    "QuickLoad003",
    "QuickLoad004",
    "QuickLoad005",
    "QuickLoad006",
    "QuickLoad007",
    "QuickLoad008",
    "QuickLoad009",
    "Rewind",
    "GrabMouse",
    "SwapControllers1and2",
    "ToggleBG0",
    "ToggleBG1",
    "ToggleBG2",
    "ToggleBG3",
    "ToggleSprites",
    "ToggleBackdrop",
    "SoundChannel0",
    "SoundChannel1",
    "SoundChannel2",
    "SoundChannel3",
    "SoundChannel4",
    "SoundChannel5",
    "SoundChannel6",
    "SoundChannel7",
    "SoundChannelsOn",
    "BeginRecordingMovie",
    "EndRecordingMovie",
    "SeekToFrame",
};

static const char *default_controller_keys[] =
{
    "Keyboard Ctrl+o", //    eOpenROM
    "Keyboard Tab", //    eFastForward
    "Keyboard `", //    eToggleFastForward
    "Keyboard p", //    ePauseContinue
    "Keyboard Ctrl+r", //    eSoftReset
    "", //    ePowerCycle
    "Keyboard Ctrl+q", //    eQuit
    "Keyboard F11", //    eToggleFullscreen
    "", //    eSaveScreenshot
    "", //    eSaveSPC
    "Keyboard F2", //    eSaveState
    "Keyboard F4", //    eLoadState
    "Keyboard F6", //    eIncreaseSlot
    "Keyboard F5", //    eDecreaseSlot
    "Keyboard 0", //    eSaveState0
    "Keyboard 1", //    eSaveState1
    "Keyboard 2", //    eSaveState2
    "Keyboard 3", //    eSaveState3
    "Keyboard 4", //    eSaveState4
    "Keyboard 5", //    eSaveState5
    "Keyboard 6", //    eSaveState6
    "Keyboard 7", //    eSaveState7
    "Keyboard 8", //    eSaveState8
    "Keyboard 9", //    eSaveState9
    "Keyboard Ctrl+0", //    eLoadState0
    "Keyboard Ctrl+1", //    eLoadState1
    "Keyboard Ctrl+2", //    eLoadState2
    "Keyboard Ctrl+3", //    eLoadState3
    "Keyboard Ctrl+4", //    eLoadState4
    "Keyboard Ctrl+5", //    eLoadState5
    "Keyboard Ctrl+6", //    eLoadState6
    "Keyboard Ctrl+7", //    eLoadState7
    "Keyboard Ctrl+8", //    eLoadState8
    "Keyboard Ctrl+9", //    eLoadState9
    "", //    eRewind
    "Keyboard Ctrl+g", //    eGrabMouse
    "", //    eSwapControllers1and2
    "", //    eToggleBG0
    "", //    eToggleBG1
    "", //    eToggleBG2
    "", //    eToggleBG3
    "", //    eToggleSprites
    "", //    eChangeBackdrop
    "", //    eToggleSoundChannel1
    "", //    eToggleSoundChannel2
    "", //    eToggleSoundChannel3
    "", //    eToggleSoundChannel4
    "", //    eToggleSoundChannel5
    "", //    eToggleSoundChannel6
    "", //    eToggleSoundChannel7
    "", //    eToggleSoundChannel8
    "", //    eToggleAllSoundChannels
    "", //    eStartRecording
    "", //    eStopRecording
    ""
};

const char **EmuConfig::getDefaultShortcutKeys()
{
    return default_controller_keys;
}

const char **EmuConfig::getShortcutNames()
{
    return shortcut_names;
}

std::string EmuConfig::findConfigDir()
{
    char *dir;
    fs::path path;

#ifndef _WIN32
    if ((dir = getenv("XDG_CONFIG_HOME")))
    {
        path = dir;
        path /= "snes9x";
    }
    else if ((dir = getenv("HOME")))
    {
        path = dir;
        path /= ".config/snes9x";
    }
    else
    {
        path = "./.snes9x";
    }
#else
    if ((dir = getenv("LOCALAPPDATA")))
    {
        path = dir;
        path /= "Snes9x";
    }
    else if ((dir = getenv("APPDATA")))
    {
        path = dir;
        path /= "Snes9x";
    }
    else
        path = "snes9x";
#endif

    if (!fs::exists(path))
        fs::create_directories(path);

    return path.string();
}

std::string EmuConfig::findConfigFile()
{
    fs::path path(findConfigDir());
    path /= "snes9x-qt.conf";
    return path.string();
}

void EmuConfig::setDefaults(int section)
{
    if (section == -1 || section == 0)
    {
        // General
        fullscreen_on_open = false;
        disable_screensaver = true;
        pause_emulation_when_unfocused = true;

        show_frame_rate = false;
        show_indicators = true;
        show_pressed_keys = false;
        show_time = false;
    }

    if (section == -1 || section == 1)
    {
        // Display
        display_driver = {};
        display_device_index = 0;
        enable_vsync = true;
        ;
        bilinear_filter = true;
        ;
        reduce_input_lag = true;
        adjust_for_vrr = false;
        use_shader = false;
        shader = {};
        last_shader_folder = {};

        scale_image = true;
        ;
        maintain_aspect_ratio = true;
        use_integer_scaling = false;
        aspect_ratio_numerator = 4;
        aspect_ratio_denominator = 3;
        show_overscan = false;
        high_resolution_effect = eLeaveAlone;

        software_filter = {};

        display_messages = eOnscreen;
        osd_size = 24;
    }

    if (section == -1 || section == 2)
    {
        // Sound
        sound_driver = {};
        sound_device = {};
        playback_rate = 48000;
        audio_buffer_size_ms = 64;

        adjust_input_rate_automatically = true;
        input_rate = 31979;
        dynamic_rate_control = false;
        dynamic_rate_limit = 0.005;
        mute_audio = false;
        mute_audio_during_alternate_speed = false;
    }

    if (section == -1 || section == 3)
    {
        speed_sync_method = eTimer;
        fixed_frame_rate = 0.0;
        fast_forward_skip_frames = 9;

        rewind_buffer_size = 0;
        rewind_frame_interval = 5;

        allow_invalid_vram_access = false;
        allow_opposing_dpad_directions = false;
        overclock = false;
        remove_sprite_limit = false;
        enable_shadow_buffer = false;
        superfx_clock_multiplier = 100;
        sound_filter = eGaussian;
    }

    if (section == -1 || section == 4)
    {
        // Controllers
        memset(binding.controller, 0, sizeof(binding.controller));

        const char *button_list[] = { "Up", "Down", "Left", "Right", "d", "c", "s", "x", "z", "a", "Return", "Space" };
        for (int i = 0; i < std::size(button_list); i++)
        {
            binding.controller[0].buttons[i * 4] = EmuBinding::from_config_string("Keyboard " + std::string(button_list[i]));
        }
    }

    if (section == -1 || section == 5)
    {
        // Shortcuts
        memset(binding.shortcuts, 0, sizeof(binding.shortcuts));
        for (auto i = 0; i < num_shortcuts; i++)
        {
            binding.shortcuts[i * 4] = EmuBinding::from_config_string(getDefaultShortcutKeys()[i]);
        }
    }

    if (section == -1 || section == 6)
    {
        // Files
        sram_folder = {};
        state_folder = {};
        cheat_folder = {};
        patch_folder = {};
        export_folder = {};

        sram_location = eROMDirectory;
        state_location = eROMDirectory;
        cheat_location = eROMDirectory;
        patch_location = eROMDirectory;
        export_location = eROMDirectory;
    }
}

void EmuConfig::config(std::string filename, bool write)
{
    toml::table root;
    toml::table *table = nullptr;
    std::string section;

    std::function<void(std::string, bool &)> Bool;
    std::function<void(std::string, int &)> Int;
    std::function<void(std::string, std::string &)> String;
    std::function<void(std::string, int &, std::vector<const char *>)> Enum;
    std::function<void(std::string, double &)> Double;
    std::function<void(std::string, EmuBinding &)> Binding;
    std::function<void(std::string)> BeginSection;
    std::function<void()> EndSection;

    if (write)
    {
        Bool = [&](std::string key, bool &value) {
            table->insert_or_assign(key, value);
        };
        Int = [&](std::string key, int &value) {
            table->insert_or_assign(key, value);
        };
        String = [&](std::string key, std::string &value) {
            table->insert_or_assign(key, value);
        };
        Enum = [&](std::string key, int &value, std::vector<const char *> map) {
            table->insert_or_assign(key, map[value]);
        };
        Double = [&](std::string key, double &value) {
            table->insert_or_assign(key, value);
        };
        Binding = [&](std::string key, EmuBinding &binding) {
            table->insert_or_assign(key, binding.to_config_string());
        };
        BeginSection = [&](std::string str) {
            section = str;
            table = new toml::table;
        };

        EndSection = [&]() {
            root.insert_or_assign(section, *table);
            delete table;
        };

        root.clear();
    }
    else
    {
        Bool = [&](std::string key, bool &value) {
            if (table && table->contains(key) && table->get(key)->is_boolean())
                value = table->get(key)->as_boolean()->get();
        };
        Int = [&](std::string key, int &value) {
            if (table && table->contains(key) && table->get(key)->is_integer())
                value = table->get(key)->as_integer()->get();
        };
        String = [&](std::string key, std::string &value) {
            if (table && table->contains(key) && table->get(key)->is_string())
                value = table->get(key)->as_string()->get();
        };
        Binding = [&](std::string key, EmuBinding &binding) {
            if (table && table->contains(key) && table->get(key)->is_string())
                binding = EmuBinding::from_config_string(table->get(key)->as_string()->get());
        };
        Double = [&](std::string key, double &value) {
            if (table && table->contains(key) && table->get(key)->is_floating_point())
                value = table->get(key)->as_floating_point()->get();
        };
        Enum = [&](std::string key, int &value, std::vector<const char *> map) {
            std::string entry;

            if (table && table->contains(key) && table->get(key)->is_string())
                entry = table->get(key)->as_string()->get();
            else
                return;

            auto tolower = [](std::string str) -> std::string {
                for (auto &c : str)
                    if (c >= 'A' && c <= 'Z')
                        c += ('a' - 'A');
                return str;
            };

            entry = tolower(entry);
            for (size_t i = 0; i < map.size(); i++)
            {
                if (tolower(map[i]) == entry)
                {
                    value = i;
                    return;
                }
            }
        };
        BeginSection = [&](std::string str) {
            section = str;
            auto root_section = root.get(section);
            if (root_section)
                table = root_section->as_table();
            else
                table = nullptr;
        };
        EndSection = [&]() {
        };

        auto parse_result = toml::parse_file(filename);
        if (parse_result.failed())
            return;
        root = std::move(parse_result.table());
    }

    BeginSection("Operational");
    String("LastROMFolder", last_rom_folder);
    Int("MainWindowWidth", main_window_width);
    Int("MainWindowHeight", main_window_height);
    int recent_count = recently_used.size();
    Int("RecentlyUsedEntries", recent_count);
    if (!write)
        recently_used.resize(recent_count);
    for (int i = 0; i < recent_count; i++)
    {
        String("RecentlyUsed" + std::to_string(i), recently_used[i]);
    }
    EndSection();

    BeginSection("General");
    Bool("FullscreenOnOpen", fullscreen_on_open);
    Bool("DisableScreensaver", disable_screensaver);
    Bool("PauseEmulationWhenUnfocused", pause_emulation_when_unfocused);

    Bool("ShowFrameRate", show_frame_rate);
    Bool("ShowIndicators", show_indicators);
    Bool("ShowPressedKeys", show_pressed_keys);
    Bool("ShowTime", show_time);
    EndSection();

    BeginSection("Display");
    String("DisplayDriver", display_driver);
    Int("DisplayDevice", display_device_index);
    Bool("VSync", enable_vsync);
    Bool("ReduceInputLag", reduce_input_lag);
    Bool("BilinearFilter", bilinear_filter);
    Bool("AdjustForVRR", adjust_for_vrr);
    Bool("UseShader", use_shader);
    String("Shader", shader);
    String("LastShaderFolder", last_shader_folder);

    Bool("ScaleImage", scale_image);
    Bool("MaintainAspectRatio", maintain_aspect_ratio);
    Bool("UseIntegerScaling", use_integer_scaling);
    Int("AspectRatioNumerator", aspect_ratio_numerator);
    Int("AspectRatioDenominator", aspect_ratio_denominator);
    Bool("ShowOverscan", show_overscan);
    Enum("HighResolutionEffect", high_resolution_effect, { "LeaveAlone", "ScaleDown", "ScaleUp" });

    String("SoftwareFilter", software_filter);

    Enum("DisplayMessages", display_messages, { "Onscreen", "Inscreen", "None" });
    Int("OSDSize", osd_size);
    EndSection();

    BeginSection("Sound");
    String("SoundDriver", sound_driver);
    String("SoundDevice", sound_device);
    Int("PlaybackRate", playback_rate);
    Int("BufferSize", audio_buffer_size_ms);
    Bool("AdjustInputRateAutomatically", adjust_input_rate_automatically);
    Int("InputRate", input_rate);
    Bool("DynamicRateControl", dynamic_rate_control);
    Double("DynamicRateLimit", dynamic_rate_limit);
    Bool("Mute", mute_audio);
    Bool("MuteAudioDuringAlternateSpeed", mute_audio_during_alternate_speed);
    EndSection();

    BeginSection("Emulation");
    Enum("SpeedSyncMethod", speed_sync_method, { "Timer", "TimerFrameskip", "SoundSync", "Unlimited" });
    Double("FixedFrameRate", fixed_frame_rate);
    Int("FastForwardSkipFrames", fast_forward_skip_frames);
    Int("RewindBufferSize", rewind_buffer_size);
    Int("RewindFrameInterval", rewind_frame_interval);
    Bool("AllowInvalidVRAMAccess", allow_invalid_vram_access);
    Bool("AllowOpposingDpadDirections", allow_opposing_dpad_directions);
    Bool("Overclock", overclock);
    Bool("RemoveSpriteLimit", remove_sprite_limit);
    Bool("EnableShadowBuffer", enable_shadow_buffer);
    Int("SuperFXClockMultiplier", superfx_clock_multiplier);
    Enum("SoundFilter", sound_filter, { "Gaussian", "Nearest", "Linear", "Cubic", "Sinc" });
    EndSection();

    const char *names[] = { "Up", "Down", "Left", "Right", "A", "B", "X", "Y", "L", "R", "Start", "Select", "Turbo A", "Turbo B", "Turbo X", "Turbo Y", "Turbo L", "Turbo R" };
    for (int c = 0; c < 5; c++)
    {
        BeginSection("Controller " + std::to_string(c));

        for (int y = 0; y < num_controller_bindings; y++)
            for (int x = 0; x < allowed_bindings; x++)
            {
                std::string keyname = names[y] + std::to_string(x);
                Binding(keyname, binding.controller[c].buttons[y * allowed_bindings + x]);
            }

        EndSection();
    }

    BeginSection("Shortcuts");
    for (int i = 0; i < num_shortcuts; i++)
    {
        Binding(getShortcutNames()[i] + std::to_string(0), binding.shortcuts[i * 4]);
        Binding(getShortcutNames()[i] + std::to_string(1), binding.shortcuts[i * 4 + 1]);
        Binding(getShortcutNames()[i] + std::to_string(2), binding.shortcuts[i * 4 + 2]);
        Binding(getShortcutNames()[i] + std::to_string(3), binding.shortcuts[i * 4 + 3]);
    }
    EndSection();

    BeginSection("Files");
    Enum("SRAMLocation", sram_location, { "ROMDirectory", "ConfigDirectory", "Custom" });
    Enum("StateLocation", state_location, { "ROMDirectory", "ConfigDirectory", "Custom" });
    Enum("CheatLocation", cheat_location, { "ROMDirectory", "ConfigDirectory", "Custom" });
    Enum("PatchLocation", patch_location, { "ROMDirectory", "ConfigDirectory", "Custom" });
    Enum("ExportLocation", export_location, { "ROMDirectory", "ConfigDirectory", "Custom" });

    String("SRAMFolder", sram_folder);
    String("StateFolder", state_folder);
    String("CheatFolder", cheat_folder);
    String("PatchFolder", patch_folder);
    String("ExportFolder", export_folder);

    Int("SRAMSaveInterval", sram_save_interval);
    EndSection();

    if (write)
    {
        std::ofstream ofs(filename);
        ofs << root;
        ofs.close();
    }
}

void EmuConfig::setVRRConfig(bool enable)
{
    if (enable == vrr_enabled)
        return;

    if (!adjust_for_vrr && enable)
        return;

    vrr_enabled = enable;

    if (enable)
    {
        saved_fixed_frame_rate = fixed_frame_rate;
        saved_input_rate = input_rate;
        saved_speed_sync_method = speed_sync_method;
        saved_enable_vsync = enable_vsync;

        fixed_frame_rate = 0.0;
        input_rate = 32040;
        enable_vsync = true;
        speed_sync_method = eTimer;
    }
    else
    {
        fixed_frame_rate = saved_fixed_frame_rate;
        input_rate = saved_input_rate;
        speed_sync_method = saved_speed_sync_method;
        enable_vsync = saved_enable_vsync;
    }
}