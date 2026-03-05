# CW1 AI Review Context

This file contains core project structure and code for AI review.
Excluded: `build/`, `third_party/`, `.git/`, `.claude/`, and generated export markdown files.

## File Structure

```text
cw1/
  .gitignore
  CMakeLists.txt
  COMP2034_CW1_Planning_Summary.md
  include\blockchain\Block.hpp
  include\blockchain\Blockchain.hpp
  include\blockchain\BlockStage.hpp
  include\blockchain\Validation.hpp
  include\models\CarRecord.hpp
  include\utils\AuditLog.hpp
  include\utils\HashUtil.hpp
  include\utils\TimeUtil.hpp
  src\blockchain\Block.cpp
  src\blockchain\Blockchain.cpp
  src\blockchain\BlockStage.cpp
  src\blockchain\Validation.cpp
  src\main.cpp
  src\main_cli.cpp
  src\main_gui.cpp
  src\models\CarRecord.cpp
  src\utils\AuditLog.cpp
  src\utils\HashUtil.cpp
  src\utils\TimeUtil.cpp
```

## Full File Contents

### .gitignore

```text
build/
.cache/
*.exe
*.o
*.obj
*.a
*.dll
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)

project(COMP2034_CW1_CarWarehouseBlockchain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# â”€â”€ Options â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
option(BUILD_CLI "Build the terminal CLI version" ON)
option(BUILD_GUI "Build the ImGui GUI version"   ON)

# â”€â”€ External packages â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
find_package(OpenSSL REQUIRED)
find_package(OpenGL REQUIRED)

# GLFW: prefer system installation, fall back to the cloned subdirectory
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    message(STATUS "System glfw3 not found -- using third_party/glfw")
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/glfw)
endif()

# â”€â”€ Shared backend sources (blockchain engine) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
set(BACKEND_SOURCES
    src/models/CarRecord.cpp
    src/blockchain/Block.cpp
    src/blockchain/BlockStage.cpp
    src/blockchain/Blockchain.cpp
    src/blockchain/Validation.cpp
    src/utils/AuditLog.cpp
    src/utils/HashUtil.cpp
    src/utils/TimeUtil.cpp
)

# â”€â”€ ImGui sources â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
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

# â”€â”€ CLI executable â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
if(BUILD_CLI)
    add_executable(car_warehouse_cli
        src/main_cli.cpp
        ${BACKEND_SOURCES}
    )
    target_include_directories(car_warehouse_cli
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(car_warehouse_cli
        PRIVATE OpenSSL::Crypto
    )
    if(MSVC)
        target_compile_options(car_warehouse_cli PRIVATE /W4 /permissive-)
    else()
        target_compile_options(car_warehouse_cli PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# â”€â”€ GUI executable â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
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
    )
    target_link_libraries(car_warehouse_gui
        PRIVATE
            OpenSSL::Crypto
            glfw
            OpenGL::GL
    )
    # Copy font file next to the executable so the runtime path works
    add_custom_command(TARGET car_warehouse_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:car_warehouse_gui>/fonts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fonts/JetBrainsMono-Regular.ttf
            $<TARGET_FILE_DIR:car_warehouse_gui>/fonts/JetBrainsMono-Regular.ttf
        COMMENT "Copying JetBrains Mono font next to executable"
    )
    if(MSVC)
        target_compile_options(car_warehouse_gui PRIVATE /W3 /permissive-)
    else()
        target_compile_options(car_warehouse_gui PRIVATE -Wall -Wextra)
    endif()
endif()
```

### COMP2034_CW1_Planning_Summary.md

```markdown
# COMP2034 CW1 â€” é¡¹ç›®è§„åˆ’æ€»ç»“
**æ—¥æœŸï¼š** 26 February 2026  
**è¯¾ç¨‹ï¼š** COMP2034 C++ Programming  
**ä½œä¸šï¼š** Coursework 1 â€” Car Warehouse Management System (50%)  
**æˆªæ­¢æ—¥æœŸï¼š** 2 April 2026, 6pm

---

## ä¸€ã€ä½œä¸šåŸºæœ¬è¦æ±‚

### æäº¤ç‰©æ¸…å•
- PDF æŠ¥å‘Šï¼ˆè§£é‡Šä»£ç ã€ç®—æ³•ã€æ•°æ®ç»“æž„ã€å›°éš¾ã€ç»“è®ºã€å‚è€ƒæ–‡çŒ®ï¼‰
- PDF æˆªå›¾ï¼ˆç¨‹åºè¾“å‡º + è¿è¡Œæ—¶é—´ï¼‰
- `.cpp` æºä»£ç æ–‡ä»¶
- `.o` ç›®æ ‡æ–‡ä»¶
- `.exe` æ‰§è¡Œæ–‡ä»¶
- å…¨éƒ¨æ‰“åŒ…æˆ `studentID_Name_CW1.zip` ä¸Šä¼  Moodle

### è¯„åˆ†åˆ†å¸ƒï¼ˆå…±50åˆ†ï¼‰
| éƒ¨åˆ† | åˆ†æ•° |
|------|------|
| æŠ¥å‘Šå†™ä½œ + ä»£ç æ³¨é‡Š | 20åˆ† |
| åŠŸèƒ½æ€§C++ç¨‹åºï¼ˆåŒºå—é“¾ã€æ•°æ®ç»“æž„ç­‰ï¼‰ | 10åˆ† |
| åˆ›æ–°æ€§ã€å¤æ‚åº¦ã€ç‹¬ç‰¹æ€§ | 15åˆ† |
| è¯¾å ‚å±•ç¤º + Q&A | 5åˆ† |

---

## äºŒã€å…³äºŽ Sample Code çš„å®šä½

è€å¸ˆæä¾›çš„ Sample Code **åªæ˜¯èµ·ç‚¹éª¨æž¶**ï¼Œä¸æ˜¯å®Œæ•´ä½œä¸šã€‚

Sample Code æä¾›äº†ï¼š
- åŸºæœ¬ Block ç»“æž„ä½“
- éšæœºç”Ÿæˆ Hash çš„å‡½æ•°ï¼ˆå‡å“ˆå¸Œï¼Œä¸æ˜¯çœŸæ­£SHAï¼‰
- æ—¶é—´æˆ³èŽ·å–
- ç®€å•è¾“å…¥å’Œæ‰“å°åŠŸèƒ½

**ä½ éœ€è¦è‡ªå·±å®žçŽ°çš„ï¼š**
- çœŸæ­£çš„ SHA-256 å“ˆå¸Œç®—æ³•
- æ±½è½¦ä»“åº“ç®¡ç†ç³»ç»Ÿå®Œæ•´ä¸šåŠ¡é€»è¾‘
- Linked Listã€Array of Pointers ç­‰æ•°æ®ç»“æž„
- å®Œæ•´èœå•ç³»ç»Ÿå’ŒåŠŸèƒ½æ¨¡å—
- ä¸“ä¸šä»£ç æ³¨é‡Šå’Œæ–‡æ¡£
- åŠ¨æ€åŒºå—é“¾æŠ€æœ¯

---

## ä¸‰ã€SHA-256 çš„ç†è§£ä¸Žåº”ç”¨

### ä»€ä¹ˆæ˜¯ SHA-256
- æŠŠä»»æ„é•¿åº¦æ•°æ®è½¬æ¢æˆå›ºå®š 64 å­—ç¬¦çš„"æŒ‡çº¹"
- **å•å‘ä¸å¯é€†**â€”â€”æ— æ³•ä»Žå“ˆå¸Œå€¼åæŽ¨åŽŸå§‹æ•°æ®
- åŒæ ·è¾“å…¥æ°¸è¿œå¾—åˆ°åŒæ ·è¾“å‡º
- å“ªæ€•æ”¹å˜ä¸€ä¸ªå­—ç¬¦ï¼Œè¾“å‡ºå®Œå…¨ä¸åŒ

### SHA-256 åœ¨ä½œä¸šä¸­çš„è§’è‰²

**è®¡ç®—æ–¹å¼ï¼š**
```
Block Number + Previous Hash + Timestamp + Info
        â†“ SHA-256 è®¡ç®—
    Current Hashï¼ˆè¯¥å—çš„å”¯ä¸€æŒ‡çº¹ï¼‰
```

**ä¸‰ä¸ªæ ¸å¿ƒä½œç”¨ï¼š**
1. **èº«ä»½è¯æ˜Ž** â€” æ¯ä¸ª Block æœ‰å”¯ä¸€æŒ‡çº¹
2. **é˜²ç¯¡æ”¹** â€” ä»»ä½•æ•°æ®è¢«ä¿®æ”¹ç«‹åˆ»è¢«æ£€æµ‹åˆ°
3. **é“¾å¼è¿žæŽ¥** â€” é€šè¿‡ä¼ é€’å“ˆå¸Œå€¼æŠŠæ‰€æœ‰ Block ä¸²è”æˆä¸å¯åˆ†å‰²çš„é“¾

**é“¾å¼å®‰å…¨åŽŸç†ï¼š**
```
Block 0 (Hash: AAA) â†’ Block 1 (PrevHash: AAA, Hash: BBB) â†’ Block 2 (PrevHash: BBB)
å¦‚æžœæœ‰äººç¯¡æ”¹ Block 1 â†’ BBB æ”¹å˜ â†’ Block 2 çš„ PrevHash å¯¹ä¸ä¸Š â†’ ç¯¡æ”¹è¢«å‘çŽ°
```

**å®žçŽ°æ–¹å¼ï¼ˆOpenSSLåº“ï¼‰ï¼š**
```cpp
#include <openssl/sha.h>

string sha256(const string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input.c_str(), input.size(), hash);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}
```

---

## å››ã€åŒºå—é“¾ç»“æž„è®¾è®¡

### æ¯è¾†è½¦çš„ Block æ•°é‡
ä½œä¸š**æ²¡æœ‰å¼ºåˆ¶è§„å®š**Blockæ•°é‡ï¼ŒåŽŸæ–‡è¯´ï¼š
> "The number of blocks really depends on the idea and concept you have constructed"

**å»ºè®®ï¼šæ¯è¾†è½¦è®¾è®¡ 4ï½ž5 ä¸ª Block**

### æŽ¨èçš„ Block è®¾è®¡ï¼ˆ5ä¸ªé˜¶æ®µï¼‰
```
Block 0 â€” ç”Ÿäº§å‡ºåŽ‚
  Info: åˆ¶é€ å•†IDã€åˆ¶é€ å•†åã€è½¦åž‹ã€å‡ºåŽ‚æ—¥æœŸã€è½¦æž¶å·VIN

Block 1 â€” ä»“åº“å…¥åº“
  Info: ä»“åº“åœ°å€ã€å…¥åº“æ—¥æœŸã€è´Ÿè´£äººã€åº“å­˜ä½ç½®

Block 2 â€” è´¨é‡æ£€æµ‹
  Info: è´¨æ£€å‘˜IDã€æ£€æŸ¥ç»“æžœã€æ£€æŸ¥æ—¥æœŸã€åˆæ ¼è¯ç¼–å·

Block 3 â€” ç»é”€å•†é…é€
  Info: ç»é”€å•†IDã€é…é€æ—¥æœŸã€è¿è¾“æ–¹å¼ã€ç›®çš„åœ°

Block 4 â€” å®¢æˆ·é”€å”®/å”®åŽ
  Info: ä¹°å®¶IDã€é”€å”®æ—¥æœŸã€ä¿ä¿®æœŸã€å®¢æœè®°å½•
```

### å¤šè¾†è½¦çš„æ•°æ®ç»“æž„
```cpp
// ç”¨ Map å­˜å‚¨æ¯è¾†è½¦çš„ç‹¬ç«‹åŒºå—é“¾
map<string, vector<Block>> warehouse;
// key = è½¦æž¶å·VIN, value = è¯¥è½¦çš„Blocké“¾
```

**50è¾†è½¦ Ã— 5ä¸ªBlock = 250ä¸ªBlock**ï¼Œä½†ä¸éœ€è¦æ‰‹åŠ¨åˆ›å»ºï¼Œç¨‹åºè‡ªåŠ¨ç”Ÿæˆã€‚æ¼”ç¤ºæ—¶ 3ï½ž5 è¾†è½¦å°±è¶³å¤Ÿã€‚

---

## äº”ã€UI/ä¸»é¡µè®¾è®¡æ€è·¯

### ä¸»èœå•
```
========================================
   Car Warehouse Management System
