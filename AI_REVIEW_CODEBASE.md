# AI_REVIEW_CODEBASE

This document is a code-focused walkthrough of the coursework project.
It includes the repository file structure and the full first-party source files with short explanations above each one.
Generated folders and local tooling folders `.git`, `.claude`, and `build` are omitted from the structure view, while vendored dependencies under `third_party/` are kept in the tree but not copied into the code appendix.

## Project Structure

```text
.
|-- include
|   |-- blockchain
|   |   |-- Block.hpp
|   |   |-- BlockBase.hpp
|   |   |-- Blockchain.hpp
|   |   |-- BlockFormatter.hpp
|   |   |-- BlockStage.hpp
|   |   \-- Validation.hpp
|   |-- models
|   |   \-- CarRecord.hpp
|   \-- utils
|       |-- AuditLog.hpp
|       |-- DatabaseManager.hpp
|       |-- HashUtil.hpp
|       |-- OperationTimer.hpp
|       \-- TimeUtil.hpp
|-- src
|   |-- blockchain
|   |   |-- Block.cpp
|   |   |-- Blockchain.cpp
|   |   |-- BlockStage.cpp
|   |   \-- Validation.cpp
|   |-- models
|   |   \-- CarRecord.cpp
|   |-- utils
|   |   |-- AuditLog.cpp
|   |   |-- DatabaseManager.cpp
|   |   |-- HashUtil.cpp
|   |   \-- TimeUtil.cpp
|   |-- main_cli.cpp
|   \-- main_gui.cpp
|-- third_party
|   |-- fonts
|   |   \-- JetBrainsMono-Regular.ttf
|   |-- glfw
|   |   |-- .github
|   |   |   |-- workflows
|   |   |   |   \-- build.yml
|   |   |   \-- CODEOWNERS
|   |   |-- CMake
|   |   |   |-- modules
|   |   |   |   \-- FindEpollShim.cmake
|   |   |   |-- cmake_uninstall.cmake.in
|   |   |   |-- GenerateMappings.cmake
|   |   |   |-- glfw3.pc.in
|   |   |   |-- glfw3Config.cmake.in
|   |   |   |-- i686-w64-mingw32.cmake
|   |   |   |-- i686-w64-mingw32-clang.cmake
|   |   |   |-- Info.plist.in
|   |   |   |-- x86_64-w64-mingw32.cmake
|   |   |   \-- x86_64-w64-mingw32-clang.cmake
|   |   |-- deps
|   |   |   |-- glad
|   |   |   |   |-- gl.h
|   |   |   |   |-- gles2.h
|   |   |   |   \-- vulkan.h
|   |   |   |-- wayland
|   |   |   |   |-- fractional-scale-v1.xml
|   |   |   |   |-- idle-inhibit-unstable-v1.xml
|   |   |   |   |-- pointer-constraints-unstable-v1.xml
|   |   |   |   |-- relative-pointer-unstable-v1.xml
|   |   |   |   |-- viewporter.xml
|   |   |   |   |-- wayland.xml
|   |   |   |   |-- xdg-activation-v1.xml
|   |   |   |   |-- xdg-decoration-unstable-v1.xml
|   |   |   |   \-- xdg-shell.xml
|   |   |   |-- getopt.c
|   |   |   |-- getopt.h
|   |   |   |-- linmath.h
|   |   |   |-- nuklear.h
|   |   |   |-- nuklear_glfw_gl2.h
|   |   |   |-- stb_image_write.h
|   |   |   |-- tinycthread.c
|   |   |   \-- tinycthread.h
|   |   |-- docs
|   |   |   |-- build.md
|   |   |   |-- CMakeLists.txt
|   |   |   |-- compat.md
|   |   |   |-- compile.md
|   |   |   |-- context.md
|   |   |   |-- CONTRIBUTING.md
|   |   |   |-- Doxyfile.in
|   |   |   |-- DoxygenLayout.xml
|   |   |   |-- extra.css
|   |   |   |-- extra.css.map
|   |   |   |-- extra.scss
|   |   |   |-- footer.html
|   |   |   |-- header.html
|   |   |   |-- input.md
|   |   |   |-- internal.md
|   |   |   |-- intro.md
|   |   |   |-- main.md
|   |   |   |-- monitor.md
|   |   |   |-- moving.md
|   |   |   |-- news.md
|   |   |   |-- quick.md
|   |   |   |-- spaces.svg
|   |   |   |-- SUPPORT.md
|   |   |   |-- vulkan.md
|   |   |   \-- window.md
|   |   |-- examples
|   |   |   |-- boing.c
|   |   |   |-- CMakeLists.txt
|   |   |   |-- gears.c
|   |   |   |-- glfw.icns
|   |   |   |-- glfw.ico
|   |   |   |-- glfw.rc
|   |   |   |-- heightmap.c
|   |   |   |-- offscreen.c
|   |   |   |-- particles.c
|   |   |   |-- sharing.c
|   |   |   |-- splitview.c
|   |   |   |-- triangle-opengl.c
|   |   |   |-- triangle-opengles.c
|   |   |   |-- wave.c
|   |   |   \-- windows.c
|   |   |-- include
|   |   |   \-- GLFW
|   |   |       |-- glfw3.h
|   |   |       \-- glfw3native.h
|   |   |-- src
|   |   |   |-- CMakeLists.txt
|   |   |   |-- cocoa_init.m
|   |   |   |-- cocoa_joystick.h
|   |   |   |-- cocoa_joystick.m
|   |   |   |-- cocoa_monitor.m
|   |   |   |-- cocoa_platform.h
|   |   |   |-- cocoa_time.c
|   |   |   |-- cocoa_time.h
|   |   |   |-- cocoa_window.m
|   |   |   |-- context.c
|   |   |   |-- egl_context.c
|   |   |   |-- glfw.rc.in
|   |   |   |-- glx_context.c
|   |   |   |-- init.c
|   |   |   |-- input.c
|   |   |   |-- internal.h
|   |   |   |-- linux_joystick.c
|   |   |   |-- linux_joystick.h
|   |   |   |-- mappings.h
|   |   |   |-- mappings.h.in
|   |   |   |-- monitor.c
|   |   |   |-- nsgl_context.m
|   |   |   |-- null_init.c
|   |   |   |-- null_joystick.c
|   |   |   |-- null_joystick.h
|   |   |   |-- null_monitor.c
|   |   |   |-- null_platform.h
|   |   |   |-- null_window.c
|   |   |   |-- osmesa_context.c
|   |   |   |-- platform.c
|   |   |   |-- platform.h
|   |   |   |-- posix_module.c
|   |   |   |-- posix_poll.c
|   |   |   |-- posix_poll.h
|   |   |   |-- posix_thread.c
|   |   |   |-- posix_thread.h
|   |   |   |-- posix_time.c
|   |   |   |-- posix_time.h
|   |   |   |-- vulkan.c
|   |   |   |-- wgl_context.c
|   |   |   |-- win32_init.c
|   |   |   |-- win32_joystick.c
|   |   |   |-- win32_joystick.h
|   |   |   |-- win32_module.c
|   |   |   |-- win32_monitor.c
|   |   |   |-- win32_platform.h
|   |   |   |-- win32_thread.c
|   |   |   |-- win32_thread.h
|   |   |   |-- win32_time.c
|   |   |   |-- win32_time.h
|   |   |   |-- win32_window.c
|   |   |   |-- window.c
|   |   |   |-- wl_init.c
|   |   |   |-- wl_monitor.c
|   |   |   |-- wl_platform.h
|   |   |   |-- wl_window.c
|   |   |   |-- x11_init.c
|   |   |   |-- x11_monitor.c
|   |   |   |-- x11_platform.h
|   |   |   |-- x11_window.c
|   |   |   |-- xkb_unicode.c
|   |   |   \-- xkb_unicode.h
|   |   |-- tests
|   |   |   |-- allocator.c
|   |   |   |-- clipboard.c
|   |   |   |-- CMakeLists.txt
|   |   |   |-- cursor.c
|   |   |   |-- empty.c
|   |   |   |-- events.c
|   |   |   |-- gamma.c
|   |   |   |-- glfwinfo.c
|   |   |   |-- icon.c
|   |   |   |-- iconify.c
|   |   |   |-- inputlag.c
|   |   |   |-- joysticks.c
|   |   |   |-- monitors.c
|   |   |   |-- msaa.c
|   |   |   |-- reopen.c
|   |   |   |-- tearing.c
|   |   |   |-- threads.c
|   |   |   |-- timeout.c
|   |   |   |-- title.c
|   |   |   |-- triangle-vulkan.c
|   |   |   \-- window.c
|   |   |-- .appveyor.yml
|   |   |-- .editorconfig
|   |   |-- .gitattributes
|   |   |-- .gitignore
|   |   |-- .mailmap
|   |   |-- CMakeLists.txt
|   |   |-- CONTRIBUTORS.md
|   |   |-- LICENSE.md
|   |   \-- README.md
|   |-- imgui
|   |   |-- .github
|   |   |   |-- ISSUE_TEMPLATE
|   |   |   |   |-- config.yml
|   |   |   |   \-- issue_template.yml
|   |   |   |-- workflows
|   |   |   |   |-- build.yml
|   |   |   |   |-- manual.yml
|   |   |   |   |-- scheduled.yml
|   |   |   |   \-- static-analysis.yml
|   |   |   |-- FUNDING.yml
|   |   |   \-- pull_request_template.md
|   |   |-- backends
|   |   |   |-- sdlgpu3
|   |   |   |   |-- build_instructions.txt
|   |   |   |   |-- shader.frag
|   |   |   |   \-- shader.vert
|   |   |   |-- vulkan
|   |   |   |   |-- build_instructions.txt
|   |   |   |   |-- generate_spv.sh
|   |   |   |   |-- glsl_shader.frag
|   |   |   |   \-- glsl_shader.vert
|   |   |   |-- imgui_impl_allegro5.cpp
|   |   |   |-- imgui_impl_allegro5.h
|   |   |   |-- imgui_impl_android.cpp
|   |   |   |-- imgui_impl_android.h
|   |   |   |-- imgui_impl_dx10.cpp
|   |   |   |-- imgui_impl_dx10.h
|   |   |   |-- imgui_impl_dx11.cpp
|   |   |   |-- imgui_impl_dx11.h
|   |   |   |-- imgui_impl_dx12.cpp
|   |   |   |-- imgui_impl_dx12.h
|   |   |   |-- imgui_impl_dx9.cpp
|   |   |   |-- imgui_impl_dx9.h
|   |   |   |-- imgui_impl_glfw.cpp
|   |   |   |-- imgui_impl_glfw.h
|   |   |   |-- imgui_impl_glut.cpp
|   |   |   |-- imgui_impl_glut.h
|   |   |   |-- imgui_impl_metal.h
|   |   |   |-- imgui_impl_metal.mm
|   |   |   |-- imgui_impl_opengl2.cpp
|   |   |   |-- imgui_impl_opengl2.h
|   |   |   |-- imgui_impl_opengl3.cpp
|   |   |   |-- imgui_impl_opengl3.h
|   |   |   |-- imgui_impl_opengl3_loader.h
|   |   |   |-- imgui_impl_osx.h
|   |   |   |-- imgui_impl_osx.mm
|   |   |   |-- imgui_impl_sdl2.cpp
|   |   |   |-- imgui_impl_sdl2.h
|   |   |   |-- imgui_impl_sdl3.cpp
|   |   |   |-- imgui_impl_sdl3.h
|   |   |   |-- imgui_impl_sdlgpu3.cpp
|   |   |   |-- imgui_impl_sdlgpu3.h
|   |   |   |-- imgui_impl_sdlgpu3_shaders.h
|   |   |   |-- imgui_impl_sdlrenderer2.cpp
|   |   |   |-- imgui_impl_sdlrenderer2.h
|   |   |   |-- imgui_impl_sdlrenderer3.cpp
|   |   |   |-- imgui_impl_sdlrenderer3.h
|   |   |   |-- imgui_impl_vulkan.cpp
|   |   |   |-- imgui_impl_vulkan.h
|   |   |   |-- imgui_impl_wgpu.cpp
|   |   |   |-- imgui_impl_wgpu.h
|   |   |   |-- imgui_impl_win32.cpp
|   |   |   \-- imgui_impl_win32.h
|   |   |-- docs
|   |   |   |-- BACKENDS.md
|   |   |   |-- CHANGELOG.txt
|   |   |   |-- CONTRIBUTING.md
|   |   |   |-- EXAMPLES.md
|   |   |   |-- FAQ.md
|   |   |   |-- FONTS.md
|   |   |   |-- README.md
|   |   |   \-- TODO.txt
|   |   |-- examples
|   |   |   |-- example_allegro5
|   |   |   |   |-- example_allegro5.vcxproj
|   |   |   |   |-- example_allegro5.vcxproj.filters
|   |   |   |   |-- imconfig_allegro5.h
|   |   |   |   |-- main.cpp
|   |   |   |   \-- README.md
|   |   |   |-- example_android_opengl3
|   |   |   |   |-- android
|   |   |   |   |   |-- app
|   |   |   |   |   |   |-- src
|   |   |   |   |   |   |   \-- main
|   |   |   |   |   |   |       |-- java
|   |   |   |   |   |   |       |   \-- MainActivity.kt
|   |   |   |   |   |   |       \-- AndroidManifest.xml
|   |   |   |   |   |   \-- build.gradle
|   |   |   |   |   |-- .gitignore
|   |   |   |   |   |-- build.gradle
|   |   |   |   |   \-- settings.gradle
|   |   |   |   |-- CMakeLists.txt
|   |   |   |   \-- main.cpp
|   |   |   |-- example_apple_metal
|   |   |   |   |-- example_apple_metal.xcodeproj
|   |   |   |   |   \-- project.pbxproj
|   |   |   |   |-- iOS
|   |   |   |   |   |-- Info-iOS.plist
|   |   |   |   |   \-- LaunchScreen.storyboard
|   |   |   |   |-- macOS
|   |   |   |   |   |-- Info-macOS.plist
|   |   |   |   |   \-- MainMenu.storyboard
|   |   |   |   |-- main.mm
|   |   |   |   \-- README.md
|   |   |   |-- example_apple_opengl2
|   |   |   |   |-- example_apple_opengl2.xcodeproj
|   |   |   |   |   \-- project.pbxproj
|   |   |   |   \-- main.mm
|   |   |   |-- example_glfw_metal
|   |   |   |   |-- main.mm
|   |   |   |   \-- Makefile
|   |   |   |-- example_glfw_opengl2
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_glfw_opengl2.vcxproj
|   |   |   |   |-- example_glfw_opengl2.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   \-- Makefile
|   |   |   |-- example_glfw_opengl3
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_glfw_opengl3.vcxproj
|   |   |   |   |-- example_glfw_opengl3.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   |-- Makefile
|   |   |   |   \-- Makefile.emscripten
|   |   |   |-- example_glfw_vulkan
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- build_win64.bat
|   |   |   |   |-- CMakeLists.txt
|   |   |   |   |-- example_glfw_vulkan.vcxproj
|   |   |   |   |-- example_glfw_vulkan.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_glfw_wgpu
|   |   |   |   |-- web
|   |   |   |   |   \-- index.html
|   |   |   |   |-- CMakeLists.txt
|   |   |   |   |-- main.cpp
|   |   |   |   |-- Makefile.emscripten
|   |   |   |   \-- README.md
|   |   |   |-- example_glut_opengl2
|   |   |   |   |-- example_glut_opengl2.vcxproj
|   |   |   |   |-- example_glut_opengl2.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   \-- Makefile
|   |   |   |-- example_null
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- main.cpp
|   |   |   |   \-- Makefile
|   |   |   |-- example_sdl2_directx11
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_sdl2_directx11.vcxproj
|   |   |   |   |-- example_sdl2_directx11.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_sdl2_metal
|   |   |   |   |-- main.mm
|   |   |   |   \-- Makefile
|   |   |   |-- example_sdl2_opengl2
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_sdl2_opengl2.vcxproj
|   |   |   |   |-- example_sdl2_opengl2.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   |-- Makefile
|   |   |   |   \-- README.md
|   |   |   |-- example_sdl2_opengl3
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_sdl2_opengl3.vcxproj
|   |   |   |   |-- example_sdl2_opengl3.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   |-- Makefile
|   |   |   |   |-- Makefile.emscripten
|   |   |   |   \-- README.md
|   |   |   |-- example_sdl2_sdlrenderer2
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_sdl2_sdlrenderer2.vcxproj
|   |   |   |   |-- example_sdl2_sdlrenderer2.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   |-- Makefile
|   |   |   |   \-- README.md
|   |   |   |-- example_sdl2_vulkan
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- build_win64.bat
|   |   |   |   |-- example_sdl2_vulkan.vcxproj
|   |   |   |   |-- example_sdl2_vulkan.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_sdl3_opengl3
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- build_win64.bat
|   |   |   |   |-- example_sdl3_opengl3.vcxproj
|   |   |   |   |-- example_sdl3_opengl3.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   |-- Makefile
|   |   |   |   |-- Makefile.emscripten
|   |   |   |   \-- README.md
|   |   |   |-- example_sdl3_sdlgpu3
|   |   |   |   |-- build_win64.bat
|   |   |   |   |-- example_sdl3_sdlgpu3.vcxproj
|   |   |   |   |-- example_sdl3_sdlgpu3.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   \-- Makefile
|   |   |   |-- example_sdl3_sdlrenderer3
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_sdl3_sdlrenderer3.vcxproj
|   |   |   |   |-- example_sdl3_sdlrenderer3.vcxproj.filters
|   |   |   |   |-- main.cpp
|   |   |   |   \-- Makefile
|   |   |   |-- example_sdl3_vulkan
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- build_win64.bat
|   |   |   |   |-- example_sdl3_vulkan.vcxproj
|   |   |   |   |-- example_sdl3_vulkan.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_win32_directx10
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_win32_directx10.vcxproj
|   |   |   |   |-- example_win32_directx10.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_win32_directx11
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_win32_directx11.vcxproj
|   |   |   |   |-- example_win32_directx11.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_win32_directx12
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_win32_directx12.vcxproj
|   |   |   |   |-- example_win32_directx12.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_win32_directx9
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_win32_directx9.vcxproj
|   |   |   |   |-- example_win32_directx9.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_win32_opengl3
|   |   |   |   |-- build_mingw.bat
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- example_win32_opengl3.vcxproj
|   |   |   |   |-- example_win32_opengl3.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- example_win32_vulkan
|   |   |   |   |-- build_win32.bat
|   |   |   |   |-- build_win64.bat
|   |   |   |   |-- example_win32_vulkan.vcxproj
|   |   |   |   |-- example_win32_vulkan.vcxproj.filters
|   |   |   |   \-- main.cpp
|   |   |   |-- libs
|   |   |   |   |-- emscripten
|   |   |   |   |   |-- emscripten_mainloop_stub.h
|   |   |   |   |   \-- shell_minimal.html
|   |   |   |   |-- glfw
|   |   |   |   |   |-- include
|   |   |   |   |   |   \-- GLFW
|   |   |   |   |   |       |-- glfw3.h
|   |   |   |   |   |       \-- glfw3native.h
|   |   |   |   |   |-- lib-vc2010-32
|   |   |   |   |   |   \-- glfw3.lib
|   |   |   |   |   |-- lib-vc2010-64
|   |   |   |   |   |   \-- glfw3.lib
|   |   |   |   |   \-- COPYING.txt
|   |   |   |   \-- usynergy
|   |   |   |       |-- README.txt
|   |   |   |       |-- uSynergy.c
|   |   |   |       \-- uSynergy.h
|   |   |   |-- imgui_examples.sln
|   |   |   \-- README.txt
|   |   |-- misc
|   |   |   |-- cpp
|   |   |   |   |-- imgui_stdlib.cpp
|   |   |   |   |-- imgui_stdlib.h
|   |   |   |   \-- README.txt
|   |   |   |-- debuggers
|   |   |   |   |-- imgui.gdb
|   |   |   |   |-- imgui.natstepfilter
|   |   |   |   |-- imgui.natvis
|   |   |   |   \-- README.txt
|   |   |   |-- fonts
|   |   |   |   |-- binary_to_compressed_c.cpp
|   |   |   |   |-- Cousine-Regular.ttf
|   |   |   |   |-- DroidSans.ttf
|   |   |   |   |-- Karla-Regular.ttf
|   |   |   |   |-- ProggyClean.ttf
|   |   |   |   |-- ProggyTiny.ttf
|   |   |   |   \-- Roboto-Medium.ttf
|   |   |   |-- freetype
|   |   |   |   |-- imgui_freetype.cpp
|   |   |   |   |-- imgui_freetype.h
|   |   |   |   \-- README.md
|   |   |   |-- single_file
|   |   |   |   \-- imgui_single_file.h
|   |   |   \-- README.txt
|   |   |-- .editorconfig
|   |   |-- .gitattributes
|   |   |-- .gitignore
|   |   |-- imconfig.h
|   |   |-- imgui.cpp
|   |   |-- imgui.h
|   |   |-- imgui_demo.cpp
|   |   |-- imgui_draw.cpp
|   |   |-- imgui_internal.h
|   |   |-- imgui_tables.cpp
|   |   |-- imgui_widgets.cpp
|   |   |-- imstb_rectpack.h
|   |   |-- imstb_textedit.h
|   |   |-- imstb_truetype.h
|   |   \-- LICENSE.txt
|   \-- sqlite
|       |-- sqlite3.c
|       \-- sqlite3.h
|-- .gitignore
|-- AI_REVIEW_CODEBASE.md
|-- CMakeLists.txt
\-- cw1_blockchain.db
```

