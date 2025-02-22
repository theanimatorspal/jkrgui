#include <Audio/Sound.hpp>
#include "JkrLuaExe.hpp"

namespace JkrEXE {
void CreateAudioBindings(sol::state &inState) {
    auto Jkr = inState["Jkr"].get_or_create<sol::table>();
    Jkr.new_usertype<Jkr::Audio::Sound>("Sound",
                                        sol::call_constructor,
                                        sol::factories([]() { return mu<Jkr::Audio::Sound>(); }),
                                        "Play",
                                        &Jkr::Audio::Sound::Play,
                                        "Pause",
                                        &Jkr::Audio::Sound::Pause,
                                        "SetupAudioDevice",
                                        &Jkr::Audio::Sound::SetupAudioDevice,
                                        "FromMemory",
                                        &Jkr::Audio::Sound::FromMemory);
}
} // namespace JkrEXE