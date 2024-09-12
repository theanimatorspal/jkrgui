import os
with open("jkrgui/LuaBundleAndroid.hpp", 'w') as outputFile:
          def Push(inFileName):
                    for root, _, files in os.walk("luaproject"):
                              for file in files:
                                        if file.endswith(inFileName):
                                                  file_path = os.path.join(root, file)
                                                  with open(file_path, 'r') as infile:
                                                          for line in infile:
                                                            if not line.strip().startswith('require'):
                                                                      outputFile.write(line)
                                                          outputFile.write('\n')
          Push("inspect.lua")
          Push("require.lua") 
          Push("Shader.lua")
          Push("Engine.lua")
          Push("CallBuffers.lua")
          Push("Basic.lua")