## First-Party Code Reference

The sections below cover the project-owned build and source files. External libraries in `third_party/` are part of the repository layout, but their code is intentionally not duplicated here.

### `CMakeLists.txt`

Top-level build configuration. It sets the C++17 toolchain, defines separate CLI and GUI targets, wires in OpenSSL and OpenGL, falls back to vendored GLFW when needed, and builds SQLite as a static library for persistence.

```cmake
cmake_minimum_required(VERSION 3.16)

project(COMP2034_CW1_CarWarehouseBlockchain LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(BUILD_CLI "Build the terminal CLI version" ON)
option(BUILD_GUI "Build the ImGui GUI version"   ON)


find_package(OpenSSL REQUIRED)
find_package(OpenGL REQUIRED)


find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    message(STATUS "System glfw3 not found -- using third_party/glfw")
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/glfw)
endif()


add_library(sqlite3_lib STATIC third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3_lib PUBLIC third_party/sqlite)
if(MSVC)
    target_compile_options(sqlite3_lib PRIVATE /W0)
else()
    target_compile_options(sqlite3_lib PRIVATE -w)
endif()

set(BACKEND_SOURCES
    src/models/CarRecord.cpp
    src/blockchain/Block.cpp
    src/blockchain/BlockStage.cpp
    src/blockchain/Blockchain.cpp
    src/blockchain/Validation.cpp
    src/utils/AuditLog.cpp
    src/utils/HashUtil.cpp
    src/utils/TimeUtil.cpp
    src/utils/DatabaseManager.cpp
)


set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)


if(BUILD_CLI)
    add_executable(car_warehouse_cli
        src/main_cli.cpp
        ${BACKEND_SOURCES}
    )
    target_include_directories(car_warehouse_cli
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            third_party/sqlite
    )
    target_link_libraries(car_warehouse_cli
        PRIVATE
            OpenSSL::Crypto
            sqlite3_lib
    )
    if(MSVC)
        target_compile_options(car_warehouse_cli PRIVATE /W4 /permissive-)
    else()
        target_compile_options(car_warehouse_cli PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()


if(BUILD_GUI)
    add_executable(car_warehouse_gui
        src/main_gui.cpp
        ${BACKEND_SOURCES}
        ${IMGUI_SOURCES}
    )
    target_include_directories(car_warehouse_gui
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${IMGUI_DIR}
            ${IMGUI_DIR}/backends
            third_party/sqlite
    )
    target_link_libraries(car_warehouse_gui
        PRIVATE
            OpenSSL::Crypto
            glfw
            OpenGL::GL
            sqlite3_lib
    )

    add_custom_command(TARGET car_warehouse_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:car_warehouse_gui>/fonts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fonts/JetBrainsMono-Regular.ttf
            $<TARGET_FILE_DIR:car_warehouse_gui>/fonts/JetBrainsMono-Regular.ttf
        COMMENT "Copying JetBrains Mono font next to executable"
    )
    if(MSVC)
        target_compile_options(car_warehouse_gui PRIVATE /W4 /permissive-)
    else()
        target_compile_options(car_warehouse_gui PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()
```

### `include/blockchain/BlockBase.hpp`



```cpp
#pragma once








#include <cstddef>
#include <string>

namespace cw1 {




class BlockBase {
public:
    
    
    
    virtual ~BlockBase() = default;

    

    
    virtual std::string computeHash() const = 0;

    
    virtual std::string toString() const = 0;

    
    virtual std::size_t getIndex() const noexcept = 0;

    
    virtual const std::string& getCurrentHash() const noexcept = 0;

    
    virtual const std::string& getPreviousHash() const noexcept = 0;

    
    virtual const std::string& getTimestamp() const noexcept = 0;
};

} 
```

### `include/blockchain/Block.hpp`



```cpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "models/CarRecord.hpp"
#include "blockchain/BlockBase.hpp"

namespace cw1 {




class Block : public BlockBase {
public:
    Block(std::size_t index, std::string previousHash, CarRecord record);
    Block(std::size_t index,
          std::string previousHash,
          std::string currentHash,
          std::string timestamp,
          std::uint64_t nonce,
          CarRecord record);

    std::size_t         getIndex()        const noexcept override;
    const std::string&  getCurrentHash()  const noexcept override;
    const std::string&  getPreviousHash() const noexcept override;
    const std::string&  getTimestamp()    const noexcept override;
    std::uint64_t       getNonce()        const noexcept;
    const CarRecord&    getRecord()       const noexcept;

    
    std::string computeHash() const override;

    
    std::string toString() const override;

    
    
    void debugOverrideCurrentHash(std::string forgedHash);

    
    
    void debugTamperPayloadForSimulation(const std::string& marker);

    
    void setIndex(std::size_t newIndex);
    void setPreviousHash(std::string newPrev);

    
    void setRecord(CarRecord newRecord);

private:
    static std::uint64_t generateNonce();

    std::size_t    index_;
    std::string    currentHash_;
    std::string    previousHash_;
    std::string    timestamp_;
    std::uint64_t  nonce_;
    CarRecord      record_;
};

} 
```

### `include/blockchain/Blockchain.hpp`



```cpp
#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "blockchain/Block.hpp"
#include "blockchain/BlockStage.hpp"
#include "blockchain/Validation.hpp"
#include "models/CarRecord.hpp"
#include "utils/AuditLog.hpp"
#include "utils/DatabaseManager.hpp"

namespace cw1 {









class Blockchain {
public:
    

    
    void addBlock(const CarRecord& record);

    
    const std::vector<Block>& getChain() const noexcept;

    
    std::size_t totalBlocks() const noexcept;   

    

    
    std::vector<const Block*> getCarHistory(const std::string& vin) const;

    
    bool hasVin(const std::string& vin) const;

    
    std::vector<std::string> getAllVins() const;

    
    BlockStage getLatestStage(const std::string& vin) const;

    

    
    std::vector<const Block*> searchByBrand(const std::string& brand) const;

    
    std::vector<const Block*> searchByStage(BlockStage stage) const;

    
    std::vector<const Block*> searchGeneral(const std::string& query) const;

    

    
    ValidationResult verifyIntegrity() const;

    
    
    
    void tamperBlock(std::size_t index);

    
    
    bool tamperBlockHash(std::size_t index,
                         const std::string& forgedHash,
                         std::string& message);

    

    
    
    
    bool softDeleteBlock(std::size_t index, std::string& outMessage);

    
    
    
    bool hardDeleteBlock(std::size_t index, std::string& outMessage);

    
    bool saveBlockchain(const std::string& path) const;

    
    bool loadBlockchain(const std::string& path);

    

    

    AuditLog& getAuditLog() const noexcept;


    AuditLog& getAuditLog() noexcept;

    // -- SQLite database integration --

    // Open (or create) a SQLite database at dbPath.
    // Returns false if it fails to open.
    bool openDatabase(const std::string& dbPath);

    // Returns true if a database is currently open.
    bool isDatabaseOpen() const noexcept;

    // Save entire current chain to DB (full resync).
    bool saveToDB();

    // Load chain from DB, replacing in-memory chain_ and vinIndex_.
    // Runs verifyIntegrity() after load -- returns false if chain is invalid.
    bool loadFromDB();

    // SQL-powered search -- returns Block pointers for matching blocks.
    // Falls back to searchGeneral() if no DB is open.
    std::vector<const Block*> searchBySQL(const std::string& query) const;

    // Returns a const pointer to the DatabaseManager, or nullptr if not open.
    const DatabaseManager* getDatabase() const noexcept;

private:
    
    std::vector<Block> chain_;

    
    std::map<std::string, std::vector<std::size_t>> vinIndex_;




    mutable AuditLog auditLog_;

    // Optional SQLite database for persistence and SQL queries.
    std::unique_ptr<DatabaseManager> db_;
};

} 
```

### `include/blockchain/BlockFormatter.hpp`



```cpp
#pragma once

#include <iomanip>
#include <sstream>
#include <string>

#include "blockchain/Block.hpp"
#include "blockchain/BlockStage.hpp"

namespace cw1 {



inline std::string formatBlockForDisplay(const Block& block) {
    const CarRecord& record = block.getRecord();
    const bool isGenesis = (block.getIndex() == 0);
    const bool genesisRuleSatisfied =
        (block.getPreviousHash() == block.getCurrentHash());

    std::ostringstream out;
    out << "======================================================================\n";
    out << "Block #" << block.getIndex()
        << " | Stage: " << stageToString(record.stage) << '\n';
    out << "----------------------------------------------------------------------\n";
    out << "Current Hash : " << block.getCurrentHash() << '\n';
    out << "Previous Hash: " << block.getPreviousHash() << '\n';
    if (isGenesis) {
        out << "Genesis Rule : previousHash == currentHash -> "
            << (genesisRuleSatisfied ? "YES" : "NO") << '\n';
    }
    out << "Timestamp    : " << block.getTimestamp() << '\n';
    out << "Nonce        : " << block.getNonce() << '\n';
    out << "VIN          : " << record.vin << '\n';
    out << "Mfr. ID      : " << record.manufacturerId << '\n';
    out << "Vehicle      : " << record.manufacturer << ' ' << record.model
        << " (" << record.color << ", " << record.productionYear << ")\n";

    out << "Payload      :\n";
    switch (record.stage) {
        case BlockStage::PRODUCTION:
            out << "  - Factory Location: " << record.factoryLocation << '\n';
            break;
        case BlockStage::WAREHOUSE_INTAKE:
            out << "  - Warehouse Location: " << record.warehouseLocation << '\n';
            out << "  - Received By       : " << record.receivedBy << '\n';
            out << "  - Supplier ID       : " << record.supplierId << '\n';
            break;
        case BlockStage::QUALITY_CHECK:
            out << "  - Inspector ID      : " << record.inspectorId << '\n';
            out << "  - QC Result         : " << (record.passed ? "PASSED" : "FAILED") << '\n';
            out << "  - QC Notes          : " << record.qcNotes << '\n';
            break;
        case BlockStage::DEALER_DISPATCH:
            out << "  - Dealer ID         : " << record.dealerId << '\n';
            out << "  - Destination       : " << record.destination << '\n';
            out << "  - Transport Mode    : " << record.transportMode << '\n';
            break;
        case BlockStage::CUSTOMER_SALE:
            out << "  - Buyer ID          : " << record.buyerId << '\n';
            out << "  - Retailer ID       : " << record.retailerId << '\n';
            out << "  - Sale Price        : MYR " << std::fixed
                << std::setprecision(2) << record.salePrice << '\n';
            out << "  - Warranty Expiry   : " << record.warrantyExpiry << '\n';
            break;
    }

    out << "======================================================================\n";
    return out.str();
}

} 
```

### `include/blockchain/BlockStage.hpp`



```cpp
#pragma once

#include <string>

namespace cw1 {


enum class BlockStage {
    PRODUCTION,        
    WAREHOUSE_INTAKE,  
    QUALITY_CHECK,     
    DEALER_DISPATCH,   
    CUSTOMER_SALE      
};


std::string stageToString(BlockStage stage);


BlockStage stringToStage(const std::string& text);

} 
```

### `include/blockchain/Validation.hpp`



```cpp
#pragma once

#include <string>
#include <vector>

#include "blockchain/Block.hpp"

namespace cw1 {

struct ValidationResult {
    bool        ok;
    std::string message;
};

namespace Validation {


ValidationResult verifyChain(const std::vector<Block>& blocks);

} 

} 
```

### `include/models/CarRecord.hpp`



```cpp
#pragma once

#include <string>

#include "blockchain/BlockStage.hpp"

namespace cw1 {




struct CarRecord {
    
    std::string vin;
    std::string manufacturer;
    std::string model;
    std::string color;
    int         productionYear = 0;
    BlockStage  stage          = BlockStage::PRODUCTION;
    std::string manufacturerId;  

    
    
    std::string factoryLocation;   

    
    std::string warehouseLocation; 
    std::string receivedBy;        
    std::string supplierId;        

    
    std::string inspectorId;       
    bool        passed = false;    
    std::string qcNotes;           

    
    std::string dealerId;          
    std::string destination;       
    std::string transportMode;     

    
    std::string buyerId;           
    std::string retailerId;        
    double      salePrice = 0.0;   
    std::string warrantyExpiry;    

    
    
    std::string serialize() const;
};

} 
```

### `include/utils/AuditLog.hpp`



```cpp
#pragma once





#include <cstddef>
#include <string>
#include <utility>

namespace cw1 {



enum class AuditAction {
    BLOCK_ADDED,        
    INTEGRITY_CHECK,    
    SEARCH_PERFORMED,   
    CHAIN_VIEWED,       
    TAMPER_SIMULATED,   
    PERSISTENCE_IO,     
    BLOCK_DELETED       
};


std::string actionToString(AuditAction action);






struct AuditEntry {
    AuditAction action;
    std::string details;    
    std::string timestamp;  
    AuditEntry* next;       

    AuditEntry(AuditAction act, std::string det, std::string ts)
        : action(act), details(std::move(det)), timestamp(std::move(ts)), next(nullptr) {}
};












class AuditLog {
public:
    AuditLog();
    ~AuditLog();

    
    AuditLog(const AuditLog&)            = delete;
    AuditLog& operator=(const AuditLog&) = delete;

    
    
    void log(AuditAction action, const std::string& details);

    
    std::size_t size() const noexcept;

    
    const AuditEntry* head() const noexcept;

    
    
    
    
    
    
    
    
    
    
    const AuditEntry** getRecentEntries(std::size_t maxCount,
                                        std::size_t& outCount) const;

    
    void clear();

private:
    AuditEntry* head_;   
    AuditEntry* tail_;   
    std::size_t count_;  
};






class RecentEntryArray {
public:
    RecentEntryArray(const AuditLog& log, std::size_t maxCount);
    ~RecentEntryArray();

    RecentEntryArray(const RecentEntryArray&) = delete;
    RecentEntryArray& operator=(const RecentEntryArray&) = delete;

    RecentEntryArray(RecentEntryArray&& other) noexcept;
    RecentEntryArray& operator=(RecentEntryArray&& other) noexcept;

    std::size_t size() const noexcept;
    bool empty() const noexcept;

    const AuditEntry* operator[](std::size_t index) const noexcept;
    const AuditEntry* const* begin() const noexcept;
    const AuditEntry* const* end() const noexcept;

private:
    const AuditEntry** data_;
    std::size_t count_;
};

} 
```

### `include/utils/DatabaseManager.hpp`



```cpp
#pragma once

#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

#include "blockchain/Block.hpp"
#include "utils/AuditLog.hpp"

struct sqlite3;

namespace cw1 {

// Manages SQLite persistence for the blockchain.
// Uses RAII -- the constructor opens (or creates) the database and ensures
// tables exist; the destructor closes the connection.  Non-copyable, movable.
class DatabaseManager {
public:
    // Open (or create) a SQLite database at dbPath.
    explicit DatabaseManager(const std::string& dbPath);

    // Close the database connection.
    ~DatabaseManager();

    // Non-copyable.
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Movable.
    DatabaseManager(DatabaseManager&&) noexcept;
    DatabaseManager& operator=(DatabaseManager&&) noexcept;

    // Returns true if the database connection is open and usable.
    bool isOpen() const noexcept;

    // Returns the last error message from SQLite, or an empty string.
    std::string lastError() const;

    // -- Block persistence --

    // Save the entire chain vector to the blocks table (upsert by block_index).
    // Uses a transaction for performance.
    bool saveAllBlocks(const std::vector<Block>& chain);

    // Load all blocks from DB ordered by block_index.
    // Returns an empty vector on failure.
    std::vector<Block> loadAllBlocks();

    // Insert or replace a single block row (called after addBlock).
    bool upsertBlock(const Block& block);

    // Delete a block row by block_index, then renumber all rows whose
    // block_index is greater than the deleted index (called after hardDeleteBlock).
    bool deleteBlockRow(std::size_t index);

    // -- Audit log persistence --

    // Append a single audit entry to the audit_log table.
    bool insertAuditEntry(AuditAction action, const std::string& details,
                          const std::string& timestamp);

    // Load all audit entries ordered by rowid ascending.
    // Returns vector of tuples: {action string, details, timestamp}.
    std::vector<std::tuple<std::string, std::string, std::string>> loadAuditLog();

    // -- SQL search (returns matching block indices) --

    // Search blocks table WHERE vin / manufacturer / model / destination /
    // qc_notes LIKE %query% (case-insensitive).
    std::vector<std::size_t> searchBlocksSQL(const std::string& query);

    // Get all block indices for a specific VIN ordered by block_index.
    std::vector<std::size_t> getBlockIndicesByVin(const std::string& vin);

    // Get all distinct VINs stored in the database.
    std::vector<std::string> getAllVinsSQL();

    // -- Sync --

    // Drop and recreate all tables, then repopulate from the given chain and
    // audit log.  Used after hardDeleteBlock to keep DB consistent.
    bool fullResync(const std::vector<Block>& chain, const AuditLog& auditLog);

private:
    // Create the blocks and audit_log tables if they don't already exist.
    void createTables();

    // Execute a simple SQL statement that takes no parameters.
    bool execSQL(const std::string& sql);

    sqlite3* db_;
    std::string lastError_;
    std::string dbPath_;
};

} // namespace cw1
```

### `include/utils/HashUtil.hpp`



```cpp
#pragma once

#include <string>

namespace cw1::HashUtil {



std::string sha256(const std::string& input);

} 
```

### `include/utils/OperationTimer.hpp`



```cpp
#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

namespace cw1 {


class OperationTimer {
public:
    using Clock = std::chrono::steady_clock;

    OperationTimer() : start_(Clock::now()) {}

    double elapsedSeconds() const {
        const auto elapsed = Clock::now() - start_;
        return std::chrono::duration<double>(elapsed).count();
    }

private:
    Clock::time_point start_;
};

template <typename Callable>
double measureSeconds(Callable&& callable) {
    OperationTimer timer;
    std::forward<Callable>(callable)();
    return timer.elapsedSeconds();
}

inline std::string formatSeconds(double seconds) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4) << seconds;
    return out.str();
}

} 
```

### `include/utils/TimeUtil.hpp`



```cpp
#pragma once

#include <string>

namespace cw1::TimeUtil {


std::string nowIso8601();

} 
```