========================================
1. Add New Car
2. View All Cars
3. View Car Blockchain (by VIN)
4. Search Car
5. Verify Blockchain Integrity
6. Exit
========================================
```

### View All Cars â€” æ‘˜è¦åˆ—è¡¨ï¼ˆä¸å±•å¼€æ‰€æœ‰Blockï¼‰
```
No. | VIN     | Brand   | Model | Status      | Blocks
----|---------|---------|-------|-------------|-------
1   | VIN001  | Perodua | Axia  | In Stock    | 5
2   | VIN002  | Toyota  | Vios  | Sold        | 5
```
ç”¨æˆ·é€‰æ‹©ç¼–å·åŽï¼Œæ‰å±•å¼€è¯¥è½¦çš„å®Œæ•´åŒºå—é“¾ã€‚

### Search åŠŸèƒ½ï¼ˆåˆ›æ–°åŠ åˆ†é¡¹ï¼‰
```
1. Search by VIN
2. Search by Brand
3. Search by Status
4. Search by Date
```

---

## å…­ã€UI æ¡†æž¶é€‰æ‹©

### æœ€ç»ˆå†³å®šï¼šImGui

**ä¸ºä»€ä¹ˆé€‰ ImGuiï¼š**
- çº¯ C++ åº“ï¼Œå‰åŽç«¯å…¨éƒ¨ C++ å®žçŽ°
- çœŸæ­£çš„æ¡Œé¢çª—å£ç¨‹åºï¼ˆä¸æ˜¯ Terminalï¼‰
- å¯ä»¥åšè¡¨æ ¼ã€æœç´¢æ ã€æŒ‰é’®ã€ä¾§è¾¹æ 
- AI å¯¹ ImGui éžå¸¸ç†Ÿæ‚‰ï¼Œå®¹æ˜“ç”Ÿæˆä»£ç 
- ç¼–è¯‘å‡ºæ¥æ˜¯ `.exe` æ–‡ä»¶ï¼Œå®Œå…¨ç¬¦åˆæäº¤è¦æ±‚
- åœ¨æ¸¸æˆå’Œå·¥ä¸šè½¯ä»¶ç•Œéžå¸¸ä¸“ä¸šï¼Œåå•†åŠ¡é£Žæ ¼

**ImGui å¯ä»¥å®žçŽ°çš„ç•Œé¢åŠŸèƒ½ï¼š**
- å·¦ä¾§ä¾§è¾¹æ æ˜¾ç¤ºæ‰€æœ‰è½¦è¾†åˆ—è¡¨
- å³ä¾§å±•ç¤ºé€‰ä¸­è½¦è¾†çš„å®Œæ•´åŒºå—é“¾
- é¡¶éƒ¨æœç´¢æ å®žæ—¶è¿‡æ»¤
- è¡¨æ ¼æ˜¾ç¤ºæ‰€æœ‰ Block ä¿¡æ¯
- ä¸åŒçŠ¶æ€ç”¨ä¸åŒé¢œè‰²æ ‡æ³¨

### æ—¶é—´ä¼°è®¡
- ImGui å¼€å‘æ—¶é—´çº¦ **2ï½ž3å‘¨**
- éœ€è¦åˆç†åˆ†é…æ—¶é—´ç»™æŠ¥å‘Šå†™ä½œ
- å–„ç”¨ AI è¾…åŠ©ç”Ÿæˆä»£ç å¯ä»¥å¤§å¹…èŠ‚çœæ—¶é—´

---

## ä¸ƒã€å…¶ä»–å¤‡é€‰ UI æ–¹æ¡ˆï¼ˆä¾›å‚è€ƒï¼‰

| é€‰é¡¹ | éš¾åº¦ | è§†è§‰æ•ˆæžœ | å•†åŠ¡æ„Ÿ | å¼€å‘æ—¶é—´ |
|------|------|----------|--------|----------|
| Terminal ANSI | â­ | â­â­ | â­â­ | 1ï½ž2å¤© |
| HTML Export | â­â­ | â­â­â­â­ | â­â­â­â­â­ | 3ï½ž5å¤© |
| **ImGuiï¼ˆé€‰å®šï¼‰** | â­â­â­ | â­â­â­â­ | â­â­â­â­ | 2ï½ž3å‘¨ |
| Raylib | â­â­â­ | â­â­â­â­â­ | â­â­â­ | 3ï½ž4å‘¨ |
| C++ + Web Server | â­â­â­â­ | â­â­â­â­â­ | â­â­â­â­â­ | 3ï½ž4å‘¨ |

---

## å…«ã€ä¸‹ä¸€æ­¥è¡ŒåŠ¨å»ºè®®

1. **ç¡®è®¤å¼€å‘çŽ¯å¢ƒ** â€” å®‰è£… ImGui + OpenSSL åº“
2. **è®¾è®¡æ•°æ®ç»“æž„** â€” å®šä¹‰ Block struct å’Œ Car çš„å®Œæ•´å­—æ®µ
3. **å®žçŽ°æ ¸å¿ƒé€»è¾‘** â€” SHA-256 å“ˆå¸Œ + åŒºå—é“¾é“¾æŽ¥
4. **å»ºç«‹è™šæž„å…¬å¸** â€” è®¾è®¡é©¬æ¥è¥¿äºšæ±½è½¦ä¼ä¸šèƒŒæ™¯èµ„æ–™ï¼ˆç”¨äºŽæŠ¥å‘Šï¼‰
5. **å¼€å‘ ImGui ç•Œé¢** â€” ä¸»èœå•ã€åˆ—è¡¨ã€è¯¦æƒ…ã€æœç´¢
6. **å†™æŠ¥å‘Š** â€” è§£é‡Šæ¯ä¸ªè®¾è®¡å†³å®šï¼Œå¼•ç”¨å‚è€ƒèµ„æ–™
7. **æµ‹è¯• + æˆªå›¾** â€” è®°å½•è¿è¡Œæ—¶é—´å’Œè¾“å‡ºæˆªå›¾

---

*æ­¤æ€»ç»“è®°å½•äº† COMP2034 CW1 è§„åˆ’å¯¹è¯çš„å®Œæ•´å†…å®¹ï¼Œä¾›ä¸‹æ¬¡ç»§ç»­å¼€å‘æ—¶å‚è€ƒã€‚*
```

### include\blockchain\Block.hpp

```cpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "models/CarRecord.hpp"

namespace cw1 {

class Block {
public:
    Block(std::size_t index, std::string previousHash, CarRecord record);

    std::size_t         getIndex()        const noexcept;
    const std::string&  getCurrentHash()  const noexcept;
    const std::string&  getPreviousHash() const noexcept;
    const std::string&  getTimestamp()    const noexcept;
    std::uint64_t       getNonce()        const noexcept;
    const CarRecord&    getRecord()       const noexcept;

    /// Recompute the hash from the block's data (used for integrity checks).
    std::string computeHash() const;

    /// Human-readable one-line summary.
    std::string toString() const;

private:
    static std::uint64_t generateNonce();

    std::size_t    index_;
    std::string    currentHash_;
    std::string    previousHash_;
    std::string    timestamp_;
    std::uint64_t  nonce_;
    CarRecord      record_;
};

} // namespace cw1
```

### include\blockchain\Blockchain.hpp

```cpp
#pragma once

#include <map>
#include <string>
#include <vector>

#include "blockchain/Block.hpp"
#include "blockchain/BlockStage.hpp"
#include "blockchain/Validation.hpp"
#include "models/CarRecord.hpp"
#include "utils/AuditLog.hpp"

namespace cw1 {

/// Hybrid blockchain: single global chain + VIN index for O(1) lookups.
///
/// Architecture:
///   chain_    -- vector<Block>  (immutable, chronological, source of truth)
///   vinIndex_ -- map<string, vector<size_t>>  (VIN -> block indices)
///
/// All blocks from all cars are stored in one chain for global integrity.
/// The VIN index provides fast per-car history lookups without scanning.
class Blockchain {
public:
    // -- Core operations -----------------------------------------------

    /// Append a new block to the global chain and update the VIN index.
    void addBlock(const CarRecord& record);

    /// Return the entire global chain (read-only).
    const std::vector<Block>& getChain() const noexcept;

    /// Total number of blocks across all cars.
    std::size_t totalBlocks() const noexcept;

    // -- Per-car lookups (powered by VIN index) ------------------------

    /// Get pointers to every block belonging to a given VIN (in order).
    std::vector<const Block*> getCarHistory(const std::string& vin) const;

    /// Check whether a VIN exists in the index.
    bool hasVin(const std::string& vin) const;

    /// Return a sorted list of all VINs currently tracked.
    std::vector<std::string> getAllVins() const;

    /// Get the latest stage a car has reached.
    BlockStage getLatestStage(const std::string& vin) const;

    // -- Search / filter -----------------------------------------------

    /// Find all blocks whose manufacturer matches (case-insensitive substring).
    std::vector<const Block*> searchByBrand(const std::string& brand) const;

    /// Find all blocks at a specific lifecycle stage.
    std::vector<const Block*> searchByStage(BlockStage stage) const;

    /// Find all blocks whose VIN, manufacturer, or model contain the query.
    std::vector<const Block*> searchGeneral(const std::string& query) const;

    // -- Integrity -----------------------------------------------------

    /// Verify the entire global chain (hashes + linkage).
    ValidationResult verifyIntegrity() const;

    // -- Audit log -----------------------------------------------------

    /// Access the audit log (const version -- works even on const Blockchain
    /// because auditLog_ is mutable; logging is a non-logical side effect).
    AuditLog& getAuditLog() const noexcept;

    /// Access the audit log (non-const version for direct mutation).
    AuditLog& getAuditLog() noexcept;

private:
    /// The single global chain -- source of truth.
    std::vector<Block> chain_;

    /// VIN index -- maps each VIN to the indices of its blocks in chain_.
    std::map<std::string, std::vector<std::size_t>> vinIndex_;

    /// Audit log -- mutable so it can be updated from const methods.
    /// Logging is a side effect that does not change the blockchain's
    /// logical state, so mutable is semantically correct here.
    mutable AuditLog auditLog_;
};

} // namespace cw1
```

### include\blockchain\BlockStage.hpp

```cpp
#pragma once

#include <string>

namespace cw1 {

/// Lifecycle stages a car passes through in the warehouse system.
enum class BlockStage {
    PRODUCTION,        // Factory manufacture
    WAREHOUSE_INTAKE,  // Arrival at warehouse
    QUALITY_CHECK,     // Inspection / QC
    DEALER_DISPATCH,   // Shipped to dealer
    CUSTOMER_SALE      // Sold to end customer
};

/// Convert a BlockStage value to a human-readable string.
std::string stageToString(BlockStage stage);

/// Convert a string back to BlockStage (case-insensitive). Throws on bad input.
BlockStage stringToStage(const std::string& text);

} // namespace cw1
```

### include\blockchain\Validation.hpp

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

/// Verify hash integrity and chain linkage across all blocks.
ValidationResult verifyChain(const std::vector<Block>& blocks);

} // namespace Validation

} // namespace cw1
```

### include\models\CarRecord.hpp

```cpp
#pragma once

#include <string>

#include "blockchain/BlockStage.hpp"

namespace cw1 {

/// Represents a single event/record attached to a block.
/// Common fields are always present; stage-specific fields are filled
/// depending on which lifecycle stage the block represents.
struct CarRecord {
    // â”€â”€ Common fields (every block has these) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    std::string vin;
    std::string manufacturer;
    std::string model;
    std::string color;
    int         productionYear = 0;
    BlockStage  stage          = BlockStage::PRODUCTION;

    // â”€â”€ Stage-specific fields (filled as needed) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    // PRODUCTION
    std::string factoryLocation;   // e.g. "Shah Alam Plant"

    // WAREHOUSE_INTAKE
    std::string warehouseLocation; // e.g. "WH-A1"
    std::string receivedBy;        // staff name / ID

    // QUALITY_CHECK
    std::string inspectorId;       // e.g. "QC-007"
    bool        passed = false;    // QC result
    std::string qcNotes;           // remarks

    // DEALER_DISPATCH
    std::string dealerId;          // e.g. "DLR-KL-01"
    std::string destination;       // city / address
    std::string transportMode;     // "Truck", "Rail", etc.

    // CUSTOMER_SALE
    std::string buyerId;           // e.g. "CUST-88201"
    double      salePrice = 0.0;   // MYR
    std::string warrantyExpiry;    // ISO date string

    // â”€â”€ Serialisation â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    /// Deterministic pipe-delimited string used as hash input.
    std::string serialize() const;
};

} // namespace cw1
```

### include\utils\AuditLog.hpp

```cpp
#pragma once

