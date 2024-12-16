filegroup(
    name = "data",
    srcs = [
        "input.txt",
        "p2test.txt",
        "smalltest.txt",
        "test.txt"
    ],
    visibility = ["//visibility:private"]
)

cc_binary(
    name = "p15a",
    srcs = ["solve.cpp"],
    deps = ["//support"],
    data = [":data"],
    visibility = ["//visibility:public"]
)

cc_binary(
    name = "p15b",
    srcs = ["solve2.cpp"],
    deps = ["//support"],
    data = [":data"],
    visibility = ["//visibility:public"]
)