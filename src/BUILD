load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "models",
    hdrs = ["models.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "database",
    srcs = ["database.cc"],
    hdrs = ["database.h"],
    visibility = ["//visibility:public"],
    deps = [":models"],
)

cc_library(
    name = "price_feed",
    srcs = ["price_feed.cc"],
    hdrs = ["price_feed.h"],
    visibility = ["//visibility:public"],
    deps = [":models"],
)

cc_library(
    name = "account_manager",
    srcs = ["account_manager.cc"],
    hdrs = ["account_manager.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":database",
        ":models",
    ],
)

cc_library(
    name = "order_manager",
    srcs = ["order_manager.cc"],
    hdrs = ["order_manager.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":database",
        ":models",
    ],
)

cc_library(
    name = "matching_engine",
    srcs = ["matching_engine.cc"],
    hdrs = ["matching_engine.h"],
    visibility = ["//visibility:public"],
    deps = [":models"],
)

cc_library(
    name = "executor",
    srcs = ["executor.cc"],
    hdrs = ["executor.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":account_manager",
        ":database",
        ":models",
        ":order_manager",
    ],
)

cc_library(
    name = "trading_system_lib",
    srcs = ["trading_system.cc"],
    hdrs = ["trading_system.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":account_manager",
        ":database",
        ":executor",
        ":matching_engine",
        ":models",
        ":order_manager",
        ":price_feed",
    ],
)

cc_library(
    name = "rest_api",
    srcs = ["rest_api.cc"],
    hdrs = ["rest_api.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":trading_system_lib",
        "@cpp_httplib",
        "@nlohmann_json",
    ],
)
