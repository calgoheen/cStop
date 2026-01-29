cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OSX deployment target")

file(STRINGS "VERSION" CurrentVersion)
project("cStop" VERSION ${CurrentVersion})

include(FetchContent)

FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8)
FetchContent_MakeAvailable(JUCE)

FetchContent_Declare(chowdsp_utils
    GIT_REPOSITORY https://github.com/Chowdhury-DSP/chowdsp_utils
    GIT_TAG ffc70ba399f9afaeefb996eb14e55a1d487270b8)
FetchContent_MakeAvailable(chowdsp_utils)

FetchContent_Declare(clap-juce-extensions
    GIT_REPOSITORY https://github.com/free-audio/clap-juce-extensions.git
    GIT_TAG main)
FetchContent_MakeAvailable(clap-juce-extensions)

add_subdirectory(cgo_modules)

juce_add_plugin(${PROJECT_NAME}
    FORMATS AU VST3 Standalone
    COMPANY_NAME "CalGoheen"
    COMPANY_WEBSITE "https://calgoheen.com"
    COMPANY_EMAIL "calgoheen@gmail.com"
    BUNDLE_ID "com.CalGoheen.${PROJECT_NAME}"
    PLUGIN_MANUFACTURER_CODE Calg
    PLUGIN_CODE Csto
    IS_SYNTH FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    NEEDS_WEBVIEW2 TRUE)

clap_juce_extensions_plugin(TARGET ${PROJECT_NAME}
    CLAP_ID "com.CalGoheen.${PROJECT_NAME}"
    CLAP_FEATURES "audio effect" "mono" "stereo")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/plugin/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/plugin/*.h")
target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        chowdsp::chowdsp_dsp_utils
        chowdsp::chowdsp_filters
        cgo::cgo_dsp
        cgo::cgo_plugin
        cgo::cgo_processors

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        DONT_SET_USING_JUCE_NAMESPACE=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=1
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            WIN32_LEAN_AND_MEAN
            NOMINMAX)
endif()

file(COPY "${JUCE_MODULES_DIR}/juce_gui_extra/native/javascript/" DESTINATION "${CMAKE_SOURCE_DIR}/ui/src/juce/")

if(USE_LOCAL_SERVER)
    target_compile_definitions(${PROJECT_NAME} 
        PUBLIC 
            WEB_VIEW_URL="http://localhost:5173/")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/dist.zip"
        COMMAND npm install
        COMMAND npm run build
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/ui"
        COMMAND ${CMAKE_COMMAND} -E tar cf "${CMAKE_BINARY_DIR}/dist.zip" --format=zip .
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/ui/dist")

    juce_add_binary_data(Resources SOURCES "${CMAKE_BINARY_DIR}/dist.zip")
    target_link_libraries(${PROJECT_NAME} PUBLIC Resources)
endif()

juce_generate_juce_header(${PROJECT_NAME})
