#
#  Library:   CTK
#
###########################################################################

cmake_minimum_required(VERSION 3.0)


#-----------------------------------------------------------------------------
# Superbuild
#
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(SUPERBUILD_TOPLEVEL_PROJECT "CTK")
set(EXTERNAL_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMakeExternals)
set(EXTERNAL_PROJECT_FILE_PREFIX "")
include(ExternalProject)
include(ctkMacroCheckExternalProjectDependency)


#-----------------------------------------------------------------------------
project(CTK)
#-----------------------------------------------------------------------------


#-----------------------------------------------------------------------------
# Library mode: SHARED (default) or STATIC
#
set(CTK_LIBRARY_MODE "SHARED")

option(CTK_BUILD_SHARED_LIBS "Build CTK libraries as shared module." ON)
mark_as_advanced(CTK_BUILD_SHARED_LIBS)
mark_as_superbuild(CTK_BUILD_SHARED_LIBS)
if(NOT CTK_BUILD_SHARED_LIBS)
  set(CTK_LIBRARY_MODE "STATIC")
endif()

#-----------------------------------------------------------------------------
# Set a default build type if none was specified
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  mark_as_advanced(CMAKE_BUILD_TYPE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
if(NOT CMAKE_CONFIGURATION_TYPES)
  mark_as_superbuild(VARS CMAKE_BUILD_TYPE ALL_PROJECTS)
else()
  mark_as_superbuild(VARS CMAKE_CONFIGURATION_TYPES ALL_PROJECTS)
endif()

mark_as_superbuild(
  VARS
    CMAKE_PREFIX_PATH:STRING
    CMAKE_DEBUG_POSTFIX:STRING
  ALL_PROJECTS
  )

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#
option(CTK_SUPERBUILD "Build ${PROJECT_NAME} and the projects it depends on." OFF)
mark_as_advanced(CTK_SUPERBUILD)

#-----------------------------------------------------------------------------
# Output directories.
#
foreach(type LIBRARY RUNTIME ARCHIVE)
  # Make sure the directory exists
  if(DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY
     AND NOT EXISTS ${CTK_CMAKE_${type}_OUTPUT_DIRECTORY})
    message(FATAL_ERROR "CTK_CMAKE_${type}_OUTPUT_DIRECTORY is set to a non-existing directory [${CTK_CMAKE_${type}_OUTPUT_DIRECTORY}]")
  endif()

  if(CTK_SUPERBUILD)
    set(output_dir ${CTK_BINARY_DIR}/bin)
    if(NOT DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY)
      set(CTK_CMAKE_${type}_OUTPUT_DIRECTORY ${CTK_BINARY_DIR}/CTK-build/bin)
    endif()
    mark_as_superbuild(CTK_CMAKE_${type}_OUTPUT_DIRECTORY:PATH)
  else()
    if(NOT DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY)
      set(output_dir ${CTK_BINARY_DIR}/bin)
    else()
      set(output_dir ${CTK_CMAKE_${type}_OUTPUT_DIRECTORY})
    endif()
  endif()
  set(CMAKE_${type}_OUTPUT_DIRECTORY ${output_dir} CACHE INTERNAL "Single output directory for building all libraries.")
  if(NOT DEFINED CTK_PLUGIN_${type}_OUTPUT_DIRECTORY)
    set(CTK_PLUGIN_${type}_OUTPUT_DIRECTORY ${CMAKE_${type}_OUTPUT_DIRECTORY})
  endif()
endforeach()

#-----------------------------------------------------------------------------
# CTK version number.  An even minor number corresponds to releases.
#
set(CTK_MAJOR_VERSION 0)
set(CTK_MINOR_VERSION 1)
set(CTK_PATCH_VERSION 0)
set(CTK_VERSION
    "${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}.${CTK_PATCH_VERSION}")

# Append the library version information to the library target
# properties.  A parent project may set its own properties and/or may
# block this.
if(NOT CTK_NO_LIBRARY_VERSION)
  set(CTK_LIBRARY_PROPERTIES ${CTK_LIBRARY_PROPERTIES}
    VERSION "${CTK_VERSION}"
    SOVERSION "${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}"
    )
endif()

#-----------------------------------------------------------------------------
# Install directories, used for install rules.
#
if(NOT CTK_INSTALL_BIN_DIR)
  set(CTK_INSTALL_BIN_DIR "bin")
endif()
if(NOT CTK_INSTALL_LIB_DIR)
  set(CTK_INSTALL_LIB_DIR "lib/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}")
endif()
if(NOT CTK_INSTALL_PLUGIN_DIR)
  set(CTK_INSTALL_PLUGIN_DIR "lib/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}/plugins")
endif()
if(NOT CTK_INSTALL_CMAKE_DIR)
  set(CTK_INSTALL_CMAKE_DIR "lib/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}/CMake")
endif()
if(NOT CTK_INSTALL_INCLUDE_DIR)
  set(CTK_INSTALL_INCLUDE_DIR "include/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}")
endif()
if(NOT CTK_INSTALL_PLUGIN_INCLUDE_DIR)
  set(CTK_INSTALL_PLUGIN_INCLUDE_DIR ${CTK_INSTALL_INCLUDE_DIR})
endif()
if(NOT CTK_INSTALL_QTPLUGIN_DIR)
  set(CTK_INSTALL_QTPLUGIN_DIR ${CTK_INSTALL_LIB_DIR})
endif()
if(NOT CTK_INSTALL_DOC_DIR)
  set(CTK_INSTALL_DOC_DIR "doc")
endif()

mark_as_superbuild(
  CTK_INSTALL_BIN_DIR:STRING
  CTK_INSTALL_LIB_DIR:STRING
  CTK_INSTALL_PLUGIN_DIR:STRING
  CTK_INSTALL_CMAKE_DIR:STRING
  CTK_INSTALL_INCLUDE_DIR:STRING
  CTK_INSTALL_PLUGIN_INCLUDE_DIR:STRING
  CTK_INSTALL_QTPLUGIN_DIR:STRING
  CTK_INSTALL_DOC_DIR:STRING
  )

#-----------------------------------------------------------------------------
# Update CMake module path
# Note: FindXXX.cmake script specific to utility should be copied into Utilities/CMake
#
set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/CMake"
  ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------------
# Clear CTK_BASE_LIBRARIES and CTK_WRAPPED_LIBRARIES_PYTHONQT
#
set(CTK_BASE_LIBRARIES CACHE INTERNAL "CTK base libraries" FORCE)
set(CTK_WRAPPED_LIBRARIES_PYTHONQT CACHE INTERNAL "CTK libraries wrapped using PythonQt" FORCE)

# Variable use in CTKConfig.cmake.in
set(CTK_LIBRARIES CACHE INTERNAL "CTK libraries" FORCE)
set(${PROJECT_NAME}_PLUGIN_LIBRARIES CACHE INTERNAL "CTK plugins" FORCE)

# Used by CTKGenerateCTKConfig.cmake and also used to reference script from other scripts
set(CTK_CMAKE_DIR ${CTK_SOURCE_DIR}/CMake)
set(CTK_CMAKE_UTILITIES_DIR ${CTK_SOURCE_DIR}/Utilities/CMake)

#-----------------------------------------------------------------------------
# CMake function(s) and macro(s)
#
foreach(file
  CMake/ctkListToString.cmake
  CMake/ctkMacroParseArguments.cmake
  CMake/ctkMacroSetPaths.cmake
  CMake/ctkMacroListFilter.cmake
  CMake/ctkMacroOptionUtils.cmake
  CMake/ctkMacroBuildLib.cmake
  CMake/ctkFunctionExtractOptimizedLibrary.cmake
  CMake/ctkMacroBuildLibWrapper.cmake
  CMake/ctkMacroBuildPlugin.cmake
  CMake/ctkMacroBuildApp.cmake
  CMake/ctkMacroBuildQtPlugin.cmake
  CMake/ctkMacroCompilePythonScript.cmake
  CMake/ctkMacroGenerateMocs.cmake
  CMake/ctkMacroWrapPythonQt.cmake
  CMake/ctkMacroSetupQt.cmake
  CMake/ctkMacroTargetLibraries.cmake # Import multiple macros
  CMake/ctkFunctionExtractOptionNameAndValue.cmake
  CMake/ctkMacroValidateBuildOptions.cmake
  CMake/ctkMacroAddCtkLibraryOptions.cmake
  CMake/ctkFunctionGenerateDGraphInput.cmake
  CMake/ctkFunctionGeneratePluginManifest.cmake
  CMake/ctkMacroGeneratePluginResourceFile.cmake
  CMake/ctkFunctionAddPluginRepo.cmake
  CMake/ctkFunctionCheckCompilerFlags.cmake
  CMake/ctkFunctionCheckoutRepo.cmake
  CMake/ctkFunctionGetIncludeDirs.cmake
  CMake/ctkFunctionGetLibraryDirs.cmake
  CMake/ctkFunctionGetGccVersion.cmake
  CMake/ctkFunctionGetCompilerVisibilityFlags.cmake
  CMake/ctkFunctionCompileSnippets.cmake
  )
  include(${file})
  install(FILES ${file} DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development)
endforeach()

foreach(file
  Libs/ctkExport.h.in
  CMake/ctkLinkerAsNeededFlagCheck.cmake
  CMake/ctk_compile_python_scripts.cmake.in
  CMake/CMakeFindDependencyMacro.cmake
  )
  install(FILES ${file} DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development)
endforeach()

install(FILES
  CMake/ctkLinkerAsNeededFlagCheck/CMakeLists.txt
  CMake/ctkLinkerAsNeededFlagCheck/A.cpp
  CMake/ctkLinkerAsNeededFlagCheck/B.cpp
  CMake/ctkLinkerAsNeededFlagCheck/C.cpp
  DESTINATION ${CTK_INSTALL_CMAKE_DIR}/ctkLinkerAsNeededFlagCheck
  COMPONENT Development
  )



#-----------------------------------------------------------------------------
# Additional CXX/C Flags
#
set(ADDITIONAL_C_FLAGS "" CACHE STRING "Additional C Flags")
mark_as_advanced(ADDITIONAL_C_FLAGS)
mark_as_superbuild(ADDITIONAL_C_FLAGS)

set(ADDITIONAL_CXX_FLAGS "" CACHE STRING "Additional CXX Flags")
mark_as_advanced(ADDITIONAL_CXX_FLAGS)
mark_as_superbuild(ADDITIONAL_CXX_FLAGS)

#-----------------------------------------------------------------------------
# Set symbol visibility Flags
#

ctkFunctionGetCompilerVisibilityFlags(VISIBILITY_CXX_FLAGS)


#-----------------------------------------------------------------------------
# CTK C/CXX Flags
#
set(CTK_C_FLAGS "${CMAKE_C_FLAGS_INIT} ${COVERAGE_C_FLAGS} ${ADDITIONAL_C_FLAGS}")
set(CTK_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} ${VISIBILITY_CXX_FLAGS} ${COVERAGE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

if(CMAKE_COMPILER_IS_GNUCXX)
  set(cflags "-Wall -Wextra -Wpointer-arith -Winvalid-pch -Wcast-align -Wwrite-strings")
  if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(cflags "${cflags} -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2")
  endif()

  ctkFunctionCheckCompilerFlags("-fdiagnostics-show-option" cflags)
  ctkFunctionCheckCompilerFlags("-Wl,--no-undefined" cflags)

  ctkFunctionGetGccVersion(${CMAKE_CXX_COMPILER} GCC_VERSION)

  # With older version of gcc supporting the flag -fstack-protector-all, an extra dependency to libssp.so
  # is introduced. If gcc is smaller than 4.4.0 and the build type is Release let's not include the flag.
  # Doing so should allow to build package made for distribution using older linux distro.
  if(${GCC_VERSION} VERSION_GREATER "4.4.0" OR (CMAKE_BUILD_TYPE STREQUAL "Debug" AND ${GCC_VERSION} VERSION_LESS "4.4.0"))
    ctkFunctionCheckCompilerFlags("-fstack-protector-all" cflags)
  endif()
  if(MINGW)
    # suppress warnings about auto imported symbols
    set(CTK_CXX_FLAGS "-Wl,--enable-auto-import ${CTK_CXX_FLAGS}")
  endif()

  set(CTK_C_FLAGS "${cflags} ${CTK_C_FLAGS}")
  set(CTK_CXX_FLAGS "${cflags} -Woverloaded-virtual -Wold-style-cast -Wstrict-null-sentinel -Wsign-promo ${CTK_CXX_FLAGS}")
endif()

#-----------------------------------------------------------------------------
# Check if the linker will resolve symbols of underlinked libraries
#
if(UNIX AND NOT APPLE)
  include(ctkLinkerAsNeededFlagCheck)
  if(CTK_LINKER_NO_AS_NEEDED_FLAG_REQUIRED)
    set(CTK_EXE_LINKER_FLAGS "-Wl,--no-as-needed")
  endif()
endif()

#-----------------------------------------------------------------------------
# QT
#
ctkMacroSetupQt()

#-----------------------------------------------------------------------------
# To make options show up in both CTK-SuperBuild and CTK regular build, let's add them
# before the SuperBuild script is included
#

#-----------------------------------------------------------------------------
# Special "BUILD ALL" options

# Build all CTK plug-ins
option(CTK_BUILD_ALL_PLUGINS "Build all CTK plug-ins" ON)
mark_as_superbuild(CTK_BUILD_ALL_PLUGINS)

# Build all CTK libraries
option(CTK_BUILD_ALL_LIBRARIES "Build all CTK libraries" ON)
mark_as_superbuild(CTK_BUILD_ALL_LIBRARIES)

# Build all CTK applications
option(CTK_BUILD_ALL_APPS "Build all CTK applications" ON)
mark_as_superbuild(CTK_BUILD_ALL_APPS)

# Build everything
option(CTK_BUILD_ALL "Build everything in CTK" ON)
mark_as_superbuild(CTK_BUILD_ALL)
if(CTK_BUILD_ALL)
  set(CTK_BUILD_ALL_PLUGINS 1)
  set(CTK_BUILD_ALL_LIBRARIES 1)
  set(CTK_BUILD_ALL_APPS 1)
endif()


#-----------------------------------------------------------------------------
# Other options

# Let's mark as advanced some default properties
mark_as_advanced(CMAKE_INSTALL_PREFIX)
mark_as_advanced(DART_TESTING_TIMEOUT)


# Qt Designer Plugins
option(CTK_BUILD_QTDESIGNER_PLUGINS "Build Qt Designer plugins" ON)
mark_as_advanced(CTK_BUILD_QTDESIGNER_PLUGINS)
mark_as_superbuild(CTK_BUILD_QTDESIGNER_PLUGINS)

#-----------------------------------------------------------------------------
# CTK Libraries
#

ctk_lib_option(Core
               "Build the Core library" ON)

ctk_lib_option(PluginFramework
               "Build the Plugin Framework" ON
               CTK_ENABLE_PluginFramework)

ctk_lib_option(Widgets
               "Build the Widgets library" ON
               CTK_ENABLE_Widgets OR (CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES) OR CTK_USE_QTTESTING)

ctk_lib_option(DICOM/Core
               "Build the DICOM Core library" ON
               CTK_ENABLE_DICOM OR (CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES))

ctk_lib_option(DICOM/Widgets
               "Build the DICOM Widgets library" ON
               CTK_ENABLE_DICOM AND CTK_ENABLE_Widgets OR (CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES))

#ctk_lib_option(ImageProcessing/ITK/Core
#               "Build the ITK Core library" OFF)

#ctk_lib_option(Scripting/Python/Core
#               "Build the Python Core library" OFF
#               CTK_ENABLE_Python_Wrapping)

#ctk_lib_option(Scripting/Python/Widgets
#               "Build the Python Widgets library" OFF)

#ctk_lib_option(Visualization/VTK/Core
#              "Build the VTK Core library" OFF)

#ctk_lib_option(Visualization/VTK/Widgets
#               "Build the VTK Widgets library" OFF)

ctk_lib_option(CommandLineModules/Core
               "Build the Command Line Module core library" OFF)

#ctk_lib_option(CommandLineModules/Frontend/QtWebKit
#               "Build the QtWebKit based Command Line Module front-end" OFF)

ctk_lib_option(CommandLineModules/Frontend/QtGui
               "Build the QtGui based Command Line Module front-end" OFF)

ctk_lib_option(CommandLineModules/Backend/XMLChecker
               "Build the Command Line Module back-end for checking XML" OFF)

ctk_lib_option(CommandLineModules/Backend/LocalProcess
               "Build the Command Line Module back-end for local processes" OFF)

ctk_lib_option(CommandLineModules/Backend/FunctionPointer
               "Build the Command Line Module back-end for function pointers" OFF)

# Save enabled CTK libraries in the cache to ensure expected player and translator
# are enabled in "Libs/QtTesting".
set(_enabled_libs)
foreach(_lib ${CTK_LIBS})
  if(CTK_LIB_${_lib})
    list(APPEND _enabled_libs ${_lib})
  endif()
endforeach()
set(CTK_ENABLED_LIBS ${_enabled_libs} CACHE INTERNAL "" FORCE)

#-----------------------------------------------------------------------------
# CTK Applications - Use ON or OFF to indicate if the application should be built by default
#

ctk_app_option(ctkApp "Build the my example application" ON )

ctk_app_option(ctkDICOM
               "Build the DICOM example application" ON
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)


ctk_app_option(ctkDICOM2
               "Build the new DICOM example application (experimental)" ON
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMIndexer
               "Build the DICOM example application" ON
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMDemoSCU
               "Build the DICOM example application" ON
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMQuery
               "Build the DICOM example application" ON
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMRetrieve
               "Build the DICOM example application" ON
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMQueryRetrieve
               "Build the DICOM example application" ON
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMHost
               "Build the DICOM application host example application" ON
               CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkExampleHost
               "Build the DICOM example application" ON
               CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkExampleHostedApp
              "Build the DICOM example application" ON
               CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES)

#if(CTK_QT_VERSION VERSION_LESS "5")
#ctk_app_option(ctkEventBusDemo
#               "Build the DICOM example application" OFF
#               CTK_ENABLE_PluginFramework AND CTK_BUILD_EXAMPLES)
#endif()

#ctk_app_option(ctkCommandLineModuleExplorer
#               "Build the Command Line Module Explorer" OFF
#               CTK_BUILD_EXAMPLES)

# We use the CTKWidgets library together with the Qt Designer plug-in
# in ctkCommandLineModuleExplorer, so enabling the options here.
# (We do not need to link them into the executable, hence no entries
# in target_libraries.cmake)

if(CTK_APP_ctkCommandLineModuleExplorer)
  foreach(_option CTK_BUILD_QTDESIGNER_PLUGINS)
    if(NOT ${_option})
      get_property(_docstring CACHE ${_option} PROPERTY HELPSTRING)
      set(${_option} ON CACHE BOOL "${_docstring}" FORCE)
      message("Enabling option [${_option}] required by [ctkCommandLineModuleExplorer]")
    endif()
  endforeach()
endif()

ctk_app_option(ctkPluginBrowser
               "Build the DICOM example application" ON
               CTK_ENABLE_PluginFramework AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkPluginGenerator
               "Build the DICOM example application" OFF
               CTK_ENABLE_PluginFramework AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMObjectViewer
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)


# Save the set of enabled apps in a cache file
set(_enabled_apps)
foreach(_app ${CTK_APPS})
  if(CTK_APP_${_app})
    list(APPEND _enabled_apps ${_app})
  endif()
endforeach()
set(CTK_ENABLED_APPS ${_enabled_apps} CACHE INTERNAL "" FORCE)

#-----------------------------------------------------------------------------
# CTK Plugins - none of them is build by default
#

# Plugins in the list below are not build by default
set(plugin_list
  # Optional plug-ins implementings interfaces in PluginFramework/service/
  org.commontk.configadmin
  org.commontk.eventadmin
  org.commontk.log
  org.commontk.metatype
  #org.commontk.helloctk
  )


foreach(_plugin ${plugin_list})
  ctk_plugin_option(${_plugin} "Build the ${_plugin} plugin." ON)
endforeach()

# Plug-ins related to the PluginGenerator application
ctk_plugin_option(org.commontk.plugingenerator.core "Build the org.commontk.plugingenerator.core plugin." ON)
ctk_plugin_option(org.commontk.plugingenerator.ui
                  "Build the org.commontk.plugingenerator.ui plugin." ON
                  CTK_APP_ctkPluginGenerator)

ctk_plugin_option(org.commontk.dah.core "Build the org.commontk.dah.core plugin." ON)
ctk_plugin_option(org.commontk.dah.hostedapp "Build the org.commontk.dah.hostedapp plugin." ON
                  CTK_ENABLE_DICOMApplicationHosting)
ctk_plugin_option(org.commontk.dah.host "Build the org.commontk.dah.host plugin." ON
                  CTK_ENABLE_DICOMApplicationHosting)

ctk_plugin_option(org.commontk.dah.exampleapp
                  "Build the org.commontk.dah.exampleapp plugin." ON
                  CTK_APP_ctkExampleHostedApp)

ctk_plugin_option(org.commontk.dah.cmdlinemoduleapp
                  "Build the org.commontk.dah.cmdlinemoduleapp plugin." ON
                  CTK_APP_ctkCommandLineModuleApp)

ctk_plugin_option(org.commontk.dah.examplehost
                  "Build the org.commontk.dah.examplehost plugin." ON
                  CTK_APP_ctkExampleHost)

#ctk_plugin_option(org.commontk.eventbus
#                  "Build the org.commontk.eventbus plugin." ON
#                  CTK_APP_ctkEventBusDemo)


# Add the PluginsContrib repo to the build system
option(CTK_USE_CONTRIBUTED_PLUGINS OFF "Use CTK plug-ins from the PluginsContrib repository")
mark_as_advanced(CTK_USE_CONTRIBUTED_PLUGINS)
mark_as_superbuild(CTK_USE_CONTRIBUTED_PLUGINS)
if(CTK_USE_CONTRIBUTED_PLUGINS)
  ctkFunctionAddPluginRepo(NAME PluginsContrib
                           GIT_URL github.com/commontk/PluginsContrib.git
                           GIT_TAG f016d35
                          )
  mark_as_superbuild(PluginsContrib_DIR:PATH)
endif()

#-----------------------------------------------------------------------------
# High-Level CTK options

ctk_enable_option(DICOM "Enable default DICOM support" ON
                  CTK_LIB_DICOM/Widgets)

ctk_enable_option(DICOMApplicationHosting "Enable DICOM Part 19 Application Hosting support" ON
                  CTK_PLUGIN_org.commontk.dah.host AND CTK_PLUGIN_org.commontk.dah.hostedapp)

ctk_enable_option(Widgets "Enable Qt Widget libraries" ON
                  CTK_LIB_Widgets)

ctk_enable_option(PluginFramework "Enable Plugin Framework" ON
                  CTK_LIB_PluginFramework)


# for the CTK_BUILD_EXAMPLES option to be ON
set(_build_examples_logical_expr)
foreach(_app ${CTK_ENABLED_APPS})
  list(APPEND _build_examples_logical_expr CTK_APP_${_app} AND)
endforeach()
if(_build_examples_logical_expr)
  list(REMOVE_AT _build_examples_logical_expr -1)
endif()

ctk_enable_option_raw(CTK_BUILD_EXAMPLES "Build examples for CTK components" ON
                  ${_build_examples_logical_expr})

#-----------------------------------------------------------------------------
# Generate target_directories list - List of directory corresponding to the different
# libraries, plugins and applications associated with the corresponding option name.
#
#       )


# Create list of directories corresponding to the enabled targets
set(target_directories)

foreach(lib ${CTK_LIBS})
  if(CTK_LIB_${lib})
    ctkMacroAddCtkLibraryOptions(${lib})
  endif()
  list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}^^CTK_LIB_${lib}")
endforeach()

foreach(plugin ${CTK_PLUGINS})
  if(${plugin}_SOURCE_DIR)
    list(APPEND target_directories "${${plugin}_SOURCE_DIR}^^CTK_PLUGIN_${plugin}")
  else()
    list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Plugins/${plugin}^^CTK_PLUGIN_${plugin}")
  endif()
endforeach()

foreach(app ${CTK_APPS})
  list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Applications/${app}^^CTK_APP_${app}")
endforeach()


#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
try_compile(RESULT_VAR ${CTK_BINARY_DIR}/Utilities/DGraph ${CTK_SOURCE_DIR}/Utilities/DGraph
              DGraph
              CMAKE_FLAGS
                -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
                -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
                -DCMAKE_VERBOSE_MAKEFILE:BOOL=FALSE
              OUTPUT_VARIABLE output)
if(NOT RESULT_VAR)
  message(FATAL_ERROR "Failed to compile DGraph application.\n${output}")
endif()
find_program(DGraph_EXECUTABLE DGraph
  "${CTK_BINARY_DIR}/Utilities/DGraph/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/bin/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/Debug/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/Release/")
mark_as_advanced(DGraph_EXECUTABLE)

ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}")
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}" WITH_EXTERNALS)
ctkMacroValidateBuildOptions("${CTK_BINARY_DIR}" "${DGraph_EXECUTABLE}" "${target_directories}")


