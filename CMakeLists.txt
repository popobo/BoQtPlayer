cmake_minimum_required(VERSION 3.5)

project(BoPlayer VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.h
        widget.ui
        BoLog.h BoLog.cpp

        Interface/IDemux.h Interface/IDemux.cpp
        Interface/IObserver.h Interface/IObserver.cpp
        Interface/ISubject.h Interface/ISubject.cpp
        Interface/IDecoder.h Interface/IDecoder.cpp
        Interface/IResampler.h Interface/IResampler.cpp
        Interface/IAudioPlayer.h Interface/IAudioPlayer.cpp
        Interface/IPlayer.h Interface/IPlayer.cpp
        Interface/IVideoView.h Interface/IVideoView.cpp
        Interface/IPlayerBuilder.h Interface/IPlayerBuilder.cpp
        Interface/IParameter.h Interface/IParameter.cpp

        FFDemux.h FFDemux.cpp
        BoUtil.h BoUtil.cpp
        BoThread.h BoThread.cpp
        FFParameter.h FFParameter.cpp
        FFDecoder.h FFDecoder.cpp
        FFResampler.h FFResampler.cpp
        FFQtPlayerBuilder.h FFQtPlayerBuilder.cpp
        FFQtPlayer.h FFQtPlayer.cpp

        QAudioPlayer.h QAudioPlayer.cpp
        Data/IBoData.h Data/IBoData.cpp
        Data/BoAVPacketData.h Data/BoAVPacketData.cpp
        Data/BoAVFrameData.h Data/BoAVFrameData.cpp
        Data/BoAudioData.h Data/BoAudioData.cpp

        OpenGLRender/OpenGLMesh.h OpenGLRender/OpenGLMesh.cpp
        OpenGLRender/OpenGLRenderingThread.h OpenGLRender/OpenGLRenderingThread.cpp
        OpenGLRender/OpenGLRenderWidget.h OpenGLRender/OpenGLRenderWidget.cpp
        OpenGLRender/ElapsedTimer.h OpenGLRender/ElapsedTimer.cpp
        OpenGLRender/Renderer/OpenGLQuad.h OpenGLRender/Renderer/OpenGLQuad.cpp
        OpenGLRender/OpenGLShader.h OpenGLRender/OpenGLShader.cpp
        OpenGLRender/OpenGLViewportTarget.h OpenGLRender/OpenGLViewportTarget.cpp
        OpenGLRender/Renderer/IOpenGLRenderer.h
        OpenGLRender/Renderer/IRendererFactory.h
        OpenGLRender/Renderer/OpenGLQuadFactory.h OpenGLRender/Renderer/OpenGLQuadFactory.cpp
        OpenGLRender/Renderer/YUVRenderer.h OpenGLRender/Renderer/YUVRenderer.cpp
        OpenGLRender/Renderer/YUVRendererFactory.h OpenGLRender/Renderer/YUVRendererFactory.cpp
        OpenGLRender/IRenderWorker.h OpenGLRender/IRenderWorker.cpp
        OpenGLRender/YUVRenderWorker.h OpenGLRender/YUVRenderWorker.cpp

        Dependency/OpenGL/glad/glad.c
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BoPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BoPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BoPlayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BoPlayer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(BoPlayer PUBLIC Dependency/ffmpeg/include
                                           Dependency/spdlog/include
                                           Dependency/OpenGL/glad/include
                                           Dependency/OpenGL/glm
                                           OpenGLRender
                                           Interface)

target_link_directories(BoPlayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Dependency/ffmpeg/lib)

target_link_libraries(BoPlayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                               PRIVATE Qt6::OpenGL
                               PRIVATE Qt6::OpenGLWidgets
                               PRIVATE Qt6::Multimedia
                               libavformatd
                               libswscaled
                               libswresampled
                               libpostprocd
                               libavutild
                               libavfilterd
                               libavdeviced
                               libavcodecd)

set_target_properties(BoPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS BoPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BoPlayer)
endif()