### `src/blockchain/Block.cpp`



```cpp
#include "blockchain/Block.hpp"

#include <random>
#include <sstream>
#include <utility>

#include "utils/HashUtil.hpp"
#include "utils/TimeUtil.hpp"

namespace cw1 {

Block::Block(std::size_t index, std::string previousHash, CarRecord record)
    : index_(index),
      previousHash_(std::move(previousHash)),
      timestamp_(TimeUtil::nowIso8601()),
      nonce_(generateNonce()),
      record_(std::move(record)) {
    currentHash_ = computeHash();

    
    
    
    
    
    if (index_ == 0 && previousHash_ == "0") {
        previousHash_ = currentHash_;
    }
}

Block::Block(std::size_t index,
             std::string previousHash,
             std::string currentHash,
             std::string timestamp,
             std::uint64_t nonce,
             CarRecord record)
    : index_(index),
      currentHash_(std::move(currentHash)),
      previousHash_(std::move(previousHash)),
      timestamp_(std::move(timestamp)),
      nonce_(nonce),
      record_(std::move(record)) {}

std::size_t Block::getIndex() const noexcept {
    return index_;
}

const std::string& Block::getCurrentHash() const noexcept {
    return currentHash_;
}

const std::string& Block::getPreviousHash() const noexcept {
    return previousHash_;
}

const std::string& Block::getTimestamp() const noexcept {
    return timestamp_;
}

std::uint64_t Block::getNonce() const noexcept {
    return nonce_;
}

const CarRecord& Block::getRecord() const noexcept {
    return record_;
}

std::string Block::computeHash() const {
    std::ostringstream payload;
    
    
    
    
    
    const std::string prevForHash = (index_ == 0) ? std::string("0") : previousHash_;
    payload << index_ << prevForHash << timestamp_ << nonce_ << record_.serialize();
    return HashUtil::sha256(payload.str());
}

std::string Block::toString() const {
    std::ostringstream out;
    out << "Block(index=" << index_
        << ", hash=" << currentHash_.substr(0, 12) << "..."
        << ", prev=" << (previousHash_.size() > 12 ? previousHash_.substr(0, 12) + "..." : previousHash_)
        << ", stage=" << stageToString(record_.stage)
        << ", vin=" << record_.vin << ")";
    return out.str();
}

void Block::debugOverrideCurrentHash(std::string forgedHash) {
    currentHash_ = std::move(forgedHash);
}

void Block::debugTamperPayloadForSimulation(const std::string& marker) {
    
    record_.destination = marker;
}

void Block::setIndex(std::size_t newIndex) {
    index_ = newIndex;
}

void Block::setPreviousHash(std::string newPrev) {
    previousHash_ = std::move(newPrev);
}

void Block::setRecord(CarRecord newRecord) {
    record_ = std::move(newRecord);
}

std::uint64_t Block::generateNonce() {
    static thread_local std::mt19937_64 engine{std::random_device{}()};
    static thread_local std::uniform_int_distribution<std::uint64_t> dist;
    return dist(engine);
}

} 
```

### `src/blockchain/Blockchain.cpp`



```cpp
#include "blockchain/Blockchain.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "utils/TimeUtil.hpp"

namespace cw1 {

namespace {


bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;

    auto toLower = [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    };

    std::string h, n;
    h.resize(haystack.size());
    n.resize(needle.size());
    std::transform(haystack.begin(), haystack.end(), h.begin(), toLower);
    std::transform(needle.begin(), needle.end(), n.begin(), toLower);

    return h.find(n) != std::string::npos;
}

} 



void Blockchain::addBlock(const CarRecord& record) {
    
    const std::string prevHash =
        chain_.empty() ? "0" : chain_.back().getCurrentHash();

    
    chain_.emplace_back(chain_.size(), prevHash, record);

    
    vinIndex_[record.vin].push_back(chain_.size() - 1);


    const Block& added = chain_.back();
    std::ostringstream detail;
    detail << "Added block #" << added.getIndex()
           << " for " << record.vin
           << " (" << stageToString(record.stage) << ")";
    auditLog_.log(AuditAction::BLOCK_ADDED, detail.str());

    // Mirror to SQLite if a database is open.
    if (db_ && db_->isOpen()) {
        db_->upsertBlock(chain_.back());
        db_->insertAuditEntry(AuditAction::BLOCK_ADDED, detail.str(),
                              TimeUtil::nowIso8601());
    }
}

const std::vector<Block>& Blockchain::getChain() const noexcept {
    return chain_;
}

std::size_t Blockchain::totalBlocks() const noexcept {
    return chain_.size();
}



std::vector<const Block*> Blockchain::getCarHistory(const std::string& vin) const {
    std::vector<const Block*> history;
    auto it = vinIndex_.find(vin);
    if (it != vinIndex_.end()) {
        history.reserve(it->second.size());
        for (std::size_t idx : it->second) {
            history.push_back(&chain_[idx]);
        }
    }
    return history;
}

bool Blockchain::hasVin(const std::string& vin) const {
    return vinIndex_.count(vin) > 0;
}

std::vector<std::string> Blockchain::getAllVins() const {
    std::vector<std::string> vins;
    vins.reserve(vinIndex_.size());
    for (const auto& [vin, indices] : vinIndex_) {
        vins.push_back(vin);
    }
    return vins;
}

BlockStage Blockchain::getLatestStage(const std::string& vin) const {
    auto it = vinIndex_.find(vin);
    if (it == vinIndex_.end() || it->second.empty()) {
        throw std::runtime_error("VIN not found: " + vin);
    }
    
    return chain_[it->second.back()].getRecord().stage;
}



std::vector<const Block*> Blockchain::searchByBrand(const std::string& brand) const {
    std::vector<const Block*> results;
    for (const auto& block : chain_) {
        if (containsIgnoreCase(block.getRecord().manufacturer, brand)) {
            results.push_back(&block);
        }
    }
    
    std::ostringstream detail;
    detail << "searchByBrand(\"" << brand << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

std::vector<const Block*> Blockchain::searchByStage(BlockStage stage) const {
    std::vector<const Block*> results;
    for (const auto& block : chain_) {
        if (block.getRecord().stage == stage) {
            results.push_back(&block);
        }
    }
    
    std::ostringstream detail;
    detail << "searchByStage(" << stageToString(stage) << ") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

std::vector<const Block*> Blockchain::searchGeneral(const std::string& query) const {
    std::vector<const Block*> results;
    for (const auto& block : chain_) {
        const CarRecord& rec = block.getRecord();
        if (containsIgnoreCase(rec.vin, query) ||
            containsIgnoreCase(rec.manufacturer, query) ||
            containsIgnoreCase(rec.model, query) ||
            containsIgnoreCase(rec.color, query) ||
            containsIgnoreCase(stageToString(rec.stage), query)) {
            results.push_back(&block);
        }
    }
    
    std::ostringstream detail;
    detail << "searchGeneral(\"" << query << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}



ValidationResult Blockchain::verifyIntegrity() const {
    ValidationResult result = Validation::verifyChain(chain_);

    std::ostringstream detail;
    detail << "verifyIntegrity() -> " << (result.ok ? "PASS" : "FAIL")
           << ": " << result.message;
    auditLog_.log(AuditAction::INTEGRITY_CHECK, detail.str());

    // Mirror to SQLite if a database is open.
    if (db_ && db_->isOpen()) {
        db_->insertAuditEntry(AuditAction::INTEGRITY_CHECK, detail.str(),
                              TimeUtil::nowIso8601());
    }
    return result;
}

void Blockchain::tamperBlock(std::size_t index) {
    if (index >= chain_.size()) {
        std::ostringstream detail;
        detail << "Debug / Simulation Feature: tamperBlock(" << index
               << ") failed (index out of range).";
        auditLog_.log(AuditAction::TAMPER_SIMULATED, detail.str());
        if (db_ && db_->isOpen()) {
            db_->insertAuditEntry(AuditAction::TAMPER_SIMULATED, detail.str(),
                                  TimeUtil::nowIso8601());
        }
        return;
    }

    chain_[index].debugTamperPayloadForSimulation("Tampered Location");

    std::ostringstream detail;
    detail << "Debug / Simulation Feature: payload tampered for block #"
           << index << " (destination set to \"Tampered Location\").";
    auditLog_.log(AuditAction::TAMPER_SIMULATED, detail.str());

    // Mirror to SQLite if a database is open.
    if (db_ && db_->isOpen()) {
        db_->insertAuditEntry(AuditAction::TAMPER_SIMULATED, detail.str(),
                              TimeUtil::nowIso8601());
    }
}

bool Blockchain::tamperBlockHash(std::size_t index,
                                 const std::string& forgedHash,
                                 std::string& message) {
    if (index >= chain_.size()) {
        std::ostringstream err;
        err << "Tamper failed: block index " << index
            << " out of range (0.." << (chain_.empty() ? 0 : chain_.size() - 1) << ").";
        message = err.str();
        return false;
    }

    std::string finalHash = forgedHash;
    if (finalHash.empty()) {
        finalHash = chain_[index].getCurrentHash();
        if (finalHash.empty()) {
            finalHash = "tampered_hash";
        } else {
            finalHash[0] = (finalHash[0] == '0') ? '1' : '0';
        }
    }

    if (finalHash == chain_[index].getCurrentHash()) {
        finalHash.push_back('x');
    }

    chain_[index].debugOverrideCurrentHash(finalHash);

    std::ostringstream ok;
    ok << "Tampered block #" << index
       << ". Stored currentHash overridden for integrity-failure demo.";
    message = ok.str();
    auditLog_.log(AuditAction::TAMPER_SIMULATED, message);
    return true;
}



bool Blockchain::softDeleteBlock(std::size_t index, std::string& outMessage) {
    if (index >= chain_.size()) {
        std::ostringstream err;
        err << "Soft delete failed: block index " << index
            << " out of range (0.." << (chain_.empty() ? 0 : chain_.size() - 1) << ").";
        outMessage = err.str();
        return false;
    }

    const std::string originalVin = chain_[index].getRecord().vin;

    
    CarRecord tombstone;
    tombstone.vin            = originalVin;
    tombstone.manufacturer   = "[DELETED]";
    tombstone.model          = "[DELETED]";
    tombstone.color          = "";
    tombstone.productionYear = 0;
    tombstone.stage          = BlockStage::PRODUCTION;
    tombstone.passed         = false;
    tombstone.salePrice      = 0.0;
    tombstone.manufacturerId = "";
    tombstone.factoryLocation   = "";
    tombstone.warehouseLocation = "";
    tombstone.receivedBy        = "";
    tombstone.supplierId        = "";
    tombstone.inspectorId       = "";
    tombstone.qcNotes           = "";
    tombstone.dealerId          = "";
    tombstone.destination       = "";
    tombstone.transportMode     = "";
    tombstone.buyerId           = "";
    tombstone.retailerId        = "";
    tombstone.warrantyExpiry    = "";

    chain_[index].setRecord(std::move(tombstone));

    std::ostringstream detail;
    detail << "Soft delete block #" << index
           << " for VIN " << originalVin
           << " (tombstone applied, hash now invalid)";
    outMessage = detail.str();
    auditLog_.log(AuditAction::BLOCK_DELETED, outMessage);

    // Mirror to SQLite if a database is open.
    if (db_ && db_->isOpen()) {
        db_->upsertBlock(chain_[index]);
        db_->insertAuditEntry(AuditAction::BLOCK_DELETED, outMessage,
                              TimeUtil::nowIso8601());
    }
    return true;
}

bool Blockchain::hardDeleteBlock(std::size_t index, std::string& outMessage) {
    if (index >= chain_.size()) {
        std::ostringstream err;
        err << "Hard delete failed: block index " << index
            << " out of range (0.." << (chain_.empty() ? 0 : chain_.size() - 1) << ").";
        outMessage = err.str();
        return false;
    }

    if (chain_.size() == 1) {
        outMessage = "Hard delete failed: cannot delete the only remaining block.";
        return false;
    }

    const std::string deletedVin = chain_[index].getRecord().vin;
    const std::size_t cascadeStart = index;

    
    chain_.erase(chain_.begin() + static_cast<std::ptrdiff_t>(index));

    
    for (std::size_t i = cascadeStart; i < chain_.size(); ++i) {
        chain_[i].setIndex(i);

        if (i == 0) {
            
            
            chain_[i].setPreviousHash("0");
            chain_[i].debugOverrideCurrentHash(chain_[i].computeHash());
            chain_[i].setPreviousHash(chain_[i].getCurrentHash());
        } else {
            chain_[i].setPreviousHash(chain_[i - 1].getCurrentHash());
            chain_[i].debugOverrideCurrentHash(chain_[i].computeHash());
        }
    }

    
    vinIndex_.clear();
    for (std::size_t i = 0; i < chain_.size(); ++i) {
        vinIndex_[chain_[i].getRecord().vin].push_back(i);
    }

    const std::size_t cascadeCount = chain_.size() - cascadeStart;
    std::ostringstream detail;
    detail << "Hard delete block #" << index
           << " (VIN: " << deletedVin
           << ") -- cascade re-hash applied to " << cascadeCount
           << " subsequent blocks. Chain length now: " << chain_.size();
    outMessage = detail.str();
    auditLog_.log(AuditAction::BLOCK_DELETED, outMessage);

    // Full resync to SQLite if a database is open.
    if (db_ && db_->isOpen()) {
        db_->fullResync(chain_, auditLog_);
    }
    return true;
}

bool Blockchain::saveBlockchain(const std::string& path) const {
    std::ofstream out(path);
    if (!out) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "saveBlockchain failed: cannot open file " + path);
        return false;
    }

    out << "CW1_BLOCKCHAIN_V1\n";
    out << chain_.size() << '\n';

    for (const auto& block : chain_) {
        const CarRecord& r = block.getRecord();
        out << block.getIndex() << '\t'
            << std::quoted(block.getPreviousHash()) << '\t'
            << std::quoted(block.getCurrentHash()) << '\t'
            << std::quoted(block.getTimestamp()) << '\t'
            << block.getNonce() << '\t'
            << std::quoted(r.vin) << '\t'
            << std::quoted(r.manufacturer) << '\t'
            << std::quoted(r.model) << '\t'
            << std::quoted(r.color) << '\t'
            << r.productionYear << '\t'
            << static_cast<int>(r.stage) << '\t'
            << std::quoted(r.factoryLocation) << '\t'
            << std::quoted(r.warehouseLocation) << '\t'
            << std::quoted(r.receivedBy) << '\t'
            << std::quoted(r.inspectorId) << '\t'
            << (r.passed ? 1 : 0) << '\t'
            << std::quoted(r.qcNotes) << '\t'
            << std::quoted(r.dealerId) << '\t'
            << std::quoted(r.destination) << '\t'
            << std::quoted(r.transportMode) << '\t'
            << std::quoted(r.buyerId) << '\t'
            << r.salePrice << '\t'
            << std::quoted(r.warrantyExpiry)
            << '\t' << std::quoted(r.manufacturerId)
            << '\t' << std::quoted(r.supplierId)
            << '\t' << std::quoted(r.retailerId) << '\n';

    }
    if (!out.good()) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "saveBlockchain failed while writing file " + path);
        return false;
    }

    auditLog_.log(AuditAction::PERSISTENCE_IO,
                  "saveBlockchain succeeded: " + path);
    return true;
}

bool Blockchain::loadBlockchain(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "loadBlockchain failed: cannot open file " + path);
        return false;
    }

    std::string header;
    if (!std::getline(in, header) || header != "CW1_BLOCKCHAIN_V1") {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "loadBlockchain failed: invalid file header in " + path);
        return false;
    }

    std::string countLine;
    if (!std::getline(in, countLine)) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "loadBlockchain failed: missing block count in " + path);
        return false;
    }

    std::size_t expectedCount = 0;
    try {
        expectedCount = static_cast<std::size_t>(std::stoull(countLine));
    } catch (...) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "loadBlockchain failed: invalid block count in " + path);
        return false;
    }

    std::vector<Block> loadedChain;
    loadedChain.reserve(expectedCount);
    std::map<std::string, std::vector<std::size_t>> loadedIndex;

    std::string line;
    std::size_t lineNo = 2;
    while (std::getline(in, line)) {
        ++lineNo;
        if (line.empty()) {
            continue;
        }

        std::istringstream row(line);
        std::size_t index = 0;
        std::string previousHash;
        std::string currentHash;
        std::string timestamp;
        std::uint64_t nonce = 0;
        CarRecord record;
        int stageInt = 0;
        int passedInt = 0;

        if (!(row >> index
                  >> std::quoted(previousHash)
                  >> std::quoted(currentHash)
                  >> std::quoted(timestamp)
                  >> nonce
                  >> std::quoted(record.vin)
                  >> std::quoted(record.manufacturer)
                  >> std::quoted(record.model)
                  >> std::quoted(record.color)
                  >> record.productionYear
                  >> stageInt
                  >> std::quoted(record.factoryLocation)
                  >> std::quoted(record.warehouseLocation)
                  >> std::quoted(record.receivedBy)
                  >> std::quoted(record.inspectorId)
                  >> passedInt
                  >> std::quoted(record.qcNotes)
                  >> std::quoted(record.dealerId)
                  >> std::quoted(record.destination)
                  >> std::quoted(record.transportMode)
                  >> std::quoted(record.buyerId)
                  >> record.salePrice
                  >> std::quoted(record.warrantyExpiry)
                  >> std::quoted(record.manufacturerId)
                  >> std::quoted(record.supplierId)
                  >> std::quoted(record.retailerId))) {
            std::ostringstream detail;
            detail << "loadBlockchain failed: parse error at line " << lineNo;
            auditLog_.log(AuditAction::PERSISTENCE_IO, detail.str());
            return false;
        }

        if (stageInt < static_cast<int>(BlockStage::PRODUCTION) ||
            stageInt > static_cast<int>(BlockStage::CUSTOMER_SALE)) {
            std::ostringstream detail;
            detail << "loadBlockchain failed: invalid stage value at line " << lineNo;
            auditLog_.log(AuditAction::PERSISTENCE_IO, detail.str());
            return false;
        }
        record.stage = static_cast<BlockStage>(stageInt);
        record.passed = (passedInt != 0);

        loadedChain.emplace_back(index,
                                 std::move(previousHash),
                                 std::move(currentHash),
                                 std::move(timestamp),
                                 nonce,
                                 record);
        loadedIndex[record.vin].push_back(loadedChain.size() - 1);
    }

    if (loadedChain.size() != expectedCount) {
        std::ostringstream detail;
        detail << "loadBlockchain failed: expected " << expectedCount
               << " blocks, parsed " << loadedChain.size() << ".";
        auditLog_.log(AuditAction::PERSISTENCE_IO, detail.str());
        return false;
    }

    const ValidationResult loadedCheck = Validation::verifyChain(loadedChain);
    if (!loadedCheck.ok) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "loadBlockchain failed integrity validation: " + loadedCheck.message);
        return false;
    }

    chain_ = std::move(loadedChain);
    vinIndex_ = std::move(loadedIndex);
    auditLog_.log(AuditAction::PERSISTENCE_IO,
                  "loadBlockchain succeeded: " + path);
    return true;
}



AuditLog& Blockchain::getAuditLog() const noexcept {
    
    
    
    return auditLog_;
}

AuditLog& Blockchain::getAuditLog() noexcept {
    return auditLog_;
}

// ---------------------------------------------------------------------------
// SQLite database integration
// ---------------------------------------------------------------------------

bool Blockchain::openDatabase(const std::string& dbPath) {
    db_ = std::make_unique<DatabaseManager>(dbPath);
    if (!db_->isOpen()) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "openDatabase failed: " + db_->lastError());
        db_.reset();
        return false;
    }
    auditLog_.log(AuditAction::PERSISTENCE_IO,
                  "openDatabase succeeded: " + dbPath);
    return true;
}

bool Blockchain::isDatabaseOpen() const noexcept {
    return db_ && db_->isOpen();
}

bool Blockchain::saveToDB() {
    if (!isDatabaseOpen()) return false;

    bool ok = db_->fullResync(chain_, auditLog_);
    std::string msg = ok ? "saveToDB succeeded"
                         : ("saveToDB failed: " + db_->lastError());
    auditLog_.log(AuditAction::PERSISTENCE_IO, msg);
    if (isDatabaseOpen()) {
        db_->insertAuditEntry(AuditAction::PERSISTENCE_IO, msg,
                              TimeUtil::nowIso8601());
    }
    return ok;
}

bool Blockchain::loadFromDB() {
    if (!isDatabaseOpen()) return false;

    std::vector<Block> loaded = db_->loadAllBlocks();
    if (loaded.empty() && !db_->lastError().empty()) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "loadFromDB failed: " + db_->lastError());
        return false;
    }

    // Rebuild vinIndex from loaded blocks.
    std::map<std::string, std::vector<std::size_t>> loadedIndex;
    for (std::size_t i = 0; i < loaded.size(); ++i) {
        loadedIndex[loaded[i].getRecord().vin].push_back(i);
    }

    chain_ = std::move(loaded);
    vinIndex_ = std::move(loadedIndex);

    // Verify integrity after loading.
    ValidationResult result = Validation::verifyChain(chain_);
    auditLog_.log(AuditAction::PERSISTENCE_IO,
                  "loadFromDB: " + std::string(result.ok ? "PASS" : "FAIL") +
                  " - " + result.message);
    return result.ok;
}

std::vector<const Block*> Blockchain::searchBySQL(const std::string& query) const {
    // Fall back to searchGeneral() if no DB is open.
    if (!isDatabaseOpen()) {
        return searchGeneral(query);
    }

    std::vector<std::size_t> indices = db_->searchBlocksSQL(query);
    std::vector<const Block*> results;
    results.reserve(indices.size());
    for (std::size_t idx : indices) {
        if (idx < chain_.size()) {
            results.push_back(&chain_[idx]);
        }
    }

    std::ostringstream detail;
    detail << "searchBySQL(\"" << query << "\") -> " << results.size()
           << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    if (isDatabaseOpen()) {
        db_->insertAuditEntry(AuditAction::SEARCH_PERFORMED, detail.str(),
                              TimeUtil::nowIso8601());
    }
    return results;
}

const DatabaseManager* Blockchain::getDatabase() const noexcept {
    return db_.get();
}

}
```

