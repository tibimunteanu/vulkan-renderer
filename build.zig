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

    const source_files = try getSourceFiles(b, .{
        .paths = &.{ "src", "include" },
        .allowed_exts = &.{".c"},
    });
    exe.addCSourceFiles(.{ .files = source_files.items, .flags = &cflags });

    exe.addIncludePath(b.path("src"));
    exe.addIncludePath(b.path("include"));
    exe.addIncludePath(try getVulkanSDKIncludePath(b));

    if (optimize == .Debug) {
        exe.defineCMacro("_DEBUG", null);
    }

    switch (builtin.os.tag) {
        .windows => {
            exe.defineCMacro("NOMINMAX", null);
            exe.defineCMacro("WIN32_LEAN_AND_MEAN", null);
            exe.defineCMacro("_CRT_SECURE_NO_WARNINGS", null);
            exe.defineCMacro("VK_USE_PLATFORM_WIN32_KHR", null);

            exe.addLibraryPath(b.path("lib/x86_64"));
            exe.linkSystemLibrary("glfw3dll");
            exe.linkLibC();
        },
        .linux => {
            exe.defineCMacro("VK_USE_PLATFORM_WAYLAND_KHR", null);

            exe.addLibraryPath(b.path("lib/x86_64"));
            exe.linkSystemLibrary("glfw3");
            exe.linkLibC();
        },
        .macos => {
            exe.defineCMacro("VK_USE_PLATFORM_METAL_EXT", null);

            exe.addLibraryPath(b.path("lib/arm64"));
            exe.linkSystemLibrary("glfw.3");
            exe.linkFramework("Foundation");
        },
        else => unreachable,
    }

    const shader_steps = try compileShaders(b, &.{
        "triangle",
    });
    for (shader_steps.items) |step| {
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

fn getVulkanSDKIncludePath(b: *std.Build) !std.Build.LazyPath {
    const vulkan_sdk = try std.process.getEnvVarOwned(b.allocator, "VULKAN_SDK");
    const path = try std.fmt.allocPrintZ(b.allocator, "{s}/Include", .{vulkan_sdk});

    return .{ .cwd_relative = path };
}

fn compileShaders(b: *std.Build, comptime shaders: []const []const u8) !std.ArrayList(*std.Build.Step) {
    const shader_types = [_][]const u8{ "vertex", "fragment" };

    var steps = std.ArrayList(*std.Build.Step).init(b.allocator);

    inline for (shader_types) |shader_type| {
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

            try steps.append(&compile_shader.step);
        }
    }

    return steps;
}

fn getSourceFiles(
    b: *std.Build,
    options: struct {
        paths: []const []const u8,
        allowed_exts: []const []const u8,
    },
) !std.ArrayList([]const u8) {
    var sources = std.ArrayList([]const u8).init(b.allocator);

    for (options.paths) |path| {
        var dir = try std.fs.cwd().openDir(path, .{ .iterate = true });

        var walker = try dir.walk(b.allocator);
        defer walker.deinit();

        while (try walker.next()) |entry| {
            const ext = std.fs.path.extension(entry.basename);

            const include_file = for (options.allowed_exts) |e| {
                if (std.mem.eql(u8, ext, e)) {
                    break true;
                }
            } else false;

            if (include_file) {
                const source_path = try std.fmt.allocPrintZ(b.allocator, "{s}/{s}", .{ path, entry.path });
                try sources.append(source_path);
            }
        }
    }

    return sources;
}