// AuditLog.hpp -- Hand-written singly linked list for audit logging.
// Coursework requirement: demonstrates Linked List and Array of Pointers
// as data structures beyond the vector/map used elsewhere.

#include <cstddef>
#include <string>

namespace cw1 {

// -- Audit action types ------------------------------------------------

enum class AuditAction {
    BLOCK_ADDED,        // A new block was appended to the blockchain
    INTEGRITY_CHECK,    // verifyIntegrity() was called
    SEARCH_PERFORMED,   // A search function was called
    CHAIN_VIEWED        // The chain or a car record was displayed
};

/// Convert an AuditAction enum value to a human-readable string.
std::string actionToString(AuditAction action);

// -- Linked list node --------------------------------------------------

/// A single node in the audit linked list.
/// Each node holds the action, a description, a timestamp, and a raw
/// pointer to the next node -- forming a singly linked list.
struct AuditEntry {
    AuditAction action;
    std::string details;    // e.g. "Added block #5 for VIN1001 (Quality Check)"
    std::string timestamp;  // ISO-8601 from TimeUtil::nowIso8601()
    AuditEntry* next;       // Singly linked list pointer to next node

    AuditEntry(AuditAction act, std::string det, std::string ts)
        : action(act), details(std::move(det)), timestamp(std::move(ts)), next(nullptr) {}
};

// -- Singly linked list class ------------------------------------------

/// Hand-written singly linked list for audit logging.
/// New entries are appended at the tail for chronological order (O(1)).
///
/// Coursework requirement:
///   - Demonstrates Linked List: nodes, head/tail pointers, manual traversal,
///     dynamic node allocation and deallocation with new/delete.
///   - Demonstrates Array of Pointers: getRecentEntries() allocates and returns
///     a raw heap array of const AuditEntry* (caller must delete[]).
///   - Demonstrates manual memory management throughout.
class AuditLog {
public:
    AuditLog();
    ~AuditLog();

    // Non-copyable -- owns raw heap-allocated nodes.
    AuditLog(const AuditLog&)            = delete;
    AuditLog& operator=(const AuditLog&) = delete;

    /// Append a new audit entry at the tail -- O(1) due to tail pointer.
    /// Automatically captures the current UTC timestamp via TimeUtil.
    void log(AuditAction action, const std::string& details);

    /// Return the total number of entries -- O(1).
    std::size_t size() const noexcept;

    /// Return the head pointer for manual linked-list traversal (read-only).
    const AuditEntry* head() const noexcept;

    /// Return the most recent maxCount entries as a raw array of pointers.
    ///
    /// Demonstrates: Array of Pointers -- allocates a heap array of
    /// const AuditEntry* using new[]. Array slots point to nodes owned by
    /// this AuditLog (caller must NOT delete the pointed-to entries).
    /// The ARRAY ITSELF must be freed with delete[] by the caller.
    ///
    /// @param maxCount  Maximum number of entries to return.
    /// @param outCount  Set to the actual number of entries returned.
    /// @return Raw heap array of const AuditEntry* -- CALLER MUST delete[].
    const AuditEntry** getRecentEntries(std::size_t maxCount,
                                        std::size_t& outCount) const;

    /// Free all nodes and reset the list to empty.
    void clear();

private:
    AuditEntry* head_;   // First node (oldest entry)
    AuditEntry* tail_;   // Last node (newest entry) -- kept for O(1) append
    std::size_t count_;  // Total node count -- O(1) size()
};

} // namespace cw1
```

### include\utils\HashUtil.hpp

```cpp
#pragma once

#include <string>

namespace cw1::HashUtil {

/// Compute the SHA-256 hash of the input string and return it as a
/// 64-character lowercase hex string.
std::string sha256(const std::string& input);

} // namespace cw1::HashUtil
```

### include\utils\TimeUtil.hpp

```cpp
#pragma once

#include <string>

namespace cw1::TimeUtil {

/// Return the current UTC time as an ISO-8601 string (e.g. "2026-03-02T12:30:00Z").
std::string nowIso8601();

} // namespace cw1::TimeUtil
```

### src\blockchain\Block.cpp

```cpp
#include "blockchain/Block.hpp"

#include <random>
#include <sstream>

#include "utils/HashUtil.hpp"
#include "utils/TimeUtil.hpp"

using namespace std;

namespace cw1 {

Block::Block(size_t index, string previousHash, CarRecord record)
    : index_(index),
      previousHash_(move(previousHash)),
      timestamp_(TimeUtil::nowIso8601()),
      nonce_(generateNonce()),
      record_(move(record)) {
    currentHash_ = computeHash();

    // Coursework spec: the genesis block's previousHash must equal its
    // currentHash. After computing the hash with "0" as previousHash,
    // we update previousHash_ to match currentHash_.
    // computeHash() detects this case (index_ == 0) and always uses "0"
    // so that re-computation during integrity checks remains consistent.
    if (index_ == 0 && previousHash_ == "0") {
        previousHash_ = currentHash_;
    }
}

size_t Block::getIndex() const noexcept {
    return index_;
}

const string& Block::getCurrentHash() const noexcept {
    return currentHash_;
}

const string& Block::getPreviousHash() const noexcept {
    return previousHash_;
}

const string& Block::getTimestamp() const noexcept {
    return timestamp_;
}

uint64_t Block::getNonce() const noexcept {
    return nonce_;
}

const CarRecord& Block::getRecord() const noexcept {
    return record_;
}

string Block::computeHash() const {
    ostringstream payload;
    // For the genesis block (index_ == 0), always use "0" as the previousHash
    // in the hash computation, regardless of the stored previousHash_ value.
    // This is necessary because the constructor sets previousHash_ = currentHash_
    // after the hash is first computed, so we must reproduce the original "0"
    // input to verify the hash correctly during integrity checks.
    const string prevForHash = (index_ == 0) ? string("0") : previousHash_;
    payload << index_ << prevForHash << timestamp_ << nonce_ << record_.serialize();
    return HashUtil::sha256(payload.str());
}

string Block::toString() const {
    ostringstream out;
    out << "Block(index=" << index_
        << ", hash=" << currentHash_.substr(0, 12) << "..."
        << ", prev=" << (previousHash_.size() > 12 ? previousHash_.substr(0, 12) + "..." : previousHash_)
        << ", stage=" << stageToString(record_.stage)
        << ", vin=" << record_.vin << ")";
    return out.str();
}

uint64_t Block::generateNonce() {
    static thread_local mt19937_64 engine{random_device{}()};
    static thread_local uniform_int_distribution<uint64_t> dist;
    return dist(engine);
}

} // namespace cw1
```

### src\blockchain\Blockchain.cpp

```cpp
#include "blockchain/Blockchain.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace cw1 {

namespace {

/// Case-insensitive substring search.
bool containsIgnoreCase(const string& haystack, const string& needle) {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;

    auto toLower = [](unsigned char c) { return static_cast<char>(tolower(c)); };

    string h, n;
    h.resize(haystack.size());
    n.resize(needle.size());
    transform(haystack.begin(), haystack.end(), h.begin(), toLower);
    transform(needle.begin(), needle.end(), n.begin(), toLower);

    return h.find(n) != string::npos;
}

} // namespace

// -- Core operations --------------------------------------------------

void Blockchain::addBlock(const CarRecord& record) {
    // 1. Determine the previous hash (genesis uses "0").
    const string prevHash =
        chain_.empty() ? "0" : chain_.back().getCurrentHash();

    // 2. Create the block and append to the global chain.
    chain_.emplace_back(chain_.size(), prevHash, record);

    // 3. Update the VIN index.
    vinIndex_[record.vin].push_back(chain_.size() - 1);

    // 4. Log the block addition to the audit trail.
    const Block& added = chain_.back();
    ostringstream detail;
    detail << "Added block #" << added.getIndex()
           << " for " << record.vin
           << " (" << stageToString(record.stage) << ")";
    auditLog_.log(AuditAction::BLOCK_ADDED, detail.str());
}

const vector<Block>& Blockchain::getChain() const noexcept {
    return chain_;
}

size_t Blockchain::totalBlocks() const noexcept {
    return chain_.size();
}

// -- Per-car lookups --------------------------------------------------

vector<const Block*> Blockchain::getCarHistory(const string& vin) const {
    vector<const Block*> history;
    auto it = vinIndex_.find(vin);
    if (it != vinIndex_.end()) {
        history.reserve(it->second.size());
        for (size_t idx : it->second) {
            history.push_back(&chain_[idx]);
        }
    }
    return history;
}

bool Blockchain::hasVin(const string& vin) const {
    return vinIndex_.count(vin) > 0;
}

vector<string> Blockchain::getAllVins() const {
    vector<string> vins;
    vins.reserve(vinIndex_.size());
    for (const auto& [vin, indices] : vinIndex_) {
        vins.push_back(vin);
    }
    return vins;
}

BlockStage Blockchain::getLatestStage(const string& vin) const {
    auto it = vinIndex_.find(vin);
    if (it == vinIndex_.end() || it->second.empty()) {
        throw runtime_error("VIN not found: " + vin);
    }
    // The last index in the vector is the most recent event for this car.
    return chain_[it->second.back()].getRecord().stage;
}

// -- Search / filter --------------------------------------------------

vector<const Block*> Blockchain::searchByBrand(const string& brand) const {
    vector<const Block*> results;
    for (const auto& block : chain_) {
        if (containsIgnoreCase(block.getRecord().manufacturer, brand)) {
            results.push_back(&block);
        }
    }
    // Log the search in the audit trail (auditLog_ is mutable -- see header).
    ostringstream detail;
    detail << "searchByBrand(\"" << brand << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

vector<const Block*> Blockchain::searchByStage(BlockStage stage) const {
    vector<const Block*> results;
    for (const auto& block : chain_) {
        if (block.getRecord().stage == stage) {
            results.push_back(&block);
        }
    }
    // Log the search in the audit trail (auditLog_ is mutable -- see header).
    ostringstream detail;
    detail << "searchByStage(" << stageToString(stage) << ") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

vector<const Block*> Blockchain::searchGeneral(const string& query) const {
    vector<const Block*> results;
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
    // Log the search in the audit trail (auditLog_ is mutable -- see header).
    ostringstream detail;
    detail << "searchGeneral(\"" << query << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

// -- Integrity --------------------------------------------------------

ValidationResult Blockchain::verifyIntegrity() const {
    ValidationResult result = Validation::verifyChain(chain_);
    // Log the integrity check result in the audit trail.
    ostringstream detail;
    detail << "verifyIntegrity() -> " << (result.ok ? "PASS" : "FAIL")
           << ": " << result.message;
    auditLog_.log(AuditAction::INTEGRITY_CHECK, detail.str());
    return result;
}

// -- Audit log --------------------------------------------------------

AuditLog& Blockchain::getAuditLog() const noexcept {
    // auditLog_ is mutable, so we can return a non-const reference from
    // a const method. This lets callers log CHAIN_VIEWED events even when
    // they only hold a const Blockchain&.
    return auditLog_;
}

AuditLog& Blockchain::getAuditLog() noexcept {
    return auditLog_;
}

} // namespace cw1
```

### src\blockchain\BlockStage.cpp

```cpp
#include "blockchain/BlockStage.hpp"

#include <algorithm>
#include <stdexcept>

using namespace std;

namespace cw1 {

string stageToString(BlockStage stage) {
    switch (stage) {
        case BlockStage::PRODUCTION:       return "Production";
        case BlockStage::WAREHOUSE_INTAKE: return "Warehouse Intake";
        case BlockStage::QUALITY_CHECK:    return "Quality Check";
        case BlockStage::DEALER_DISPATCH:  return "Dealer Dispatch";
        case BlockStage::CUSTOMER_SALE:    return "Customer Sale";
    }
    return "Unknown";
}

BlockStage stringToStage(const string& text) {
    // Work with a lowercase copy for case-insensitive matching.
    string lower = text;
    transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(tolower(c)); });

    if (lower == "production")       return BlockStage::PRODUCTION;
    if (lower == "warehouse intake") return BlockStage::WAREHOUSE_INTAKE;
    if (lower == "quality check")    return BlockStage::QUALITY_CHECK;
    if (lower == "dealer dispatch")  return BlockStage::DEALER_DISPATCH;
    if (lower == "customer sale")    return BlockStage::CUSTOMER_SALE;

    throw invalid_argument("Unknown stage: " + text);
}

} // namespace cw1
```

### src\blockchain\Validation.cpp

```cpp
#include "blockchain/Validation.hpp"

using namespace std;

