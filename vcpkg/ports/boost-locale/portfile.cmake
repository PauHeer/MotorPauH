# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/locale
    REF boost-${VERSION}
    SHA512 e66d2f11a29637a13dfb90fd67fb69374a869553e665452fbcb7b0909535526c57e66dd69c766cd2ade2ba74d790b07a80012937f86c0c7752e683b08d7ccd4d
    HEAD_REF master
)

set(FEATURE_OPTIONS "")
include("${CMAKE_CURRENT_LIST_DIR}/features.cmake")
boost_configure_and_install(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${FEATURE_OPTIONS}
)