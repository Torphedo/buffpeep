const std = @import("std");
const fs = std.fs;

const cwd = fs.cwd();

var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();

pub fn add_c_files(dir: []const u8, target: *const *std.build.Step.Compile) !void {
    // Recursively add
    const cwd_iterable = try fs.Dir.openIterableDir(cwd, dir, .{ .access_sub_paths = true });

    var walker = try cwd_iterable.walk(allocator);
    defer walker.deinit();

    while (true) {
        if (try walker.next()) |entry| {
            const path = try std.fmt.allocPrint(allocator, "{s}/{s}", .{ dir, entry.path });
            defer allocator.free(path);

            // Skip anything that's not a file
            if (entry.kind != fs.IterableDir.Entry.Kind.file) {
                continue;
            }

            // Skip non-C & non-Zig files
            if (!std.mem.endsWith(u8, entry.path, ".c") and !std.mem.endsWith(u8, entry.path, ".zig")) {
                continue;
            }

            const lazy_path = std.build.LazyPath.relative(path);
            target.*.addCSourceFile(.{ .file = lazy_path, .flags = &.{} });
            // std.debug.print("{s}\n", .{lazy_path.path});
        } else {
            break;
        }
    }
}

pub fn build(b: *std.build.Builder) !void {
    // Allow user to specify target & build mode, but default to ReleaseSafe.
    const target = b.standardTargetOptions(.{});
    const opt_level = b.standardOptimizeOption(.{ .preferred_optimize_mode = std.builtin.OptimizeMode.ReleaseSafe });

    const gl_box = b.addExecutable(.{
        .name = "gl_box",
        .link_libc = true,
        .target = target,
        .optimize = opt_level,
    });

    const glfw = b.addStaticLibrary(.{
        .name = "glfw",
        .link_libc = true,
        .target = target,
        .optimize = opt_level,
    });
    const glad = b.addStaticLibrary(.{
        .name = "glad",
        .link_libc = true,
        .target = target,
        .optimize = opt_level,
    });

    if (gl_box.target.isWindows()) {
        glfw.defineCMacro("_GLFW_WIN32", "");
    }
    if (gl_box.target.isLinux()) {
        glfw.defineCMacro("_GLFW_X11", "");
        // Can't be bothered to figure out Wayland development dependencies rn
        // glfw.defineCMacro("_GLFW_WAYLAND", "");
    }
    try add_c_files("ext/glfw/src", &glfw);
    glfw.defineCMacro("_GLFW_X11", "");

    try add_c_files("ext/glad/src", &glad);
    glad.addIncludePath(.{ .path = "ext/glad/include" });

    // Monstrosity to get CWD and print it to a buffer with the quotes
    var buf: [fs.MAX_PATH_BYTES]u8 = undefined;
    const cwd_str = try std.os.getcwd(&buf);
    var src_root_buf: [fs.MAX_PATH_BYTES + 2]u8 = undefined;
    const src_root = try std.fmt.bufPrint(&src_root_buf, "\"{s}\"", .{cwd_str});

    gl_box.defineCMacro("SRC_ROOT", src_root);
    gl_box.addIncludePath(.{ .path = "src" });
    gl_box.addIncludePath(.{ .path = "ext/cglm/include" });
    gl_box.addIncludePath(.{ .path = "ext/cgltf" });
    gl_box.addIncludePath(.{ .path = "ext/glad/include" });

    gl_box.linkLibrary(glad);
    gl_box.linkLibrary(glfw);

    try add_c_files("src", &gl_box);
    b.installArtifact(gl_box);
}
