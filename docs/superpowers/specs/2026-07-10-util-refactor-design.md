# Mana Util 模块重构设计规范

**日期:** 2026-07-10
**版本:** 1.0
**状态:** 草案

## 1. 概述

将现有的 `mana::util` 工具模块重构为分类组织的单头文件库，采用纯 header-only 实现，优先使用 `constexpr` 和 `inline`。

### 设计哲学

1. **编译时优先**: 所有能在编译时计算的函数标记为 `constexpr`
2. **零开销抽象**: 使用模板和内联避免运行时开销
3. **单头文件**: 无 `.cpp` 文件，所有实现在头文件中
4. **现代 C++23**: 使用 `std::string_view`、concepts、`constexpr` 算法
5. **分类清晰**: 按功能域组织到独立头文件

## 2. 目录结构

```
include/mana/
├── util/
│   ├── util.hpp      # 通用工具（类型转换、容器操作、时间戳）
│   ├── string.hpp    # 字符串操作
│   ├── io.hpp        # 文件 IO 工具（无状态函数）
│   ├── math.hpp      # 数学函数（constexpr 三角函数）
│   └── geo.hpp       # 几何/地理（点、线、矩形、坐标转换、空间索引）
├── context.h         # 保持不变
├── event_bus.h       # 保持不变
├── export.h          # 保持不变
├── log.h             # 保持不变
├── mana.h            # 更新 include
└── thread_pool.h     # 保持不变
```

## 3. 命名空间结构

```cpp
namespace mana::util {
    namespace string { ... }  // 字符串操作
    namespace io { ... }      // 文件工具
    namespace math { ... }    // 数学函数
    namespace geo { ... }     // 几何/地理
}
```

## 4. 模块规格

### 4.1 string.hpp — 字符串操作

**命名空间:** `mana::util::string`

```cpp
#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

namespace mana::util::string {

// 分割字符串
inline std::vector<std::string> split(std::string_view str, std::string_view delimiter);

// 去除首尾空白
inline std::string trim(std::string_view str);

// 大小写转换
inline std::string to_lower(std::string_view str);
inline std::string to_upper(std::string_view str);

// 前缀/后缀检查
inline bool starts_with(std::string_view str, std::string_view prefix);
inline bool ends_with(std::string_view str, std::string_view suffix);

// 包含检查
inline bool contains(std::string_view str, std::string_view sub);

// 替换
inline std::string replace(std::string_view str, std::string_view from, std::string_view to);

// 连接
inline std::string join(const std::vector<std::string>& parts, std::string_view delimiter);

} // namespace mana::util::string
```

**设计决策:**
- 使用 `std::string_view` 作为输入参数（零拷贝）
- 返回 `std::string`（需要所有权的结果）
- 所有函数 `inline`（header-only）

### 4.2 io.hpp — 文件工具

**命名空间:** `mana::util::io`

```cpp
#pragma once
#include <string>
#include <string_view>
#include <optional>
#include <vector>

namespace mana::util::io {

// 读取整个文件
inline std::optional<std::string> read_all(std::string_view path);

// 写入文件
inline bool write(std::string_view path, std::string_view content);

// 追加写入
inline bool append(std::string_view path, std::string_view content);

// 检查文件是否存在
inline bool exists(std::string_view path);

// 获取文件大小
inline std::optional<size_t> file_size(std::string_view path);

// 读取所有行
inline std::vector<std::string> read_lines(std::string_view path);

} // namespace mana::util::io
```

**设计决策:**
- 无状态函数，移除 FileManager 类
- 使用 `std::string_view` 参数
- 错误用 `std::optional` 表示
- 无 Pimpl，无 RAII 状态

### 4.3 math.hpp — 数学函数

**命名空间:** `mana::util::math`

```cpp
#pragma once
#include <cmath>
#include <cstdint>
#include <limits>

namespace mana::util::math {

// 常量
inline constexpr double pi = 3.14159265358979323846;
inline constexpr double e  = 2.71828182845904523536;
inline constexpr double epsilon = std::numeric_limits<double>::epsilon();

// 基础函数（constexpr）
template<typename T>
inline constexpr T clamp(T value, T min, T max);

template<typename T>
inline constexpr T lerp(T a, T b, T t);

template<typename T>
inline constexpr T abs(T value);

template<typename T>
inline constexpr T min(T a, T b);

template<typename T>
inline constexpr T max(T a, T b);

// 度/弧度转换
inline constexpr double to_radians(double degrees);
inline constexpr double to_degrees(double radians);

// 三角函数（constexpr 近似，精度 ~1e-10）
inline constexpr double sin(double x);
inline constexpr double cos(double x);
inline constexpr double tan(double x);

// 平方根（constexpr 近似）
inline constexpr double sqrt(double x);

} // namespace mana::util::math
```

