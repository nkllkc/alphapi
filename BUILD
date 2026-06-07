load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "trading_system",
    srcs = ["main.cc"],
    deps = [
        "//src:trading_system_lib",
    ],
)

cc_binary(
    name = "rest_server",
    srcs = ["rest_server.cc"],
    deps = [
        "//src:rest_api",
        "//src:trading_system_lib",
    ],
)

cc_binary(
    name = "streaming_server",
    srcs = ["streaming_server.cc"],
    deps = [
        "//src:rest_api",
        "//src:trading_system_lib",
        "//src:websocket_server",
    ],
)
