#
# Build environment configuration file for QCefView
#

#
# The Qt SDK path
#
set(QT_SDK_DIR
  # Change this value to the Qt SDK path of your build environment
  "C:\\Qt\\Qt5.13.1\\5.13.1\\msvc2017\\"
)

#
# The root dir of the CEF SDK
#
set(CEF_SDK_DIR
  # Change this value to the CEF binary distribution path of your build environment
  "${CMAKE_CURRENT_SOURCE_DIR}/dep/cef_binary_77.1.18+g8e8d602+chromium-77.0.3865.120_windows32"
)

#################################################################################
#
# For CI system
#
if (DEFINED ENV{APPVEYOR})
  set(QT_SDK_DIR
    # Change this value to the Qt SDK path of your build environment
    "C:\\Qt\\Qt5.13.1\\5.13.1\\msvc2017\\"
  )
endif()