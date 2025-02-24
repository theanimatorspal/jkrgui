local RELEASE = true
MAIN_JKR = "main.jkr"

-- Add all functions here, it will be compiled and called serially
function Resources()
          COMPILE_FUNCTIONS = {
                    jkrgui,
                    main,
          }
          COMPILE_RESOURCES = {
                    "res/font.ttf"
          }
end

do
          MAIN_JKR_FILE = Jkr.File(MAIN_JKR)
          if RELEASE then
                    -- you cannot use "require", in any code inside src, your whole app should be single script,
                    -- if you use multiple scripts, break them into functions and require them here, and this will
                    -- execute it in the order that you mention in COMPILE_FUNCTIONS
                    require "bundle"
                    require "src.main"
                    Resources()
                    for i = 1, #COMPILE_FUNCTIONS do
                              MAIN_JKR_FILE["func" .. i] = COMPILE_FUNCTIONS[i] -- Config.COMPILE_FUNCTIONS.main
                    end
                    MAIN_JKR_FILE["func_count"] = #COMPILE_FUNCTIONS

                    for i = 1, #COMPILE_RESOURCES do
                              MAIN_JKR_FILE:WriteFile(COMPILE_RESOURCES[i], COMPILE_RESOURCES[i])
                    end
                    MAIN_JKR_FILE:Commit()
                    DIRECT_RUN = true
          else
                    local f_count = MAIN_JKR_FILE:Read("func_count", 0)
                    for i = 1, f_count do
                              load(MAIN_JKR_FILE:Read("func" .. i, function() end))()
                    end
                    DIRECT_RUN = false
          end

          if DIRECT_RUN then
                    require "bundle"
                    require "src.main"
                    MAIN_JKR_FILE = Jkr.File(MAIN_JKR)
                    main()
          end
end
