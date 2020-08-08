import os

envDebug = Environment(CXX= 'clang++', CXXFLAGS="-std=c++17")

envDebug['ENV']['TERM'] = os.environ['TERM']

sources = Glob('*.cpp')

envDebug.Append(CPPPATH = ['#Psionic/include/'])

envDebug.Append(CXXFLAGS = ' -g')
envDebug.Append(CXXFLAGS = ' -fno-rtti')

envDebug.Append(CXXFLAGS = ' -Werror')


envDebug.Append(CXXFLAGS = ' -Wall')
envDebug.Append(CXXFLAGS = ' -Wextra')
envDebug.Append(CXXFLAGS = ' -Wabi')
envDebug.Append(CXXFLAGS = ' -Wwrite-strings')
envDebug.Append(CXXFLAGS = ' -D_GLIBCXX_DEBUG')

envDebug.Append(CXXFLAGS = ' -Wno-unused-parameter')
envDebug.Append(CXXFLAGS = ' -Wno-sign-compare')

ps_engine_sources = ["CompiledGame.cpp","Compiler.cpp","PSEngine.cpp","ParsedGame.cpp","Parser.cpp", "PSLogger.cpp", "TextProvider.cpp"]

ps_engine_sources = ["Psionic/src/" + f for f in ps_engine_sources]


targetDebug = envDebug.Program(target = "#build/debug/psterminal", source = ["psterminal.cpp"] + ps_engine_sources, LIBS=['ncurses','menu'], LIBPATH='.')
envDebug.Default(targetDebug)