namespace cw1::Validation {

ValidationResult verifyChain(const vector<Block>& blocks) {
    if (blocks.empty()) {
        return {true, "Blockchain is empty and therefore valid."};
    }

    for (size_t i = 0; i < blocks.size(); ++i) {
        const Block& current = blocks[i];

        // Check that the stored hash matches a fresh computation.
        if (current.getCurrentHash() != current.computeHash()) {
            return {false, "Hash mismatch at block index " + to_string(i)};
        }

        if (i == 0) {
            // Coursework spec: the genesis block's previousHash must equal
            // its own currentHash (set in the Block constructor after the
            // hash is computed with "0").
            if (current.getPreviousHash() != current.getCurrentHash()) {
                return {false, "Genesis block previousHash must equal its currentHash."};
            }
            continue;
        }

        // Every subsequent block must reference the previous block's hash.
        const Block& previous = blocks[i - 1];
        if (current.getPreviousHash() != previous.getCurrentHash()) {
            return {false, "Chain link mismatch at block index " + to_string(i)};
        }
    }

    return {true, "Blockchain integrity verified. " + to_string(blocks.size()) + " block(s) OK."};
}

} // namespace cw1::Validation
```

### src\main.cpp

```cpp
#include <iostream>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>

#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"

using namespace std;

namespace {

// Ã¢â€â‚¬Ã¢â€â‚¬ Helper: safe integer parsing Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

int parseIntOrDefault(const string& value, int fallback) {
    try {
        return stoi(value);
    } catch (...) {
        return fallback;
    }
}

double parseDoubleOrDefault(const string& value, double fallback) {
    try {
        return stod(value);
    } catch (...) {
        return fallback;
    }
}

// Ã¢â€â‚¬Ã¢â€â‚¬ Helper: read a line with prompt Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

string prompt(const string& label) {
    string value;
    cout << "  " << label << ": ";
    getline(cin, value);
    return value;
}

// Ã¢â€â‚¬Ã¢â€â‚¬ Demo data Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

void loadDemoData(cw1::Blockchain& chain) {
    cout << "\n  Loading demo fleet...\n";

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 1: Perodua Axia (full lifecycle Ã¢â‚¬â€ all 5 stages) Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1001"; r.manufacturer = "Perodua"; r.model = "Axia";
        r.color = "Silver"; r.productionYear = 2024;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Shah Alam Plant";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-A1"; r.receivedBy = "Ahmad bin Ismail";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-001"; r.passed = true; r.qcNotes = "All systems nominal";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KB-01"; r.destination = "Kota Bharu"; r.transportMode = "Truck";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::CUSTOMER_SALE;
        r.buyerId = "CUST-10201"; r.salePrice = 38000.00; r.warrantyExpiry = "2029-03-01";
        chain.addBlock(r);
    }

    // Small delay so timestamps differ between cars.
    this_thread::sleep_for(chrono::milliseconds(50));

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 2: Toyota Vios (4 stages Ã¢â‚¬â€ awaiting sale) Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1002"; r.manufacturer = "Toyota"; r.model = "Vios";
        r.color = "White"; r.productionYear = 2023;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Toyota Bukit Raja";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-B2"; r.receivedBy = "Lim Wei Jie";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-003"; r.passed = true; r.qcNotes = "Minor paint scratch Ã¢â‚¬â€ touched up";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KL-05"; r.destination = "Kuala Lumpur"; r.transportMode = "Truck";
        chain.addBlock(r);
    }

    this_thread::sleep_for(chrono::milliseconds(50));

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 3: Honda City (3 stages Ã¢â‚¬â€ in QC) Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1003"; r.manufacturer = "Honda"; r.model = "City";
        r.color = "Blue"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Honda Pegoh, Melaka";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-C3"; r.receivedBy = "Raj Kumar";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-007"; r.passed = false; r.qcNotes = "Brake pad below spec Ã¢â‚¬â€ pending replacement";
        chain.addBlock(r);
    }

    this_thread::sleep_for(chrono::milliseconds(50));

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 4: Proton X50 (2 stages Ã¢â‚¬â€ just arrived at warehouse) Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1004"; r.manufacturer = "Proton"; r.model = "X50";
        r.color = "Red"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Proton Tanjung Malim";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-D4"; r.receivedBy = "Nurul Aina";
        chain.addBlock(r);
    }

    this_thread::sleep_for(chrono::milliseconds(50));

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 5: Perodua Myvi (1 stage Ã¢â‚¬â€ just manufactured) Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1005"; r.manufacturer = "Perodua"; r.model = "Myvi";
        r.color = "Black"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Perodua Rawang";
        chain.addBlock(r);
    }

    // Print summary
    const auto vins = chain.getAllVins();
    cout << "  Loaded " << vins.size() << " cars, "
         << chain.totalBlocks() << " total blocks.\n\n";

    // Verify integrity after loading
    const auto result = chain.verifyIntegrity();
    cout << "  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " Ã¢â‚¬â€ " << result.message << "\n\n";
}

// Ã¢â€â‚¬Ã¢â€â‚¬ Display helpers Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

void printSeparator() {
    cout << "  " << string(60, '-') << '\n';
}

void printBlockDetail(const cw1::Block& block) {
    const cw1::CarRecord& r = block.getRecord();

    printSeparator();
    cout << "  Block #" << block.getIndex()
         << "  |  " << cw1::stageToString(r.stage) << '\n';
    printSeparator();
    cout << "  Timestamp   : " << block.getTimestamp() << '\n';
    cout << "  Nonce       : " << block.getNonce() << '\n';
    cout << "  PrevHash    : " << block.getPreviousHash() << '\n';
    cout << "  CurrentHash : " << block.getCurrentHash() << '\n';
    cout << "  VIN         : " << r.vin << '\n';
    cout << "  Car         : " << r.manufacturer << " " << r.model
         << " (" << r.color << ", " << r.productionYear << ")\n";

    // Print stage-specific fields
    switch (r.stage) {
        case cw1::BlockStage::PRODUCTION:
            cout << "  Factory     : " << r.factoryLocation << '\n';
            break;
        case cw1::BlockStage::WAREHOUSE_INTAKE:
            cout << "  Warehouse   : " << r.warehouseLocation << '\n';
            cout << "  Received By : " << r.receivedBy << '\n';
            break;
        case cw1::BlockStage::QUALITY_CHECK:
            cout << "  Inspector   : " << r.inspectorId << '\n';
            cout << "  QC Result   : " << (r.passed ? "PASSED" : "FAILED") << '\n';
            cout << "  QC Notes    : " << r.qcNotes << '\n';
            break;
        case cw1::BlockStage::DEALER_DISPATCH:
            cout << "  Dealer      : " << r.dealerId << '\n';
            cout << "  Destination : " << r.destination << '\n';
            cout << "  Transport   : " << r.transportMode << '\n';
            break;
        case cw1::BlockStage::CUSTOMER_SALE:
            cout << "  Buyer       : " << r.buyerId << '\n';
            cout << "  Sale Price  : MYR " << fixed << setprecision(2) << r.salePrice << '\n';
            cout << "  Warranty    : " << r.warrantyExpiry << '\n';
            break;
    }
}

// Ã¢â€â‚¬Ã¢â€â‚¬ Menu actions Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

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
        if (history.empty()) continue;

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

    // Log that the car summary was viewed.
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed all cars summary (" + to_string(vins.size()) + " VINs)");
}

void viewCarByVin(const cw1::Blockchain& chain) {
    string vin = prompt("Enter VIN");
    if (!chain.hasVin(vin)) {
        cout << "\n  VIN \"" << vin << "\" not found.\n\n";
        return;
    }

    const auto history = chain.getCarHistory(vin);
    cout << "\n  Blockchain for " << vin << " (" << history.size() << " blocks):\n";

    for (const auto* block : history) {
        printBlockDetail(*block);
    }
    printSeparator();
    cout << '\n';

    // Log that this VIN chain was viewed.
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed blockchain for " + vin + " (" + to_string(history.size()) + " blocks)");
}

void addNewCarStage(cw1::Blockchain& chain) {
    cout << "\n  Select lifecycle stage to add:\n";
    cout << "  1. Production\n";
    cout << "  2. Warehouse Intake\n";
    cout << "  3. Quality Check\n";
    cout << "  4. Dealer Dispatch\n";
    cout << "  5. Customer Sale\n";

    string choice = prompt("Stage (1-5)");
    int stageNum = parseIntOrDefault(choice, 0);
    if (stageNum < 1 || stageNum > 5) {
        cout << "  Invalid stage.\n\n";
        return;
    }

    cw1::CarRecord r;

    // Common fields
    r.vin              = prompt("VIN");
    r.manufacturer     = prompt("Manufacturer");
    r.model            = prompt("Model");
    r.color            = prompt("Color");
    r.productionYear   = parseIntOrDefault(prompt("Production Year"), 0);

    // Stage-specific fields
    switch (stageNum) {
        case 1:
            r.stage = cw1::BlockStage::PRODUCTION;
            r.factoryLocation = prompt("Factory Location");
            break;
        case 2:
            r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
            r.warehouseLocation = prompt("Warehouse Location");
            r.receivedBy = prompt("Received By");
            break;
        case 3:
            r.stage = cw1::BlockStage::QUALITY_CHECK;
            r.inspectorId = prompt("Inspector ID");
            r.passed = (prompt("Passed? (y/n)") == "y");
            r.qcNotes = prompt("QC Notes");
            break;
        case 4:
            r.stage = cw1::BlockStage::DEALER_DISPATCH;
            r.dealerId = prompt("Dealer ID");
            r.destination = prompt("Destination");
            r.transportMode = prompt("Transport Mode");
            break;
        case 5:
            r.stage = cw1::BlockStage::CUSTOMER_SALE;
            r.buyerId = prompt("Buyer ID");
            r.salePrice = parseDoubleOrDefault(prompt("Sale Price (MYR)"), 0.0);
            r.warrantyExpiry = prompt("Warranty Expiry (YYYY-MM-DD)");
            break;
    }

    chain.addBlock(r);
    cout << "\n  Block added. Total blocks: " << chain.totalBlocks() << "\n\n";
}

void searchCars(const cw1::Blockchain& chain) {
    cout << "\n  Search by:\n";
    cout << "  1. General (VIN / Brand / Model / Color)\n";
    cout << "  2. Brand\n";
    cout << "  3. Lifecycle Stage\n";

    string choice = prompt("Choice (1-3)");

    vector<const cw1::Block*> results;

    if (choice == "1") {
        string query = prompt("Search query");
        results = chain.searchGeneral(query);
    } else if (choice == "2") {
        string brand = prompt("Brand name");
        results = chain.searchByBrand(brand);
    } else if (choice == "3") {
        cout << "  1=Production  2=Intake  3=QC  4=Dispatch  5=Sale\n";
        int s = parseIntOrDefault(prompt("Stage (1-5)"), 0);
        if (s >= 1 && s <= 5) {
            results = chain.searchByStage(static_cast<cw1::BlockStage>(s - 1));
        }
    } else {
        cout << "  Invalid choice.\n\n";
        return;
    }

    cout << "\n  Found " << results.size() << " block(s):\n";
    for (const auto* block : results) {
        printBlockDetail(*block);
    }
    if (!results.empty()) printSeparator();
    cout << '\n';
}

void verifyIntegrity(const cw1::Blockchain& chain) {
    const auto result = chain.verifyIntegrity();
    cout << "\n  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " Ã¢â‚¬â€ " << result.message << "\n\n";
}

void printGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    if (blocks.empty()) {
        cout << "\n  Global chain is empty.\n\n";
        return;
    }

    cout << "\n  Global Chain (" << blocks.size() << " blocks):\n";
    for (const auto& block : blocks) {
        const auto& r = block.getRecord();
        cout << "  [" << setw(3) << block.getIndex() << "] "
             << left << setw(10) << r.vin
             << setw(18) << cw1::stageToString(r.stage)
             << setw(10) << r.manufacturer
             << block.getCurrentHash().substr(0, 16) << "...\n";
    }
    cout << '\n';

    // Log that the global chain was viewed.
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed global chain (" + to_string(blocks.size()) + " blocks)");
}
/// Display the most recent N entries from the audit log.
/// Demonstrates Array of Pointers: getRecentEntries() returns a heap-
/// allocated array of const AuditEntry* that we iterate and then delete[].
void viewAuditLog(const cw1::Blockchain& chain) {
    const cw1::AuditLog& log = chain.getAuditLog();
    cout << "\n  Audit Log -- Total entries: " << log.size() << "\n";

    string input = prompt("View last how many entries? (default 10)");
    size_t n = 10;
    if (!input.empty()) {
        int parsed = parseIntOrDefault(input, 10);
        n = (parsed > 0) ? static_cast<size_t>(parsed) : 10;
    }

    size_t actualCount = 0;
    // getRecentEntries demonstrates Array of Pointers: a heap-allocated
    // raw array (const AuditEntry**) where each slot is a pointer to a
    // linked list node. We must delete[] the array after use.
    const cw1::AuditEntry** arr = log.getRecentEntries(n, actualCount);

    if (arr == nullptr || actualCount == 0) {
        cout << "\n  No audit entries to display.\n\n";
        return;
    }

    cout << "\n  Showing last " << actualCount << " entries:\n";
    printSeparator();

    // Iterate the raw pointer array -- demonstrates array-of-pointers access.
    for (size_t i = 0; i < actualCount; ++i) {
        const cw1::AuditEntry* entry = arr[i];
        cout << "  [" << setw(4) << (i + 1) << "] "
             << left << setw(22) << entry->timestamp
             << setw(20) << cw1::actionToString(entry->action)
             << entry->details << "\n";
    }

    printSeparator();
    cout << "\n";

    // Free the array of pointers (entries themselves are owned by AuditLog).
    delete[] arr;
}

} // namespace