### `src/blockchain/BlockStage.cpp`



```cpp
#include "blockchain/BlockStage.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace cw1 {

std::string stageToString(BlockStage stage) {
    switch (stage) {
        case BlockStage::PRODUCTION:       return "Production";
        case BlockStage::WAREHOUSE_INTAKE: return "Warehouse Intake";
        case BlockStage::QUALITY_CHECK:    return "Quality Check";
        case BlockStage::DEALER_DISPATCH:  return "Dealer Dispatch";
        case BlockStage::CUSTOMER_SALE:    return "Customer Sale";
    }
    return "Unknown";
}

BlockStage stringToStage(const std::string& text) {
    
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lower == "production")       return BlockStage::PRODUCTION;
    if (lower == "warehouse intake") return BlockStage::WAREHOUSE_INTAKE;
    if (lower == "quality check")    return BlockStage::QUALITY_CHECK;
    if (lower == "dealer dispatch")  return BlockStage::DEALER_DISPATCH;
    if (lower == "customer sale")    return BlockStage::CUSTOMER_SALE;

    throw std::invalid_argument("Unknown stage: " + text);
}

} 
```

### `src/blockchain/Validation.cpp`



```cpp
#include "blockchain/Validation.hpp"

namespace cw1::Validation {

ValidationResult verifyChain(const std::vector<Block>& blocks) {
    if (blocks.empty()) {
        return {true, "Blockchain is empty and therefore valid."};
    }

    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const Block& current = blocks[i];

        
        if (current.getCurrentHash() != current.computeHash()) {
            return {false, "Hash mismatch at block index " + std::to_string(i)};
        }

        if (i == 0) {
            
            
            
            if (current.getPreviousHash() != current.getCurrentHash()) {
                return {false, "Genesis block previousHash must equal its currentHash."};
            }
            continue;
        }

        
        const Block& previous = blocks[i - 1];
        if (current.getPreviousHash() != previous.getCurrentHash()) {
            return {false, "Chain link mismatch at block index " + std::to_string(i)};
        }
    }

    return {true, "Blockchain integrity verified. " + std::to_string(blocks.size()) + " block(s) OK."};
}

} 
```

### `src/models/CarRecord.cpp`



```cpp
#include "models/CarRecord.hpp"

#include <sstream>

namespace cw1 {

namespace {


std::string escapeField(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        if (ch == '\\' || ch == '|') {
            escaped.push_back('\\');
        }
        escaped.push_back(ch);
    }
    return escaped;
}

} 

std::string CarRecord::serialize() const {
    std::ostringstream out;

    
    out << "vin=" << escapeField(vin)
        << "|manufacturer=" << escapeField(manufacturer)
        << "|model=" << escapeField(model)
        << "|color=" << escapeField(color)
        << "|productionYear=" << productionYear
        << "|stage=" << escapeField(stageToString(stage))
        << "|manufacturerId=" << escapeField(manufacturerId);

    
    
    out << "|factoryLocation=" << escapeField(factoryLocation)
        << "|warehouseLocation=" << escapeField(warehouseLocation)
        << "|receivedBy=" << escapeField(receivedBy)
        << "|supplierId=" << escapeField(supplierId)
        << "|inspectorId=" << escapeField(inspectorId)
        << "|passed=" << (passed ? "true" : "false")
        << "|qcNotes=" << escapeField(qcNotes)
        << "|dealerId=" << escapeField(dealerId)
        << "|destination=" << escapeField(destination)
        << "|transportMode=" << escapeField(transportMode)
        << "|buyerId=" << escapeField(buyerId)
        << "|retailerId=" << escapeField(retailerId)
        << "|salePrice=" << salePrice
        << "|warrantyExpiry=" << escapeField(warrantyExpiry);

    return out.str();
}

} 
```

### `src/utils/AuditLog.cpp`



```cpp









#include "utils/AuditLog.hpp"
#include "utils/TimeUtil.hpp"

#include <algorithm>  

namespace cw1 {



std::string actionToString(AuditAction action) {
    switch (action) {
        case AuditAction::BLOCK_ADDED:       return "BLOCK_ADDED";
        case AuditAction::INTEGRITY_CHECK:   return "INTEGRITY_CHECK";
        case AuditAction::SEARCH_PERFORMED:  return "SEARCH_PERFORMED";
        case AuditAction::CHAIN_VIEWED:      return "CHAIN_VIEWED";
        case AuditAction::TAMPER_SIMULATED:  return "TAMPER_SIMULATED";
        case AuditAction::PERSISTENCE_IO:    return "PERSISTENCE_IO";
        case AuditAction::BLOCK_DELETED:     return "BLOCK_DELETED";
        default:                             return "UNKNOWN";
    }
}



AuditLog::AuditLog()
    : head_(nullptr), tail_(nullptr), count_(0) {
    
}

AuditLog::~AuditLog() {
    
    clear();
}



void AuditLog::log(AuditAction action, const std::string& details) {
    
    AuditEntry* node = new AuditEntry(action, details, TimeUtil::nowIso8601());

    if (tail_ == nullptr) {
        
        head_ = node;
        tail_ = node;
    } else {
        
        
        tail_->next = node;  
        tail_ = node;        
    }

    ++count_;
}

std::size_t AuditLog::size() const noexcept {
    return count_;
}

const AuditEntry* AuditLog::head() const noexcept {
    return head_;
}

void AuditLog::clear() {
    
    
    AuditEntry* current = head_;
    while (current != nullptr) {
        AuditEntry* next = current->next;  
        delete current;                    
        current = next;                    
    }
    head_  = nullptr;
    tail_  = nullptr;
    count_ = 0;
}



const AuditEntry** AuditLog::getRecentEntries(std::size_t maxCount,
                                              std::size_t& outCount) const {
    
    
    
    

    
    std::size_t actualCount = (maxCount < count_) ? maxCount : count_;
    outCount = actualCount;

    if (actualCount == 0) {
        return nullptr;
    }

    
    
    const AuditEntry** arr = new const AuditEntry*[actualCount];

    
    
    
    std::size_t skipCount = count_ - actualCount;
    const AuditEntry* node = head_;
    for (std::size_t i = 0; i < skipCount; ++i) {
        node = node->next;  
    }

    
    for (std::size_t i = 0; i < actualCount; ++i) {
        arr[i] = node;      
        node = node->next;  
    }

    return arr;  
}

RecentEntryArray::RecentEntryArray(const AuditLog& log, std::size_t maxCount)
    : data_(nullptr), count_(0) {
    data_ = log.getRecentEntries(maxCount, count_);
}

RecentEntryArray::~RecentEntryArray() {
    delete[] data_;
}

RecentEntryArray::RecentEntryArray(RecentEntryArray&& other) noexcept
    : data_(other.data_), count_(other.count_) {
    other.data_ = nullptr;
    other.count_ = 0;
}

RecentEntryArray& RecentEntryArray::operator=(RecentEntryArray&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    delete[] data_;
    data_ = other.data_;
    count_ = other.count_;
    other.data_ = nullptr;
    other.count_ = 0;
    return *this;
}

std::size_t RecentEntryArray::size() const noexcept {
    return count_;
}

bool RecentEntryArray::empty() const noexcept {
    return count_ == 0;
}

const AuditEntry* RecentEntryArray::operator[](std::size_t index) const noexcept {
    if (data_ == nullptr || index >= count_) {
        return nullptr;
    }
    return data_[index];
}

const AuditEntry* const* RecentEntryArray::begin() const noexcept {
    return data_;
}

const AuditEntry* const* RecentEntryArray::end() const noexcept {
    return data_ == nullptr ? nullptr : (data_ + count_);
}

} 
```

### `src/utils/DatabaseManager.cpp`



```cpp
#include "utils/DatabaseManager.hpp"

#include <sqlite3.h>

#include <algorithm>
#include <utility>

namespace cw1 {

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

DatabaseManager::DatabaseManager(const std::string& dbPath)
    : db_(nullptr), dbPath_(dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = nullptr;
        return;
    }
    createTables();
}

DatabaseManager::~DatabaseManager() {
    if (db_ != nullptr) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

DatabaseManager::DatabaseManager(DatabaseManager&& other) noexcept
    : db_(other.db_),
      lastError_(std::move(other.lastError_)),
      dbPath_(std::move(other.dbPath_)) {
    other.db_ = nullptr;
}

DatabaseManager& DatabaseManager::operator=(DatabaseManager&& other) noexcept {
    if (this != &other) {
        if (db_ != nullptr) {
            sqlite3_close(db_);
        }
        db_ = other.db_;
        lastError_ = std::move(other.lastError_);
        dbPath_ = std::move(other.dbPath_);
        other.db_ = nullptr;
    }
    return *this;
}

// ---------------------------------------------------------------------------
// Status helpers
// ---------------------------------------------------------------------------

bool DatabaseManager::isOpen() const noexcept {
    return db_ != nullptr;
}

std::string DatabaseManager::lastError() const {
    return lastError_;
}

// ---------------------------------------------------------------------------
// Table creation
// ---------------------------------------------------------------------------

void DatabaseManager::createTables() {
    const char* blocksSQL =
        "CREATE TABLE IF NOT EXISTS blocks ("
        "    block_index     INTEGER PRIMARY KEY,"
        "    current_hash    TEXT NOT NULL,"
        "    previous_hash   TEXT NOT NULL,"
        "    timestamp       TEXT NOT NULL,"
        "    nonce           INTEGER NOT NULL,"
        "    vin             TEXT NOT NULL,"
        "    manufacturer    TEXT NOT NULL,"
        "    model           TEXT NOT NULL,"
        "    color           TEXT NOT NULL,"
        "    production_year INTEGER NOT NULL,"
        "    stage           INTEGER NOT NULL,"
        "    factory_location    TEXT,"
        "    warehouse_location  TEXT,"
        "    received_by         TEXT,"
        "    supplier_id         TEXT,"
        "    inspector_id        TEXT,"
        "    passed              INTEGER,"
        "    qc_notes            TEXT,"
        "    dealer_id           TEXT,"
        "    destination         TEXT,"
        "    transport_mode      TEXT,"
        "    buyer_id            TEXT,"
        "    retailer_id         TEXT,"
        "    sale_price          REAL,"
        "    warranty_expiry     TEXT,"
        "    manufacturer_id     TEXT,"
        "    is_tombstone        INTEGER NOT NULL DEFAULT 0"
        ")";

    const char* auditSQL =
        "CREATE TABLE IF NOT EXISTS audit_log ("
        "    id        INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    action    TEXT NOT NULL,"
        "    details   TEXT NOT NULL,"
        "    timestamp TEXT NOT NULL"
        ")";

    execSQL(blocksSQL);
    execSQL(auditSQL);
}

bool DatabaseManager::execSQL(const std::string& sql) {
    if (db_ == nullptr) {
        lastError_ = "Database not open";
        return false;
    }
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        lastError_ = (errMsg != nullptr) ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Block persistence
// ---------------------------------------------------------------------------

// Helper: bind all block columns to a prepared INSERT OR REPLACE statement.
static bool bindBlockToStmt(sqlite3_stmt* stmt, const Block& block) {
    const CarRecord& r = block.getRecord();

    sqlite3_bind_int64(stmt,  1, static_cast<sqlite3_int64>(block.getIndex()));
    sqlite3_bind_text(stmt,   2, block.getCurrentHash().c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   3, block.getPreviousHash().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   4, block.getTimestamp().c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt,  5, static_cast<sqlite3_int64>(block.getNonce()));
    sqlite3_bind_text(stmt,   6, r.vin.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   7, r.manufacturer.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   8, r.model.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   9, r.color.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,   10, r.productionYear);
    sqlite3_bind_int(stmt,   11, static_cast<int>(r.stage));
    sqlite3_bind_text(stmt,  12, r.factoryLocation.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  13, r.warehouseLocation.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  14, r.receivedBy.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  15, r.supplierId.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  16, r.inspectorId.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,   17, r.passed ? 1 : 0);
    sqlite3_bind_text(stmt,  18, r.qcNotes.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  19, r.dealerId.c_str(),           -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  20, r.destination.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  21, r.transportMode.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  22, r.buyerId.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  23, r.retailerId.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt,24, r.salePrice);
    sqlite3_bind_text(stmt,  25, r.warrantyExpiry.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  26, r.manufacturerId.c_str(),     -1, SQLITE_TRANSIENT);
    // is_tombstone: true when manufacturer is "[DELETED]"
    int isTombstone = (r.manufacturer == "[DELETED]") ? 1 : 0;
    sqlite3_bind_int(stmt,   27, isTombstone);

    return true;
}

static const char* k_upsertSQL =
    "INSERT OR REPLACE INTO blocks ("
    "  block_index, current_hash, previous_hash, timestamp, nonce,"
    "  vin, manufacturer, model, color, production_year, stage,"
    "  factory_location, warehouse_location, received_by, supplier_id,"
    "  inspector_id, passed, qc_notes, dealer_id, destination,"
    "  transport_mode, buyer_id, retailer_id, sale_price,"
    "  warranty_expiry, manufacturer_id, is_tombstone"
    ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

bool DatabaseManager::saveAllBlocks(const std::vector<Block>& chain) {
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    if (!execSQL("BEGIN TRANSACTION")) return false;

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, k_upsertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        execSQL("ROLLBACK");
        return false;
    }

    for (const auto& block : chain) {
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
        bindBlockToStmt(stmt, block);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            lastError_ = sqlite3_errmsg(db_);
            sqlite3_finalize(stmt);
            execSQL("ROLLBACK");
            return false;
        }
    }

    sqlite3_finalize(stmt);
    return execSQL("COMMIT");
}

std::vector<Block> DatabaseManager::loadAllBlocks() {
    std::vector<Block> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql = "SELECT * FROM blocks ORDER BY block_index ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        auto getInt    = [&](int col) { return sqlite3_column_int(stmt, col); };
        auto getInt64  = [&](int col) { return sqlite3_column_int64(stmt, col); };
        auto getText   = [&](int col) -> std::string {
            const unsigned char* t = sqlite3_column_text(stmt, col);
            return (t != nullptr) ? std::string(reinterpret_cast<const char*>(t)) : std::string();
        };
        auto getDouble = [&](int col) { return sqlite3_column_double(stmt, col); };

        std::size_t   index        = static_cast<std::size_t>(getInt64(0));
        std::string   currentHash  = getText(1);
        std::string   previousHash = getText(2);
        std::string   timestamp    = getText(3);
        std::uint64_t nonce        = static_cast<std::uint64_t>(getInt64(4));

        CarRecord rec;
        rec.vin              = getText(5);
        rec.manufacturer     = getText(6);
        rec.model            = getText(7);
        rec.color            = getText(8);
        rec.productionYear   = getInt(9);
        int stageInt         = getInt(10);
        if (stageInt >= static_cast<int>(BlockStage::PRODUCTION) &&
            stageInt <= static_cast<int>(BlockStage::CUSTOMER_SALE)) {
            rec.stage = static_cast<BlockStage>(stageInt);
        }
        rec.factoryLocation    = getText(11);
        rec.warehouseLocation  = getText(12);
        rec.receivedBy         = getText(13);
        rec.supplierId         = getText(14);
        rec.inspectorId        = getText(15);
        rec.passed             = (getInt(16) != 0);
        rec.qcNotes            = getText(17);
        rec.dealerId           = getText(18);
        rec.destination        = getText(19);
        rec.transportMode      = getText(20);
        rec.buyerId            = getText(21);
        rec.retailerId         = getText(22);
        rec.salePrice          = getDouble(23);
        rec.warrantyExpiry     = getText(24);
        rec.manufacturerId     = getText(25);
        // column 26 = is_tombstone (informational only, already reflected in manufacturer)

        result.emplace_back(index,
                            std::move(previousHash),
                            std::move(currentHash),
                            std::move(timestamp),
                            nonce,
                            std::move(rec));
    }

    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        result.clear();
    }

    sqlite3_finalize(stmt);
    return result;
}

bool DatabaseManager::upsertBlock(const Block& block) {
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, k_upsertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    bindBlockToStmt(stmt, block);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::deleteBlockRow(std::size_t index) {
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    if (!execSQL("BEGIN TRANSACTION")) return false;

    // Delete the target row.
    {
        const char* sql = "DELETE FROM blocks WHERE block_index = ?";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }
        sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(index));
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }
    }

    // Renumber all subsequent rows.
    {
        const char* sql = "UPDATE blocks SET block_index = block_index - 1 "
                          "WHERE block_index > ?";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }
        sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(index));
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }
    }

    return execSQL("COMMIT");
}

// ---------------------------------------------------------------------------
// Audit log persistence
// ---------------------------------------------------------------------------

bool DatabaseManager::insertAuditEntry(AuditAction action,
                                       const std::string& details,
                                       const std::string& timestamp) {
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    const char* sql =
        "INSERT INTO audit_log (action, details, timestamp) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    std::string actionStr = actionToString(action);
    sqlite3_bind_text(stmt, 1, actionStr.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, details.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, timestamp.c_str(),  -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::vector<std::tuple<std::string, std::string, std::string>>
DatabaseManager::loadAuditLog() {
    std::vector<std::tuple<std::string, std::string, std::string>> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql = "SELECT action, details, timestamp FROM audit_log "
                      "ORDER BY id ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        auto getText = [&](int col) -> std::string {
            const unsigned char* t = sqlite3_column_text(stmt, col);
            return (t != nullptr) ? std::string(reinterpret_cast<const char*>(t))
                                  : std::string();
        };
        result.emplace_back(getText(0), getText(1), getText(2));
    }

    sqlite3_finalize(stmt);
    return result;
}

// ---------------------------------------------------------------------------
// SQL search
// ---------------------------------------------------------------------------

std::vector<std::size_t> DatabaseManager::searchBlocksSQL(const std::string& query) {
    std::vector<std::size_t> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql =
        "SELECT block_index FROM blocks WHERE "
        "LOWER(vin) LIKE LOWER(?) OR "
        "LOWER(manufacturer) LIKE LOWER(?) OR "
        "LOWER(model) LIKE LOWER(?) OR "
        "LOWER(destination) LIKE LOWER(?) OR "
        "LOWER(qc_notes) LIKE LOWER(?) "
        "ORDER BY block_index ASC";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    std::string pattern = "%" + query + "%";
    for (int i = 1; i <= 5; ++i) {
        sqlite3_bind_text(stmt, i, pattern.c_str(), -1, SQLITE_TRANSIENT);
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        result.push_back(
            static_cast<std::size_t>(sqlite3_column_int64(stmt, 0)));
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::size_t> DatabaseManager::getBlockIndicesByVin(const std::string& vin) {
    std::vector<std::size_t> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql =
        "SELECT block_index FROM blocks WHERE vin = ? ORDER BY block_index ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    sqlite3_bind_text(stmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        result.push_back(
            static_cast<std::size_t>(sqlite3_column_int64(stmt, 0)));
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::string> DatabaseManager::getAllVinsSQL() {
    std::vector<std::string> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql = "SELECT DISTINCT vin FROM blocks ORDER BY vin ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* t = sqlite3_column_text(stmt, 0);
        if (t != nullptr) {
            result.emplace_back(reinterpret_cast<const char*>(t));
        }
    }

    sqlite3_finalize(stmt);
    return result;
}

// ---------------------------------------------------------------------------
// Full resync
// ---------------------------------------------------------------------------

bool DatabaseManager::fullResync(const std::vector<Block>& chain,
                                 const AuditLog& auditLog) {
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    if (!execSQL("BEGIN TRANSACTION")) return false;
    if (!execSQL("DELETE FROM blocks"))    { execSQL("ROLLBACK"); return false; }
    if (!execSQL("DELETE FROM audit_log")) { execSQL("ROLLBACK"); return false; }

    // Re-insert all blocks.
    {
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, k_upsertSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }

        for (const auto& block : chain) {
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
            bindBlockToStmt(stmt, block);
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                lastError_ = sqlite3_errmsg(db_);
                sqlite3_finalize(stmt);
                execSQL("ROLLBACK");
                return false;
            }
        }
        sqlite3_finalize(stmt);
    }

    // Re-insert all audit entries by traversing the linked list.
    {
        const char* auditInsert =
            "INSERT INTO audit_log (action, details, timestamp) VALUES (?, ?, ?)";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, auditInsert, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }

        const AuditEntry* entry = auditLog.head();
        while (entry != nullptr) {
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);

            std::string actionStr = actionToString(entry->action);
            sqlite3_bind_text(stmt, 1, actionStr.c_str(),         -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, entry->details.c_str(),    -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, entry->timestamp.c_str(),  -1, SQLITE_TRANSIENT);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                lastError_ = sqlite3_errmsg(db_);
                sqlite3_finalize(stmt);
                execSQL("ROLLBACK");
                return false;
            }

            entry = entry->next;
        }
        sqlite3_finalize(stmt);
    }

    return execSQL("COMMIT");
}

} // namespace cw1
```

