#include <Misc/Recycle.hpp>
#include <sol/sol.hpp>

void CreateMiscBindings(sol::state& inState) {
		auto Jkr = inState["Jkr"].get_or_create<sol::table>();
		Jkr.new_usertype<Jkr::Misc::Recycle<int>>(
			"RecycleBin",
			sol::call_constructor,
			sol::factories([]() { return Jkr::Misc::Recycle<int>(); }),
			"Add",
			&Jkr::Misc::Recycle<int>::Add,
			"Remove",
			&Jkr::Misc::Recycle<int>::Remove,
			"Exists",
			&Jkr::Misc::Recycle<int>::Exists,
			"IsEmpty",
			&Jkr::Misc::Recycle<int>::IsEmpty,
			"Get",
			&Jkr::Misc::Recycle<int>::Get
		);
}