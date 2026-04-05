cmake_minimum_required(VERSION 3.16)

# Main coursework project definition.
project(COMP2034_CW1_CarWarehouseBlockchain LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Toggle the ImGui desktop app.
option(BUILD_GUI "Build the ImGui GUI version"   ON)


find_package(OpenSSL REQUIRED)
find_package(OpenGL REQUIRED)


find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    # Fall back to the bundled GLFW copy when the system package is missing.
    message(STATUS "System glfw3 not found -- using third_party/glfw")
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/glfw)
endif()


# Build SQLite from the bundled source so the project stays self-contained.
add_library(sqlite3_lib STATIC third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3_lib PUBLIC third_party/sqlite)
if(MSVC)
    target_compile_options(sqlite3_lib PRIVATE /W0)
else()
    target_compile_options(sqlite3_lib PRIVATE -w)
endif()

# Core logic shared by the GUI targets.
set(BACKEND_SOURCES
    src/models/CarRecord.cpp
    src/blockchain/Block.cpp
    src/blockchain/LinkedBlockchainView.cpp
    src/blockchain/BlockStage.cpp
    src/blockchain/Blockchain.cpp
    src/blockchain/Validation.cpp
    src/utils/AuditLog.cpp
    src/utils/HashUtil.cpp
    src/utils/TimeUtil.cpp
    src/utils/DatabaseManager.cpp
    src/utils/FuelPriceManager.cpp
    src/utils/StringUtil.cpp
    src/utils/BranchMap.cpp
    src/utils/VehicleData.cpp
    src/security/Auth.cpp
    src/security/AccessControl.cpp
    src/security/PendingApprovalManager.cpp
    src/security/SecurityUtil.cpp
    src/ai/ChatbotService.cpp
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

# GUI-only panels and helpers.
set(UI_SOURCES
    src/ui/GuiShared.cpp
    src/ui/ReportExporter.cpp
    src/ui/Layout.cpp
    src/ui/DashboardPanel.cpp
    src/ui/VehiclePanels.cpp
    src/ui/ChainPanels.cpp
    src/ui/AuthPanels.cpp
    src/ui/AIAssistantPanel.cpp
)



if(BUILD_GUI)
    add_executable(car_warehouse_gui
        src/main_gui.cpp
        ${BACKEND_SOURCES}
        ${UI_SOURCES}
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
            winhttp
    )

    add_custom_command(TARGET car_warehouse_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:car_warehouse_gui>/fonts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fonts/JetBrainsMono-Regular.ttf
            $<TARGET_FILE_DIR:car_warehouse_gui>/fonts/JetBrainsMono-Regular.ttf
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:car_warehouse_gui>/database
        COMMENT "Copying JetBrains Mono font next to executable"
    )
    if(MSVC)
        target_compile_options(car_warehouse_gui PRIVATE /W4 /permissive-)
    else()
        target_compile_options(car_warehouse_gui PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()
