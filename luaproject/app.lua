COMPILE = true
CREATE_ANDROID_PROJECT = false

PROJECT_NAME = "Jkr_project" -- first letter should be uppercase (Java things)
ANDROID_DIRECTORY_NAME = "android"
ANDORID_BUILD_DIRECTORY = "android-arm64-v8a Release" or "android-arm64-v8a"
ANDROID_ARCHITECTURE = "arm64-v8a"

---@note Add all requires here, ALL of the files which you have created should be here
function Requires()
          require "bundle"
          require "src.main"
end

---@note Add all the functions here, it will be compiled and called in order you put it, Add other functions
---@note jkrgui should always be at the top

---@warning You cannot run anything without making it a function and mentioning it here
function Resources()
          COMPILE_FUNCTIONS = {
                    jkrgui,
                    -- func 1,
                    -- func 2,
                    -- func 3,
                    main,
          }
          COMPILE_RESOURCES = {
                    "res/font.ttf"
          }
end

do
          MAIN_JKR = "main.jkr" -- this cannot be changed
          MAIN_JKR_FILE = Jkr.File(MAIN_JKR)
          if COMPILE then
                    if CREATE_ANDROID_PROJECT then
                              Jkr.CreateAndroidEnvironment(PROJECT_NAME, ANDROID_DIRECTORY_NAME, ANDORID_BUILD_DIRECTORY,
                                        ANDROID_ARCHITECTURE)
                    end
                    MAIN_JKR_FILE:Clear()
                    -- you cannot use "require", in any code inside src, your whole app should be single script,
                    -- if you use multiple scripts, break them into functions and require them here, and this will
                    -- execute it in the order that you mention in COMPILE_FUNCTIONS
                    Requires()
                    Resources()
                    for i = 1, #COMPILE_FUNCTIONS do
                              MAIN_JKR_FILE["func" .. i] = COMPILE_FUNCTIONS[i] -- Config.COMPILE_FUNCTIONS.main
                    end
                    MAIN_JKR_FILE["func_count"] = #COMPILE_FUNCTIONS

                    for i = 1, #COMPILE_RESOURCES do
                              MAIN_JKR_FILE:WriteFile(COMPILE_RESOURCES[i], COMPILE_RESOURCES[i])
                    end
                    MAIN_JKR_FILE:Commit()
                    main()
          else
                    local f_count = MAIN_JKR_FILE:Read("func_count", 0)
                    for i = 1, f_count do
                              load(MAIN_JKR_FILE:Read("func" .. i, function() end))()
                    end
                    DIRECT_RUN = false
          end
end