// Ã¢â€â‚¬Ã¢â€â‚¬ Main Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

int main() {
    cw1::Blockchain chain;

    cout << "\n  ========================================\n";
    cout << "    Car Warehouse Blockchain System\n";
    cout << "    Hybrid Architecture (Global + Index)\n";
    cout << "  ========================================\n\n";

    loadDemoData(chain);

    while (true) {
        cout << "  ---- MENU ----\n";
        cout << "  1. View All Cars (summary)\n";
        cout << "  2. View Car Blockchain (by VIN)\n";
        cout << "  3. Add New Block (lifecycle stage)\n";
        cout << "  4. Search\n";
        cout << "  5. View Global Chain\n";
        cout << "  6. Verify Integrity\n";
        cout << "  7. View Audit Log\n";
    cout << "  8. Exit\n";

        string choice = prompt("Select (1-8)");

        if (choice == "1") { viewAllCars(chain); continue; }
        if (choice == "2") { viewCarByVin(chain); continue; }
        if (choice == "3") { addNewCarStage(chain); continue; }
        if (choice == "4") { searchCars(chain); continue; }
        if (choice == "5") { printGlobalChain(chain); continue; }
        if (choice == "6") { verifyIntegrity(chain); continue; }
        if (choice == "7") { viewAuditLog(chain); continue; }
        if (choice == "8") { cout << "\n  Exiting.\n"; break; }

        cout << "  Invalid choice.\n\n";
    }

    return 0;
}
```

### src\main_cli.cpp

```cpp
#include <iostream>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>

#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"

using namespace std;

namespace {

// Ã¢â€â‚¬Ã¢â€â‚¬ Helper: safe integer parsing Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

int parseIntOrDefault(const string& value, int fallback) {
    try {
        return stoi(value);
    } catch (...) {
        return fallback;
    }
}

double parseDoubleOrDefault(const string& value, double fallback) {
    try {
        return stod(value);
    } catch (...) {
        return fallback;
    }
}

// Ã¢â€â‚¬Ã¢â€â‚¬ Helper: read a line with prompt Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

string prompt(const string& label) {
    string value;
    cout << "  " << label << ": ";
    getline(cin, value);
    return value;
}

// Ã¢â€â‚¬Ã¢â€â‚¬ Demo data Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

void loadDemoData(cw1::Blockchain& chain) {
    cout << "\n  Loading demo fleet...\n";

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 1: Perodua Axia (full lifecycle Ã¢â‚¬â€ all 5 stages) Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1001"; r.manufacturer = "Perodua"; r.model = "Axia";
        r.color = "Silver"; r.productionYear = 2024;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Shah Alam Plant";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-A1"; r.receivedBy = "Ahmad bin Ismail";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-001"; r.passed = true; r.qcNotes = "All systems nominal";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KB-01"; r.destination = "Kota Bharu"; r.transportMode = "Truck";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::CUSTOMER_SALE;
        r.buyerId = "CUST-10201"; r.salePrice = 38000.00; r.warrantyExpiry = "2029-03-01";
        chain.addBlock(r);
    }

    // Small delay so timestamps differ between cars.
    this_thread::sleep_for(chrono::milliseconds(50));

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 2: Toyota Vios (4 stages Ã¢â‚¬â€ awaiting sale) Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1002"; r.manufacturer = "Toyota"; r.model = "Vios";
        r.color = "White"; r.productionYear = 2023;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Toyota Bukit Raja";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-B2"; r.receivedBy = "Lim Wei Jie";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-003"; r.passed = true; r.qcNotes = "Minor paint scratch Ã¢â‚¬â€ touched up";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KL-05"; r.destination = "Kuala Lumpur"; r.transportMode = "Truck";
        chain.addBlock(r);
    }

    this_thread::sleep_for(chrono::milliseconds(50));

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 3: Honda City (3 stages Ã¢â‚¬â€ in QC) Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1003"; r.manufacturer = "Honda"; r.model = "City";
        r.color = "Blue"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Honda Pegoh, Melaka";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-C3"; r.receivedBy = "Raj Kumar";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-007"; r.passed = false; r.qcNotes = "Brake pad below spec Ã¢â‚¬â€ pending replacement";
        chain.addBlock(r);
    }

    this_thread::sleep_for(chrono::milliseconds(50));

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 4: Proton X50 (2 stages Ã¢â‚¬â€ just arrived at warehouse) Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1004"; r.manufacturer = "Proton"; r.model = "X50";
        r.color = "Red"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Proton Tanjung Malim";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-D4"; r.receivedBy = "Nurul Aina";
        chain.addBlock(r);
    }

    this_thread::sleep_for(chrono::milliseconds(50));

    // Ã¢â€â‚¬Ã¢â€â‚¬ Car 5: Perodua Myvi (1 stage Ã¢â‚¬â€ just manufactured) Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    {
        cw1::CarRecord r;
        r.vin = "VIN1005"; r.manufacturer = "Perodua"; r.model = "Myvi";
        r.color = "Black"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Perodua Rawang";
        chain.addBlock(r);
    }

    // Print summary
    const auto vins = chain.getAllVins();
    cout << "  Loaded " << vins.size() << " cars, "
         << chain.totalBlocks() << " total blocks.\n\n";

    // Verify integrity after loading
    const auto result = chain.verifyIntegrity();
    cout << "  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " Ã¢â‚¬â€ " << result.message << "\n\n";
}

// Ã¢â€â‚¬Ã¢â€â‚¬ Display helpers Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

void printSeparator() {
    cout << "  " << string(60, '-') << '\n';
}

void printBlockDetail(const cw1::Block& block) {
    const cw1::CarRecord& r = block.getRecord();

    printSeparator();
    cout << "  Block #" << block.getIndex()
         << "  |  " << cw1::stageToString(r.stage) << '\n';
    printSeparator();
    cout << "  Timestamp   : " << block.getTimestamp() << '\n';
    cout << "  Nonce       : " << block.getNonce() << '\n';
    cout << "  PrevHash    : " << block.getPreviousHash() << '\n';
    cout << "  CurrentHash : " << block.getCurrentHash() << '\n';
    cout << "  VIN         : " << r.vin << '\n';
    cout << "  Car         : " << r.manufacturer << " " << r.model
         << " (" << r.color << ", " << r.productionYear << ")\n";

    // Print stage-specific fields
    switch (r.stage) {
        case cw1::BlockStage::PRODUCTION:
            cout << "  Factory     : " << r.factoryLocation << '\n';
            break;
        case cw1::BlockStage::WAREHOUSE_INTAKE:
            cout << "  Warehouse   : " << r.warehouseLocation << '\n';
            cout << "  Received By : " << r.receivedBy << '\n';
            break;
        case cw1::BlockStage::QUALITY_CHECK:
            cout << "  Inspector   : " << r.inspectorId << '\n';
            cout << "  QC Result   : " << (r.passed ? "PASSED" : "FAILED") << '\n';
            cout << "  QC Notes    : " << r.qcNotes << '\n';
            break;
        case cw1::BlockStage::DEALER_DISPATCH:
            cout << "  Dealer      : " << r.dealerId << '\n';
            cout << "  Destination : " << r.destination << '\n';
            cout << "  Transport   : " << r.transportMode << '\n';
            break;
        case cw1::BlockStage::CUSTOMER_SALE:
            cout << "  Buyer       : " << r.buyerId << '\n';
            cout << "  Sale Price  : MYR " << fixed << setprecision(2) << r.salePrice << '\n';
            cout << "  Warranty    : " << r.warrantyExpiry << '\n';
            break;
    }
}

// Ã¢â€â‚¬Ã¢â€â‚¬ Menu actions Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

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
        if (history.empty()) continue;

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

    // Log that the car summary was viewed.
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed all cars summary (" + to_string(vins.size()) + " VINs)");
}

void viewCarByVin(const cw1::Blockchain& chain) {
    string vin = prompt("Enter VIN");
    if (!chain.hasVin(vin)) {
        cout << "\n  VIN \"" << vin << "\" not found.\n\n";
        return;
    }

    const auto history = chain.getCarHistory(vin);
    cout << "\n  Blockchain for " << vin << " (" << history.size() << " blocks):\n";

    for (const auto* block : history) {
        printBlockDetail(*block);
    }
    printSeparator();
    cout << '\n';

    // Log that this VIN chain was viewed.
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed blockchain for " + vin + " (" + to_string(history.size()) + " blocks)");
}

void addNewCarStage(cw1::Blockchain& chain) {
    cout << "\n  Select lifecycle stage to add:\n";
    cout << "  1. Production\n";
    cout << "  2. Warehouse Intake\n";
    cout << "  3. Quality Check\n";
    cout << "  4. Dealer Dispatch\n";
    cout << "  5. Customer Sale\n";

    string choice = prompt("Stage (1-5)");
    int stageNum = parseIntOrDefault(choice, 0);
    if (stageNum < 1 || stageNum > 5) {
        cout << "  Invalid stage.\n\n";
        return;
    }

    cw1::CarRecord r;

    // Common fields
    r.vin              = prompt("VIN");
    r.manufacturer     = prompt("Manufacturer");
    r.model            = prompt("Model");
    r.color            = prompt("Color");
    r.productionYear   = parseIntOrDefault(prompt("Production Year"), 0);

    // Stage-specific fields
    switch (stageNum) {
        case 1:
            r.stage = cw1::BlockStage::PRODUCTION;
            r.factoryLocation = prompt("Factory Location");
            break;
        case 2:
            r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
            r.warehouseLocation = prompt("Warehouse Location");
            r.receivedBy = prompt("Received By");
            break;
        case 3:
            r.stage = cw1::BlockStage::QUALITY_CHECK;
            r.inspectorId = prompt("Inspector ID");
            r.passed = (prompt("Passed? (y/n)") == "y");
            r.qcNotes = prompt("QC Notes");
            break;
        case 4:
            r.stage = cw1::BlockStage::DEALER_DISPATCH;
            r.dealerId = prompt("Dealer ID");
            r.destination = prompt("Destination");
            r.transportMode = prompt("Transport Mode");
            break;
        case 5:
            r.stage = cw1::BlockStage::CUSTOMER_SALE;
            r.buyerId = prompt("Buyer ID");
            r.salePrice = parseDoubleOrDefault(prompt("Sale Price (MYR)"), 0.0);
            r.warrantyExpiry = prompt("Warranty Expiry (YYYY-MM-DD)");
            break;
    }

    chain.addBlock(r);
    cout << "\n  Block added. Total blocks: " << chain.totalBlocks() << "\n\n";
}

void searchCars(const cw1::Blockchain& chain) {
    cout << "\n  Search by:\n";
    cout << "  1. General (VIN / Brand / Model / Color)\n";
    cout << "  2. Brand\n";
    cout << "  3. Lifecycle Stage\n";

    string choice = prompt("Choice (1-3)");

    vector<const cw1::Block*> results;

    if (choice == "1") {
        string query = prompt("Search query");
        results = chain.searchGeneral(query);
    } else if (choice == "2") {
        string brand = prompt("Brand name");
        results = chain.searchByBrand(brand);
    } else if (choice == "3") {
        cout << "  1=Production  2=Intake  3=QC  4=Dispatch  5=Sale\n";
        int s = parseIntOrDefault(prompt("Stage (1-5)"), 0);
        if (s >= 1 && s <= 5) {
            results = chain.searchByStage(static_cast<cw1::BlockStage>(s - 1));
        }
    } else {
        cout << "  Invalid choice.\n\n";
        return;
    }

    cout << "\n  Found " << results.size() << " block(s):\n";
    for (const auto* block : results) {
        printBlockDetail(*block);
    }
    if (!results.empty()) printSeparator();
    cout << '\n';
}

