# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/random
    REF boost-${VERSION}
    SHA512 eb436d7cec4ff70b4532dc63b8d2f32ab1bc21a8734072dde91424b8beaf76a72c172c00ddea821882509ddd63385a260ea412064a74542cbe537b3e66ef0d89
    HEAD_REF master
)

set(FEATURE_OPTIONS "")
boost_configure_and_install(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${FEATURE_OPTIONS}
)