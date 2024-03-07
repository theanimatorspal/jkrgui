#include <Misc/Recycle.hpp>
#include <sol/sol.hpp>

void CreateMiscBindings(sol::state &inState) {
          auto Jkr = inState["Jkr"].get_or_create<sol::table>();
          Jkr.new_usertype<Jkr::Misc::Recycle<int>>("Recycle",
          sol::call_constructor,
          sol::default_constructor, 
          "Add", &Jkr::Misc::Recycle<int>::Add,
          "Remove", &Jkr::Misc::Recycle<int>::Remove,
          "Exists", &Jkr::Misc::Recycle<int>::Exists
          );
}
