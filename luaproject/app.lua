local COMPILE = false
local DIRECT_RUN = true
MAIN_JKR = "main.jkr"

function Resources()
          COMPILE_FUNCTIONS = {
                    main
          }
          COMPILE_RESOURCES = {
                    "res/font.ttf"
          }
end

do
          -- Add all functions here, it will be compiled and called serially

          if COMPILE then
                    MAIN_JKR_FILE = Jkr.File(MAIN_JKR)
                    -- you cannot use "require", in any code inside src, your whole app should be single script,
                    -- if you use multiple scripts, break them into functions and require them here, and this will
                    -- execute it in the order that you mention in COMPILE_FUNCTIONS
                    require "src.main"
                    Resources()
                    for i = 1, #COMPILE_FUNCTIONS do
                              MAIN_JKR_FILE["func" .. i] = COMPILE_FUNCTIONS[i] -- Config.COMPILE_FUNCTIONS.main
                    end
                    MAIN_JKR_FILE["func_count"] = #COMPILE_FUNCTIONS

                    for i = 1, #COMPILE_RESOURCES do
                              MAIN_JKR_FILE:WriteFile(COMPILE_FUNCTIONS[i], COMPILE_FUNCTIONS[i])
                    end
          else
                    MAIN_JKR_FILE = Jkr.File(MAIN_JKR)
                    local f_count = MAIN_JKR_FILE:Read("func_count", 0)
                    for i = 1, f_count do
                              load(MAIN_JKR_FILE:Read("func" .. i, function() end))()
                    end
                    DIRECT_RUN = false
          end

          if DIRECT_RUN then
                    require "src.main"
                    MAIN_JKR_FILE = Jkr.File(MAIN_JKR)
                    main()
          end
end
