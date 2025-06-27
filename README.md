# 🏗️ DXF 文件解析工具 (Qt 版)

[![Qt 版本](https://img.shields.io/badge/Qt-5.12%2B-41cd52?logo=qt)](https://www.qt.io/)
[![构建状态](https://img.shields.io/badge/build-passing-brightgreen)]()
[![许可证](https://img.shields.io/badge/license-Apache%202.0-4285F4?logo=apache)](https://www.apache.org/licenses/LICENSE-2.0)
[![文档状态](https://img.shields.io/badge/docs-100%25-34d058)]()

基于 dxflib 封装的现代化 DXF 文件解析工具，为 CAD 数据处理提供简洁高效的解决方案。

## 🌟 核心特性

| 特性 | 描述 |
|------|------|
| **统一实体接口** | 所有图形实体继承自 `DL_Entity` 基类，支持多态操作 |
| **完整属性支持** | 提供图层、线型、颜色、线宽等完整属性访问 |
| **结构化数据** | 优化多段线和样条曲线的数据结构，便于访问 |
| **高性能解析** | 采用智能指针管理内存，支持大文件快速解析 |
| **扩展性强** | 易于添加新的实体类型和自定义属性 |

## 🚀 快速入门

### 基本用法

```cpp
#include "dxfreader.h"

int main() {
    // 1. 初始化解析器
    DxfReader dxf("design.dxf");
    
    // 2. 获取所有实体
    auto entities = dxf.getAllEntities();
    
    // 3. 遍历处理实体
    for (const auto& entity : entities) {
        // 打印基本信息
        qDebug().noquote() 
            << "🔹 类型:" << entity->typeName().ljust(10)
            << "| 图层:" << entity->layer().ljust(8)
            << "| 颜色:" << entity->color();
        
        // 类型特定处理
        if (entity->isLine()) {
            auto line = entity->asLine();
            // 处理直线数据...
        }
    }
    
    return 0;
}
```

## 📚 详细使用指南

### 1. 实体属性访问

```cpp
// 获取通用属性
QString layer = entity->layer();      // 图层名称
int color = entity->color();          // 颜色索引
QString lineType = entity->lineType();// 线型名称
double width = entity->width();       // 线宽值

// 设置属性 (支持链式调用)
entity->setLayer("WALL")
      ->setColor(1)
      ->setWidth(0.5);
```

### 2. 多段线处理

```cpp
if (entity->isPolyline()) {
    auto polyline = entity->asPolyline();
    
    qDebug() << "顶点数量:" << polyline->vertexCount();
    qDebug() << "闭合状态:" << (polyline->isClosed() ? "是" : "否");
    
    // 遍历顶点
    for (const auto& vertex : polyline->vertices()) {
        qDebug().nospace() 
            << "↳ 顶点 (" << vertex.x << ", " << vertex.y << ")"
            << " | 凸度: " << vertex.bulge;
    }
}
```

### 3. 样条曲线处理

```cpp
if (entity->isSpline()) {
    auto spline = entity->asSpline();
    
    qDebug() << "阶数:" << spline->degree();
    qDebug() << "控制点数量:" << spline->controlPointCount();
    
    // 输出控制点
    for (const auto& point : spline->controlPoints()) {
        qDebug() << "控制点:" << point.x << point.y;
    }
}
```

## 🖼️ 效果展示

| 原始DXF文件 | 解析渲染效果 |
|-------------|-------------|
| ![原图](https://img-blog.csdnimg.cn/direct/37826cdb9fc141a1b3b2803be55ca081.png) | ![解析后](https://img-blog.csdnimg.cn/direct/1512fb7adeb54fb59682212b0a2b3c7d.png) |

## 📦 项目集成

### 使用 qmake

```qmake
# 在.pro文件中添加
QT += core
INCLUDEPATH += $$PWD/dxflib
DEPENDPATH += $$PWD/dxflib

SOURCES += \
    $$PWD/dxflib/dl_dxf.cpp \
    $$PWD/dxflib/dl_creationadapter.cpp \
    $$PWD/dxfreader.cpp

HEADERS += \
    $$PWD/dxflib/dl_dxf.h \
    $$PWD/dxflib/dl_creationadapter.h \
    $$PWD/dxfreader.h
```

### 使用 CMake

```cmake
# 添加dxflib为子模块
add_subdirectory(dxflib)

# 链接到主项目
target_link_libraries(your_target PRIVATE dxflib)
```

## 📜 开源协议

本项目采用 Apache-2.0 许可证，允许自由使用于商业和非商业项目。
