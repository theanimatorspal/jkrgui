local COMPILE = true
local RELEASE = false

if COMPILE then
          -- you cannot use "require", in any code inside src, your whole app should be single script,
          -- if you use multiple scripts, break them into functions require them here, and execute it in the
          -- order that it should be executed
          require "src.main"
          local Config = {
                    COMPILE_RESOURCES = {
                              "res/font.ttf"
                    },
                    -- Add all functions here, it will be compiled and called serially
                    COMPILE_FUNCTIONS = {
                              main
                    }
          }


          local compiled_file = Jkr.File("main.jkr")
          for i = 1, #Config.COMPILE_FUNCTIONS do
                    compiled_file["func" .. i] = Config.COMPILE_FUNCTIONS.main
          end
          compiled_file["func_count"] = #Config.COMPILE_FUNCTIONS
else
          if RELEASE then
                    local compiled_file = Jkr.File("main.jkr")
                    local f_count = compiled_file["func_count"]
                    for i = 1, f_count do
                              load(compiled_file["func" .. i])()
                    end
          else
                    require "src.main"
                    main()
          end
end