void verifyIntegrity(const cw1::Blockchain& chain) {
    const auto result = chain.verifyIntegrity();
    cout << "\n  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " Ã¢â‚¬â€ " << result.message << "\n\n";
}

void printGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    if (blocks.empty()) {
        cout << "\n  Global chain is empty.\n\n";
        return;
    }

    cout << "\n  Global Chain (" << blocks.size() << " blocks):\n";
    for (const auto& block : blocks) {
        const auto& r = block.getRecord();
        cout << "  [" << setw(3) << block.getIndex() << "] "
             << left << setw(10) << r.vin
             << setw(18) << cw1::stageToString(r.stage)
             << setw(10) << r.manufacturer
             << block.getCurrentHash().substr(0, 16) << "...\n";
    }
    cout << '\n';

    // Log that the global chain was viewed.
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed global chain (" + to_string(blocks.size()) + " blocks)");
}
/// Display the most recent N entries from the audit log.
/// Demonstrates Array of Pointers: getRecentEntries() returns a heap-
/// allocated array of const AuditEntry* that we iterate and then delete[].
void viewAuditLog(const cw1::Blockchain& chain) {
    const cw1::AuditLog& log = chain.getAuditLog();
    cout << "\n  Audit Log -- Total entries: " << log.size() << "\n";

    string input = prompt("View last how many entries? (default 10)");
    size_t n = 10;
    if (!input.empty()) {
        int parsed = parseIntOrDefault(input, 10);
        n = (parsed > 0) ? static_cast<size_t>(parsed) : 10;
    }

    size_t actualCount = 0;
    // getRecentEntries demonstrates Array of Pointers: a heap-allocated
    // raw array (const AuditEntry**) where each slot is a pointer to a
    // linked list node. We must delete[] the array after use.
    const cw1::AuditEntry** arr = log.getRecentEntries(n, actualCount);

    if (arr == nullptr || actualCount == 0) {
        cout << "\n  No audit entries to display.\n\n";
        return;
    }

    cout << "\n  Showing last " << actualCount << " entries:\n";
    printSeparator();

    // Iterate the raw pointer array -- demonstrates array-of-pointers access.
    for (size_t i = 0; i < actualCount; ++i) {
        const cw1::AuditEntry* entry = arr[i];
        cout << "  [" << setw(4) << (i + 1) << "] "
             << left << setw(22) << entry->timestamp
             << setw(20) << cw1::actionToString(entry->action)
             << entry->details << "\n";
    }

    printSeparator();
    cout << "\n";

    // Free the array of pointers (entries themselves are owned by AuditLog).
    delete[] arr;
}

} // namespace

// Ã¢â€â‚¬Ã¢â€â‚¬ Main Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

int main() {
    cw1::Blockchain chain;

    cout << "\n  ========================================\n";
    cout << "    Car Warehouse Blockchain System\n";
    cout << "    Hybrid Architecture (Global + Index)\n";
    cout << "  ========================================\n\n";

    loadDemoData(chain);

    while (true) {
        cout << "  ---- MENU ----\n";
        cout << "  1. View All Cars (summary)\n";
        cout << "  2. View Car Blockchain (by VIN)\n";
        cout << "  3. Add New Block (lifecycle stage)\n";
        cout << "  4. Search\n";
        cout << "  5. View Global Chain\n";
        cout << "  6. Verify Integrity\n";
        cout << "  7. View Audit Log\n";
    cout << "  8. Exit\n";

        string choice = prompt("Select (1-8)");

        if (choice == "1") { viewAllCars(chain); continue; }
        if (choice == "2") { viewCarByVin(chain); continue; }
        if (choice == "3") { addNewCarStage(chain); continue; }
        if (choice == "4") { searchCars(chain); continue; }
        if (choice == "5") { printGlobalChain(chain); continue; }
        if (choice == "6") { verifyIntegrity(chain); continue; }
        if (choice == "7") { viewAuditLog(chain); continue; }
        if (choice == "8") { cout << "\n  Exiting.\n"; break; }

        cout << "  Invalid choice.\n\n";
    }

    return 0;
}
```

### src\main_gui.cpp

```cpp
// main_gui.cpp -- ImGui/GLFW/OpenGL3 GUI for the Car Warehouse Blockchain.
// GitHub Dark theme, 3-panel layout (header | sidebar | content area).
// Views: Dashboard, Car Detail, Add Block, Global Chain, Audit Log, Integrity.

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"

// =================================================================
//  SECTION 1: Color palette (GitHub Dark)
// =================================================================

/// Convert packed RGB hex to ImVec4 RGBA float.
static ImVec4 HexColor(uint32_t hex, float a = 1.0f) {
    return ImVec4(
        static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
        static_cast<float>((hex >>  8) & 0xFF) / 255.0f,
        static_cast<float>( hex        & 0xFF) / 255.0f, a);
}

static const ImVec4 COL_BG_MAIN    = HexColor(0x0d1117);  // main window
static const ImVec4 COL_BG_PANEL   = HexColor(0x161b22);  // panel / sidebar
static const ImVec4 COL_BG_CARD    = HexColor(0x21262d);  // cards / frames
static const ImVec4 COL_BG_HOVER   = HexColor(0x30363d);  // hover / borders
static const ImVec4 COL_TEXT       = HexColor(0xe6edf3);  // primary text
static const ImVec4 COL_MUTED      = HexColor(0x8b949e);  // secondary text
static const ImVec4 COL_VERY_MUTED = HexColor(0x484f58);  // very muted
static const ImVec4 COL_ACCENT     = HexColor(0x1f6feb);  // primary blue
static const ImVec4 COL_ACCENT_HO  = HexColor(0x388bfd);  // blue hover
static const ImVec4 COL_GREEN      = HexColor(0x238636);  // success / pass
static const ImVec4 COL_GREEN_BR   = HexColor(0x2ea043);  // green hover
static const ImVec4 COL_RED        = HexColor(0xda3633);  // error / fail
static const ImVec4 COL_YELLOW     = HexColor(0xd29922);  // warning / QC
static const ImVec4 COL_PURPLE     = HexColor(0x8957e5);  // hash values
static const ImVec4 COL_ORANGE     = HexColor(0xdb6d28);  // nonce / dispatch

// =================================================================
//  SECTION 2: GitHub Dark theme
// =================================================================

static void ApplyGitHubDarkTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 8.0f;  s.FrameRounding    = 6.0f;
    s.PopupRounding     = 8.0f;  s.TabRounding      = 6.0f;
    s.ScrollbarRounding = 6.0f;  s.GrabRounding     = 4.0f;
    s.ChildRounding     = 6.0f;
    s.FramePadding      = ImVec2(10.0f, 6.0f);
    s.ItemSpacing       = ImVec2(10.0f, 8.0f);
    s.WindowPadding     = ImVec2(16.0f, 16.0f);
    s.ItemInnerSpacing  = ImVec2(8.0f,  4.0f);
    s.ScrollbarSize     = 12.0f; s.GrabMinSize      = 8.0f;
    s.WindowBorderSize  = 1.0f;  s.FrameBorderSize  = 0.0f;
    s.ChildBorderSize   = 1.0f;  s.PopupBorderSize  = 1.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                  = COL_TEXT;
    c[ImGuiCol_TextDisabled]          = COL_MUTED;
    c[ImGuiCol_WindowBg]              = COL_BG_MAIN;
    c[ImGuiCol_ChildBg]               = COL_BG_PANEL;
    c[ImGuiCol_PopupBg]               = COL_BG_PANEL;
    c[ImGuiCol_Border]                = COL_BG_HOVER;
    c[ImGuiCol_BorderShadow]          = ImVec4(0,0,0,0);
    c[ImGuiCol_FrameBg]               = COL_BG_CARD;
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
    c[ImGuiCol_Button]                = COL_BG_CARD;
    c[ImGuiCol_ButtonHovered]         = COL_BG_HOVER;
    c[ImGuiCol_ButtonActive]          = HexColor(0x1f6feb, 0.4f);
    c[ImGuiCol_Header]                = COL_BG_CARD;
    c[ImGuiCol_HeaderHovered]         = COL_BG_HOVER;
    c[ImGuiCol_HeaderActive]          = HexColor(0x1f6feb, 0.4f);
    c[ImGuiCol_Separator]             = COL_BG_HOVER;
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
    c[ImGuiCol_TableHeaderBg]         = COL_BG_CARD;
    c[ImGuiCol_TableBorderStrong]     = COL_BG_HOVER;
    c[ImGuiCol_TableBorderLight]      = HexColor(0x21262d);
    c[ImGuiCol_TableRowBg]            = ImVec4(0,0,0,0);
    c[ImGuiCol_TableRowBgAlt]         = HexColor(0x161b22, 0.5f);
    c[ImGuiCol_TextSelectedBg]        = HexColor(0x1f6feb, 0.3f);
    c[ImGuiCol_DragDropTarget]        = COL_ACCENT;
    c[ImGuiCol_NavHighlight]          = COL_ACCENT;
    c[ImGuiCol_NavWindowingHighlight] = COL_ACCENT;
    c[ImGuiCol_NavWindowingDimBg]     = HexColor(0x0d1117, 0.7f);
    c[ImGuiCol_ModalWindowDimBg]      = HexColor(0x0d1117, 0.6f);
}

// =================================================================
//  SECTION 3: Application state
// =================================================================

enum class View { DASHBOARD, CAR_DETAIL, ADD_BLOCK, GLOBAL_CHAIN, AUDIT_LOG, INTEGRITY };

static View        g_view        = View::DASHBOARD;
static std::string g_selectedVin;
static char        g_searchBuf[256] = {};
static cw1::ValidationResult g_lastVerify = {false, "Not yet verified"};
static bool        g_verifyDone = false;

// Toast notification system
struct Toast { std::string msg; float remainingSecs; ImVec4 color; };
static std::vector<Toast> g_toasts;
static void PushToast(const std::string& msg, ImVec4 color, float dur = 3.0f) {
    g_toasts.push_back({msg, dur, color});
}

// Add-block form state
static int    g_formStage           = 0;
static char   g_formVin[64]         = {};
static char   g_formMfr[64]         = {};
static char   g_formModel[64]       = {};
static char   g_formColor[64]       = {};
static int    g_formYear            = 2025;
static char   g_formFactory[128]    = {};
static char   g_formWarehouse[64]   = {};
static char   g_formReceivedBy[64]  = {};
static char   g_formInspector[64]   = {};
static bool   g_formPassed          = true;
static char   g_formQcNotes[256]    = {};
static char   g_formDealerId[64]    = {};
static char   g_formDestination[64] = {};
static char   g_formTransport[64]   = {};
static char   g_formBuyerId[64]     = {};
static double g_formSalePrice       = 0.0;
static char   g_formWarranty[32]    = {};
static int    g_auditN              = 20;

// =================================================================
//  SECTION 4: Demo data (no cout, no sleep_for)
// =================================================================

static void loadDemoData(cw1::Blockchain& chain) {
    { cw1::CarRecord r;
      r.vin="VIN1001"; r.manufacturer="Perodua"; r.model="Axia";
      r.color="Silver"; r.productionYear=2024;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Shah Alam Plant";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-A1"; r.receivedBy="Ahmad bin Ismail";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-001"; r.passed=true; r.qcNotes="All systems nominal";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::DEALER_DISPATCH;
      r.dealerId="DLR-KB-01"; r.destination="Kota Bharu"; r.transportMode="Truck";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::CUSTOMER_SALE;
      r.buyerId="CUST-10201"; r.salePrice=38000.00; r.warrantyExpiry="2029-03-01";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1002"; r.manufacturer="Toyota"; r.model="Vios";
      r.color="White"; r.productionYear=2023;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Toyota Bukit Raja";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-B2"; r.receivedBy="Lim Wei Jie";
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
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Honda Pegoh, Melaka";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-C3"; r.receivedBy="Raj Kumar";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-007"; r.passed=false;
      r.qcNotes="Brake pad below spec - pending replacement";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1004"; r.manufacturer="Proton"; r.model="X50";
      r.color="Red"; r.productionYear=2025;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Proton Tanjung Malim";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-D4"; r.receivedBy="Nurul Aina";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1005"; r.manufacturer="Perodua"; r.model="Myvi";
      r.color="Black"; r.productionYear=2025;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Perodua Rawang";
      chain.addBlock(r); }
}

// =================================================================
//  SECTION 5: Helper utilities
// =================================================================

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