**设计决策:**
- `clamp`, `lerp`, `abs`, `min`, `max` 是真正的 `constexpr`
- 三角函数使用泰勒级数展开实现 `constexpr` 近似
- 精度约 1e-10，足够大多数用途
- 使用模板支持多种数值类型

### 4.4 geo.hpp — 几何/地理

**命名空间:** `mana::util::geo`

#### 几何基础结构

```cpp
#pragma once
#include <cmath>
#include <array>
#include <vector>
#include <optional>

namespace mana::util::geo {

// 2D 点
struct Point {
    double x, y;
    
    constexpr double distance_to(const Point& other) const;
    constexpr Point operator+(const Point& other) const;
    constexpr Point operator-(const Point& other) const;
};

// 2D 矩形
struct Rect {
    Point min, max;  // 左下角, 右上角
    
    constexpr double width() const;
    constexpr double height() const;
    constexpr double area() const;
    constexpr bool contains(const Point& p) const;
    constexpr bool intersects(const Rect& other) const;
    constexpr Rect intersection(const Rect& other) const;
};

// 2D 圆
struct Circle {
    Point center;
    double radius;
    
    constexpr double area() const;
    constexpr double circumference() const;
    constexpr bool contains(const Point& p) const;
    constexpr bool intersects(const Circle& other) const;
};

// 2D 线段
struct Segment {
    Point a, b;
    
    constexpr double length() const;
    constexpr double distance_to(const Point& p) const;
    constexpr bool intersects(const Segment& other) const;
    constexpr std::optional<Point> intersection(const Segment& other) const;
};
```

#### 地理坐标

```cpp
// 地理坐标（经纬度）
struct GeoPoint {
    double latitude;   // 纬度 [-90, 90]
    double longitude;  // 经度 [-180, 180]
};

// 常量
inline constexpr double earth_radius_m = 6371000.0;  // 地球半径（米）

// 距离计算
inline double haversine_distance(const GeoPoint& a, const GeoPoint& b);

// 方位角（弧度）
inline double bearing(const GeoPoint& from, const GeoPoint& to);

// 目标点（给定起点、距离、方位角）
inline GeoPoint destination(const GeoPoint& from, double distance, double bearing_rad);
```

#### 坐标转换

```cpp
// 坐标系枚举
enum class CoordSystem {
    WGS84,   // GPS 坐标
    GCJ02,   // 火星坐标（中国）
    BD09     // 百度坐标
};

// 坐标转换
inline GeoPoint convert(const GeoPoint& point, CoordSystem from, CoordSystem to);

// 便捷函数
inline GeoPoint wgs84_to_gcj02(const GeoPoint& point);
inline GeoPoint gcj02_to_wgs84(const GeoPoint& point);
inline GeoPoint gcj02_to_bd09(const GeoPoint& point);
inline GeoPoint bd09_to_gcj02(const GeoPoint& point);
```

#### 空间索引（四叉树）

```cpp
// 四叉树
class QuadTree {
public:
    QuadTree(const Rect& bounds, size_t capacity = 8);
    
    // 插入点
    bool insert(const Point& point);
    
    // 范围查询
    std::vector<Point> query(const Rect& range) const;
    
    // 最近邻查询
    std::vector<Point> query_nearest(const Point& point, size_t count) const;
    
    // 清空
    void clear();
    
    // 统计
    size_t size() const;
    
private:
    struct Node;
    std::unique_ptr<Node> root_;
    Rect bounds_;
    size_t capacity_;
};
```

**设计决策:**
- 几何结构体全部 `constexpr`（编译时可计算）
- 地理函数 `inline`（运行时，涉及浮点精度）
- 坐标转换基于 GCJ02 偏移算法
- 四叉树是唯一有状态的类（用于空间索引）

### 4.5 util.hpp — 通用工具

