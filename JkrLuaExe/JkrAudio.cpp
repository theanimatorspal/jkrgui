#include <Audio/Sound.hpp>
#include <sol/sol.hpp>

namespace JkrEXE {
void CreateAudioBindings(sol::state& inState) {
    auto Jkr = inState["Jkr"].get_or_create<sol::table>();
    Jkr.new_usertype<Jkr::Audio::Sound>(
         "Sound",
         sol::call_constructor,
         sol::constructors<Jkr::Audio::Sound(std::string_view inView)>(),
         "Play",
         &Jkr::Audio::Sound::Play,
         "Pause",
         &Jkr::Audio::Sound::Pause,
         "SetupAudioDevice",
         &Jkr::Audio::Sound::SetupAudioDevice);
}
} // namespace JkrEXE