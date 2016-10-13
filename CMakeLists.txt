cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(ImageStack VERSION 0.1.0 LANGUAGES CXX)

set(DEFAULT_COMPILE_FEATURES
  cxx_alias_templates
  cxx_alignas
  cxx_auto_type
  cxx_binary_literals
  cxx_constexpr
  cxx_contextual_conversions
  cxx_decltype_incomplete_return_types
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
    -Wno-reserved-id-macro
    -Qunused-arguments
  )
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


ADD_LIBRARY(ImageStack INTERFACE)
target_compile_features(ImageStack INTERFACE ${DEFAULT_COMPILE_FEATURES})
target_compile_options(ImageStack INTERFACE ${DEFAULT_COMPILER_OPTIONS})
target_include_directories(ImageStack SYSTEM INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Dependencies/eigen>
  $<INSTALL_INTERFACE:include/ImageStack/eigen>
)
target_include_directories(ImageStack INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

add_library(ImageStack::ImageStack ALIAS ImageStack)

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
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
  DESTINATION include
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Dependencies/eigen/Eigen
  DESTINATION include/ImageStack/eigen
)
export(EXPORT ImageStackExport NAMESPACE ImageStack::)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/cmake/ImageStackConfig.cmake.in
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


