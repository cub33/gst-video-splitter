cmake_minimum_required(VERSION 3.5)
project(gst-splitter VERSION 1.0.0)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB REQUIRED glib-2.0)

include_directories(
        ${GLIB_INCLUDE_DIRS}
)

link_directories(
        ${GLIB_LIBRARY_DIRS}
)

set(GSTREAMER_MINIMUM_VERSION 1.0.5)

pkg_check_modules(GST1_TEST gstreamer-1.0)
if ( GST1_TEST_FOUND AND NOT ${GST1_TEST_VERSION} VERSION_LESS ${GSTREAMER_MINIMUM_VERSION} )
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
    pkg_check_modules(GSTREAMER-APP REQUIRED gstreamer-app-1.0)
    pkg_check_modules(GSTREAMER-AUDIO REQUIRED gstreamer-audio-1.0)
    pkg_check_modules(GSTREAMER-PBUTILS REQUIRED gstreamer-pbutils-1.0)
    pkg_check_modules(GSTREAMER-FFT REQUIRED gstreamer-fft-1.0)
    add_definitions(-DGST_API_VERSION_1=1)
else()
   message(STATUS "gstreamer-1.0 not found")
   return(-1)
endif()

include_directories(
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER-APP_INCLUDE_DIRS}
        ${GSTREAMER-AUDIO_INCLUDE_DIRS}
        ${GSTREAMER-PBUTILS_INCLUDE_DIRS}
        ${GSTREAMER-FFT_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

link_directories(
        ${GSTREAMER_LIBRARY_DIRS}
        ${GSTREAMER-APP_LIBRARY_DIRS}
        ${GSTREAMER-AUDIO_LIBRARY_DIRS}
        ${GSTREAMER-PBUTILS_LIBRARY_DIRS}
        ${GSTREAMER-FFT_LIBRARY_DIRS}
)

set(project_SOURCES main.c)

set(gst_LIBRARIES
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER-APP_LIBRARIES}
        ${GSTREAMER-AUDIO_LIBRARIES}
        ${GSTREAMER-PBUTILS_LIBRARIES}
        ${GSTREAMER-FFT_LIBRARIES}
        pthread
        ${GLIB_LIBRARIES}
        ${GLIB_GIO_LIBRARIES}
        ${GLIB_GOBJECT_LIBRARIES}
        )

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${gst_LIBRARIES})