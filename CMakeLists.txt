cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(ImageStack VERSION 0.1.0 LANGUAGES C CXX)

if (NOT TARGET doc)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
    )
    add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
  endif()
endif()

set(DEFAULT_COMPILE_FEATURES
  cxx_alias_templates
  cxx_alignas
  cxx_auto_type
  cxx_binary_literals
  cxx_constexpr
  cxx_decltype
  cxx_decltype_auto
  cxx_default_function_template_args
  cxx_defaulted_functions
  cxx_defaulted_move_initializers
  cxx_delegating_constructors
  cxx_deleted_functions
  cxx_explicit_conversions
  cxx_generalized_initializers
  cxx_generic_lambdas
  cxx_inheriting_constructors
  cxx_inline_namespaces
  cxx_lambdas
  cxx_lambda_init_captures
  cxx_long_long_type
  cxx_noexcept
  cxx_nullptr
  cxx_override
  cxx_range_for
  cxx_raw_string_literals
  cxx_reference_qualified_functions
  cxx_return_type_deduction
  cxx_right_angle_brackets
  cxx_rvalue_references
  cxx_sizeof_member
  cxx_static_assert
  cxx_strong_enums
  cxx_trailing_return_types
  cxx_uniform_initialization
  cxx_variadic_templates
  cxx_template_template_parameters
)

if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "AppleClang"))
  set(DEFAULT_COMPILER_OPTIONS
    -Werror
    -Weverything
#disable warnings
    -Wno-c++98-compat
    -Wno-c++98-compat-bind-to-temporary-copy
    -Wno-c++98-compat-pedantic
    -Qunused-arguments
    -Wno-unknown-warning-option
  )
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.5)
    set(DEFAULT_COMPILER_OPTIONS
      ${DEFAULT_COMPILER_OPTIONS}
      -Wno-reserved-id-macro
    )
  endif()
endif()
if (${CMAKE_COMPILER_IS_GNUCXX})
  set(DEFAULT_COMPILER_OPTIONS
    -Werror
    -Wall -Wextra -pedantic
#disable warnings
    -Wno-unknown-pragmas
    -Wno-comment
  )
endif()

set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(OpenMP INTERFACE IMPORTED)

find_package(OpenMP)
if (OPENMP_FOUND)
  message(STATUS "OpenMP found")
  option(WITH_OPENMP "Use OpenMP" NO)
  if (WITH_OPENMP)
    set_property(TARGET OpenMP PROPERTY INTERFACE_COMPILE_OPTIONS
      ${OpenMP_C_FLAGS}
    )
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
      set_property(TARGET OpenMP APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
        -Wno-source-uses-openmp
        -fopenmp-use-tls
      )
    endif()
  endif()
endif()

include(ImportGSL)
include(ImportMKL)
include(ImportEigen)

add_library(ImageStack INTERFACE)
target_compile_features(ImageStack INTERFACE ${DEFAULT_COMPILE_FEATURES})
target_compile_options(ImageStack INTERFACE ${DEFAULT_COMPILER_OPTIONS})
target_link_libraries(ImageStack INTERFACE GSL Eigen)
target_include_directories(ImageStack INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(ImageStack::ImageStack ALIAS ImageStack)

if (NOT MSVC)
  option(BUILD_TESTING "Build tests" ON)
  if(BUILD_TESTING)
	enable_testing()
	include(ImportGTest)
	add_subdirectory(Tests)
  endif()
endif()

option(BUILD_EXAMPLES "Build examples" ON)
if (BUILD_EXAMPLES)
  add_subdirectory(Examples)
endif()

###############################
#Install and packaging

install(
  TARGETS
    ImageStack
  EXPORT
    ImageStackExport
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(EXPORT ImageStackExport NAMESPACE ImageStack::
  DESTINATION lib/cmake/ImageStack
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION include
)
export(EXPORT ImageStackExport NAMESPACE ImageStack::)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ImageStackConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ImageStackConfig.cmake
  INSTALL_DESTINATION lib/cmake/ImageStack
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ImageStackConfigVersion.cmake
  VERSION ${ImageStack_VERSION}
  COMPATIBILITY AnyNewerVersion
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ImageStackConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/ImageStackConfigVersion.cmake
  DESTINATION lib/cmake/ImageStack
)


