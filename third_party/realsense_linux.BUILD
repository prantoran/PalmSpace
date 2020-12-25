# Description:
#   RealSense SDK libraries for depth camera on Linux

licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

# The following build rule assumes that RealSense is built and installed
# on Ubuntu 20.04 LTS.
# If you install RealSense separately, please modify the build rule accordingly.
cc_library(
    name = "realsense",
    srcs = glob(
        [
            "lib/librealsense2-gl.so",
            "lib/librealsense2-gl.so.2.40",
            "lib/librealsense2-gl.so.2.40.0",
            "lib/librealsense2.so",
            "lib/librealsense2.so.2.40",
            "lib/librealsense2.so.2.40.0",
            "lib/librealsense-file.a",
        ],
    ),
    hdrs = glob([
        "include/librealsense2/*.h*",
        "include/librealsense2/**/*.h*",
        "include/librealsense2-gl/**/*.h*",
    ]),
    includes = [
        "include/",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)
