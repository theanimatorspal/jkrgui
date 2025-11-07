PROJECT_NAME = "jkrgui"
PROJECT_TYPE = "binary" # this doesn't do anything
PROJECT_LANGUAGE = "C++"
PROJECT_LANGUAGE_STANDARD = "20"
PROJECT_COMPILER = "clang" # "gcc" or "emcc"
PROJECT_STANDARD_LIBRARY = "default" # can change to "none" for no std library (will add this feature later)
PROJECT_PLATFORM = "x64-windows" # Can be any one from x64-windows, x64-windows-static, x64-linux (untested), x64-linux-dynamic (untested), x64-osx(untested), arm64-android (will be supported later), wasm32-emscripten [VCPKG triplets]
PROJECT_STRUCTURE = {
    "ksaicore" : {
        "type" : "static-library", # can be "binary", "dynamic-library", OR "static-library"
        "deps" : {
          "sdl3":{},
        }
    },

    "ksaieng" : {
        "type" : "static-library",
        "deps" : {
            "ksaicore" : {},
            "cairo": {},
            "pango" : {},
            "ffmpeg" : {}
        }
    },

    "jkrgui" : {
              "type" : "binary",
              "deps" : {
                    "ksaicore": {},
                    "ksaieng": {},
                    "lua" : {},        
                    "sol2" : {},
              }
    }
}
PROJECT_COMPILE_DEFINITIONS = {
        "SDL_MAIN_HANDLED"
}