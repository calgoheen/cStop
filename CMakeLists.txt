cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OSX deployment target")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_OSX_ARCHITECTURES x86_64 arm64)
endif()

set(PROJECT_NAME "cStop")
file(STRINGS "VERSION" CURRENT_VERSION)

project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set(FORMATS Standalone AU VST3)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Show all module sources in IDE projects" ON)

add_subdirectory(JUCE)
add_subdirectory(chowdsp_utils)

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "CalGoheen"
    COMPANY_WEBSITE "https://calgoheen.com"
    COMPANY_EMAIL "calgoheen@gmail.com"
    BUNDLE_ID com.CalGoheen.cStop
    PLUGIN_MANUFACTURER_CODE Cals
    PLUGIN_CODE Csto
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    FORMATS ${FORMATS}
    PRODUCT_NAME ${PROJECT_NAME}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Project code
file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX "" FILES ${SourceFiles})

# Binary data
# file(GLOB_RECURSE AssetFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*")
# juce_add_binary_data(Assets SOURCES ${AssetFiles})
# set_target_properties(Assets 
#     PROPERTIES 
#         POSITION_INDEPENDENT_CODE TRUE 
#         FOLDER "Targets"
# )

foreach(target ${FORMATS} "All")
    if(TARGET ${PROJECT_NAME}_${target})
        set_target_properties(${PROJECT_NAME}_${target} 
            PROPERTIES
                FOLDER "Targets"
                XCODE_GENERATE_SCHEME ON
        )
    endif()
endforeach()

set(JUCE_DEPENDENCIES
    juce::juce_audio_utils
    juce::juce_dsp
	chowdsp::chowdsp_plugin_base
	chowdsp::chowdsp_plugin_state
    chowdsp::chowdsp_filters
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
#        Assets
        ${JUCE_DEPENDENCIES}
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_generate_juce_header(${PROJECT_NAME})
