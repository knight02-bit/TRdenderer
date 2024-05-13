cmake_minimum_required(VERSION 3.15)

project(TRenderer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(glm_DIR ./packages/glm_x64-windows/share/glm)
#set(TBB_DIR ./packages/oneapi-tbb-2021.1.1/lib/cmake/tbb)
set(assimp_DIR ./packages/assimp5.2.5/lib/cmake/assimp-5.2)

find_package(glm CONFIG REQUIRED)
#find_package(TBB CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(OpenMP REQUIRED)

set(QRC_FILE
    qdarkstyle/dark/style.qrc
    qdarkstyle/light/style.qrc
    resource.qrc
)

set(PROJECT_SOURCES
        main.cpp
        TRendererWidget.cpp
        TRendererWidget.h
        TRendererWidget.ui
        TRenderer.ui
        TRenderer.h
        TRenderer.cpp
        ${QRC_FILE}
        TRendererCore/BasicDataStructure.hpp
        TRendererCore/FrameBuffer.cpp
        TRendererCore/FrameBuffer.h
        TRendererCore/Shader.hpp
        TRendererCore/TRendererDevice.cpp
        TRendererCore/TRendererDevice.h
        TRendererCore/Texture.cpp
        TRendererCore/Texture.h
        TRendererCore/BlinnPhongShader.cpp
        TRendererCore/BlinnPhongShader.h
        TRendererCore/Mesh.cpp
        TRendererCore/Mesh.h
        TRendererCore/Model.cpp
        TRendererCore/Model.h
        Camera/Camera.cpp
        Camera/Camera.h
)

if(ANDROID)
    add_library(TRenderer SHARED ${PROJECT_SOURCES})
else()
    add_executable(TRenderer ${PROJECT_SOURCES})
endif()

include_directories("./packages/Assimp/include")
target_link_libraries(TRenderer PRIVATE Qt5::Widgets)
target_link_libraries(TRenderer PUBLIC glm::glm)
#target_link_libraries(TRenderer PUBLIC TBB::tbb)
target_link_libraries(TRenderer PUBLIC assimp::assimp)
target_link_libraries(TRenderer PRIVATE OpenMP::OpenMP_CXX)



set_target_properties(TRenderer PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TRenderer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
