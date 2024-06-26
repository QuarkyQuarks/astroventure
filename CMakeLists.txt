cmake_minimum_required(VERSION 3.18.1)

project(Astroventure VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)

# silence warnings from sol2
add_compile_options(-Wno-unknown-warning-option)

include("${algine_dir}/cmake/mold.cmake")
try_use_mold()

macro(set_global name value)
    set(${name} ${value} CACHE INTERNAL ${name})
endmacro()

set_global(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set_global(ASSIMP_BUILD_OBJ_IMPORTER ON)
set_global(ASSIMP_BUILD_ZLIB OFF)
set_global(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set_global(ASSIMP_BUILD_TESTS OFF)
set_global(LUA_ENABLE_SHARED OFF)

set(PCC_BUILD_SHARED ON) # set(PCC_BUILD_STATIC ON) for release

add_subdirectory(${algine_dir} algine)
add_subdirectory(contrib/PointCloudCrust)

include(cmake/SoLoud.cmake)
SoLoud_collect_sources(
        BACKENDS alsa miniaudio winmm
        AUDIOSOURCES wav)

set(SoLoud_SOURCES
        ${SoLoud_core}
        ${SoLoud_audiosources})

if (ANDROID)
    set(SoLoud_SOURCES ${SoLoud_SOURCES} ${SoLoud_backend_miniaudio})
    set(Astroventure_link_extra OpenSLES) # for api [16, 27)
    add_definitions(-DWITH_MINIAUDIO)
elseif (UNIX)
    set(SoLoud_SOURCES ${SoLoud_SOURCES} ${SoLoud_backend_alsa})
    set(Astroventure_link_extra asound)
    add_definitions(-DWITH_ALSA)
elseif (WIN32)
    set(SoLoud_SOURCES ${SoLoud_SOURCES} ${SoLoud_backend_winmm})
    set(Astroventure_link_extra winmm)
    add_definitions(-DWITH_WINMM)
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

set(Astroventure_SOURCES ${SoLoud_SOURCES}
        src/Astroventure.cpp
        src/game/GameContent.cpp src/game/GameContent.h
        src/game/planet/PlanetGenerator.cpp src/game/planet/PlanetGenerator.h
        src/game/planet/Planet.cpp src/game/planet/Planet.h
        src/game/spacecraft/Spacecraft.cpp src/game/spacecraft/Spacecraft.h
        src/game/spacecraft/SpacecraftManager.cpp src/game/spacecraft/SpacecraftManager.h
        src/game/math/MathConstants.h
        src/game/math/VecTools.h
        src/game/mechanics/ODE.h
        src/game/GameBackground.cpp src/game/GameBackground.h
        src/game/planet/AtmosphereRenderer.cpp src/game/planet/AtmosphereRenderer.h
        src/game/Cameraman.cpp
        src/game/Cameraman.h
        src/game/planet/PlanetOrbit.cpp
        src/game/planet/PlanetOrbit.h
        src/game/planet/OrbitGenerator.cpp
        src/game/planet/OrbitGenerator.h
        src/game/audio/AudioPlayer.cpp src/game/audio/AudioPlayer.h
        src/game/base/SpaceModel.h
        src/game/base/ClusterModel.cpp src/game/base/ClusterModel.h
        src/loader/LoaderContent.cpp src/loader/LoaderContent.h
        src/loader/ui/LoadingLabel.cpp src/loader/ui/LoadingLabel.h
        src/game/SettingsManager.cpp src/game/SettingsManager.h
        src/loader/ui/RotatingOrbitsWidget.cpp src/loader/ui/RotatingOrbitsWidget.h
        src/game/mechanics/Mechanics.cpp src/game/mechanics/Mechanics.h
        src/game/planet/PlanetGraph.cpp src/game/planet/PlanetGraph.h
        src/game/planet/IcoSphere.cpp src/game/planet/IcoSphere.h
        src/game/planet/Triangle.h
        src/game/base/ParticleSystem.cpp src/game/base/ParticleSystem.h
        src/game/planet/CrystalParticleSystem.cpp src/game/planet/CrystalParticleSystem.h
        src/game/ui/GameUIScene.cpp src/game/ui/GameUIScene.h
        src/game/ui/GameLayer.cpp src/game/ui/GameLayer.h
        src/game/ui/StartLayer.cpp src/game/ui/StartLayer.h
        src/game/ui/AnimTools.cpp src/game/ui/AnimTools.h
        src/game/ui/menu/SettingsLayer.cpp src/game/ui/menu/SettingsLayer.h
        src/game/ui/menu/MenuLayer.cpp src/game/ui/menu/MenuLayer.h
        src/game/ui/menu/PauseLayer.cpp src/game/ui/menu/PauseLayer.h
        src/game/ui/menu/GameOverLayer.cpp src/game/ui/menu/GameOverLayer.h
        src/game/ui/ButtonAnimation.cpp src/game/ui/ButtonAnimation.h
        src/game/scheme/ColorScheme.cpp src/game/scheme/ColorScheme.h
        src/game/ui/Layer.cpp src/game/ui/Layer.h
        src/game/scheme/ColorSchemeManager.cpp src/game/scheme/ColorSchemeManager.h
        src/game/audio/AudioManager.cpp src/game/audio/AudioManager.h
        src/loader/Loadable.h
        src/loader/ui/LoaderUIScene.cpp src/loader/ui/LoaderUIScene.h
        src/loader/ui/LoaderUILayer.cpp src/loader/ui/LoaderUILayer.h
        src/game/base/SpaceModel.cpp
        src/game/render/GameRenderer.cpp src/game/render/GameRenderer.h
        src/game/render/GameRenderPass.cpp src/game/render/GameRenderPass.h
        src/game/GameScene.cpp src/game/GameScene.h
        src/game/planet/PlanetManager.cpp src/game/planet/PlanetManager.h
        src/loader/Loader.cpp src/loader/Loader.h
        src/game/render/BloomPass.cpp
        src/game/render/BloomPass.h
        src/loader/LoaderConfig.h
        src/loader/LoaderConfig.cpp
        src/game/render/DisplayPass.cpp
        src/game/render/DisplayPass.h
        src/game/render/MergePass.cpp
        src/game/render/MergePass.h
        src/game/render/UIPass.cpp
        src/game/render/UIPass.h
        src/game/FPSMeter.cpp
        src/game/FPSMeter.h
        src/game/controller/Controller.cpp
        src/game/controller/Controller.h
        src/game/controller/StartController.cpp
        src/game/controller/StartController.h
        src/game/controller/GameController.cpp
        src/game/controller/GameController.h
        src/game/planet/PlanetArray.cpp
        src/game/planet/PlanetArray.h
        src/game/mechanics/types.h
        src/game/action/StateAction.cpp
        src/game/action/StateAction.h
        src/init/init.h
        src/init/android.cpp
        src/init/glfw.cpp
        src/init/qt.cpp)

if (ANDROID)
    add_library(Astroventure SHARED ${Astroventure_SOURCES})
else()
    add_executable(Astroventure ${Astroventure_SOURCES})
endif()

target_link_libraries(Astroventure algine pcc ${Astroventure_link_extra})

target_include_directories(Astroventure PRIVATE
        contrib/random/include
        contrib/delaunator-cpp/include
        contrib/poisson-disk-sampling/thinks/poisson_disk_sampling
        contrib/soloud/include
        contrib/PointCloudCrust/include
        contrib
        src
        src/core)

if (WIN32)
    # https://stackoverflow.com/questions/22744262/cant-call-stdmax-because-minwindef-h-defines-max
    target_compile_definitions(Astroventure PRIVATE NOMINMAX)

    algine_target_mklink(
            TARGET Astroventure
            ALGINE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/algine")

    add_custom_command(TARGET Astroventure
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            -D CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}"
            -P "cmake/win32/PostBuild.cmake"
            COMMENT "Running win32/PostBuild.cmake..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
