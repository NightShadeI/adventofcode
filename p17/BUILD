filegroup(
    name = "data",
    srcs = [
        "input.txt",
        "test.txt",
        "test2.txt"
    ],
    visibility = ["//visibility:private"]
)

cc_binary(
    name = "p17a",
    srcs = ["solve.cpp"],
    deps = ["//support"],
    data = [":data"],
    visibility = ["//visibility:public"]
)

cc_binary(
    name = "p17b",
    srcs = ["solve2.cpp"],
    deps = ["//support"],
    data = [":data"],
    visibility = ["//visibility:public"]
)