**命名空间:** `mana::util`

```cpp
#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <chrono>
#include <type_traits>
#include <optional>

namespace mana::util {

// 类型转换
template<typename T>
inline std::string to_string(T value);

template<typename T>
inline std::optional<T> from_string(std::string_view str);

// 容器操作
template<typename T>
inline bool contains(const std::vector<T>& vec, const T& value);

template<typename T>
inline void remove(std::vector<T>& vec, const T& value);

template<typename T>
inline std::vector<T> filter(const std::vector<T>& vec, auto predicate);

template<typename T, typename F>
inline auto map(const std::vector<T>& vec, F func);

// 时间戳
inline int64_t timestamp_ms();
inline int64_t timestamp_s();
inline std::string timestamp_iso8601();

// 范围检查
template<typename T>
inline constexpr bool in_range(T value, T min, T max);

// 字节操作
inline std::string to_hex(std::string_view data);
inline std::optional<std::string> from_hex(std::string_view hex);

} // namespace mana::util
```

## 5. 文件变更清单

| 操作 | 文件 | 说明 |
|------|------|------|
| 删除 | `include/mana/util.h` | 旧头文件 |
| 删除 | `src/util.cpp` | 旧实现 |
| 重构 | `include/mana/util/string.hpp` | 字符串操作 |
| 重构 | `include/mana/util/io.hpp` | 文件工具（无状态） |
| 新增 | `include/mana/util/util.hpp` | 通用工具 |
| 新增 | `include/mana/util/math.hpp` | 数学函数 |
| 新增 | `include/mana/util/geo.hpp` | 几何/地理 |
| 修改 | `include/mana/mana.h` | 更新 include 路径 |
| 修改 | `CMakeLists.txt` | 移除 src/util.cpp |
| 修改 | `tests/test_util.cpp` | 更新命名空间 |

## 6. 命名空间迁移

| 旧接口 | 新接口 |
|--------|--------|
| `mana::util::split()` | `mana::util::string::split()` |
| `mana::util::trim()` | `mana::util::string::trim()` |
| `mana::util::to_lower()` | `mana::util::string::to_lower()` |
| `mana::util::to_upper()` | `mana::util::string::to_upper()` |
| `mana::util::starts_with()` | `mana::util::string::starts_with()` |
| `mana::util::ends_with()` | `mana::util::string::ends_with()` |
| `mana::util::current_timestamp_ms()` | `mana::util::timestamp_ms()` |
| `mana::util::current_timestamp_s()` | `mana::util::timestamp_s()` |
| `mana::io::FileManager::read_all()` | `mana::util::io::read_all()` |
| `mana::io::FileManager::write()` | `mana::util::io::write()` |
| `mana::io::FileManager::exists()` | `mana::util::io::exists()` |
| `mana::io::FileManager::file_size()` | `mana::util::io::file_size()` |

## 7. 测试策略

每个模块独立测试文件：

- `tests/test_string.cpp` — 字符串操作测试
- `tests/test_io.cpp` — 文件工具测试
- `tests/test_math.cpp` — 数学函数测试（精度验证）
- `tests/test_geo.cpp` — 几何/地理测试
- `tests/test_util.cpp` — 通用工具测试

## 8. 构建变更

**CMakeLists.txt 修改:**

```cmake
# 移除
src/util.cpp

# 头文件自动包含（无需修改）
# include/mana/util/*.hpp 已在 target_include_directories 中
```

## 9. 编译时优化

- `constexpr` 函数可在编译时求值
- `inline` 避免多重定义
- `std::string_view` 避免字符串拷贝
- 模板实例化由编译器优化

## 10. 设计决策日志

| 决策 | 选择 | 理由 |
|------|------|------|
| 实现风格 | 纯 header-only | 现代 C++ 最佳实践，无链接问题 |
| 输入参数 | `std::string_view` | 零拷贝，兼容 `std::string` 和字面量 |
| 返回类型 | `std::string` | 需要所有权时返回拥有的字符串 |
| 错误表示 | `std::optional` | 无异常，明确表示可能失败 |
| 数学精度 | ~1e-10 | constexpr 近似足够大多数用途 |
| 空间索引 | 四叉树 | 简单高效，适合 2D 点查询 |

---

**文档作者:** Claude (Brainstorming Mode)
**最后更新:** 2026-07-10
