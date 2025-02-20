Note: JKRGUI_DIR should be defined as an environment variable pointing to the directory of JkrGUI repo

# To Generate/Create JkrGUI Application (cd into a folder):

jkrgui --gr // For generating and running it 
jkrgui --g // To generate

# To Generate Android Environment For an application:

jkrgui --and-env --appname appname --dirname dirname --builddir builddirname

example:
jkrgui --and-env --appname JkrGUIConnect --dirname android
jkrgui --and-env --appname JkrGUIConnect --dirname android android-arm64-v8a\ Release

Here,
-> android-arm64-v8a is default builddir, it refers to the location of jkrgui.so file
