#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Quarter::Quarter" for configuration "Debug"
set_property(TARGET Quarter::Quarter APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Quarter::Quarter PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/Quarter1d.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/Quarter1d.dll"
  )

list(APPEND _cmake_import_check_targets Quarter::Quarter )
list(APPEND _cmake_import_check_files_for_Quarter::Quarter "${_IMPORT_PREFIX}/lib/Quarter1d.lib" "${_IMPORT_PREFIX}/bin/Quarter1d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
