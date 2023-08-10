# CMake package configuration file for Quarter
#
# Defines the target "Quarter::Quarter"
#
# Add the following lines to your CMakeLists.txt to depend on Quarter
#
#    find_package(Quarter REQUIRED)
#    target_link_libraries(my_target_name Quarter::Quarter)
#
# Additionally you may one of the following variables (or their corresponding
# upper case version) that are also defined.
#
# Quarter_COMPILE_DEFINITIONS
# Quarter_DEFINITIONS
# Quarter_INCLUDE_DIRS
# Quarter_INCLUDE_DIR
# Quarter_LIBRARY
# Quarter_LIBRARIES
# Quarter_LIBRARY_DIRS
# Quarter_LIBRARY_DIR
#
# However, in most cases using the Quarter::Quarter target is sufficient,
# and you won't need these variables.


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was quarter-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)

find_dependency(Coin)

set(Quarter_HAVE_QT6 )
set(Quarter_HAVE_QT5 1)
set(Quarter_HAVE_QT4 )
if(Quarter_HAVE_QT6)
  find_dependency(Qt6Widgets)
  find_dependency(Qt6UiTools)
  find_dependency(Qt6OpenGL)
  find_dependency(Qt6OpenGLWidgets)
elseif(Quarter_HAVE_QT5)
  find_dependency(Qt5Widgets)
  find_dependency(Qt5UiTools)
  find_dependency(Qt5OpenGL)
elseif(Quarter_HAVE_QT4)
  find_dependency(Qt4 COMPONENTS QtGui QtUiTools QtOpenGL)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/quarter-export.cmake")

get_property(Quarter_COMPILE_DEFINITIONS TARGET Quarter::Quarter PROPERTY INTERFACE_COMPILE_DEFINITIONS)
foreach(_def ${Quarter_COMPILE_DEFINITIONS})
  list(APPEND Quarter_DEFINITIONS -D${_def})
endforeach()

set(Quarter_VERSION 1.1.0)

get_property(Quarter_INCLUDE_DIRS TARGET Quarter::Quarter PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
set(Quarter_INCLUDE_DIR ${Quarter_INCLUDE_DIRS})
set(Quarter_LIBRARY Quarter::Quarter)
get_property(Quarter_LIBRARIES TARGET Quarter::Quarter PROPERTY INTERFACE_LINK_LIBRARIES)
set(Quarter_LIBRARIES Quarter::Quarter ${Quarter_LIBRARIES})

set_and_check(Quarter_LIBRARY_DIRS "${PACKAGE_PREFIX_DIR}/lib")
set(Quarter_LIBRARY_DIR ${Quarter_LIBRARY_DIRS})

# For backwards compatibility define upper case versions of output variables
foreach(_var
  Quarter_COMPILE_DEFINITIONS
  Quarter_DEFINITIONS
  Quarter_INCLUDE_DIRS
  Quarter_INCLUDE_DIR
  Quarter_LIBRARY
  Quarter_LIBRARIES
  Quarter_LIBRARY_DIRS
  Quarter_LIBRARY_DIR
  Quarter_VERSION
  )
  string(TOUPPER ${_var} _uppercase_var)
  set(${_uppercase_var} ${${_var}})
endforeach()