static std::string FormatMYR(double v) {
    std::ostringstream ss;
    ss << "MYR " << std::fixed << std::setprecision(2) << v;
    return ss.str();
}

// =================================================================
//  SECTION 6: Header bar
// =================================================================

static void RenderHeader(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 12.0f));
    ImGui::BeginChild("##header", ImVec2(-1.0f, 54.0f), true);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
    ImGui::TextColored(COL_ACCENT, "%s", "* ");
    ImGui::SameLine();
    ImGui::Text("Car Warehouse Blockchain");

    float statsWidth = 340.0f;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - statsWidth
                    + ImGui::GetCursorPosX() - 16.0f);

    char badge[64];
    snprintf(badge, sizeof(badge), "  %zu blocks  ", chain.totalBlocks());
    ImGui::TextColored(COL_ACCENT, "%s", badge);
    ImGui::SameLine();
    snprintf(badge, sizeof(badge), "  %zu cars  ", chain.getAllVins().size());
    ImGui::TextColored(COL_GREEN, "%s", badge);
    ImGui::SameLine();
    if (g_verifyDone) {
        if (g_lastVerify.ok)
            ImGui::TextColored(COL_GREEN, "  Verified  ");
        else
            ImGui::TextColored(COL_RED, "  Invalid  ");
    } else {
        ImGui::TextColored(COL_MUTED, "  Checking...  ");
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// =================================================================
//  SECTION 7: Left sidebar
// =================================================================

static void RenderSidebar(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##sidebar", ImVec2(280.0f, -1.0f), true);

    // Search box
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##search", "Search VIN, brand, model...",
                             g_searchBuf, sizeof(g_searchBuf));
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Navigation buttons
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
            ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
            ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(1,1,1,1));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0,0,0,0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        }
        if (ImGui::Button(item.label, ImVec2(-1.0f, 36.0f))) {
            g_view = item.view;
            g_selectedVin.clear();
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Car list header
    auto allVins = chain.getAllVins();
    ImGui::TextColored(COL_MUTED, "VEHICLES (%zu)", allVins.size());
    ImGui::Spacing();

    // Build filtered list
    std::vector<std::string> displayVins;
    if (g_searchBuf[0]) {
        const auto hits = chain.searchGeneral(std::string(g_searchBuf));
        std::vector<std::string> seen;
        for (const auto* blk : hits) {
            const auto& v = blk->getRecord().vin;
            if (std::find(seen.begin(), seen.end(), v) == seen.end()) {
                seen.push_back(v);
                displayVins.push_back(v);
            }
        }
    } else {
        displayVins = allVins;
    }

    for (const auto& vin : displayVins) {
        const auto history = chain.getCarHistory(vin);
        if (history.empty()) continue;
        const auto& rec    = history.front()->getRecord();
        bool        sel    = (vin == g_selectedVin);
        cw1::BlockStage st = chain.getLatestStage(vin);

        // Blue left border for selected card
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


// =================================================================

// =================================================================
//  SECTION 8: Dashboard view
// =================================================================

static void RenderDashboard(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    size_t totalBlocks = chain.totalBlocks();
    size_t totalCars   = chain.getAllVins().size();

    // â”€â”€ Stat cards â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    if (ImGui::BeginTable("##stats", 4,
            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoPadOuterX,
            ImVec2(-1, 90))) {
        ImGui::TableNextRow();

        auto StatCard = [](const char* value, const char* label, ImVec4 col) {
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
            ImGui::BeginChild(label, ImVec2(-1, 80), true);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
            ImGui::TextColored(col, "%s", value);
            ImGui::TextColored(COL_MUTED, "%s", label);
            ImGui::EndChild();
            ImGui::PopStyleColor();
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

    // â”€â”€ Recent blocks table â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    ImGuiTableFlags tflags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                             ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##recent", 6, tflags, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed,   60);
        ImGui::TableSetupColumn("Hash",         ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN",          ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage",        ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Timestamp",    ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        // Most-recent first
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
}

// =================================================================
//  SECTION 9: Car Detail view
// =================================================================

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

    // Car header from first block
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
        // Use hdrLabel as unique child ID
        ImGui::BeginChild(hdrLabel, ImVec2(-1, 0), true);

        // Stage badge + timestamp
        ImGui::TextColored(StageColor(rec.stage), "[%s]",
                           cw1::stageToString(rec.stage).c_str());
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, "  %s", blk.getTimestamp().c_str());

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        // Helper: label + value on one line
        auto Field = [](const char* lbl, const char* val, ImVec4 col) {
            ImGui::TextColored(COL_MUTED, "%-20s", lbl);
            ImGui::SameLine();
            ImGui::TextColored(col, "%s", val);
        };

        Field("Hash:",          blk.getCurrentHash().c_str(),  COL_PURPLE);
        Field("Prev Hash:",     blk.getPreviousHash().c_str(), COL_PURPLE);

        char nonceBuf[32];
        snprintf(nonceBuf, sizeof(nonceBuf), "%llu",
                 static_cast<unsigned long long>(blk.getNonce()));
        Field("Nonce:",         nonceBuf, COL_ORANGE);

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "Record Fields");
        ImGui::Separator(); ImGui::Spacing();

        Field("VIN:",           rec.vin.c_str(),          COL_TEXT);
        Field("Manufacturer:",  rec.manufacturer.c_str(), COL_TEXT);
        Field("Model:",         rec.model.c_str(),        COL_TEXT);
        Field("Color:",         rec.color.c_str(),        COL_TEXT);

        char yearBuf[16];
        snprintf(yearBuf, sizeof(yearBuf), "%d", rec.productionYear);
        Field("Production Year:", yearBuf, COL_TEXT);
        Field("Stage:",           cw1::stageToString(rec.stage).c_str(),
                                  StageColor(rec.stage));

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "Stage Details");
        ImGui::Separator(); ImGui::Spacing();

        switch (rec.stage) {
        case cw1::BlockStage::PRODUCTION:
            Field("Factory:",   rec.factoryLocation.c_str(), COL_TEXT);
            break;
        case cw1::BlockStage::WAREHOUSE_INTAKE:
            Field("Warehouse:", rec.warehouseLocation.c_str(), COL_TEXT);
            Field("Received By:",rec.receivedBy.c_str(),       COL_TEXT);
            break;
        case cw1::BlockStage::QUALITY_CHECK:
            Field("Inspector:", rec.inspectorId.c_str(), COL_TEXT);
            Field("Passed:",    rec.passed ? "Yes" : "No",
                                rec.passed ? COL_GREEN_BR : COL_RED);
            Field("QC Notes:",  rec.qcNotes.c_str(), COL_TEXT);
            break;
        case cw1::BlockStage::DEALER_DISPATCH:
            Field("Dealer ID:", rec.dealerId.c_str(),      COL_TEXT);
            Field("Destination:",rec.destination.c_str(),  COL_TEXT);
            Field("Transport:", rec.transportMode.c_str(), COL_ORANGE);
            break;
        case cw1::BlockStage::CUSTOMER_SALE: {
            Field("Buyer ID:",  rec.buyerId.c_str(), COL_TEXT);
            char priceBuf[48];
            snprintf(priceBuf, sizeof(priceBuf), "%s", FormatMYR(rec.salePrice).c_str());
            Field("Sale Price:", priceBuf, COL_GREEN_BR);
            Field("Warranty:",  rec.warrantyExpiry.c_str(), COL_TEXT);
            break;
        }
        }

        ImGui::Spacing();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
                            "Car detail viewed: " + g_selectedVin);
}

// =================================================================
//  SECTION 10: Add Block form
// =================================================================

static const char* k_stageNames[] = {
    "PRODUCTION", "WAREHOUSE_INTAKE", "QUALITY_CHECK",
    "DEALER_DISPATCH", "CUSTOMER_SALE"
};

static void RenderAddBlock(cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Add Block to Chain");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "COMMON FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    ImGui::SetNextItemWidth(240);
    ImGui::Combo("Stage##addstage", &g_formStage, k_stageNames, 5);
    ImGui::Spacing();

    // Helper: label + InputText on one row
    auto LabelText = [](const char* lbl, char* buf, size_t sz) {
        ImGui::TextColored(COL_MUTED, "%-18s", lbl);
        ImGui::SameLine();
        char id[64]; snprintf(id, sizeof(id), "##add_%s", lbl);
        ImGui::SetNextItemWidth(300);
        ImGui::InputText(id, buf, sz);
    };

    LabelText("VIN",          g_formVin,   sizeof(g_formVin));
    // VIN status badge
    ImGui::SameLine(0, 12);
    if (g_formVin[0] != '\0') {
        if (chain.hasVin(std::string(g_formVin)))
            ImGui::TextColored(COL_GREEN_BR, "[+ Existing VIN]");
        else
            ImGui::TextColored(COL_ACCENT,   "[* New VIN]");
    }

    LabelText("Manufacturer", g_formMfr,   sizeof(g_formMfr));
    LabelText("Model",        g_formModel, sizeof(g_formModel));
    LabelText("Color",        g_formColor, sizeof(g_formColor));

    ImGui::TextColored(COL_MUTED, "%-18s", "Production Year");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("##add_year", &g_formYear);

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "STAGE-SPECIFIC FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    switch (g_formStage) {
    case 0: // PRODUCTION
        LabelText("Factory Location", g_formFactory, sizeof(g_formFactory));
        break;
    case 1: // WAREHOUSE_INTAKE
        LabelText("Warehouse",  g_formWarehouse,   sizeof(g_formWarehouse));
        LabelText("Received By",g_formReceivedBy,  sizeof(g_formReceivedBy));
        break;
    case 2: // QUALITY_CHECK
        LabelText("Inspector ID", g_formInspector, sizeof(g_formInspector));
        ImGui::TextColored(COL_MUTED, "%-18s", "Passed");
        ImGui::SameLine();
        ImGui::Checkbox("##add_passed", &g_formPassed);
        LabelText("QC Notes", g_formQcNotes, sizeof(g_formQcNotes));
        break;
    case 3: // DEALER_DISPATCH
        LabelText("Dealer ID",    g_formDealerId,   sizeof(g_formDealerId));
        LabelText("Destination",  g_formDestination,sizeof(g_formDestination));
        LabelText("Transport",    g_formTransport,  sizeof(g_formTransport));
        break;
    case 4: // CUSTOMER_SALE
        LabelText("Buyer ID",     g_formBuyerId, sizeof(g_formBuyerId));
        ImGui::TextColored(COL_MUTED, "%-18s", "Sale Price (MYR)");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputDouble("##add_price", &g_formSalePrice, 1000.0, 10000.0, "%.2f");
        LabelText("Warranty Expiry", g_formWarranty, sizeof(g_formWarranty));
        break;
    }

    ImGui::Spacing(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1158c7));
    bool submit = ImGui::Button("  Add Block  ", ImVec2(150, 36));
    ImGui::PopStyleColor(3);

    if (submit) {
        cw1::CarRecord r;
        r.vin            = g_formVin;
        r.manufacturer   = g_formMfr;
        r.model          = g_formModel;
        r.color          = g_formColor;
        r.productionYear = g_formYear;
        r.stage          = static_cast<cw1::BlockStage>(g_formStage);

        switch (g_formStage) {
        case 0:
            r.factoryLocation  = g_formFactory;
            break;
        case 1:
            r.warehouseLocation = g_formWarehouse;
            r.receivedBy        = g_formReceivedBy;
            break;
        case 2:
            r.inspectorId = g_formInspector;
            r.passed      = g_formPassed;
            r.qcNotes     = g_formQcNotes;
            break;
        case 3:
            r.dealerId      = g_formDealerId;
            r.destination   = g_formDestination;
            r.transportMode = g_formTransport;
            break;
        case 4:
            r.buyerId        = g_formBuyerId;
            r.salePrice      = g_formSalePrice;
            r.warrantyExpiry = g_formWarranty;
            break;
        }

        chain.addBlock(r);
        PushToast("Block #" + std::to_string(chain.totalBlocks() - 1) +
                  " added for " + std::string(g_formVin),
                  COL_GREEN_BR);

        // Clear all form buffers
        g_formVin[0]    = '\0'; g_formMfr[0]         = '\0';
        g_formModel[0]  = '\0'; g_formColor[0]        = '\0';
        g_formYear      = 2025;
        g_formFactory[0]= '\0'; g_formWarehouse[0]    = '\0';
        g_formReceivedBy[0]= '\0'; g_formInspector[0] = '\0';
        g_formPassed    = true;  g_formQcNotes[0]     = '\0';
        g_formDealerId[0]= '\0'; g_formDestination[0] = '\0';
        g_formTransport[0]= '\0'; g_formBuyerId[0]    = '\0';
        g_formSalePrice = 0.0;   g_formWarranty[0]    = '\0';
    }
}

