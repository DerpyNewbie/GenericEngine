#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EffekseerRendererCommon" for configuration "Debug"
set_property(TARGET EffekseerRendererCommon APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(EffekseerRendererCommon PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/EffekseerRendererCommond.lib"
  )

list(APPEND _cmake_import_check_targets EffekseerRendererCommon )
list(APPEND _cmake_import_check_files_for_EffekseerRendererCommon "${_IMPORT_PREFIX}/lib/EffekseerRendererCommond.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
