cmake_minimum_required(VERSION 3.12)

project(NervosFuzzer VERSION 0.5.2 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# BOLOS SDK
set(BOLOS_SDK $ENV{BOLOS_SDK})
add_compile_definitions(
        IO_HID_EP_LENGTH=64
        OS_IO_SEPROXYHAL
        HAVE_BAGL
        BAGL_WIDTH=128
        BAGL_HEIGHT=32
        IO_SEPROXYHAL_BUFFER_SIZE_B=128
        HAVE_UX_FLOW
)

add_compile_definitions(
        HAVE_ECC
        HAVE_ECC_WEIERSTRASS
        HAVE_SECP_CURVES
        HAVE_ECC_TWISTED_EDWARDS
        HAVE_ED_CURVES
        HAVE_ECDSA
        HAVE_HASH
        HAVE_BLAKE2
        HAVE_SHA256
)

include_directories(.
        "${BOLOS_SDK}/include"
        "${BOLOS_SDK}/lib_cxng/include"
        "${BOLOS_SDK}/lib_ux/include"
)

add_compile_options(-g -ggdb2 -fno-omit-frame-pointer)

# Build with code coverage generation
if(CODE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
        add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
        add_link_options(-fprofile-instr-generate -fcoverage-mapping)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
        add_compile_options(-fprofile-arcs -ftest-coverage)
        link_libraries(gcov)
    else()
        message(FATAL_ERROR "Unsupported compiler used with code coverage generation")
    endif()
endif()

# Fuzzer target
set(APP_SRC_DIR "../src")

set(APP_SOURCES
        ${APP_SRC_DIR}/apdu_sign.c
        ${APP_SRC_DIR}/apdu.c
        ${APP_SRC_DIR}/apdu_pubkey.c
        ${APP_SRC_DIR}/globals.c
        ${APP_SRC_DIR}/keys.c
        ${APP_SRC_DIR}/segwit_addr.c
        ${APP_SRC_DIR}/to_string.c
        ${APP_SRC_DIR}/ui_common.c
        ${APP_SRC_DIR}/version.h
)

add_executable(fuzz
        fuzz_message.c
        mocks.c
        ui_fuzzer.c
        ${APP_SOURCES}
)

target_include_directories(fuzz PRIVATE ../src/)

target_compile_definitions(fuzz PRIVATE
        APPVERSION_M=${CMAKE_PROJECT_VERSION_MAJOR}
        APPVERSION_N=${CMAKE_PROJECT_VERSION_MINOR}
        APPVERSION_P=${CMAKE_PROJECT_VERSION_PATCH}
        PRINTF=printf
        COMMIT="xxx"  # fake commit value
)
target_link_libraries(fuzz PRIVATE bsd)

target_compile_options(fuzz PRIVATE -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=undefined)
target_link_options(fuzz PRIVATE -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=undefined)
