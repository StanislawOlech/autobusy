
# Add .cpp files
set(SOURCE_FILES
        Tram.cpp
        Graph.cpp
        Station.cpp
        Bees.cpp
        Problem.cpp
        gui_utility.cpp
        )

set(GUI_SOURCE_FILES
        gui.cpp
)


add_library(autobusLib ${SOURCE_FILES})
target_include_directories(autobusLib PUBLIC "")


# ==========================================


FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG master
)

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_TEST OFF CACHE BOOL  "GLFW lib only" )

# ==========================================

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG docking
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    message("Fetching imgui")
    FetchContent_Populate(imgui)

    add_library(imgui
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)

    target_include_directories(imgui PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends
            ${glfw_SOURCE_DIR}/include)

    target_link_libraries(imgui PRIVATE glfw)

    # copy font
    configure_file(${imgui_SOURCE_DIR}/misc/fonts/Roboto-Medium.ttf  ${CMAKE_BINARY_DIR} COPYONLY)
endif ()

# ==========================================

FetchContent_Declare(
        implot
        GIT_REPOSITORY https://github.com/epezent/implot.git
        GIT_TAG master
)
FetchContent_GetProperties(implot)
if(NOT implot_POPULATED)
    message("Fetching implot")
    FetchContent_Populate(implot)

    add_library(implot
            ${implot_SOURCE_DIR}/implot.cpp
            ${implot_SOURCE_DIR}/implot_items.cpp
            ${implot_SOURCE_DIR}/implot_demo.cpp
    )

    target_include_directories(implot PUBLIC
            ${implot_SOURCE_DIR}
    )

    target_link_libraries(implot PRIVATE imgui)
endif ()

# ==========================================


FetchContent_MakeAvailable(glfw)

add_library(autobusLibGui ${GUI_SOURCE_FILES})
target_link_libraries(autobusLibGui PUBLIC opengl32 glfw imgui implot autobusLib)


# Allows to omit cast to char* from char8_t on UTF-8 strings C++>=20
if(MSVC)
    target_compile_options(autobusLibGui PUBLIC /Zc:char8_t)
else()
    target_compile_options(autobusLibGui PUBLIC -fno-char8_t)
endif()
