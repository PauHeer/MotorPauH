# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/any
    REF boost-${VERSION}
    SHA512 721d3f1cd479994f8a6562333e76409c53c1baf2f7c77a074c9f1d07e896a88e3302eb570c3476d2fb7af5811fe7692a9e9c1b0b171deb00e8649cc588544f27
    HEAD_REF master
)

set(FEATURE_OPTIONS "")
boost_configure_and_install(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${FEATURE_OPTIONS}
)