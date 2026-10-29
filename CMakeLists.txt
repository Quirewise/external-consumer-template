cmake_minimum_required(VERSION 3.21)
project(QuirewiseExternalConsumer LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)

set(_quirewise_supported_packages
    Archive Audio Charset Comic Compression Data Document Ebook Image Page PDG Runtime
    Spreadsheet Subtitle Text Vector)
set(QUIREWISE_CONSUMER_PACKAGES "${_quirewise_supported_packages}" CACHE STRING
    "Semicolon-separated Quirewise packages to discover and consume")

if(NOT QUIREWISE_CONSUMER_PACKAGES)
    message(FATAL_ERROR "QUIREWISE_CONSUMER_PACKAGES must select at least one package")
endif()

foreach(_package IN LISTS QUIREWISE_CONSUMER_PACKAGES)
    if(NOT "${_package}" IN_LIST _quirewise_supported_packages)
        message(FATAL_ERROR "Unsupported Quirewise consumer package: ${_package}")
    endif()

    find_package(Quirewise${_package} CONFIG REQUIRED)
    if(NOT TARGET Quirewise::${_package})
        message(FATAL_ERROR "Quirewise${_package} did not export Quirewise::${_package}")
    endif()

    string(TOLOWER "${_package}" _package_lower)
    set(_consumer_target qw_external_consumer_${_package_lower})
    add_executable(${_consumer_target} "src/${_package_lower}.c")
    target_compile_features(${_consumer_target} PRIVATE c_std_11)
    target_compile_definitions(${_consumer_target} PRIVATE
        "QUIREWISE_CONSUMER_FIXTURE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/fixtures\"")
    target_link_libraries(${_consumer_target} PRIVATE Quirewise::${_package})

    if(BUILD_TESTING)
        add_test(NAME external_${_package_lower} COMMAND ${_consumer_target})
    endif()
endforeach()
