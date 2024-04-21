const std = @import("std");
const builtin = @import("builtin");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "vulkan-renderer",
        .target = target,
        .optimize = optimize,
    });

    const cflags = [_][]const u8{
        "-std=c17",
        "-fno-sanitize=undefined",
    };

    exe.addCSourceFile(.{
        .file = b.path("src/main.c"),
        .flags = &cflags,
    });

    exe.addIncludePath(b.path("src"));
    exe.addIncludePath(b.path("include"));
    exe.addIncludePath(try getVulkanSDKIncludePath());

    if (optimize == .Debug) {
        exe.defineCMacro("_DEBUG", null);
    }

    switch (builtin.os.tag) {
        .windows => {
            exe.defineCMacro("NOMINMAX", null);
            exe.defineCMacro("WIN32_LEAN_AND_MEAN", null);
            exe.defineCMacro("_CRT_SECURE_NO_WARNINGS", null);
            exe.defineCMacro("VK_USE_PLATFORM_WIN32_KHR", null);

            exe.addLibraryPath(b.path("lib/lib-x86_64"));
            exe.linkSystemLibrary("glfw3");
            exe.linkLibC();
        },
        .linux => {
            exe.defineCMacro("VK_USE_PLATFORM_WAYLAND_KHR", null);

            exe.addLibraryPath(b.path("lib/lib-x86_64"));
            exe.linkSystemLibrary("glfw3");
            exe.linkLibC();
        },
        .macos => {
            exe.defineCMacro("VK_USE_PLATFORM_METAL_EXT", null);

            exe.addLibraryPath(b.path("lib/lib-arm64"));
            exe.linkSystemLibrary("glfw.3");
            exe.linkFramework("Foundation");
        },
        else => unreachable,
    }

    const shader_steps = compileShaders(b, &.{
        "triangle",
    });
    for (shader_steps) |step| {
        exe.step.dependOn(step);
    }

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}

fn getVulkanSDKIncludePath() !std.Build.LazyPath {
    const allocator = std.heap.page_allocator;
    const vulkan_sdk = try std.process.getEnvVarOwned(allocator, "VULKAN_SDK");
    const path = try std.fmt.allocPrintZ(allocator, "{s}/Include", .{vulkan_sdk});

    return .{ .cwd_relative = path };
}

inline fn compileShaders(b: *std.Build, shaders: []const []const u8) []*std.Build.Step {
    const shader_types = [_][]const u8{ "vertex", "fragment" };

    var steps: [shader_types.len]*std.Build.Step = undefined;

    inline for (shader_types, 0..) |shader_type, i| {
        inline for (shaders) |shader| {
            const shader_path = "assets/shaders/" ++ shader ++ "." ++ shader_type[0..4];

            const compile_shader = b.addSystemCommand(&.{
                "glslc",
                "-fshader-stage=" ++ shader_type,
                shader_path ++ ".glsl",
                "--target-env=vulkan1.1",
                "-o",
                shader_path ++ ".spv",
            });

            steps[i] = &compile_shader.step;
        }
    }

    return &steps;
}