// =================================================================
//  SECTION 11: Global Chain view
// =================================================================

static void RenderGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();

    ImGui::TextColored(COL_ACCENT, "Global Chain");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu block(s))", blocks.size());
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGuiTableFlags tf = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
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
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED, "Global chain viewed");
}

// =================================================================
//  SECTION 12: Audit Log view
// =================================================================

static void RenderAuditLog(const cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Audit Log");
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

    // Retrieve via Array of Pointers pattern
    std::size_t outCount = 0;
    const cw1::AuditEntry** entries =
        chain.getAuditLog().getRecentEntries(
            static_cast<std::size_t>(g_auditN), outCount);

    auto ActionColor = [](cw1::AuditAction act) -> ImVec4 {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return COL_GREEN_BR;
        case cw1::AuditAction::INTEGRITY_CHECK:  return COL_YELLOW;
        case cw1::AuditAction::SEARCH_PERFORMED: return COL_ACCENT;
        case cw1::AuditAction::CHAIN_VIEWED:     return COL_PURPLE;
        default:                                  return COL_TEXT;
        }
    };
    auto ActionName = [](cw1::AuditAction act) -> const char* {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return "BLOCK_ADDED";
        case cw1::AuditAction::INTEGRITY_CHECK:  return "INTEGRITY_CHECK";
        case cw1::AuditAction::SEARCH_PERFORMED: return "SEARCH_PERFORMED";
        case cw1::AuditAction::CHAIN_VIEWED:     return "CHAIN_VIEWED";
        default:                                  return "UNKNOWN";
        }
    };

    ImGuiTableFlags tf = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##auditlog", 3, tf, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Action",    ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Details",   ImGuiTableColumnFlags_WidthStretch, 4.0f);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < outCount; ++i) {
            const cw1::AuditEntry* e = entries[i];
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

    // Caller must delete[] the raw array (Array of Pointers pattern)
    delete[] entries;
}

// =================================================================
//  SECTION 13: Integrity check view
// =================================================================

static void RenderIntegrity(cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Blockchain Integrity");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1158c7));
    if (ImGui::Button("  Run Verification  ", ImVec2(200, 36))) {
        g_lastVerify = chain.verifyIntegrity();
        g_verifyDone = true;
        PushToast(g_lastVerify.ok ? "Chain VERIFIED OK" : "Chain TAMPERED!",
                  g_lastVerify.ok ? COL_GREEN_BR : COL_RED);
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing(); ImGui::Spacing();

    if (!g_verifyDone) {
        ImGui::TextColored(COL_MUTED, "Press Run Verification to check the chain.");
        return;
    }

    // Large PASS / FAIL banner
    ImVec4 bannerCol  = g_lastVerify.ok ? COL_GREEN : COL_RED;
    const char* bannerTxt = g_lastVerify.ok ? "VERIFIED" : "TAMPERED";

    ImGui::PushStyleColor(ImGuiCol_ChildBg, bannerCol);
    ImGui::BeginChild("##intbanner", ImVec2(-1, 80), true);
    ImGui::SetCursorPos(ImVec2(20, 18));
    ImGui::SetWindowFontScale(2.0f);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", bannerTxt);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "Result: ");
    ImGui::SameLine();
    ImGui::TextWrapped("%s", g_lastVerify.message.c_str());

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    size_t total = chain.totalBlocks();
    size_t cars  = chain.getAllVins().size();

    ImGui::TextColored(COL_MUTED, "Total blocks:    ");
    ImGui::SameLine(); ImGui::TextColored(COL_TEXT, "%zu", total);

    ImGui::TextColored(COL_MUTED, "Unique vehicles: ");
    ImGui::SameLine(); ImGui::TextColored(COL_TEXT, "%zu", cars);

    if (total > 0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "Block range: 0 .. %zu", total - 1);
    }
}

// =================================================================
//  SECTION 14: Toast renderer
// =================================================================

static void RenderToasts(float deltaTime) {
    // Remove expired toasts first
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

        // Alpha fade-out in last 0.5 s
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

// =================================================================
//  SECTION 15: main()
// =================================================================

int main() {
    // â”€â”€ 1. GLFW â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
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

    // â”€â”€ 2. ImGui â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ApplyGitHubDarkTheme();

    // â”€â”€ 3. Font (fallback to default if file absent) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    {
        const char* fontPath = "fonts/JetBrainsMono-Regular.ttf";
        FILE* fp = fopen(fontPath, "rb");
        if (fp) {
            fclose(fp);
            io.Fonts->AddFontFromFileTTF(fontPath, 16.0f);
        } else {
            io.Fonts->AddFontDefault();
        }
    }

    // â”€â”€ 4. Blockchain + demo data â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    cw1::Blockchain chain;
    loadDemoData(chain);
    g_lastVerify = chain.verifyIntegrity();
    g_verifyDone = true;

    // â”€â”€ 5. Main loop â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Full-screen root window
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

        // Sidebar
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
        ImGui::BeginChild("##sbwrap", ImVec2(280, -1), false);
        ImGui::PopStyleColor();
        RenderSidebar(chain);
        ImGui::EndChild();

        ImGui::SameLine();

        // Content area
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
        }

        ImGui::EndChild();
        ImGui::End();

        // Toast overlay (rendered after main window)
        RenderToasts(io.DeltaTime);

        // â”€â”€ Draw â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        ImGui::Render();
        int dispW, dispH;
        glfwGetFramebufferSize(window, &dispW, &dispH);
        glViewport(0, 0, dispW, dispH);
        glClearColor(0.051f, 0.067f, 0.090f, 1.0f);  // #0d1117
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // â”€â”€ 6. Cleanup â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

### src\models\CarRecord.cpp

```cpp
#include "models/CarRecord.hpp"

#include <sstream>

using namespace std;

namespace cw1 {

namespace {

/// Escape pipe and backslash so the serialised string is unambiguous.
string escapeField(const string& value) {
    string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        if (ch == '\\' || ch == '|') {
            escaped.push_back('\\');
        }
        escaped.push_back(ch);
    }
    return escaped;
}

} // namespace

string CarRecord::serialize() const {
    ostringstream out;

    // Common fields
    out << "vin=" << escapeField(vin)
        << "|manufacturer=" << escapeField(manufacturer)
        << "|model=" << escapeField(model)
        << "|color=" << escapeField(color)
        << "|productionYear=" << productionYear
        << "|stage=" << escapeField(stageToString(stage));

    // Stage-specific fields â€” include all so that the hash is deterministic
    // regardless of which fields happen to be empty.
    out << "|factoryLocation=" << escapeField(factoryLocation)
        << "|warehouseLocation=" << escapeField(warehouseLocation)
        << "|receivedBy=" << escapeField(receivedBy)
        << "|inspectorId=" << escapeField(inspectorId)
        << "|passed=" << (passed ? "true" : "false")
        << "|qcNotes=" << escapeField(qcNotes)
        << "|dealerId=" << escapeField(dealerId)
        << "|destination=" << escapeField(destination)
        << "|transportMode=" << escapeField(transportMode)
        << "|buyerId=" << escapeField(buyerId)
        << "|salePrice=" << salePrice
        << "|warrantyExpiry=" << escapeField(warrantyExpiry);

    return out.str();
}

} // namespace cw1
```

### src\utils\AuditLog.cpp

```cpp
// AuditLog.cpp -- Implementation of the hand-written singly linked list
// used for audit logging in the Car Warehouse Blockchain system.
//
// Coursework requirement: demonstrates:
//   1. Singly Linked List  -- manual node management, head/tail pointers,
//      O(1) tail append, O(n) traversal/deletion.
//   2. Array of Pointers   -- getRecentEntries() returns a raw new[] array
//      of const AuditEntry* that the caller must delete[].

#include "utils/AuditLog.hpp"
#include "utils/TimeUtil.hpp"

#include <algorithm>  // std::min

using namespace std;

namespace cw1 {

// â”€â”€ Helper â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

string actionToString(AuditAction action) {
    switch (action) {
        case AuditAction::BLOCK_ADDED:      return "BLOCK_ADDED";
        case AuditAction::INTEGRITY_CHECK:  return "INTEGRITY_CHECK";
        case AuditAction::SEARCH_PERFORMED: return "SEARCH_PERFORMED";
        case AuditAction::CHAIN_VIEWED:     return "CHAIN_VIEWED";
        default:                            return "UNKNOWN";
    }
}

// â”€â”€ AuditLog constructor / destructor â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

AuditLog::AuditLog()
    : head_(nullptr), tail_(nullptr), count_(0) {
    // Linked list starts empty: both head and tail are null.
}

AuditLog::~AuditLog() {
    // Traverse the linked list and delete every node to avoid memory leaks.
    clear();
}

// â”€â”€ Core linked list operations â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

void AuditLog::log(AuditAction action, const string& details) {
    // Allocate a new node on the heap (demonstrates new/delete pattern).
    AuditEntry* node = new AuditEntry(action, details, TimeUtil::nowIso8601());

    if (tail_ == nullptr) {
        // List is empty: new node becomes both head and tail.
        head_ = node;
        tail_ = node;
    } else {
        // Append at the tail -- O(1) because we maintain a tail pointer.
        // Without the tail pointer this would be O(n) traversal.
        tail_->next = node;  // Link old tail to new node
        tail_ = node;        // Advance tail to the new node
    }

    ++count_;
}

size_t AuditLog::size() const noexcept {
    return count_;
}

const AuditEntry* AuditLog::head() const noexcept {
    return head_;
}

void AuditLog::clear() {
    // Traverse linked list from head, deleting each node.
    // Classic linked-list deletion: save next before freeing current.
    AuditEntry* current = head_;
    while (current != nullptr) {
        AuditEntry* next = current->next;  // Save next before delete
        delete current;                    // Free the current node
        current = next;                    // Advance to saved next
    }
    head_  = nullptr;
    tail_  = nullptr;
    count_ = 0;
}

// â”€â”€ Array of Pointers â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

const AuditEntry** AuditLog::getRecentEntries(size_t maxCount,
                                               size_t& outCount) const {
    // Demonstrates: Array of Pointers (raw pointer array, manual memory
    // management). The returned array is a heap-allocated array whose
    // elements are pointers into the linked list (not copies). The caller
    // must delete[] the array but must NOT delete the pointed-to nodes.

    // Determine how many entries we can actually return.
    size_t actualCount = (maxCount < count_) ? maxCount : count_;
    outCount = actualCount;

    if (actualCount == 0) {
        return nullptr;
    }

    // Allocate the raw array of pointers on the heap.
    // This is the "Array of Pointers" data structure for the coursework.
    const AuditEntry** arr = new const AuditEntry*[actualCount];

    // Skip the first (count_ - actualCount) nodes to reach the start of
    // the last actualCount entries. This requires a linear traversal from
    // the head because we only have a singly linked list (no random access).
    size_t skipCount = count_ - actualCount;
    const AuditEntry* node = head_;
    for (size_t i = 0; i < skipCount; ++i) {
        node = node->next;  // Advance through nodes we want to skip
    }

    // Fill the array with pointers to the remaining (most recent) nodes.
    for (size_t i = 0; i < actualCount; ++i) {
        arr[i] = node;      // Store pointer to node (not a copy of the node)
        node = node->next;  // Advance linked list pointer
    }

    return arr;  // Caller must delete[] this array
}

} // namespace cw1
```

### src\utils\HashUtil.cpp

```cpp
#include "utils/HashUtil.hpp"

#include <iomanip>
#include <openssl/sha.h>
#include <sstream>

using namespace std;

namespace cw1::HashUtil {

string sha256(const string& input) {
    unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

    SHA256(
        reinterpret_cast<const unsigned char*>(input.data()),
        input.size(),
        digest
    );

    ostringstream output;
    output << hex << setfill('0');

    for (unsigned char byte : digest) {
        output << setw(2) << static_cast<int>(byte);
    }

    return output.str();
}

} // namespace cw1::HashUtil
```

### src\utils\TimeUtil.cpp

```cpp
#include "utils/TimeUtil.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

namespace cw1::TimeUtil {

string nowIso8601() {
    const auto now = chrono::system_clock::now();
    const time_t currentTime = chrono::system_clock::to_time_t(now);

    tm utcTime = {};
#ifdef _WIN32
    gmtime_s(&utcTime, &currentTime);
#else
    gmtime_r(&currentTime, &utcTime);
#endif

    ostringstream output;
    output << put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

} // namespace cw1::TimeUtil
```

