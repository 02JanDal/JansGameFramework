set(JGF_SRCS
 BaseAIManager.h
 BaseAIManager.cpp
 BaseCacheManager.h
 BaseCacheManager.cpp
 BaseEntityManager.h
 BaseEntityManager.cpp
 BaseNetworkManager.h
 BaseNetworkManager.cpp
 BasePermissionManager.h
 BasePermissionManager.cpp
 BasePhysicsManager.h
 BasePhysicsManager.cpp
 BasePluginManager.h
 BasePluginManager.cpp
 BaseSaveManager.h
 BaseSaveManager.cpp

 Logging.h
 Logging.cpp
)

add_library(JansGameFramework STATIC ${JGF_SRCS})
qt5_use_modules(JansGameFramework Core Qml Network Sql)
