
# Add .cpp files
set(SOURCE_FILES
        Tram.cpp
        Graph.cpp
        Station.cpp
        )


add_library(autobusLib ${SOURCE_FILES})
target_include_directories(autobusLib PUBLIC "")
target_precompile_headers(autobusLib PRIVATE
        <unordered_map>
        <vector>
        <algorithm>
)