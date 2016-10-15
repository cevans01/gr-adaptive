INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_ADAPTIVE adaptive)

FIND_PATH(
    ADAPTIVE_INCLUDE_DIRS
    NAMES adaptive/api.h
    HINTS $ENV{ADAPTIVE_DIR}/include
        ${PC_ADAPTIVE_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    ADAPTIVE_LIBRARIES
    NAMES gnuradio-adaptive
    HINTS $ENV{ADAPTIVE_DIR}/lib
        ${PC_ADAPTIVE_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ADAPTIVE DEFAULT_MSG ADAPTIVE_LIBRARIES ADAPTIVE_INCLUDE_DIRS)
MARK_AS_ADVANCED(ADAPTIVE_LIBRARIES ADAPTIVE_INCLUDE_DIRS)