### `src/utils/HashUtil.cpp`



```cpp
#include "utils/HashUtil.hpp"

#include <iomanip>
#include <openssl/sha.h>
#include <sstream>

namespace cw1::HashUtil {

std::string sha256(const std::string& input) {
    unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

    SHA256(
        reinterpret_cast<const unsigned char*>(input.data()),
        input.size(),
        digest
    );

    std::ostringstream output;
    output << std::hex << std::setfill('0');

    for (unsigned char byte : digest) {
        output << std::setw(2) << static_cast<int>(byte);
    }

    return output.str();
}

} 
```

### `src/utils/TimeUtil.cpp`



```cpp
#include "utils/TimeUtil.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace cw1::TimeUtil {

std::string nowIso8601() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm utcTime = {};
#ifdef _WIN32
    gmtime_s(&utcTime, &currentTime);
#else
    gmtime_r(&currentTime, &utcTime);
#endif

    std::ostringstream output;
    output << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

} 
```

### `src/main_cli.cpp`



```cpp
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "blockchain/BlockFormatter.hpp"
#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"
#include "utils/OperationTimer.hpp"

namespace {

using std::cin;
using std::cout;
using std::fixed;
using std::getline;
using std::left;
using std::setprecision;
using std::setw;
using std::string;
using std::to_string;
using std::vector;

int parseIntOrDefault(const string& value, int fallback) {
    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}

double parseDoubleOrDefault(const string& value, double fallback) {
    try {
        return std::stod(value);
    } catch (...) {
        return fallback;
    }
}

string prompt(const string& label) {
    string value;
    cout << "  " << label << ": ";
    getline(cin, value);
    return value;
}

void printOperationDuration(double seconds) {
    cout << "  Operation took: " << cw1::formatSeconds(seconds) << " s\n";
}

void printSeparator() {
    cout << "  " << string(60, '-') << '\n';
}

void printBlockDetail(const cw1::Block& block) {
    cout << cw1::formatBlockForDisplay(block);
}

void loadDemoData(cw1::Blockchain& chain) {
    cout << "\n  Loading demo fleet...\n";

    {
        cw1::CarRecord r;
        r.vin = "VIN1001"; r.manufacturer = "Perodua"; r.model = "Axia";
        r.color = "Silver"; r.productionYear = 2024;

        r.manufacturerId = "MFR-2522";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Shah Alam Plant";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-A1"; r.receivedBy = "Ahmad bin Ismail";
        r.supplierId = "SUP-197588";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-001"; r.passed = true; r.qcNotes = "All systems nominal";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KB-01"; r.destination = "Kota Bharu"; r.transportMode = "Truck";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::CUSTOMER_SALE;
        r.buyerId = "CUST-10201"; r.salePrice = 38000.00; r.warrantyExpiry = "2029-03-01";
        r.retailerId = "RTL-91428";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1002"; r.manufacturer = "Toyota"; r.model = "Vios";
        r.color = "White"; r.productionYear = 2023;

        r.manufacturerId = "MFR-3011";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Toyota Bukit Raja";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-B2"; r.receivedBy = "Lim Wei Jie";
        r.supplierId = "SUP-203344";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-003"; r.passed = true; r.qcNotes = "Minor paint scratch - touched up";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KL-05"; r.destination = "Kuala Lumpur"; r.transportMode = "Truck";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1003"; r.manufacturer = "Honda"; r.model = "City";
        r.color = "Blue"; r.productionYear = 2025;

        r.manufacturerId = "MFR-4500";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Honda Pegoh, Melaka";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-C3"; r.receivedBy = "Raj Kumar";
        r.supplierId = "SUP-310021";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-007"; r.passed = false; r.qcNotes = "Brake pad below spec - pending replacement";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1004"; r.manufacturer = "Proton"; r.model = "X50";
        r.color = "Red"; r.productionYear = 2025;

        r.manufacturerId = "MFR-1088";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Proton Tanjung Malim";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-D4"; r.receivedBy = "Nurul Aina";
        r.supplierId = "SUP-405512";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1005"; r.manufacturer = "Perodua"; r.model = "Myvi";
        r.color = "Black"; r.productionYear = 2025;

        r.manufacturerId = "MFR-2523";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Perodua Rawang";
        chain.addBlock(r);
    }

    const auto vins = chain.getAllVins();
    cout << "  Loaded " << vins.size() << " cars, "
         << chain.totalBlocks() << " total blocks.\n\n";

    const auto result = chain.verifyIntegrity();
    cout << "  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " - " << result.message << "\n\n";
}

void viewAllCars(const cw1::Blockchain& chain) {
    const auto vins = chain.getAllVins();
    if (vins.empty()) {
        cout << "\n  No cars in the system.\n\n";
        return;
    }

    cout << "\n  " << left
         << setw(5)  << "No."
         << setw(12) << "VIN"
         << setw(12) << "Brand"
         << setw(10) << "Model"
         << setw(8)  << "Color"
         << setw(6)  << "Year"
         << setw(20) << "Latest Stage"
         << setw(7)  << "Blocks" << '\n';
    printSeparator();

    int num = 1;
    for (const auto& vin : vins) {
        const auto history = chain.getCarHistory(vin);
        if (history.empty()) {
            continue;
        }

        const auto& rec = history.front()->getRecord();
        const auto latestStage = chain.getLatestStage(vin);

        cout << "  " << left
             << setw(5)  << num++
             << setw(12) << rec.vin
             << setw(12) << rec.manufacturer
             << setw(10) << rec.model
             << setw(8)  << rec.color
             << setw(6)  << rec.productionYear
             << setw(20) << cw1::stageToString(latestStage)
             << setw(7)  << history.size() << '\n';
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed all cars summary (" + to_string(vins.size()) + " VINs)");
}

void viewCarByVin(const cw1::Blockchain& chain) {
    const string vin = prompt("Enter VIN");
    if (!chain.hasVin(vin)) {
        cout << "\n  VIN \"" << vin << "\" not found.\n\n";
        return;
    }

    const auto history = chain.getCarHistory(vin);
    cout << "\n  Blockchain for " << vin << " (" << history.size() << " block(s)):\n\n";

    for (const auto* block : history) {
        printBlockDetail(*block);
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed blockchain for " + vin + " (" + to_string(history.size()) + " blocks)");
}

const char* k_manufacturers[] = {
    "Perodua", "Proton", "Toyota", "Honda", "Nissan", "Mazda",
    "Mitsubishi", "Suzuki", "Hyundai", "Kia", "BMW", "Mercedes-Benz",
    "Volkswagen", "Subaru", "Isuzu", "Ford", "Lexus", "Volvo"
};
constexpr int k_mfrCount = 18;

const char* k_models_perodua[]  = {"Myvi","Axia","Bezza","Aruz","Ativa","Alza"};
const char* k_models_proton[]   = {"Saga","Persona","X50","X70","X90","Iriz","Exora","S70"};
const char* k_models_toyota[]   = {"Vios","Camry","Yaris","Corolla Cross","Hilux","Innova","Rush","Fortuner","Veloz"};
const char* k_models_honda[]    = {"City","Civic","HR-V","CR-V","Accord","BR-V","WR-V"};
const char* k_models_nissan[]   = {"Almera","X-Trail","Navara","Serena","Kicks"};
const char* k_models_mazda[]    = {"Mazda 2","Mazda 3","CX-3","CX-5","CX-8","CX-30","BT-50","MX-5"};
const char* k_models_mitsubishi[] = {"Triton","Xpander","Outlander","ASX"};
const char* k_models_suzuki[]   = {"Swift","Vitara","Jimny","S-Cross","Ertiga","XL7"};
const char* k_models_hyundai[]  = {"Kona","Tucson","Santa Fe","Stargazer","Ioniq 5","Ioniq 6","Palisade"};
const char* k_models_kia[]      = {"Seltos","Sportage","Carnival","Cerato","EV6","EV9"};
const char* k_models_bmw[]      = {"3 Series","5 Series","X1","X3","X5","iX","i4","i5"};
const char* k_models_merc[]     = {"A-Class","C-Class","E-Class","GLA","GLC","GLE","EQA","EQB","EQE"};
const char* k_models_vw[]       = {"Golf","Tiguan","Arteon","ID.4","T-Cross","Passat"};
const char* k_models_subaru[]   = {"XV","Forester","Outback","WRX","BRZ"};
const char* k_models_isuzu[]    = {"D-Max","MU-X"};
const char* k_models_ford[]     = {"Ranger","Everest","Territory"};
const char* k_models_lexus[]    = {"NX","RX","ES","UX","IS","LC"};
const char* k_models_volvo[]    = {"XC40","XC60","XC90","S60","S90","C40","EX30"};

struct ModelList { const char** models; int count; };
const ModelList k_modelsByMfr[] = {
    {k_models_perodua,6},{k_models_proton,8},{k_models_toyota,9},
    {k_models_honda,7},{k_models_nissan,5},{k_models_mazda,8},
    {k_models_mitsubishi,4},{k_models_suzuki,6},{k_models_hyundai,7},
    {k_models_kia,6},{k_models_bmw,8},{k_models_merc,9},
    {k_models_vw,6},{k_models_subaru,5},{k_models_isuzu,2},
    {k_models_ford,3},{k_models_lexus,6},{k_models_volvo,7},
};

const char* k_colors[] = {
    "White","Black","Silver","Grey","Red","Blue",
    "Brown","Green","Yellow","Orange","Beige","Maroon"
};
constexpr int k_colorCount = 12;

const char* k_transportModes[] = {"Truck","Trailer","Car Carrier","Self-Drive"};
constexpr int k_transportCount = 4;

int pickFromList(const char* label, const char* const* items, int count) {
    cout << "\n  " << label << ":\n";
    for (int i = 0; i < count; ++i) {
        cout << "  " << (i + 1) << ". " << items[i] << '\n';
    }
    int choice;
    do {
        choice = parseIntOrDefault(prompt("Select (1-" + to_string(count) + ")"), 0);
        if (choice < 1 || choice > count) {
            cout << "  Invalid. Please enter 1-" << count << ".\n";
        }
    } while (choice < 1 || choice > count);
    return choice - 1;
}

void addNewCarStage(cw1::Blockchain& chain) {
    cout << "\n  Select lifecycle stage to add:\n";
    cout << "  1. Production\n";
    cout << "  2. Warehouse Intake\n";
    cout << "  3. Quality Check\n";
    cout << "  4. Dealer Dispatch\n";
    cout << "  5. Customer Sale\n";

    int stageNum;
    do {
        const string choice = prompt("Stage (1-5)");
        stageNum = parseIntOrDefault(choice, 0);
        if (stageNum < 1 || stageNum > 5) {
            cout << "  Invalid stage. Please enter a number between 1 and 5.\n";
        }
    } while (stageNum < 1 || stageNum > 5);

    cw1::CarRecord r;
    r.vin = prompt("VIN");

    int mfrIdx = pickFromList("Manufacturer", k_manufacturers, k_mfrCount);
    r.manufacturer = k_manufacturers[mfrIdx];

    const ModelList& ml = k_modelsByMfr[mfrIdx];
    int modelIdx = pickFromList("Model", ml.models, ml.count);
    r.model = ml.models[modelIdx];

    int colorIdx = pickFromList("Color", k_colors, k_colorCount);
    r.color = k_colors[colorIdx];

    r.productionYear = parseIntOrDefault(prompt("Production Year"), 0);
    r.manufacturerId = prompt("Manufacturer ID");

    switch (stageNum) {
        case 1:
            r.stage = cw1::BlockStage::PRODUCTION;
            r.factoryLocation = prompt("Factory Location");
            break;
        case 2:
            r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
            r.warehouseLocation = prompt("Warehouse Location");
            r.receivedBy = prompt("Received By");
            r.supplierId = prompt("Supplier ID");
            break;
        case 3:
            r.stage = cw1::BlockStage::QUALITY_CHECK;
            r.inspectorId = prompt("Inspector ID");
            r.passed = (prompt("Passed? (y/n)") == "y");
            r.qcNotes = prompt("QC Notes");
            break;
        case 4: {
            r.stage = cw1::BlockStage::DEALER_DISPATCH;
            r.dealerId = prompt("Dealer ID");
            r.destination = prompt("Destination");
            int tIdx = pickFromList("Transport Mode", k_transportModes, k_transportCount);
            r.transportMode = k_transportModes[tIdx];
            break;
        }
        case 5:
            r.stage = cw1::BlockStage::CUSTOMER_SALE;
            r.buyerId = prompt("Buyer ID");
            r.retailerId = prompt("Retailer ID");
            r.salePrice = parseDoubleOrDefault(prompt("Sale Price (MYR)"), 0.0);
            r.warrantyExpiry = prompt("Warranty Expiry (YYYY-MM-DD)");
            break;
        default:
            break;
    }

    if (r.vin.empty()) {
        cout << "  Error: VIN is required.\n\n";
        return;
    }
    if (r.productionYear < 1900 || r.productionYear > 2030) {
        cout << "  Error: Production year must be between 1900 and 2030.\n\n";
        return;
    }

    const double seconds = cw1::measureSeconds([&]() {
        chain.addBlock(r);
    });

    cout << "\n  Block added. Total blocks: " << chain.totalBlocks() << '\n';
    printOperationDuration(seconds);
    cout << '\n';
}

void searchCars(const cw1::Blockchain& chain) {
    cout << "\n  Search by:\n";
    cout << "  1. General (VIN / Brand / Model / Color)\n";
    cout << "  2. Brand\n";
    cout << "  3. Lifecycle Stage\n";

    const string choice = prompt("Choice (1-3)");
    vector<const cw1::Block*> results;
    double seconds = 0.0;

    if (choice == "1") {
        const string query = prompt("Search query");
        seconds = cw1::measureSeconds([&]() {
            results = chain.searchGeneral(query);
        });
    } else if (choice == "2") {
        const string brand = prompt("Brand name");
        seconds = cw1::measureSeconds([&]() {
            results = chain.searchByBrand(brand);
        });
    } else if (choice == "3") {
        cout << "  1=Production  2=Intake  3=QC  4=Dispatch  5=Sale\n";
        const int s = parseIntOrDefault(prompt("Stage (1-5)"), 0);
        if (s >= 1 && s <= 5) {
            seconds = cw1::measureSeconds([&]() {
                results = chain.searchByStage(static_cast<cw1::BlockStage>(s - 1));
            });
        } else {
            cout << "  Invalid stage.\n\n";
            return;
        }
    } else {
        cout << "  Invalid choice.\n\n";
        return;
    }

    cout << "\n  Found " << results.size() << " block(s):\n\n";
    for (const auto* block : results) {
        printBlockDetail(*block);
    }
    printOperationDuration(seconds);
    cout << '\n';
}

void verifyIntegrity(const cw1::Blockchain& chain) {
    cw1::ValidationResult result{};
    const double seconds = cw1::measureSeconds([&]() {
        result = chain.verifyIntegrity();
    });

    cout << "\n  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " - " << result.message << '\n';
    printOperationDuration(seconds);
    cout << '\n';
}

void printGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    if (blocks.empty()) {
        cout << "\n  Global chain is empty.\n\n";
        return;
    }

    cout << "\n  Global Chain (" << blocks.size() << " block(s)):\n\n";
    for (const auto& block : blocks) {
        printBlockDetail(block);
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed global chain (" + to_string(blocks.size()) + " blocks)");
}

void viewAuditLog(const cw1::Blockchain& chain) {
    const cw1::AuditLog& log = chain.getAuditLog();
    cout << "\n  Audit Log - Total entries: " << log.size() << '\n';

    const string input = prompt("View last how many entries? (default 10)");
    std::size_t n = 10;
    if (!input.empty()) {
        const int parsed = parseIntOrDefault(input, 10);
        n = (parsed > 0) ? static_cast<std::size_t>(parsed) : 10;
    }

    cw1::RecentEntryArray entries(log, n);
    if (entries.empty()) {
        cout << "\n  No audit entries to display.\n\n";
        return;
    }

    cout << "\n  Showing last " << entries.size() << " entries:\n";
    printSeparator();

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const cw1::AuditEntry* entry = entries[i];
        if (entry == nullptr) {
            continue;
        }
        cout << "  [" << setw(4) << (i + 1) << "] "
             << left << setw(22) << entry->timestamp
             << setw(20) << cw1::actionToString(entry->action)
             << entry->details << '\n';
    }

    printSeparator();
    cout << '\n';
}

void simulateTamper(cw1::Blockchain& chain) {
    const std::size_t total = chain.totalBlocks();
    if (total == 0) {
        cout << "\n  Cannot tamper: chain is empty.\n\n";
        return;
    }

    cout << "\n  Tamper Simulation (Debug/Admin)\n";
    cout << "  This intentionally breaks blockchain integrity.\n";

    const string promptText = "Block index to tamper (0-" + to_string(total - 1) + ", blank=last)";
    const string indexInput = prompt(promptText);

    std::size_t index = total - 1;
    if (!indexInput.empty()) {
        const int parsed = parseIntOrDefault(indexInput, -1);
        if (parsed < 0 || static_cast<std::size_t>(parsed) >= total) {
            cout << "  Invalid block index.\n\n";
            return;
        }
        index = static_cast<std::size_t>(parsed);
    }

    const double seconds = cw1::measureSeconds([&]() {
        chain.tamperBlock(index);
    });

    cout << '\n';
    cout << "  Debug / Simulation Feature executed.\n";
    cout << "  Payload for block #" << index
         << " was modified without recomputing hashes.\n";
    printOperationDuration(seconds);
    cout << '\n';
}

void deleteBlock(cw1::Blockchain& chain) {
    const std::size_t total = chain.totalBlocks();
    if (total == 0) {
        cout << "\n  Cannot delete: chain is empty.\n\n";
        return;
    }

    cout << "\n  Delete Block\n";
    cout << "  Current chain size: " << total << " block(s)\n";

    const string indexInput = prompt("Block index to delete (0-" + to_string(total - 1) + ")");
    const int parsed = parseIntOrDefault(indexInput, -1);
    if (parsed < 0 || static_cast<std::size_t>(parsed) >= total) {
        cout << "  Invalid block index.\n\n";
        return;
    }
    const std::size_t index = static_cast<std::size_t>(parsed);

    const string mode = prompt("Soft delete (s) or Hard delete (h)?");
    if (mode != "s" && mode != "h") {
        cout << "  Invalid choice. Aborting.\n\n";
        return;
    }

    string outMessage;
    double seconds = 0.0;

    if (mode == "s") {
        seconds = cw1::measureSeconds([&]() {
            chain.softDeleteBlock(index, outMessage);
        });
    } else {
        cout << "  WARNING: Hard delete will physically remove the block and\n";
        cout << "  re-hash all subsequent blocks. This cannot be undone.\n";
        const string confirm = prompt("Type YES to confirm");
        if (confirm != "YES") {
            cout << "  Hard delete cancelled.\n\n";
            return;
        }
        seconds = cw1::measureSeconds([&]() {
            chain.hardDeleteBlock(index, outMessage);
        });
    }

    cout << "\n  " << outMessage << '\n';
    printOperationDuration(seconds);

    cw1::ValidationResult result{};
    const double verifySeconds = cw1::measureSeconds([&]() {
        result = chain.verifyIntegrity();
    });
    cout << "  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " - " << result.message << '\n';
    cout << "  Verify took: " << cw1::formatSeconds(verifySeconds) << " s\n\n";
}

} 

int main() {
    cw1::Blockchain chain;

    cout << "\n  ========================================\n";
    cout << "    Car Warehouse Blockchain System\n";
    cout << "    Hybrid Architecture (Global + Index)\n";
    cout << "  ========================================\n\n";

    chain.openDatabase("cw1_blockchain.db");
    bool loadedFromDB = false;
    if (chain.isDatabaseOpen()) {
        loadedFromDB = chain.loadFromDB() && chain.totalBlocks() > 0;
    }
    if (!loadedFromDB) {
        loadDemoData(chain);
        if (chain.isDatabaseOpen()) {
            chain.saveToDB();
        }
    } else {
        const auto vins = chain.getAllVins();
        cout << "  Loaded " << vins.size() << " cars, "
             << chain.totalBlocks() << " total blocks from database.\n\n";
        const auto result = chain.verifyIntegrity();
        cout << "  Integrity: " << (result.ok ? "PASS" : "FAIL")
             << " - " << result.message << "\n\n";
    }

    string choice;
    do {
        cout << "  ---- MENU ----\n";
        cout << "  1. View All Cars (summary)\n";
        cout << "  2. View Car Blockchain (by VIN)\n";
        cout << "  3. Add New Block (lifecycle stage)\n";
        cout << "  4. Search\n";
        cout << "  5. View Global Chain\n";
        cout << "  6. Verify Integrity\n";
        cout << "  7. View Audit Log\n";
        cout << "  8. Tamper Simulation (Debug)\n";
        cout << "  9. Delete Block\n";
        cout << "  10. Exit\n";

        choice = prompt("Select (1-10)");

        if (choice == "1") { viewAllCars(chain); }
        else if (choice == "2") { viewCarByVin(chain); }
        else if (choice == "3") { addNewCarStage(chain); }
        else if (choice == "4") { searchCars(chain); }
        else if (choice == "5") { printGlobalChain(chain); }
        else if (choice == "6") { verifyIntegrity(chain); }
        else if (choice == "7") { viewAuditLog(chain); }
        else if (choice == "8") { simulateTamper(chain); }
        else if (choice == "9") { deleteBlock(chain); }
        else if (choice == "10") { cout << "\n  Exiting.\n"; }
        else { cout << "  Invalid choice.\n\n"; }
    } while (choice != "10");

    return 0;
}
```

