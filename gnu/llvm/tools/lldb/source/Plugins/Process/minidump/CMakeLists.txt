add_lldb_library(lldbPluginProcessMinidump PLUGIN
  MinidumpTypes.cpp
  MinidumpParser.cpp
  RegisterContextMinidump_x86_32.cpp
  RegisterContextMinidump_x86_64.cpp
  ProcessMinidump.cpp
  ThreadMinidump.cpp

  LINK_LIBS
    lldbCore
    lldbTarget
    lldbUtility
    lldbPluginProcessUtility
    lldbPluginProcessElfCore
  LINK_COMPONENTS
    Support
  )
