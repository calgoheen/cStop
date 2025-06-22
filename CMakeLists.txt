cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OSX deployment target")

file(STRINGS "VERSION" CURRENT_VERSION)
project("cStop" VERSION ${CURRENT_VERSION})

include(FetchContent)

FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8)
FetchContent_MakeAvailable(JUCE)

FetchContent_Declare(chowdsp_utils
    GIT_REPOSITORY https://github.com/Chowdhury-DSP/chowdsp_utils
    GIT_TAG v2.3.0)
FetchContent_MakeAvailable(chowdsp_utils)

FetchContent_Declare(clap-juce-extensions
    GIT_REPOSITORY https://github.com/free-audio/clap-juce-extensions.git
    GIT_TAG main)
FetchContent_MakeAvailable(clap-juce-extensions)

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "CalGoheen"
    COMPANY_WEBSITE "https://calgoheen.com"
    COMPANY_EMAIL "calgoheen@gmail.com"
    BUNDLE_ID com.CalGoheen.${PROJECT_NAME}
    PLUGIN_MANUFACTURER_CODE Cals
    PLUGIN_CODE Csto
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    FORMATS Standalone AU VST3
    PRODUCT_NAME ${PROJECT_NAME})

clap_juce_extensions_plugin(TARGET ${PROJECT_NAME}
    CLAP_ID "com.CalGoheen.${PROJECT_NAME}"
    CLAP_FEATURES "audio effect" "mono" "stereo")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Project code
file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")

target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        chowdsp::chowdsp_plugin_base
        chowdsp::chowdsp_plugin_state
        chowdsp::chowdsp_filters
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

juce_generate_juce_header(${PROJECT_NAME})
