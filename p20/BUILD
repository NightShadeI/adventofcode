filegroup(
    name = "data",
    srcs = [
        "input.txt",
        "test.txt"
    ],
    visibility = ["//visibility:private"]
)

cc_binary(
    name = "p20a",
    srcs = ["solve.cpp"],
    deps = ["//support"],
    data = [":data"],
    visibility = ["//visibility:public"]
)

cc_binary(
    name = "p20b",
    srcs = ["solve2.cpp"],
    deps = ["//support"],
    data = [":data"],
    visibility = ["//visibility:public"]
)