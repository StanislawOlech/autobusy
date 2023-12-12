
# Add .cpp files
set(SOURCE_FILES
        Tram.cpp
        Graph.cpp
        )


add_library(autobusLib ${SOURCE_FILES})
target_include_directories(autobusLib PUBLIC "")