### `src/main_gui.cpp`



```cpp
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "blockchain/BlockFormatter.hpp"
#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"
#include "utils/OperationTimer.hpp"






static ImVec4 HexColor(uint32_t hex, float a = 1.0f) {
    return ImVec4(
        static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
        static_cast<float>((hex >>  8) & 0xFF) / 255.0f,
        static_cast<float>( hex        & 0xFF) / 255.0f, a);
}

static const ImVec4 COL_BG_MAIN    = HexColor(0x0d1117);  
static const ImVec4 COL_BG_PANEL   = HexColor(0x161b22);  
static const ImVec4 COL_BG_CARD    = HexColor(0x21262d);  
static const ImVec4 COL_BG_ELEV    = HexColor(0x1b222c);  
static const ImVec4 COL_BG_HOVER   = HexColor(0x30363d);  
static const ImVec4 COL_TEXT       = HexColor(0xe6edf3);  
static const ImVec4 COL_MUTED      = HexColor(0x8b949e);  
static const ImVec4 COL_VERY_MUTED = HexColor(0x484f58);  
static const ImVec4 COL_ACCENT     = HexColor(0x1f6feb);  
static const ImVec4 COL_ACCENT_HO  = HexColor(0x388bfd);  
static const ImVec4 COL_ACCENT_SOFT = HexColor(0x1f6feb, 0.20f);
static const ImVec4 COL_GREEN      = HexColor(0x238636);  
static const ImVec4 COL_GREEN_BR   = HexColor(0x2ea043);  
static const ImVec4 COL_RED        = HexColor(0xda3633);  
static const ImVec4 COL_YELLOW     = HexColor(0xd29922);  
static const ImVec4 COL_PURPLE     = HexColor(0x8957e5);  
static const ImVec4 COL_ORANGE     = HexColor(0xdb6d28);  
static const ImVec4 COL_BORDER_SOFT = HexColor(0x30363d, 0.80f);


static ImFont* g_fontBody = nullptr;
static ImFont* g_fontSection = nullptr;
static ImFont* g_fontTitle = nullptr;





static void ApplyGitHubDarkTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 10.0f;  s.FrameRounding    = 7.0f;
    s.PopupRounding     = 8.0f;   s.TabRounding      = 7.0f;
    s.ScrollbarRounding = 8.0f;   s.GrabRounding     = 6.0f;
    s.ChildRounding     = 8.0f;
    s.FramePadding      = ImVec2(10.0f, 7.0f);
    s.ItemSpacing       = ImVec2(10.0f, 10.0f);
    s.WindowPadding     = ImVec2(16.0f, 16.0f);
    s.ItemInnerSpacing  = ImVec2(8.0f,  6.0f);
    s.CellPadding       = ImVec2(10.0f, 8.0f);
    s.ScrollbarSize     = 12.0f;  s.GrabMinSize      = 8.0f;
    s.WindowBorderSize  = 1.0f;   s.FrameBorderSize  = 1.0f;
    s.ChildBorderSize   = 1.0f;   s.PopupBorderSize  = 1.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                  = COL_TEXT;
    c[ImGuiCol_TextDisabled]          = COL_MUTED;
    c[ImGuiCol_WindowBg]              = COL_BG_MAIN;
    c[ImGuiCol_ChildBg]               = COL_BG_PANEL;
    c[ImGuiCol_PopupBg]               = COL_BG_PANEL;
    c[ImGuiCol_Border]                = COL_BORDER_SOFT;
    c[ImGuiCol_BorderShadow]          = ImVec4(0,0,0,0);
    c[ImGuiCol_FrameBg]               = COL_BG_ELEV;
    c[ImGuiCol_FrameBgHovered]        = COL_BG_HOVER;
    c[ImGuiCol_FrameBgActive]         = HexColor(0x1f6feb, 0.35f);
    c[ImGuiCol_TitleBg]               = COL_BG_PANEL;
    c[ImGuiCol_TitleBgActive]         = COL_BG_PANEL;
    c[ImGuiCol_TitleBgCollapsed]      = COL_BG_MAIN;
    c[ImGuiCol_MenuBarBg]             = COL_BG_PANEL;
    c[ImGuiCol_ScrollbarBg]           = COL_BG_MAIN;
    c[ImGuiCol_ScrollbarGrab]         = COL_VERY_MUTED;
    c[ImGuiCol_ScrollbarGrabHovered]  = COL_MUTED;
    c[ImGuiCol_ScrollbarGrabActive]   = COL_ACCENT;
    c[ImGuiCol_CheckMark]             = COL_ACCENT;
    c[ImGuiCol_SliderGrab]            = COL_ACCENT;
    c[ImGuiCol_SliderGrabActive]      = COL_ACCENT_HO;
    c[ImGuiCol_Button]                = COL_BG_ELEV;
    c[ImGuiCol_ButtonHovered]         = COL_BG_HOVER;
    c[ImGuiCol_ButtonActive]          = COL_ACCENT_SOFT;
    c[ImGuiCol_Header]                = COL_BG_CARD;
    c[ImGuiCol_HeaderHovered]         = COL_BG_HOVER;
    c[ImGuiCol_HeaderActive]          = COL_ACCENT_SOFT;
    c[ImGuiCol_Separator]             = COL_BORDER_SOFT;
    c[ImGuiCol_SeparatorHovered]      = COL_MUTED;
    c[ImGuiCol_SeparatorActive]       = COL_ACCENT;
    c[ImGuiCol_ResizeGrip]            = COL_BG_HOVER;
    c[ImGuiCol_ResizeGripHovered]     = COL_MUTED;
    c[ImGuiCol_ResizeGripActive]      = COL_ACCENT;
    c[ImGuiCol_Tab]                   = COL_BG_PANEL;
    c[ImGuiCol_TabHovered]            = COL_BG_HOVER;
    c[ImGuiCol_TabSelected]           = COL_BG_CARD;
    c[ImGuiCol_TabSelectedOverline]   = COL_ACCENT;
    c[ImGuiCol_TabDimmed]             = COL_BG_PANEL;
    c[ImGuiCol_TabDimmedSelected]     = COL_BG_CARD;
    c[ImGuiCol_PlotLines]             = COL_ACCENT;
    c[ImGuiCol_PlotLinesHovered]      = COL_ACCENT_HO;
    c[ImGuiCol_PlotHistogram]         = COL_ACCENT;
    c[ImGuiCol_PlotHistogramHovered]  = COL_ACCENT_HO;
    c[ImGuiCol_TableHeaderBg]         = COL_BG_ELEV;
    c[ImGuiCol_TableBorderStrong]     = COL_BORDER_SOFT;
    c[ImGuiCol_TableBorderLight]      = HexColor(0x30363d, 0.55f);
    c[ImGuiCol_TableRowBg]            = ImVec4(0,0,0,0);
    c[ImGuiCol_TableRowBgAlt]         = HexColor(0x161b22, 0.78f);
    c[ImGuiCol_TextSelectedBg]        = HexColor(0x1f6feb, 0.3f);
    c[ImGuiCol_DragDropTarget]        = COL_ACCENT;
    c[ImGuiCol_NavHighlight]          = COL_ACCENT;
    c[ImGuiCol_NavWindowingHighlight] = COL_ACCENT;
    c[ImGuiCol_NavWindowingDimBg]     = HexColor(0x0d1117, 0.7f);
    c[ImGuiCol_ModalWindowDimBg]      = HexColor(0x0d1117, 0.6f);
}





enum class View { DASHBOARD, CAR_DETAIL, ADD_BLOCK, GLOBAL_CHAIN, AUDIT_LOG, INTEGRITY, DELETE_BLOCK };

static View        g_view        = View::DASHBOARD;
static std::string g_selectedVin;
static char        g_searchBuf[256] = {};
static cw1::ValidationResult g_lastVerify = {false, "Not yet verified"};
static bool        g_verifyDone = false;
static double      g_lastVerifySeconds = 0.0;
static double      g_lastAddBlockSeconds = 0.0;
static double      g_lastSearchSeconds = 0.0;
static int         g_tamperIndex = -1;


static int    g_deleteBlockIndex  = -1;
static bool   g_deleteConfirm     = false;
static double g_lastDeleteSeconds = 0.0;


static std::string              g_cachedSearchQuery;
static std::vector<std::string> g_cachedSearchVins;



static View        g_lastLoggedView = View::DASHBOARD;
static std::string g_lastLoggedVin;


static void LogViewOnce(const cw1::Blockchain& chain, View currentView,
                        const std::string& vin, const std::string& details) {
    if (currentView != g_lastLoggedView || vin != g_lastLoggedVin) {
        chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED, details);
        g_lastLoggedView = currentView;
        g_lastLoggedVin  = vin;
    }
}


struct Toast { std::string msg; float remainingSecs; ImVec4 color; };
static std::vector<Toast> g_toasts;
static void PushToast(const std::string& msg, ImVec4 color, float dur = 3.0f) {
    g_toasts.push_back({msg, dur, color});
}


static int    g_formStage           = 0;
static char   g_formVin[64]         = {};
static int    g_formMfrIndex        = 0;
static int    g_formModelIndex      = 0;
static int    g_formColorIndex      = 0;
static int    g_formYear            = 2025;
static char   g_formFactory[128]    = {};
static char   g_formWarehouse[64]   = {};
static char   g_formReceivedBy[64]  = {};
static char   g_formInspector[64]   = {};
static bool   g_formPassed          = true;
static char   g_formQcNotes[256]    = {};
static char   g_formDealerId[64]    = {};
static char   g_formDestination[64] = {};
static int    g_formTransportIndex  = 0;
static char   g_formBuyerId[64]     = {};
static double g_formSalePrice       = 0.0;
static char   g_formWarranty[32]    = {};
static char   g_formMfrId[64]       = {};
static char   g_formSupplierId[64]  = {};
static char   g_formRetailerId[64]  = {};
static int    g_auditN              = 20;






static void loadDemoData(cw1::Blockchain& chain) {
    { cw1::CarRecord r;
      r.vin="VIN1001"; r.manufacturer="Perodua"; r.model="Axia";
      r.color="Silver"; r.productionYear=2024;
      r.manufacturerId="MFR-2522";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Shah Alam Plant";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-A1"; r.receivedBy="Ahmad bin Ismail";
      r.supplierId="SUP-197588";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-001"; r.passed=true; r.qcNotes="All systems nominal";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::DEALER_DISPATCH;
      r.dealerId="DLR-KB-01"; r.destination="Kota Bharu"; r.transportMode="Truck";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::CUSTOMER_SALE;
      r.buyerId="CUST-10201"; r.salePrice=38000.00; r.warrantyExpiry="2029-03-01";
      r.retailerId="RTL-91428";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1002"; r.manufacturer="Toyota"; r.model="Vios";
      r.color="White"; r.productionYear=2023;
      r.manufacturerId="MFR-3011";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Toyota Bukit Raja";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-B2"; r.receivedBy="Lim Wei Jie";
      r.supplierId="SUP-203344";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-003"; r.passed=true; r.qcNotes="Minor paint scratch - touched up";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::DEALER_DISPATCH;
      r.dealerId="DLR-KL-05"; r.destination="Kuala Lumpur"; r.transportMode="Truck";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1003"; r.manufacturer="Honda"; r.model="City";
      r.color="Blue"; r.productionYear=2025;
      r.manufacturerId="MFR-4500";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Honda Pegoh, Melaka";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-C3"; r.receivedBy="Raj Kumar";
      r.supplierId="SUP-310021";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-007"; r.passed=false;
      r.qcNotes="Brake pad below spec - pending replacement";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1004"; r.manufacturer="Proton"; r.model="X50";
      r.color="Red"; r.productionYear=2025;
      r.manufacturerId="MFR-1088";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Proton Tanjung Malim";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-D4"; r.receivedBy="Nurul Aina";
      r.supplierId="SUP-405512";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1005"; r.manufacturer="Perodua"; r.model="Myvi";
      r.color="Black"; r.productionYear=2025;
      r.manufacturerId="MFR-2523";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Perodua Rawang";
      chain.addBlock(r); }
}





static ImVec4 StageColor(cw1::BlockStage stage) {
    switch (stage) {
        case cw1::BlockStage::PRODUCTION:       return COL_MUTED;
        case cw1::BlockStage::WAREHOUSE_INTAKE: return COL_ACCENT;
        case cw1::BlockStage::QUALITY_CHECK:    return COL_YELLOW;
        case cw1::BlockStage::DEALER_DISPATCH:  return COL_ORANGE;
        case cw1::BlockStage::CUSTOMER_SALE:    return COL_GREEN;
        default:                                return COL_TEXT;
    }
}

static float StageProgress(cw1::BlockStage stage) {
    return (static_cast<float>(stage) + 1.0f) / 5.0f;
}

static std::string Truncate(const std::string& s, size_t maxLen) {
    return (s.size() <= maxLen) ? s : s.substr(0, maxLen) + "...";
}

static int ExtractFailedBlockIndex(const std::string& message) {
    const std::string needle = "block index ";
    const std::size_t pos = message.find(needle);
    if (pos == std::string::npos) {
        return -1;
    }

    const char* start = message.c_str() + pos + needle.size();
    char* end = nullptr;
    const long value = std::strtol(start, &end, 10);
    if (end == start || value < 0) {
        return -1;
    }
    return static_cast<int>(value);
}

static void DrawSectionHeading(const char* text) {
    if (g_fontSection != nullptr) {
        ImGui::PushFont(g_fontSection);
    }
    ImGui::TextColored(COL_ACCENT, "%s", text);
    if (g_fontSection != nullptr) {
        ImGui::PopFont();
    }
}

static bool DrawPrimaryButton(const char* label, const ImVec2& size = ImVec2(0.0f, 0.0f)) {
    ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1158c7));
    const bool clicked = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return clicked;
}

static bool DrawDangerButton(const char* label, const ImVec2& size = ImVec2(0.0f, 0.0f)) {
    ImGui::PushStyleColor(ImGuiCol_Button,        HexColor(0x5d2227));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x7c2f35));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x94353d));
    const bool clicked = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return clicked;
}

static void DrawMetricBadge(const char* text, ImVec4 textColor, ImVec4 bgColor) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgColor);
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::Button(text);
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
}





static void RenderHeader(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 12.0f));
    ImGui::BeginChild("##header", ImVec2(-1.0f, 66.0f), true);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
    ImGui::TextColored(COL_ACCENT, "%s", ">> ");
    ImGui::SameLine();
    if (g_fontTitle != nullptr) {
        ImGui::PushFont(g_fontTitle);
    }
    ImGui::Text("Car Warehouse Blockchain");
    if (g_fontTitle != nullptr) {
        ImGui::PopFont();
    }

    float statsWidth = 430.0f;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - statsWidth
                    + ImGui::GetCursorPosX() - 16.0f);

    char badge[64];
    snprintf(badge, sizeof(badge), "%zu blocks", chain.totalBlocks());
    DrawMetricBadge(badge, COL_ACCENT_HO, HexColor(0x1f6feb, 0.14f));
    ImGui::SameLine();
    snprintf(badge, sizeof(badge), "%zu cars", chain.getAllVins().size());
    DrawMetricBadge(badge, COL_GREEN_BR, HexColor(0x238636, 0.16f));
    ImGui::SameLine();
    if (g_verifyDone) {
        if (g_lastVerify.ok) {
            DrawMetricBadge("Verified", COL_GREEN_BR, HexColor(0x238636, 0.16f));
        } else {
            DrawMetricBadge("Invalid", COL_RED, HexColor(0xda3633, 0.18f));
        }
    } else {
        DrawMetricBadge("Checking...", COL_MUTED, HexColor(0x30363d, 0.60f));
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}





static void RenderSidebar(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##sidebar", ImVec2(280.0f, -1.0f), true);

    
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##search", "Search VIN, brand, model...",
                             g_searchBuf, sizeof(g_searchBuf));
    ImGui::PopStyleColor(4);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    
    struct NavItem { const char* label; View view; };
    static const NavItem navItems[] = {
        { "  Dashboard",        View::DASHBOARD    },
        { "  + Add New Block",  View::ADD_BLOCK     },
        { "  Global Chain",     View::GLOBAL_CHAIN  },
        { "  Audit Log",        View::AUDIT_LOG     },
        { "  Verify Integrity", View::INTEGRITY     },
    };

    for (const auto& item : navItems) {
        bool active = (g_view == item.view);
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT_SOFT);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1f6feb, 0.35f));
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,        COL_BG_ELEV);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        if (ImGui::Button(item.label, ImVec2(-1.0f, 34.0f))) {
            g_view = item.view;
            g_selectedVin.clear();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);
    }

    
    {
        bool active = (g_view == View::DELETE_BLOCK);
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button,        HexColor(0x5d2227));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x7c2f35));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x94353d));
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,        HexColor(0x3d1519));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x5d2227));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x7c2f35));
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_RED);
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        if (ImGui::Button("  Delete Block", ImVec2(-1.0f, 34.0f))) {
            g_view = View::DELETE_BLOCK;
            g_selectedVin.clear();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    
    auto allVins = chain.getAllVins();
    ImGui::TextColored(COL_MUTED, "VEHICLES (%zu)", allVins.size());
    ImGui::Spacing();

    
    
    
    std::vector<std::string> displayVins;
    const std::string query(g_searchBuf);
    if (!query.empty()) {
        if (query != g_cachedSearchQuery) {
            g_cachedSearchQuery = query;
            g_cachedSearchVins.clear();

            const double seconds = cw1::measureSeconds([&]() {
                const auto hits = chain.searchBySQL(query);
                std::vector<std::string> seen;
                for (const auto* blk : hits) {
                    const std::string& vin = blk->getRecord().vin;
                    if (std::find(seen.begin(), seen.end(), vin) == seen.end()) {
                        seen.push_back(vin);
                        g_cachedSearchVins.push_back(vin);
                    }
                }
            });
            g_lastSearchSeconds = seconds;
        }
        displayVins = g_cachedSearchVins;
    } else {
        g_cachedSearchQuery.clear();
        g_cachedSearchVins.clear();
        displayVins = allVins;
    }

    if (!query.empty()) {
        ImGui::TextColored(COL_VERY_MUTED, "Search operation took: %s s",
                           cw1::formatSeconds(g_lastSearchSeconds).c_str());
        ImGui::Spacing();
    }

    for (const auto& vin : displayVins) {
        const auto history = chain.getCarHistory(vin);
        if (history.empty()) continue;
        const auto& rec    = history.front()->getRecord();
        bool        sel    = (vin == g_selectedVin);
        cw1::BlockStage st = chain.getLatestStage(vin);

        
        if (sel) {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(
                p, ImVec2(p.x + 3.0f, p.y + 72.0f),
                ImGui::ColorConvertFloat4ToU32(COL_ACCENT));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_Header,        sel ? COL_BG_HOVER : COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  HexColor(0x1f6feb, 0.25f));

        char cardId[32]; snprintf(cardId, sizeof(cardId), "##car_%s", vin.c_str());
        bool clicked = ImGui::Selectable(cardId, sel, 0, ImVec2(-1.0f, 68.0f));
        ImGui::PopStyleColor(3);

        if (clicked) { g_selectedVin = vin; g_view = View::CAR_DETAIL; }

        float  topY = ImGui::GetCursorPos().y - 68.0f;
        float  offX = sel ? 22.0f : 16.0f;

        ImGui::SetCursorPos(ImVec2(offX, topY + 6.0f));
        ImGui::TextColored(sel ? COL_ACCENT : COL_TEXT, "%s", vin.c_str());

        ImGui::SetCursorPos(ImVec2(offX, topY + 24.0f));
        ImGui::TextColored(COL_MUTED, "%s %s - %s",
            rec.manufacturer.c_str(), rec.model.c_str(), rec.color.c_str());

        ImGui::SetCursorPos(ImVec2(offX, topY + 42.0f));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, StageColor(st));
        ImGui::PushStyleColor(ImGuiCol_FrameBg,       COL_BG_MAIN);
        ImGui::ProgressBar(StageProgress(st), ImVec2(150.0f, 8.0f), "");
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, " %s", cw1::stageToString(st).c_str());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY());
        ImGui::Spacing();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}








static void RenderDashboard(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    size_t totalBlocks = chain.totalBlocks();
    size_t totalCars   = chain.getAllVins().size();
    DrawSectionHeading("Dashboard");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    
    if (ImGui::BeginTable("##stats", 4,
            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoPadOuterX,
            ImVec2(-1, 90))) {
        ImGui::TableNextRow();

        auto StatCard = [](const char* value, const char* label, ImVec4 col) {
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_ELEV);
            ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
            ImGui::BeginChild(label, ImVec2(-1, 86), true);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);
            if (g_fontSection != nullptr) {
                ImGui::PushFont(g_fontSection);
            }
            ImGui::TextColored(col, "%s", value);
            if (g_fontSection != nullptr) {
                ImGui::PopFont();
            }
            ImGui::TextColored(COL_MUTED, "%s", label);
            ImGui::EndChild();
            ImGui::PopStyleColor(2);
        };

        char buf1[32], buf2[32], buf4[32];
        snprintf(buf1, sizeof(buf1), "%zu", totalBlocks);
        snprintf(buf2, sizeof(buf2), "%zu", totalCars);
        snprintf(buf4, sizeof(buf4), "%zu", chain.getAuditLog().size());

        StatCard(buf1, "Total Blocks",    COL_ACCENT);
        StatCard(buf2, "Unique Vehicles", COL_GREEN_BR);
        StatCard(g_verifyDone && g_lastVerify.ok ? "VERIFIED" : (g_verifyDone ? "TAMPERED" : "N/A"),
                 "Integrity",
                 g_verifyDone && g_lastVerify.ok ? COL_GREEN_BR : COL_RED);
        StatCard(buf4, "Audit Events",    COL_PURPLE);

        ImGui::EndTable();
    }

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "RECENT BLOCKS");
    ImGui::Separator();
    ImGui::Spacing();

    
    ImGuiTableFlags tflags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH |
                             ImGuiTableFlags_RowBg |
                             ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
    if (ImGui::BeginTable("##recent", 6, tflags, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed,   60);
        ImGui::TableSetupColumn("Hash",         ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN",          ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage",        ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Timestamp",    ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        
        size_t count = blocks.size();
        for (size_t ri = 0; ri < count; ++ri) {
            size_t i = count - 1 - ri;
            const cw1::Block& b = blocks[i];
            const cw1::CarRecord& rec = b.getRecord();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", b.getIndex());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getCurrentHash(), 12).c_str());

            ImGui::TableNextColumn();
            char selId[80];
            snprintf(selId, sizeof(selId), "%s##rv%zu", rec.vin.c_str(), i);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_ACCENT);
            if (ImGui::Selectable(selId, false, ImGuiSelectableFlags_SpanAllColumns)) {
                g_selectedVin = rec.vin;
                g_view = View::CAR_DETAIL;
            }
            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text("%s", rec.manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(rec.stage), "%s",
                               cw1::stageToString(rec.stage).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", b.getTimestamp().c_str());
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}





static void RenderCarDetail(const cw1::Blockchain& chain) {
    if (ImGui::Button("< Back")) {
        g_view = View::DASHBOARD;
        return;
    }
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " / ");
    ImGui::SameLine();
    ImGui::TextColored(COL_ACCENT, "%s", g_selectedVin.c_str());

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    if (g_selectedVin.empty() || !chain.hasVin(g_selectedVin)) {
        ImGui::TextColored(COL_RED, "VIN not found.");
        return;
    }

    auto history = chain.getCarHistory(g_selectedVin);
    if (history.empty()) {
        ImGui::TextColored(COL_MUTED, "No blocks for this VIN.");
        return;
    }

    
    const cw1::CarRecord& first = history.front()->getRecord();
    ImGui::TextColored(COL_TEXT, "%s %s  (%d)   %s",
        first.manufacturer.c_str(), first.model.c_str(),
        first.productionYear, first.color.c_str());
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "LIFECYCLE TIMELINE  (%zu block(s))", history.size());
    ImGui::Separator(); ImGui::Spacing();

    for (size_t idx = 0; idx < history.size(); ++idx) {
        const cw1::Block& blk = *history[idx];
        const cw1::CarRecord& rec = blk.getRecord();

        char hdrLabel[128];
        snprintf(hdrLabel, sizeof(hdrLabel), "Block #%zu  |  %s##det%zu",
                 blk.getIndex(), cw1::stageToString(rec.stage).c_str(), idx);

        ImGui::PushStyleColor(ImGuiCol_Header,        COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  HexColor(0x1f6feb, 0.2f));
        bool open = ImGui::CollapsingHeader(hdrLabel,
                        idx == history.size() - 1
                            ? ImGuiTreeNodeFlags_DefaultOpen
                            : ImGuiTreeNodeFlags_None);
        ImGui::PopStyleColor(3);
        if (!open) continue;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        
        ImGui::BeginChild(hdrLabel, ImVec2(-1, 0), true);

        ImGui::TextColored(COL_MUTED, "Canonical Block Output");
        ImGui::Spacing();

        const std::string formatted = cw1::formatBlockForDisplay(blk);
        ImGui::PushStyleColor(ImGuiCol_Text, COL_TEXT);
        ImGui::TextUnformatted(formatted.c_str());
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }
    LogViewOnce(chain, View::CAR_DETAIL, g_selectedVin,
                "Car detail viewed: " + g_selectedVin);
}





static const char* k_stageNames[] = {
    "PRODUCTION", "WAREHOUSE_INTAKE", "QUALITY_CHECK",
    "DEALER_DISPATCH", "CUSTOMER_SALE"
};

static const char* k_manufacturers[] = {
    "Perodua", "Proton", "Toyota", "Honda", "Nissan", "Mazda",
    "Mitsubishi", "Suzuki", "Hyundai", "Kia", "BMW", "Mercedes-Benz",
    "Volkswagen", "Subaru", "Isuzu", "Ford", "Lexus", "Volvo"
};

struct ModelList { const char** models; int count; };

static const char* k_models_perodua[]  = {"Myvi","Axia","Bezza","Aruz","Ativa","Alza"};
static const char* k_models_proton[]   = {"Saga","Persona","X50","X70","X90","Iriz","Exora","S70"};
static const char* k_models_toyota[]   = {"Vios","Camry","Yaris","Corolla Cross","Hilux","Innova","Rush","Fortuner","Veloz"};
static const char* k_models_honda[]    = {"City","Civic","HR-V","CR-V","Accord","BR-V","WR-V"};
static const char* k_models_nissan[]   = {"Almera","X-Trail","Navara","Serena","Kicks"};
static const char* k_models_mazda[]    = {"Mazda 2","Mazda 3","CX-3","CX-5","CX-8","CX-30","BT-50","MX-5"};
static const char* k_models_mitsubishi[] = {"Triton","Xpander","Outlander","ASX"};
static const char* k_models_suzuki[]   = {"Swift","Vitara","Jimny","S-Cross","Ertiga","XL7"};
static const char* k_models_hyundai[]  = {"Kona","Tucson","Santa Fe","Stargazer","Ioniq 5","Ioniq 6","Palisade"};
static const char* k_models_kia[]      = {"Seltos","Sportage","Carnival","Cerato","EV6","EV9"};
static const char* k_models_bmw[]      = {"3 Series","5 Series","X1","X3","X5","iX","i4","i5"};
static const char* k_models_merc[]     = {"A-Class","C-Class","E-Class","GLA","GLC","GLE","EQA","EQB","EQE"};
static const char* k_models_vw[]       = {"Golf","Tiguan","Arteon","ID.4","T-Cross","Passat"};
static const char* k_models_subaru[]   = {"XV","Forester","Outback","WRX","BRZ"};
static const char* k_models_isuzu[]    = {"D-Max","MU-X"};
static const char* k_models_ford[]     = {"Ranger","Everest","Territory"};
static const char* k_models_lexus[]    = {"NX","RX","ES","UX","IS","LC"};
static const char* k_models_volvo[]    = {"XC40","XC60","XC90","S60","S90","C40","EX30"};

static const ModelList k_modelsByMfr[] = {
    {k_models_perodua,    6}, {k_models_proton,     8}, {k_models_toyota,    9},
    {k_models_honda,      7}, {k_models_nissan,     5}, {k_models_mazda,     8},
    {k_models_mitsubishi, 4}, {k_models_suzuki,     6}, {k_models_hyundai,   7},
    {k_models_kia,        6}, {k_models_bmw,        8}, {k_models_merc,      9},
    {k_models_vw,         6}, {k_models_subaru,     5}, {k_models_isuzu,     2},
    {k_models_ford,       3}, {k_models_lexus,      6}, {k_models_volvo,     7},
};

static const char* k_colors[] = {
    "White","Black","Silver","Grey","Red","Blue",
    "Brown","Green","Yellow","Orange","Beige","Maroon"
};

static const char* k_transportModes[] = {
    "Truck","Trailer","Car Carrier","Self-Drive"
};

static void RenderAddBlock(cw1::Blockchain& chain) {
    DrawSectionHeading("Add Block to Chain");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);

    ImGui::TextColored(COL_MUTED, "COMMON FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    ImGui::SetNextItemWidth(240);
    ImGui::Combo("Stage##addstage", &g_formStage, k_stageNames, 5);
    ImGui::Spacing();

    
    auto LabelText = [](const char* lbl, char* buf, size_t sz) {
        ImGui::TextColored(COL_MUTED, "%-18s", lbl);
        ImGui::SameLine();
        char id[64]; snprintf(id, sizeof(id), "##add_%s", lbl);
        ImGui::SetNextItemWidth(300);
        ImGui::InputText(id, buf, sz);
    };

    LabelText("VIN",          g_formVin,   sizeof(g_formVin));
    
    ImGui::SameLine(0, 12);
    if (g_formVin[0] != '\0') {
        if (chain.hasVin(std::string(g_formVin)))
            ImGui::TextColored(COL_GREEN_BR, "[+ Existing VIN]");
        else
            ImGui::TextColored(COL_ACCENT,   "[* New VIN]");
    }

    ImGui::TextColored(COL_MUTED, "%-18s", "Manufacturer");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    if (ImGui::Combo("##add_Manufacturer", &g_formMfrIndex, k_manufacturers, IM_ARRAYSIZE(k_manufacturers))) {
        g_formModelIndex = 0;
    }

    ImGui::TextColored(COL_MUTED, "%-18s", "Model");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    const ModelList& ml = k_modelsByMfr[g_formMfrIndex];
    ImGui::Combo("##add_Model", &g_formModelIndex, ml.models, ml.count);

    ImGui::TextColored(COL_MUTED, "%-18s", "Color");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::Combo("##add_Color", &g_formColorIndex, k_colors, IM_ARRAYSIZE(k_colors));

    ImGui::TextColored(COL_MUTED, "%-18s", "Production Year");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("##add_year", &g_formYear);
    if (g_formYear < 1900) g_formYear = 1900;

    if (g_formYear > 2030) g_formYear = 2030;
    ImGui::Spacing(); ImGui::Spacing();
    LabelText("Manufacturer ID", g_formMfrId, sizeof(g_formMfrId));
    ImGui::TextColored(COL_MUTED, "STAGE-SPECIFIC FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    switch (g_formStage) {
    case 0: 
        LabelText("Factory Location", g_formFactory, sizeof(g_formFactory));
        break;
    case 1: 
        LabelText("Warehouse",  g_formWarehouse,   sizeof(g_formWarehouse));
        LabelText("Received By",g_formReceivedBy,  sizeof(g_formReceivedBy));
        LabelText("Supplier ID", g_formSupplierId, sizeof(g_formSupplierId));
        break;
    case 2: 
        LabelText("Inspector ID", g_formInspector, sizeof(g_formInspector));
        ImGui::TextColored(COL_MUTED, "%-18s", "Passed");
        ImGui::SameLine();
        ImGui::Checkbox("##add_passed", &g_formPassed);
        LabelText("QC Notes", g_formQcNotes, sizeof(g_formQcNotes));
        break;
    case 3:
        LabelText("Dealer ID",    g_formDealerId,   sizeof(g_formDealerId));
        LabelText("Destination",  g_formDestination,sizeof(g_formDestination));
        ImGui::TextColored(COL_MUTED, "%-18s", "Transport Mode");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(300);
        ImGui::Combo("##add_Transport", &g_formTransportIndex, k_transportModes, IM_ARRAYSIZE(k_transportModes));
        break;
    case 4: 
        LabelText("Buyer ID",     g_formBuyerId, sizeof(g_formBuyerId));
        LabelText("Retailer ID",  g_formRetailerId, sizeof(g_formRetailerId));
        ImGui::TextColored(COL_MUTED, "%-18s", "Sale Price (MYR)");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputDouble("##add_price", &g_formSalePrice, 1000.0, 10000.0, "%.2f");
        LabelText("Warranty Expiry", g_formWarranty, sizeof(g_formWarranty));
        break;
    }

    ImGui::Spacing(); ImGui::Spacing();

    bool submit = DrawPrimaryButton("  Add Block  ", ImVec2(150, 36));

    bool canSubmit = (g_formVin[0] != '\0');

    if (!canSubmit) {
        ImGui::SameLine();
        ImGui::TextColored(COL_RED, "  VIN is required.");
    }

    if (submit && canSubmit) {
        cw1::CarRecord r;
        r.vin            = g_formVin;
        r.manufacturer   = k_manufacturers[g_formMfrIndex];
        r.model          = k_modelsByMfr[g_formMfrIndex].models[g_formModelIndex];
        r.color          = k_colors[g_formColorIndex];
        r.productionYear = g_formYear;
        r.stage          = static_cast<cw1::BlockStage>(g_formStage);
        r.manufacturerId = g_formMfrId;

        switch (g_formStage) {
        case 0:
            r.factoryLocation  = g_formFactory;
            break;
        case 1:
            r.warehouseLocation = g_formWarehouse;
            r.receivedBy        = g_formReceivedBy;
            r.supplierId        = g_formSupplierId;
            break;
        case 2:
            r.inspectorId = g_formInspector;
            r.passed      = g_formPassed;
            r.qcNotes     = g_formQcNotes;
            break;
        case 3:
            r.dealerId      = g_formDealerId;
            r.destination   = g_formDestination;
            r.transportMode = k_transportModes[g_formTransportIndex];
            break;
        case 4:
            r.buyerId        = g_formBuyerId;
            r.retailerId     = g_formRetailerId;
            r.salePrice      = g_formSalePrice;
            r.warrantyExpiry = g_formWarranty;
            break;
        }

        const double seconds = cw1::measureSeconds([&]() {
            chain.addBlock(r);
        });
        g_lastAddBlockSeconds = seconds;
        PushToast("Block #" + std::to_string(chain.totalBlocks() - 1) +
                  " added for " + std::string(g_formVin) +
                  " | Operation took: " + cw1::formatSeconds(seconds) + " s",
                  COL_GREEN_BR);


        g_formVin[0]    = '\0';
        g_formMfrIndex  = 0;  g_formModelIndex = 0;  g_formColorIndex = 0;
        g_formYear      = 2025;
        g_formFactory[0]= '\0'; g_formWarehouse[0]    = '\0';
        g_formReceivedBy[0]= '\0'; g_formInspector[0] = '\0';
        g_formPassed    = true;  g_formQcNotes[0]     = '\0';
        g_formDealerId[0]= '\0'; g_formDestination[0] = '\0';
        g_formTransportIndex = 0; g_formBuyerId[0]    = '\0';
        g_formSalePrice = 0.0;   g_formWarranty[0]    = '\0';
        g_formMfrId[0] = '\0'; g_formSupplierId[0] = '\0'; g_formRetailerId[0] = '\0';
    }

    if (g_lastAddBlockSeconds > 0.0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_VERY_MUTED, "Last add-block operation took: %s s",
                           cw1::formatSeconds(g_lastAddBlockSeconds).c_str());
    }

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
}





static void RenderGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();

    DrawSectionHeading("Global Chain");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu block(s))", blocks.size());
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "VISUAL LINKAGE");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##global_visual_link", ImVec2(-1.0f, 300.0f), true);
    ImGui::PopStyleColor();

    if (blocks.empty()) {
        ImGui::TextColored(COL_MUTED, "No blocks in the chain.");
    } else {
        for (size_t i = 0; i < blocks.size(); ++i) {
            const cw1::Block& b = blocks[i];
            const cw1::CarRecord& rec = b.getRecord();
            const ImVec2 cardMin = ImGui::GetCursorScreenPos();
            const float cardWidth = ImGui::GetContentRegionAvail().x;
            const float lineH = ImGui::GetTextLineHeightWithSpacing();
            const float cardHeight = (lineH * 5.0f) + 20.0f;
            const ImVec2 cardMax(cardMin.x + cardWidth, cardMin.y + cardHeight);
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddRectFilled(cardMin, cardMax,
                ImGui::ColorConvertFloat4ToU32(COL_BG_ELEV), 8.0f);
            dl->AddRect(cardMin, cardMax,
                ImGui::ColorConvertFloat4ToU32(COL_BORDER_SOFT), 8.0f);

            ImGui::SetCursorScreenPos(ImVec2(cardMin.x + 12.0f, cardMin.y + 9.0f));
            ImGui::BeginGroup();
            ImGui::TextColored(COL_ACCENT, "Block #%zu", b.getIndex());
            ImGui::TextColored(COL_PURPLE, "Hash: %s", Truncate(b.getCurrentHash(), 8).c_str());
            ImGui::TextColored(COL_PURPLE, "Prev: %s", Truncate(b.getPreviousHash(), 8).c_str());
            ImGui::TextColored(StageColor(rec.stage), "Stage: %s", cw1::stageToString(rec.stage).c_str());
            ImGui::TextColored(COL_ACCENT, "VIN: %s", rec.vin.c_str());
            ImGui::EndGroup();

            ImGui::SetCursorScreenPos(ImVec2(cardMin.x, cardMax.y + 8.0f));

            if (i + 1 < blocks.size()) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14.0f);
                ImGui::TextColored(COL_MUTED, "v");
            }
        }
    }

    ImGui::EndChild();
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "TABULAR CHAIN DATA");
    ImGui::Spacing();

    ImGuiTableFlags tf = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH |
                         ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
    if (ImGui::BeginTable("##global", 8, tf, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed,   60);
        ImGui::TableSetupColumn("Hash",         ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Prev Hash",    ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN",          ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage",        ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Nonce",        ImGuiTableColumnFlags_WidthFixed,   90);
        ImGui::TableSetupColumn("Timestamp",    ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < blocks.size(); ++i) {
            const cw1::Block& b   = blocks[i];
            const cw1::CarRecord& rec = b.getRecord();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", b.getIndex());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getCurrentHash(), 12).c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(COL_PURPLE, "%s", b.getCurrentHash().c_str());
                ImGui::EndTooltip();
            }

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getPreviousHash(), 12).c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(COL_PURPLE, "%s", b.getPreviousHash().c_str());
                ImGui::EndTooltip();
            }

            ImGui::TableNextColumn();
            char selId[80];
            snprintf(selId, sizeof(selId), "%s##gc%zu", rec.vin.c_str(), i);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_ACCENT);
            if (ImGui::Selectable(selId, false, ImGuiSelectableFlags_SpanAllColumns)) {
                g_selectedVin = rec.vin;
                g_view = View::CAR_DETAIL;
            }
            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text("%s", rec.manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(rec.stage), "%s",
                               cw1::stageToString(rec.stage).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_ORANGE, "%llu",
                               static_cast<unsigned long long>(b.getNonce()));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", b.getTimestamp().c_str());
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    LogViewOnce(chain, View::GLOBAL_CHAIN, "",
                "Global chain viewed");
}





static void RenderAuditLog(const cw1::Blockchain& chain) {
    DrawSectionHeading("Audit Log");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu total event(s))", chain.getAuditLog().size());
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Show last");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##auditn", &g_auditN);
    if (g_auditN < 1)  g_auditN = 1;
    if (g_auditN > 500) g_auditN = 500;
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, "entries");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    
    cw1::RecentEntryArray entries(chain.getAuditLog(),
                                  static_cast<std::size_t>(g_auditN));

    auto ActionColor = [](cw1::AuditAction act) -> ImVec4 {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return COL_GREEN_BR;
        case cw1::AuditAction::INTEGRITY_CHECK:  return COL_YELLOW;
        case cw1::AuditAction::SEARCH_PERFORMED: return COL_ACCENT;
        case cw1::AuditAction::CHAIN_VIEWED:     return COL_PURPLE;
        case cw1::AuditAction::TAMPER_SIMULATED: return COL_RED;
        case cw1::AuditAction::PERSISTENCE_IO:   return COL_ORANGE;
        case cw1::AuditAction::BLOCK_DELETED:    return COL_RED;
        default:                                  return COL_TEXT;
        }
    };
    auto ActionName = [](cw1::AuditAction act) -> const char* {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return "BLOCK_ADDED";
        case cw1::AuditAction::INTEGRITY_CHECK:  return "INTEGRITY_CHECK";
        case cw1::AuditAction::SEARCH_PERFORMED: return "SEARCH_PERFORMED";
        case cw1::AuditAction::CHAIN_VIEWED:     return "CHAIN_VIEWED";
        case cw1::AuditAction::TAMPER_SIMULATED: return "TAMPER_SIMULATED";
        case cw1::AuditAction::PERSISTENCE_IO:   return "PERSISTENCE_IO";
        case cw1::AuditAction::BLOCK_DELETED:    return "BLOCK_DELETED";
        default:                                  return "UNKNOWN";
        }
    };

    ImGuiTableFlags tf = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH |
                         ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
    if (ImGui::BeginTable("##auditlog", 3, tf, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Action",    ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Details",   ImGuiTableColumnFlags_WidthStretch, 4.0f);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < entries.size(); ++i) {
            const cw1::AuditEntry* e = entries[i];
            if (e == nullptr) {
                continue;
            }
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", e->timestamp.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(ActionColor(e->action), "%s", ActionName(e->action));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_TEXT, "%s", e->details.c_str());
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}





static void RenderIntegrity(cw1::Blockchain& chain) {
    DrawSectionHeading("Blockchain Integrity Verification");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    
    if (DrawPrimaryButton("Run Verification Again##verify", ImVec2(230.0f, 36.0f))) {
        cw1::OperationTimer timer;
        g_lastVerify = chain.verifyIntegrity();
        g_lastVerifySeconds = timer.elapsedSeconds();
        g_verifyDone = true;
        PushToast(g_lastVerify.ok ? "Integrity check passed."
                                  : "Integrity check failed.",
                  g_lastVerify.ok ? COL_GREEN_BR : COL_RED);
    }

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "Debug / Simulation Feature");
    ImGui::SetNextItemWidth(140.0f);
    ImGui::InputInt("Tamper block index##tamper", &g_tamperIndex);
    ImGui::SameLine();
    if (DrawDangerButton("Tamper Block##payload_tamper", ImVec2(160.0f, 34.0f))) {
        const std::size_t total = chain.totalBlocks();
        if (total == 0) {
            PushToast("Tamper failed: chain is empty.", COL_RED);
        } else {
            std::size_t target = total - 1;
            bool validIndex = true;
            if (g_tamperIndex >= 0) {
                const std::size_t requested = static_cast<std::size_t>(g_tamperIndex);
                if (requested < total) {
                    target = requested;
                } else {
                    validIndex = false;
                    PushToast("Tamper failed: index out of range.", COL_RED);
                }
            }

            if (validIndex) {
                cw1::OperationTimer timer;
                chain.tamperBlock(target);
                const double tamperSeconds = timer.elapsedSeconds();

                std::ostringstream msg;
                msg << "Debug / Simulation Feature: payload tampered at block #"
                    << target << " | Operation took: "
                    << cw1::formatSeconds(tamperSeconds) << " s";
                PushToast(msg.str(), COL_RED);
            }
        }
    }

    ImGui::Spacing(); ImGui::Spacing();
    if (!g_verifyDone) {
        ImGui::TextColored(COL_MUTED, "Press Run Verification to check the chain.");
        return;
    }

    const std::size_t blocksChecked = chain.totalBlocks();

    
    const ImVec4 bannerCol = g_lastVerify.ok ? HexColor(0x1b4d30) : HexColor(0x5a2429);
    const char* bannerTxt = g_lastVerify.ok ? "BLOCKCHAIN VERIFIED" : "INTEGRITY FAILURE";
    ImGui::PushStyleColor(ImGuiCol_ChildBg, bannerCol);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##intbanner", ImVec2(-1, 80), true);
    ImGui::SetCursorPos(ImVec2(20.0f, 18.0f));
    ImGui::SetWindowFontScale(1.7f);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", bannerTxt);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "Integrity Status:");
    ImGui::SameLine();
    ImGui::TextColored(g_lastVerify.ok ? COL_GREEN_BR : COL_RED,
                       "%s", g_lastVerify.ok ? "Verified [OK]" : "FAILED");

    ImGui::TextColored(COL_MUTED, "Blocks Checked:");
    ImGui::SameLine();
    ImGui::TextColored(COL_TEXT, "%zu", blocksChecked);

    if (!g_lastVerify.ok) {
        const int failedIndex = ExtractFailedBlockIndex(g_lastVerify.message);
        if (failedIndex >= 0) {
            ImGui::TextColored(COL_MUTED, "Error Block:");
            ImGui::SameLine();
            ImGui::TextColored(COL_RED, "#%d", failedIndex);
        }
    }

    ImGui::TextColored(COL_MUTED, "Verification Time:");
    ImGui::SameLine();
    ImGui::TextColored(COL_TEXT, "%s seconds",
                       cw1::formatSeconds(g_lastVerifySeconds).c_str());

    ImGui::TextColored(COL_MUTED, "Details:");
    ImGui::SameLine();
    ImGui::TextWrapped("%s", g_lastVerify.message.c_str());
}





static void RenderDeleteBlock(cw1::Blockchain& chain) {
    DrawSectionHeading("Delete Block");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Chain size: %zu block(s)", chain.totalBlocks());
    ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Block index to delete:");
    ImGui::SetNextItemWidth(160.0f);
    ImGui::InputInt("##delete_block_index", &g_deleteBlockIndex);
    ImGui::Spacing();

    
    if (DrawDangerButton("Soft Delete##soft_del", ImVec2(180.0f, 36.0f))) {
        if (g_deleteBlockIndex < 0 ||
            static_cast<std::size_t>(g_deleteBlockIndex) >= chain.totalBlocks()) {
            PushToast("Invalid block index.", COL_RED);
        } else {
            std::string msg;
            cw1::OperationTimer timer;
            const bool ok = chain.softDeleteBlock(
                static_cast<std::size_t>(g_deleteBlockIndex), msg);
            g_lastDeleteSeconds = timer.elapsedSeconds();

            PushToast(ok ? msg : ("Soft delete failed: " + msg),
                      ok ? COL_GREEN_BR : COL_RED);

            
            cw1::OperationTimer vt;
            g_lastVerify = chain.verifyIntegrity();
            g_lastVerifySeconds = vt.elapsedSeconds();
            g_verifyDone = true;
            g_deleteConfirm = false;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    
    ImGui::TextColored(COL_RED, "HARD DELETE (destructive)");
    ImGui::Spacing();

    ImGui::Checkbox("I confirm hard delete##hd_confirm", &g_deleteConfirm);
    ImGui::Spacing();

    ImGui::BeginDisabled(!g_deleteConfirm);
    if (DrawDangerButton("Hard Delete##hard_del", ImVec2(180.0f, 36.0f))) {
        if (g_deleteBlockIndex < 0 ||
            static_cast<std::size_t>(g_deleteBlockIndex) >= chain.totalBlocks()) {
            PushToast("Invalid block index.", COL_RED);
        } else {
            std::string msg;
            cw1::OperationTimer timer;
            const bool ok = chain.hardDeleteBlock(
                static_cast<std::size_t>(g_deleteBlockIndex), msg);
            g_lastDeleteSeconds = timer.elapsedSeconds();

            PushToast(ok ? msg : ("Hard delete failed: " + msg),
                      ok ? COL_GREEN_BR : COL_RED);

            
            cw1::OperationTimer vt;
            g_lastVerify = chain.verifyIntegrity();
            g_lastVerifySeconds = vt.elapsedSeconds();
            g_verifyDone = true;
            g_deleteConfirm = false;
        }
    }
    ImGui::EndDisabled();

    if (g_lastDeleteSeconds > 0.0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_VERY_MUTED, "Last delete operation took: %s s",
                           cw1::formatSeconds(g_lastDeleteSeconds).c_str());
    }
}





static void RenderToasts(float deltaTime) {
    
    g_toasts.erase(
        std::remove_if(g_toasts.begin(), g_toasts.end(),
                       [](const Toast& t) { return t.remainingSecs <= 0.0f; }),
        g_toasts.end());

    const float toastW = 340.0f;
    const float toastH = 44.0f;
    const float margin = 20.0f;
    const float topY   = 70.0f;

    ImGuiIO& io = ImGui::GetIO();

    for (size_t i = 0; i < g_toasts.size(); ++i) {
        Toast& t = g_toasts[i];

        
        float alpha = (t.remainingSecs < 0.5f) ? (t.remainingSecs / 0.5f) : 1.0f;
        if (alpha < 0.0f) alpha = 0.0f;

        float posX = io.DisplaySize.x - toastW - margin;
        float posY = topY + static_cast<float>(i) * (toastH + 8.0f);

        char wname[32];
        snprintf(wname, sizeof(wname), "##toast%zu", i);

        ImGui::SetNextWindowPos(ImVec2(posX, posY));
        ImGui::SetNextWindowSize(ImVec2(toastW, toastH));
        ImGui::SetNextWindowBgAlpha(0.92f * alpha);

        ImGuiWindowFlags wf =
            ImGuiWindowFlags_NoDecoration       |
            ImGuiWindowFlags_NoMove             |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNav              |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, t.color);
        ImGui::PushStyleColor(ImGuiCol_Border,   t.color);
        if (ImGui::Begin(wname, nullptr, wf)) {
            ImGui::SetCursorPos(ImVec2(12.0f, 12.0f));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha),
                               "%s", t.msg.c_str());
        }
        ImGui::End();
        ImGui::PopStyleColor(2);

        t.remainingSecs -= deltaTime;
    }
}





int main() {
    
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1400, 900, "Car Warehouse Blockchain System", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ApplyGitHubDarkTheme();

    
    {
        auto TryLoadFont = [&](const char* path, float sizePx) -> ImFont* {
            FILE* fp = fopen(path, "rb");
            if (fp == nullptr) {
                return nullptr;
            }
            fclose(fp);
            return io.Fonts->AddFontFromFileTTF(path, sizePx);
        };

#ifdef _WIN32
        g_fontBody = TryLoadFont("C:/Windows/Fonts/segoeui.ttf", 16.0f);
        g_fontSection = TryLoadFont("C:/Windows/Fonts/segoeuib.ttf", 22.0f);
        g_fontTitle = TryLoadFont("C:/Windows/Fonts/segoeuib.ttf", 26.0f);
#endif

        if (g_fontBody == nullptr) {
            g_fontBody = TryLoadFont("fonts/SegoeUI.ttf", 16.0f);
        }
        if (g_fontSection == nullptr) {
            g_fontSection = TryLoadFont("fonts/SegoeUI-Bold.ttf", 22.0f);
        }
        if (g_fontTitle == nullptr) {
            g_fontTitle = TryLoadFont("fonts/SegoeUI-Bold.ttf", 26.0f);
        }

        if (g_fontBody == nullptr) {
            g_fontBody = TryLoadFont("fonts/JetBrainsMono-Regular.ttf", 16.0f);
        }
        if (g_fontSection == nullptr) {
            g_fontSection = TryLoadFont("fonts/JetBrainsMono-Regular.ttf", 22.0f);
        }
        if (g_fontTitle == nullptr) {
            g_fontTitle = TryLoadFont("fonts/JetBrainsMono-Regular.ttf", 26.0f);
        }

        if (g_fontBody == nullptr) {
            g_fontBody = io.Fonts->AddFontDefault();
        }
        if (g_fontSection == nullptr) {
            g_fontSection = g_fontBody;
        }
        if (g_fontTitle == nullptr) {
            g_fontTitle = g_fontSection;
        }
        io.FontDefault = g_fontBody;
    }


    cw1::Blockchain chain;
    chain.openDatabase("cw1_blockchain.db");
    bool loadedFromDB = false;
    if (chain.isDatabaseOpen()) {
        loadedFromDB = chain.loadFromDB() && chain.totalBlocks() > 0;
    }
    if (!loadedFromDB) {
        loadDemoData(chain);
        if (chain.isDatabaseOpen()) {
            chain.saveToDB();
        }
    }
    g_lastVerifySeconds = cw1::measureSeconds([&]() {
        g_lastVerify = chain.verifyIntegrity();
    });
    g_verifyDone = true;

    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_MAIN);
        ImGui::Begin("##root", nullptr,
            ImGuiWindowFlags_NoDecoration          |
            ImGuiWindowFlags_NoMove                |
            ImGuiWindowFlags_NoResize              |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar           |
            ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PopStyleColor();

        RenderHeader(chain);
        ImGui::Spacing();

        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
        ImGui::BeginChild("##sbwrap", ImVec2(280, -1), false);
        ImGui::PopStyleColor();
        RenderSidebar(chain);
        ImGui::EndChild();

        ImGui::SameLine();

        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_MAIN);
        ImGui::BeginChild("##content", ImVec2(-1, -1), false,
                          ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PopStyleColor();
        ImGui::Spacing(); ImGui::Spacing();

        switch (g_view) {
        case View::DASHBOARD:    RenderDashboard(chain);    break;
        case View::CAR_DETAIL:   RenderCarDetail(chain);    break;
        case View::ADD_BLOCK:    RenderAddBlock(chain);     break;
        case View::GLOBAL_CHAIN: RenderGlobalChain(chain);  break;
        case View::AUDIT_LOG:    RenderAuditLog(chain);     break;
        case View::INTEGRITY:    RenderIntegrity(chain);    break;
        case View::DELETE_BLOCK: RenderDeleteBlock(chain);  break;
        }

        ImGui::EndChild();
        ImGui::End();

        
        RenderToasts(io.DeltaTime);

        
        ImGui::Render();
        int dispW, dispH;
        glfwGetFramebufferSize(window, &dispW, &dispH);
        glViewport(0, 0, dispW, dispH);
        glClearColor(0.051f, 0.067f, 0.090f, 1.0f);  
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```


