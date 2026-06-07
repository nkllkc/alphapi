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
