set(JGFC_SRCS
 BaseWindow.h
 BaseWindow.cpp
 BaseResourceCacheManager.h
 BaseResourceCacheManager.cpp
 BaseClientNetworkManager.h
 BaseClientNetworkManager.cpp

 ClientLogging.h
 ClientLogging.cpp
)

add_library(JansGameFrameworkClient STATIC ${JGFC_SRCS})
qt5_use_modules(JansGameFrameworkClient Core Gui Quick Qml Network)
