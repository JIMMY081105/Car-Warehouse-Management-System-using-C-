# COMP2034 CW1 — 项目规划总结
**日期：** 26 February 2026  
**课程：** COMP2034 C++ Programming  
**作业：** Coursework 1 — Car Warehouse Management System (50%)  
**截止日期：** 2 April 2026, 6pm

---

## 一、作业基本要求

### 提交物清单
- PDF 报告（解释代码、算法、数据结构、困难、结论、参考文献）
- PDF 截图（程序输出 + 运行时间）
- `.cpp` 源代码文件
- `.o` 目标文件
- `.exe` 执行文件
- 全部打包成 `studentID_Name_CW1.zip` 上传 Moodle

### 评分分布（共50分）
| 部分 | 分数 |
|------|------|
| 报告写作 + 代码注释 | 20分 |
| 功能性C++程序（区块链、数据结构等） | 10分 |
| 创新性、复杂度、独特性 | 15分 |
| 课堂展示 + Q&A | 5分 |

---

## 二、关于 Sample Code 的定位

老师提供的 Sample Code **只是起点骨架**，不是完整作业。

Sample Code 提供了：
- 基本 Block 结构体
- 随机生成 Hash 的函数（假哈希，不是真正SHA）
- 时间戳获取
- 简单输入和打印功能

**你需要自己实现的：**
- 真正的 SHA-256 哈希算法
- 汽车仓库管理系统完整业务逻辑
- Linked List、Array of Pointers 等数据结构
- 完整菜单系统和功能模块
- 专业代码注释和文档
- 动态区块链技术

---

## 三、SHA-256 的理解与应用

### 什么是 SHA-256
- 把任意长度数据转换成固定 64 字符的"指纹"
- **单向不可逆**——无法从哈希值反推原始数据
- 同样输入永远得到同样输出
- 哪怕改变一个字符，输出完全不同

### SHA-256 在作业中的角色

**计算方式：**
```
Block Number + Previous Hash + Timestamp + Info
        ↓ SHA-256 计算
    Current Hash（该块的唯一指纹）
```

**三个核心作用：**
1. **身份证明** — 每个 Block 有唯一指纹
2. **防篡改** — 任何数据被修改立刻被检测到
3. **链式连接** — 通过传递哈希值把所有 Block 串联成不可分割的链

**链式安全原理：**
```
Block 0 (Hash: AAA) → Block 1 (PrevHash: AAA, Hash: BBB) → Block 2 (PrevHash: BBB)
如果有人篡改 Block 1 → BBB 改变 → Block 2 的 PrevHash 对不上 → 篡改被发现
```

**实现方式（OpenSSL库）：**
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

## 四、区块链结构设计

### 每辆车的 Block 数量
作业**没有强制规定**Block数量，原文说：
> "The number of blocks really depends on the idea and concept you have constructed"

**建议：每辆车设计 4～5 个 Block**

### 推荐的 Block 设计（5个阶段）
```
Block 0 — 生产出厂
  Info: 制造商ID、制造商名、车型、出厂日期、车架号VIN

Block 1 — 仓库入库
  Info: 仓库地址、入库日期、负责人、库存位置

Block 2 — 质量检测
  Info: 质检员ID、检查结果、检查日期、合格证编号

Block 3 — 经销商配送
  Info: 经销商ID、配送日期、运输方式、目的地

Block 4 — 客户销售/售后
  Info: 买家ID、销售日期、保修期、客服记录
```

### 多辆车的数据结构
```cpp
// 用 Map 存储每辆车的独立区块链
map<string, vector<Block>> warehouse;
// key = 车架号VIN, value = 该车的Block链
```

**50辆车 × 5个Block = 250个Block**，但不需要手动创建，程序自动生成。演示时 3～5 辆车就足够。

---

## 五、UI/主页设计思路

### 主菜单
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

### View All Cars — 摘要列表（不展开所有Block）
```
No. | VIN     | Brand   | Model | Status      | Blocks
----|---------|---------|-------|-------------|-------
1   | VIN001  | Perodua | Axia  | In Stock    | 5
2   | VIN002  | Toyota  | Vios  | Sold        | 5
```
用户选择编号后，才展开该车的完整区块链。

### Search 功能（创新加分项）
```
1. Search by VIN
2. Search by Brand
3. Search by Status
4. Search by Date
```

---

## 六、UI 框架选择

### 最终决定：ImGui

**为什么选 ImGui：**
- 纯 C++ 库，前后端全部 C++ 实现
- 真正的桌面窗口程序（不是 Terminal）
- 可以做表格、搜索栏、按钮、侧边栏
- AI 对 ImGui 非常熟悉，容易生成代码
- 编译出来是 `.exe` 文件，完全符合提交要求
- 在游戏和工业软件界非常专业，偏商务风格

**ImGui 可以实现的界面功能：**
- 左侧侧边栏显示所有车辆列表
- 右侧展示选中车辆的完整区块链
- 顶部搜索栏实时过滤
- 表格显示所有 Block 信息
- 不同状态用不同颜色标注

### 时间估计
- ImGui 开发时间约 **2～3周**
- 需要合理分配时间给报告写作
- 善用 AI 辅助生成代码可以大幅节省时间

---

## 七、其他备选 UI 方案（供参考）

| 选项 | 难度 | 视觉效果 | 商务感 | 开发时间 |
|------|------|----------|--------|----------|
| Terminal ANSI | ⭐ | ⭐⭐ | ⭐⭐ | 1～2天 |
| HTML Export | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | 3～5天 |
| **ImGui（选定）** | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | 2～3周 |
| Raylib | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | 3～4周 |
| C++ + Web Server | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | 3～4周 |

---

## 八、下一步行动建议

1. **确认开发环境** — 安装 ImGui + OpenSSL 库
2. **设计数据结构** — 定义 Block struct 和 Car 的完整字段
3. **实现核心逻辑** — SHA-256 哈希 + 区块链链接
4. **建立虚构公司** — 设计马来西亚汽车企业背景资料（用于报告）
5. **开发 ImGui 界面** — 主菜单、列表、详情、搜索
6. **写报告** — 解释每个设计决定，引用参考资料
7. **测试 + 截图** — 记录运行时间和输出截图

---

*此总结记录了 COMP2034 CW1 规划对话的完整内容，供下次继续开发时参考。*
