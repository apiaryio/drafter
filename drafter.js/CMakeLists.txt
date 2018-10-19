cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(DrafterJs VERSION 4.0 LANGUAGES CXX)

set(DrafterJs_COMPILE_FEATURES
    cxx_alias_templates
    cxx_alignas
    cxx_alignof
    cxx_attribute_deprecated
    cxx_auto_type
    cxx_constexpr
    cxx_contextual_conversions
    cxx_decltype
    cxx_decltype_auto
    cxx_defaulted_functions
    cxx_defaulted_move_initializers
    cxx_delegating_constructors
    cxx_deleted_functions
    cxx_digit_separators
    cxx_explicit_conversions
    cxx_final
    cxx_generalized_initializers
    cxx_generic_lambdas
    cxx_inheriting_constructors
    cxx_lambdas
    cxx_lambda_init_captures
    cxx_noexcept
    cxx_nonstatic_member_init
    cxx_nullptr
    cxx_override
    cxx_range_for
    cxx_raw_string_literals
    cxx_return_type_deduction
    cxx_rvalue_references
    cxx_sizeof_member
    cxx_static_assert
    cxx_strong_enums
    cxx_thread_local
    cxx_trailing_return_types
    cxx_unicode_literals
    cxx_uniform_initialization
    cxx_unrestricted_unions
    cxx_user_literals
    cxx_variable_templates
    cxx_variadic_macros
    cxx_variadic_templates
    cxx_template_template_parameters
    )

# production dependencies
find_package(Drafter 4.0 REQUIRED)

# mem
add_executable(drafter-js src/cparse.cc)

target_link_libraries(drafter-js
    PRIVATE
        drafter::drafter-static

        "--pre-js ${CMAKE_CURRENT_LIST_DIR}/src/pre.js"
        "--post-js ${CMAKE_CURRENT_LIST_DIR}/src/post.js"

        "-s EXPORTED_FUNCTIONS=\"['_c_parse', '_c_validate']\""
        "-s DISABLE_EXCEPTION_CATCHING=0"
        "-s EXPORTED_RUNTIME_METHODS=\"['stringToUTF8', 'getValue', 'Pointer_stringify', 'lengthBytesUTF8', 'UTF8ToString']\""
        #"-s ASSERTIONS=${ASSERT}"
        "-s DOUBLE_MODE=0"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s NO_EXIT_RUNTIME=1"
        "-s INVOKE_RUN=0"
        "-s PRECISE_I64_MATH=0"
        "-s INLINING_LIMIT=50"
        "-s NO_FILESYSTEM=1"
        "-s NODEJS_CATCH_EXIT=0"
        "-s ELIMINATE_DUPLICATE_FUNCTIONS=1"
        "-s AGGRESSIVE_VARIABLE_ELIMINATION=1"
        "-s WASM=0"
    )

target_include_directories(drafter-js PUBLIC
    $<BUILD_INTERFACE:${DrafterJs_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${DrafterJs_SOURCE_DIR}/src>
    )

target_compile_features(drafter-js PUBLIC ${DrafterJs_COMPILE_FEATURES})

set_target_properties(drafter-js PROPERTIES OUTPUT_NAME drafter)

# nomem
add_executable(drafter-nomem-js src/cparse.cc)

target_link_libraries(drafter-nomem-js
    PRIVATE
        drafter::drafter-static
        "--memory-init-file 0"
        "--pre-js ${CMAKE_CURRENT_LIST_DIR}/src/pre.js"
        "--post-js ${CMAKE_CURRENT_LIST_DIR}/src/post.js"

        "-s EXPORTED_FUNCTIONS=\"['_c_parse', '_c_validate']\""
        "-s DISABLE_EXCEPTION_CATCHING=0"
        "-s EXPORTED_RUNTIME_METHODS=\"['stringToUTF8', 'getValue', 'Pointer_stringify', 'lengthBytesUTF8', 'UTF8ToString']\""
        #"-s ASSERTIONS=${ASSERT}"
        "-s DOUBLE_MODE=0"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s NO_EXIT_RUNTIME=1"
        "-s INVOKE_RUN=0"
        "-s PRECISE_I64_MATH=0"
        "-s INLINING_LIMIT=50"
        "-s NO_FILESYSTEM=1"
        "-s NODEJS_CATCH_EXIT=0"
        "-s ELIMINATE_DUPLICATE_FUNCTIONS=1"
        "-s AGGRESSIVE_VARIABLE_ELIMINATION=1"
        "-s WASM=0"
    )

target_include_directories(drafter-nomem-js PUBLIC
    $<BUILD_INTERFACE:${DrafterJs_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${DrafterJs_SOURCE_DIR}/src>
    )

target_compile_features(drafter-nomem-js PUBLIC ${DrafterJs_COMPILE_FEATURES})

set_target_properties(drafter-nomem-js PROPERTIES OUTPUT_NAME drafter.nomem)

install(TARGETS drafter-js drafter-nomem-js EXPORT drafter-js-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib
    FRAMEWORK DESTINATION lib
    BUNDLE DESTINATION lib
    PRIVATE_HEADER DESTINATION lib
    PUBLIC_HEADER DESTINATION lib
    RESOURCE DESTINATION lib
    )

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/drafter.js.mem"
    DESTINATION
        lib
    )

