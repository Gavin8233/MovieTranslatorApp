cmake_minimum_required(VERSION 3.1...4.0)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(MovieTranslatorApp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/MovieTranslatorApp")
elseif(UNIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()


include(FetchContent)

# NLOHMANN JSON 
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# HTTPLIB
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.12.3
)
FetchContent_MakeAvailable(httplib)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/premake-libs/glad.git
)
FetchContent_MakeAvailable(glad)

# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# STBIMAGE
FetchContent_Declare(
  stbimage
  GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stbimage)

# FREETYPE
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG VER-2-13-3
)
FetchContent_MakeAvailable(freetype)

# OPENAL
FetchContent_Declare(
  openal
  GIT_REPOSITORY https://github.com/kcat/openal-soft.git
  GIT_TAG 1.24.3
)
FetchContent_MakeAvailable(openal)


# FIND FFMPEG
if(WIN32)

  set(FFMPEG_ROOT "C:/ffmpeg")
  set(FFMPEG_INCLUDE_DIR "${FFMPEG_ROOT}/include")
  set(FFMPEG_LIB_DIR "${FFMPEG_ROOT}/lib")

  if(NOT EXISTS "${FFMPEG_INCLUDE_DIR}" OR NOT EXISTS "${FFMPEG_LIB_DIR}")
    message(FATAL_ERROR "FFmpeg not found in ${FFMPEG_ROOT}. Please install FFmpeg manually and extract headers/libs there.")
  endif()

  find_library(FFMPEG_avcodec_LIBRARY NAMES avcodec PATHS "${FFMPEG_LIB_DIR}")
  find_library(FFMPEG_avformat_LIBRARY NAMES avformat PATHS "${FFMPEG_LIB_DIR}")
  find_library(FFMPEG_avutil_LIBRARY NAMES avutil PATHS "${FFMPEG_LIB_DIR}")
  find_library(FFMPEG_swscale_LIBRARY NAMES swscale PATHS "${FFMPEG_LIB_DIR}")
  find_library(FFMPEG_swresample_LIBRARY NAMES swresample PATHS "${FFMPEG_LIB_DIR}")

  if(NOT FFMPEG_avcodec_LIBRARY OR NOT FFMPEG_avformat_LIBRARY OR NOT FFMPEG_avutil_LIBRARY OR NOT FFMPEG_swscale_LIBRARY OR NOT FFMPEG_swresample_LIBRARY)
    message(FATAL_ERROR "Could not find all FFmpeg libraries in ${FFMPEG_LIB_DIR}")
  endif()

  set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR})
  set(FFMPEG_LIBRARIES
    ${FFMPEG_avcodec_LIBRARY}
    ${FFMPEG_avformat_LIBRARY}
    ${FFMPEG_avutil_LIBRARY}
    ${FFMPEG_swscale_LIBRARY}
    ${FFMPEG_swresample_LIBRARY}
  )

  set(FFMPEG_FOUND TRUE)

else() # IF LINUX OR MAC
  find_package(PkgConfig REQUIRED)

  pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec 
    libavformat 
    libavutil 
    libswscale 
    libswresample
  )

  if (FFMPEG_FOUND)
    set(FFMPEG_LIBRARIES PkgConfig::FFMPEG)
  endif()
endif()

if(NOT FFMPEG_FOUND)
  message(FATAL_ERROR "FFMPEG NOT FOUND!")
endif()

# Link 
add_executable(   
  MovieTranslatorApp
  source/main.cpp
  source/utility/utility.cpp
  source/GL/shader.cpp
  source/video_player/video_decoder.cpp
  source/subtitles/subtitle_decoder.cpp
  source/audio/audio_player.cpp
  source/GL/renderer.cpp
  source/video_player/video_player.cpp
  source/GL/texture_atlas.cpp
  source/GL/button.cpp
  source/utility/GLutil.cpp
  source/user/preferences.cpp
  source/variables.cpp
  source/utility/subtitleUtil.cpp
  ${glad_SOURCE_DIR}/src/glad.c
)

target_include_directories(
  MovieTranslatorApp PRIVATE
  ${FFMPEG_INCLUDE_DIRS}
  ${glad_SOURCE_DIR}/include
  ${freetype_SOURCE_DIR}/include
  ${openal_SOURCE_DIR}/include
  ${stbimage_SOURCE_DIR}
  ${json_SOURCE_DIR}/include
  ${httplib_SOURCE_DIR}
  ${glfw_SOURCE_DIR}/include
  ${glm_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
)

if (WIN32)
  set(OPENGL_LIB opengl32)
elseif (APPLE)
  find_library(OpenGL_LIBRARY OpenGL REQUIRED)
  set(OPENGL_LIB ${OpenGL_LIBRARY})
elseif (UNIX)
  find_package(OpenGL REQUIRED)
  set(OPENGL_LIB ${OPENGL_gl_LIBRARY})
endif()

target_link_libraries(
  MovieTranslatorApp PRIVATE
  ${FFMPEG_LIBRARIES}
  ${OPENGL_LIB}
  glfw
  freetype
  OpenAL::OpenAL
  nlohmann_json::nlohmann_json
)

# Copy DLL files & Link ws2_32 for HTTPLib to work correctly. 
if (WIN32) 
  # Required DLLs
  set(
    FFMPEG_DLLS
    avcodec-62.dll
    avformat-62.dll
    avutil-60.dll
    swscale-9.dll
    swresample-6.dll
  )
  
  foreach(dll_name IN LISTS FFMPEG_DLLS)
    install(FILES "${FFMPEG_ROOT}/bin/${dll_name}"
      DESTINATION bin
    )
  endforeach()

  install(DIRECTORY "${CMAKE_BINARY_DIR}/_deps/openal-build/Release/"
    DESTINATION bin
    FILES_MATCHING PATTERN "*.dll"
  )

  install(DIRECTORY "${CMAKE_BINARY_DIR}/_deps/freetype-build/Release"
    DESTINATION bin
    FILES_MATCHING PATTERN "*.dll"
  )

  target_link_libraries(MovieTranslatorApp PRIVATE ws2_32)
endif()


# INSTALL RESOURCES
if(APPLE)
  set_target_properties(MovieTranslatorApp PROPERTIES MACOSX_BUNDLE true)

  install(TARGETS MovieTranslatorApp
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
  )

  install(TARGETS MovieTranslatorApp
    BUNDLE DESTINATION .
  )

  install(DIRECTORY "${CMAKE_SOURCE_DIR}/resources"
    DESTINATION MovieTranslatorApp.app/Contents
  )

  install(DIRECTORY "${CMAKE_SOURCE_DIR}/shaders"
    DESTINATION MovieTranslatorApp.app/Contents
  )
else() # Linux and Windows
  install(TARGETS MovieTranslatorApp
    RUNTIME DESTINATION bin
  )

  install(DIRECTORY "${CMAKE_SOURCE_DIR}/resources"
    DESTINATION share/MovieTranslatorApp
  )

  install(DIRECTORY "${CMAKE_SOURCE_DIR}/shaders"
    DESTINATION share/MovieTranslatorApp
  )
endif()