#-----------------------------------------------------------------------------
# CTK dependencies - Projects should be TOPOLOGICALLY ordered
#-----------------------------------------------------------------------------
set(CTK_DEPENDENCIES
#  Log4Qt
#  VTK
#  PythonQt
#  DCMTK
#  ZMQ
   QtSOAP
#   qxmlrpc
#  qRestAPI
#  OpenIGTLink
#  ITK
   QtTesting
  )



# Check out the ExternalProjectsContrib repository
if(CTK_USE_CONTRIBUTED_PLUGINS)
  if(CTK_SUPERBUILD)
    ctkFunctionCheckoutRepo(
      NAME ExternalProjectsContrib
      GIT_URL github.com/commontk/ExternalProjectsContrib.git
      GIT_TAG 4c944ef
      )
    mark_as_superbuild(ExternalProjectsContrib_DIR:PATH)
  endif()

  file(GLOB _contrib_scripts ${ExternalProjectsContrib_DIR}/*.cmake)
  foreach(_contrib_script ${_contrib_scripts})
    get_filename_component(_script_name ${_contrib_script} NAME_WE)
    list(APPEND CTK_DEPENDENCIES ${_script_name})
    set(${_script_name}_FILEPATH ${_contrib_script})
  endforeach()
endif()

include(CMake/ctkBlockCheckDependencies.cmake)

#-----------------------------------------------------------------------------
# Superbuild script

if(CTK_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()

find_package(DCMTK REQUIRED)


#-----------------------------------------------------------------------------
# Expand variables containing include and library directories for external projects
# This relies on the variable EXTERNAL_TARGETS set in ctkMacroValidateBuildOptions
foreach(_external_target ${EXTERNAL_TARGETS})
  set(_package_name ${${_external_target}_FIND_PACKAGE_CMD})
  if(_package_name)
    superbuild_is_external_project_includable(${_package_name} required)
    if(required)
      message("Calling find_package(${_package_name})")
      find_package(${_package_name} REQUIRED)
    endif()
  endif()
endforeach()

foreach(_external_target ${EXTERNAL_TARGETS})
  set(_package_name ${${_external_target}_FIND_PACKAGE_CMD})
  if("${_package_name}" STREQUAL "")
    set(_package_name ${_external_target})
  endif()
  if(${_external_target}_INCLUDE_DIRS)
    #message("[${_package_name}] Resolving include variables: ${${_external_target}_INCLUDE_DIRS}")
    set(_updated_include_dirs)
    foreach(_include_variable ${${_external_target}_INCLUDE_DIRS})
      set(_expanded_include_variable ${_include_variable})
      if(${_include_variable})
        #message("[${_package_name}] Expanding [${_include_variable}] into [${${_include_variable}}]")
        set(_expanded_include_variable ${${_include_variable}})
      endif()
      foreach(_include_dir ${_expanded_include_variable})
        if(EXISTS ${_include_dir})
          #message("[${_package_name}] Appending ${_include_dir}")
          list(APPEND _updated_include_dirs ${_include_dir})
        else()
          message(STATUS "[${_package_name}] Skipping nonexistent include directory or not set variable [${_include_dir}]")
        endif()
      endforeach()
      #message("[${_package_name}] New dirs: ${_updated_include_dirs}")
    endforeach()
    set(${_external_target}_INCLUDE_DIRS ${_updated_include_dirs})
    #message("[${_package_name}] Appended dirs: ${${_external_target}_INCLUDE_DIRS}")
  endif()
  if(${_external_target}_LIBRARY_DIRS)
    #message("[${_package_name}] Resolving library variables: ${${_external_target}_LIBRARY_DIRS}")
    set(_updated_library_dirs)
    foreach(_library_variable ${${_external_target}_LIBRARY_DIRS})
      set(_expanded_library_variable ${_library_variable})
      if(${_library_variable})
        #message("[${_package_name}] Expanding [${_library_variable}] into [${${_library_variable}}]")
        set(_expanded_library_variable ${${_library_variable}})
      endif()
      foreach(_library_dir ${_expanded_library_variable})
        if(EXISTS ${_library_dir})
          #message("[${_package_name}] Appending ${_library_dir}")
          list(APPEND _updated_library_dirs ${_library_dir})
        else()
          message(STATUS "[${_package_name}] Skipping nonexistent library directory or not set variable [${_library_dir}]")
        endif()
      endforeach()
      #message("[${_package_name}] New dirs: ${_updated_library_dirs}")
    endforeach()
    set(${_external_target}_LIBRARY_DIRS ${_updated_library_dirs})
    #message("[${_package_name}] Appended dirs: ${${_external_target}_LIBRARY_DIRS}")
  endif()
endforeach()

set(CTK_WRAP_PYTHONQT_USE_VTK ${CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK})

#-----------------------------------------------------------------------------
# CTK_SUPERBUILD_BINARY_DIR

# If CTK_SUPERBUILD_BINARY_DIR isn't defined, it means CTK is *NOT* build using Superbuild.
# In that specific case, CTK_SUPERBUILD_BINARY_DIR should default to CTK_BINARY_DIR
if(NOT DEFINED CTK_SUPERBUILD_BINARY_DIR)
  set(CTK_SUPERBUILD_BINARY_DIR ${CTK_BINARY_DIR})
endif()

#-----------------------------------------------------------------------------
# Configure files with settings
#
configure_file(${CTK_SOURCE_DIR}/CMake/UseCTK.cmake.in
               ${CTK_SUPERBUILD_BINARY_DIR}/UseCTK.cmake COPYONLY)

install(
  FILES ${CTK_SUPERBUILD_BINARY_DIR}/UseCTK.cmake
  DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development
  )

set(CTK_CONFIG_H_INCLUDE_DIR ${CTK_BINARY_DIR})

#-----------------------------------------------------------------------------
# Set C/CXX Flags
#
set(CMAKE_CXX_FLAGS ${CTK_CXX_FLAGS} CACHE STRING "CMake C Flags" FORCE)
set(CMAKE_C_FLAGS ${CTK_C_FLAGS} CACHE STRING "CMake CXX Flags" FORCE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS_INIT} ${CTK_EXE_LINKER_FLAGS}" CACHE STRING "Flags used when linking executables" FORCE)

#-----------------------------------------------------------------------------
# Set the header template which defines custom export/import macros
# for shared libraries
#
set(CTK_EXPORT_HEADER_TEMPLATE "${CTK_SOURCE_DIR}/Libs/ctkExport.h.in")

#-----------------------------------------------------------------------------
# Add CTK library subdirectories
#


foreach(lib ${CTK_LIBS})
  if(CTK_LIB_${lib})
   add_subdirectory(Libs/${lib})
  endif()
endforeach()

add_subdirectory(Libs/CommandLineModules)


#-----------------------------------------------------------------------------
# Add CTK plugin subdirectories
#
foreach(plugin ${CTK_PLUGINS})
  if(CTK_PLUGIN_${plugin})
    if(${plugin}_SOURCE_DIR)
      add_subdirectory(${${plugin}_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Plugins/${plugin})
    else()
      add_subdirectory(Plugins/${plugin})
    endif()
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Add CTK application subdirectories
#
foreach(app ${CTK_APPS})
  IF (CTK_APP_${app})
    add_subdirectory(Applications/${app})
  endif()
endforeach()



#-----------------------------------------------------------------------------
# Add general purpose subdirectories
#
add_subdirectory(Documentation)
add_subdirectory(CMake/LastConfigureStep)

