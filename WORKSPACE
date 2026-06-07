workspace(name = "alphapi")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Rules for C++
http_archive(
    name = "rules_cc",
    urls = ["https://github.com/bazelbuild/rules_cc/releases/download/0.0.9/rules_cc-0.0.9.tar.gz"],
    sha256 = "2037875b9a432767e3283cb873ac63cc473d24db6b73afb2891cee7e5e9f74da",
    strip_prefix = "rules_cc-0.0.9",
)

load("@rules_cc//cc:repositories.bzl", "rules_cc_dependencies")
rules_cc_dependencies()

# nlohmann/json
http_archive(
    name = "nlohmann_json",
    urls = ["https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz"],
    sha256 = "0d8ab112dfcc7ff1129d6fa2105708e150e4542320f178ba038a0e25173867b3",
    strip_prefix = "json-3.11.2",
    build_file = "@//:nlohmann_json.BUILD",
)

# cpp-httplib
http_archive(
    name = "cpp_httplib",
    urls = ["https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.11.0.tar.gz"],
    sha256 = "f72dbfbb5fa153cd33ea5c72c4ae66b0c1ee88d64d0b6aee68a14abf7ea3a9d7",
    strip_prefix = "cpp-httplib-0.11.0",
    build_file = "@//:cpp_httplib.BUILD",
)
