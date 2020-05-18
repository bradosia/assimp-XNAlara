cmake_minimum_required(VERSION 3.0)

set(lib_base_name assimp-xnalara)
project(${lib_base_name} VERSION 1.0.0 LANGUAGES CXX)
set(RELEASE_DIR "${CMAKE_BINARY_DIR}/${lib_base_name}/")

# Configurable Options
OPTION(BUILD_SHARED_LIBS "Build shared libraries" ON)
OPTION(BUILD_STATIC_LIBS "Build static libraries" ON)

set(CMAKE_CXX_STANDARD 17)

# third party
set(INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(LINK_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set up source files
set(SOURCES
  code/AssetLib/XNAlara/ascii_ops.cpp
  code/AssetLib/XNAlara/bin_ops.cpp
  code/AssetLib/XNAlara/read_ascii_xps.cpp
  code/AssetLib/XNAlara/read_bin_xps.cpp
  code/AssetLib/XNAlara/XnalaraImporter.cpp
)

set(HEADERS
  code/AssetLib/XNAlara/xps_const.hpp

  code/AssetLib/XNAlara/ascii_ops.hpp
  code/AssetLib/XNAlara/bin_ops.hpp
  code/AssetLib/XNAlara/read_ascii_xps.hpp
  code/AssetLib/XNAlara/read_bin_xps.hpp
  code/AssetLib/XNAlara/XnalaraImporter.hpp
)

set(SHARED_COMPILE_DEFINITIONS
  -DSHARED_LIBRARY
)

set(STATIC_COMPILE_DEFINITIONS
  -DSTATIC_LIBRARY
)

set(LIBRARIES ${LIBRARIES}
    IrrXML
    assimp
)

set(SHARED_LINK_LIBRARIES ${LIBRARIES})
set(STATIC_LINK_LIBRARIES ${LIBRARIES})

# Set up library names
if(BUILD_SHARED_LIBS)
  set(lib_shared_name ${lib_base_name}.dll)
  set(lib_shared_name_decorated ${lib_shared_name}${libname_addition})
  set(${lib_base_name}_SHARED_LIBRARY ${lib_shared_name_decorated}  CACHE STRING "name of static library for ${lib_base_name}" FORCE)
  message( STATUS "-- Building ${lib_shared_name}" )
endif(BUILD_SHARED_LIBS)

if(BUILD_STATIC_LIBS)
  set(lib_name ${lib_base_name})
  set(lib_name_decorated ${lib_name}${libname_addition})
  set(${lib_base_name}_STATIC_LIBRARY ${lib_name_decorated}  CACHE STRING "name of static library for ${lib_base_name}" FORCE)
  message( STATUS "-- Building ${lib_name}" )
endif(BUILD_STATIC_LIBS)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(CMakePackageConfigHelpers)

if(BUILD_SHARED_LIBS)
  add_library(${lib_shared_name} SHARED ${SOURCES} ${HEADERS})
  target_compile_features(${lib_shared_name} PUBLIC cxx_std_17)
  set_property(TARGET ${lib_shared_name} PROPERTY VERSION "${PROJECT_VERSION}")
  set_property(TARGET ${lib_shared_name} PROPERTY OUTPUT_NAME  "${lib_shared_name_decorated}")
  target_include_directories(${lib_shared_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${INCLUDE_DIRECTORIES}
  )
  set_target_properties(${lib_shared_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS "ON")
  if(UNIX)
      #set_property(TARGET ${lib_shared_name} PROPERTY POSITION_INDEPENDENT_CODE ON)
  endif()
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_shared_name}Version.cmake
    VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion )

  # Compile Definitions
  target_compile_definitions(${lib_shared_name} PUBLIC ${SHARED_COMPILE_DEFINITIONS})
  # Libraries to link
  target_link_libraries(${lib_shared_name} PUBLIC ${SHARED_LINK_LIBRARIES})
endif()

if(BUILD_STATIC_LIBS)
  add_library(${lib_name} STATIC ${SOURCES} ${HEADERS})
  target_compile_features(${lib_name} PUBLIC cxx_std_17)
  set_property(TARGET ${lib_name} PROPERTY VERSION "${PROJECT_VERSION}")
  set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME  "${lib_name_decorated}")
  target_include_directories(${lib_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${INCLUDE_DIRECTORIES}
  )
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake
    VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion )

  # Compile Definitions
  target_compile_definitions(${lib_name} PUBLIC ${STATIC_COMPILE_DEFINITIONS})
  # Libraries to link
  target_link_libraries(${lib_name} PUBLIC ${STATIC_LINK_LIBRARIES})
endif()

# Post build
if(BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${lib_shared_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/bin/"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_shared_name}> "${RELEASE_DIR}/bin/"
    )
endif()

# Installation
if(BUILD_SHARED_LIBS)
  message(STATUS "INSTALL: ${lib_shared_name} TO root=${CMAKE_BINARY_DIR} and current=${CMAKE_CURRENT_BINARY_DIR}")
  install(TARGETS ${lib_shared_name}
    RUNTIME DESTINATION "${RELEASE_DIR}/modules"
    ARCHIVE DESTINATION "${RELEASE_DIR}/modules"
    LIBRARY DESTINATION "${RELEASE_DIR}/modules"
  )
endif()

if(BUILD_STATIC_LIBS)
  install(TARGETS ${lib_name} EXPORT ${lib_name}_TARGETS
    RUNTIME DESTINATION "${RELEASE_DIR}/lib"
    ARCHIVE DESTINATION "${RELEASE_DIR}/lib"
    LIBRARY DESTINATION "${RELEASE_DIR}/lib"
  )
endif()

install(FILES ${HEADERS}
  DESTINATION "${RELEASE_DIR}/headers"
COMPONENT Headers)

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION "${RELEASE_DIR}/doc/" RENAME "${lib_name}_LICENSE.txt" )

add_subdirectory(test/XNAlara)
