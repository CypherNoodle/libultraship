#include "ship/controller/controldevice/controller/mapping/sdl/SDLRumbleMapping.h"

#ifdef __SWITCH__
#include <array>
#include <mutex>
#include <switch.h>
#endif

#include "ship/config/ConsoleVariable.h"
#include "ship/utils/StringHelper.h"
#include "ship/Context.h"
#include "ship/controller/controldeck/ControlDeck.h"

namespace Ship {
#ifdef __SWITCH__
namespace {
struct SwitchVibrationDevices {
    std::array<HidVibrationDeviceHandle, 2> handles {};
    int count = 0;
};

std::array<SwitchVibrationDevices, 4> sDockedVibration;
SwitchVibrationDevices sHandheldVibration;
std::once_flag sVibrationInit;
std::mutex sVibrationMutex;

int InitVibrationDevices(SwitchVibrationDevices& devices, HidNpadIdType id, HidNpadStyleTag style) {
    if (R_SUCCEEDED(hidInitializeVibrationDevices(devices.handles.data(), 2, id, style))) {
        return 2;
    }
    if (R_SUCCEEDED(hidInitializeVibrationDevices(devices.handles.data(), 1, id, style))) {
        return 1;
    }
    return 0;
}

void InitSwitchVibration() {
    for (size_t port = 0; port < sDockedVibration.size(); ++port) {
        sDockedVibration[port].count = InitVibrationDevices(
            sDockedVibration[port], static_cast<HidNpadIdType>(HidNpadIdType_No1 + port),
            HidNpadStyleSet_NpadFullCtrl
        );
    }
    sHandheldVibration.count =
        InitVibrationDevices(sHandheldVibration, HidNpadIdType_Handheld, HidNpadStyleTag_NpadHandheld);
}

void SetSwitchRumble(uint8_t port, uint16_t low, uint16_t high) {
    if (port >= sDockedVibration.size()) {
        return;
    }

    std::call_once(sVibrationInit, InitSwitchVibration);
    std::lock_guard<std::mutex> lock(sVibrationMutex);

    SwitchVibrationDevices* devices = &sDockedVibration[port];
    if (port == 0 && (hidGetNpadStyleSet(HidNpadIdType_Handheld) & HidNpadStyleTag_NpadHandheld)) {
        devices = &sHandheldVibration;
    }
    if (devices->count <= 0) {
        return;
    }

    std::array<HidVibrationValue, 2> values {};
    for (int i = 0; i < devices->count; ++i) {
        values[i].amp_low = static_cast<float>(low) / UINT16_MAX;
        values[i].freq_low = 160.0f;
        values[i].amp_high = static_cast<float>(high) / UINT16_MAX;
        values[i].freq_high = 320.0f;
    }
    hidSendVibrationValues(devices->handles.data(), values.data(), devices->count);
}
} // namespace
#endif

SDLRumbleMapping::SDLRumbleMapping(uint8_t portIndex, uint8_t lowFrequencyIntensityPercentage,
                                   uint8_t highFrequencyIntensityPercentage)
    : ControllerRumbleMapping(PhysicalDeviceType::SDLGamepad, portIndex, lowFrequencyIntensityPercentage,
                              highFrequencyIntensityPercentage) {
    SetLowFrequencyIntensity(lowFrequencyIntensityPercentage);
    SetHighFrequencyIntensity(highFrequencyIntensityPercentage);
}

void SDLRumbleMapping::StartRumble() {
#ifdef __SWITCH__
    SetSwitchRumble(mPortIndex, mLowFrequencyIntensity, mHighFrequencyIntensity);
#else
    for (const auto& [instanceId, gamepad] : Context::GetRawInstance()
                                                 ->GetControlDeck()
                                                 ->GetConnectedPhysicalDeviceManager()
                                                 ->GetConnectedSDLGamepadsForPort(mPortIndex)) {
        SDL_GameControllerRumble(gamepad, mLowFrequencyIntensity, mHighFrequencyIntensity, 0);
    }
#endif
}

void SDLRumbleMapping::StopRumble() {
#ifdef __SWITCH__
    SetSwitchRumble(mPortIndex, 0, 0);
#else
    for (const auto& [instanceId, gamepad] : Context::GetRawInstance()
                                                 ->GetControlDeck()
                                                 ->GetConnectedPhysicalDeviceManager()
                                                 ->GetConnectedSDLGamepadsForPort(mPortIndex)) {
        SDL_GameControllerRumble(gamepad, 0, 0, 0);
    }
#endif
}

void SDLRumbleMapping::SetLowFrequencyIntensity(uint8_t intensityPercentage) {
    mLowFrequencyIntensityPercentage = intensityPercentage;
    mLowFrequencyIntensity = UINT16_MAX * (intensityPercentage / 100.0f);
}

void SDLRumbleMapping::SetHighFrequencyIntensity(uint8_t intensityPercentage) {
    mHighFrequencyIntensityPercentage = intensityPercentage;
    mHighFrequencyIntensity = UINT16_MAX * (intensityPercentage / 100.0f);
}

std::string SDLRumbleMapping::GetRumbleMappingId() {
    return StringHelper::Sprintf("P%d", mPortIndex);
}

void SDLRumbleMapping::SaveToConfig() {
    const std::string mappingCvarKey = CVAR_PREFIX_CONTROLLERS ".RumbleMappings." + GetRumbleMappingId();
    Ship::Context::GetRawInstance()->GetConsoleVariables()->SetString(
        StringHelper::Sprintf("%s.RumbleMappingClass", mappingCvarKey.c_str()).c_str(), "SDLRumbleMapping");
    Ship::Context::GetRawInstance()->GetConsoleVariables()->SetInteger(
        StringHelper::Sprintf("%s.LowFrequencyIntensity", mappingCvarKey.c_str()).c_str(),
        mLowFrequencyIntensityPercentage);
    Ship::Context::GetRawInstance()->GetConsoleVariables()->SetInteger(
        StringHelper::Sprintf("%s.HighFrequencyIntensity", mappingCvarKey.c_str()).c_str(),
        mHighFrequencyIntensityPercentage);
    Ship::Context::GetRawInstance()->GetConsoleVariables()->Save();
}

void SDLRumbleMapping::EraseFromConfig() {
    const std::string mappingCvarKey = CVAR_PREFIX_CONTROLLERS ".RumbleMappings." + GetRumbleMappingId();

    Ship::Context::GetRawInstance()->GetConsoleVariables()->ClearVariable(
        StringHelper::Sprintf("%s.RumbleMappingClass", mappingCvarKey.c_str()).c_str());
    Ship::Context::GetRawInstance()->GetConsoleVariables()->ClearVariable(
        StringHelper::Sprintf("%s.LowFrequencyIntensity", mappingCvarKey.c_str()).c_str());
    Ship::Context::GetRawInstance()->GetConsoleVariables()->ClearVariable(
        StringHelper::Sprintf("%s.HighFrequencyIntensity", mappingCvarKey.c_str()).c_str());

    Ship::Context::GetRawInstance()->GetConsoleVariables()->Save();
}

std::string SDLRumbleMapping::GetPhysicalDeviceName() {
    return "SDL Gamepad";
}
} // namespace Ship
