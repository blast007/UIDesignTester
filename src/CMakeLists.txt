add_executable(UIDesignTester main.cpp BzfKeys.h BzfPlatform.cxx BzfPlatform.h bzicon.h GLCube.cxx GLCube.h GLUtils.cxx GLUtils.h PlatformFactory.cxx PlatformFactory.h SDL2Platform.cxx SDL2Platform.h)
target_link_libraries(UIDesignTester PRIVATE RmlCore RmlDebugger)
target_link_libraries(UIDesignTester PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static)
target_link_libraries(UIDesignTester PRIVATE PNG::PNG)
target_link_libraries(UIDesignTester PRIVATE CURL::libcurl)
target_link_libraries(UIDesignTester PRIVATE glm::glm)
target_link_libraries(UIDesignTester PRIVATE glad::glad)
target_link_libraries(UIDesignTester PRIVATE fmt::fmt)

# Set the C++ standard
if(CMAKE_VERSION VERSION_LESS 3.8.0)
    set_target_properties(UIDesignTester PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
    )
else()
    target_compile_features(UIDesignTester PUBLIC cxx_std_14)
endif()
set_target_properties(UIDesignTester PROPERTIES CXX_EXTENSIONS OFF)
