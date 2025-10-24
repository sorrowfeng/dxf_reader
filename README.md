# 🏗️ DXF 文件解析工具

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

int main(int argc, char *argv[])
{
    DxfReader dxf("test.dxf");

    for (const auto& entity : dxf.getAllEntities()) {
        // 打印通用信息
        std::cout << "--------------------" << std::endl;
        std::cout << "Entity Type:" << (int)entity->type() << std::endl;
        std::cout << "Layer:" << entity->attribute.getLayer() << std::endl;
        std::cout << "Linetype:" << entity->attribute.getLinetype() << std::endl;
        std::cout << "Color:" << entity->attribute.getColor() << std::endl;
        std::cout << "Width:" << entity->attribute.getWidth() << std::endl;

        // 根据实体类型进行处理
        switch (entity->type()) {
        case DL_EntityType::TYPE_DL_ENTITY_LINE: {
            auto item = dynamic_cast<DL_LineData*>(entity.get());
            std::cout << item->x1 << item->y1 << item->x2 << item->y2 << std::endl;
            break;
        }
        case DL_EntityType::TYPE_DL_ENTITY_CompositePolyline:
        {
            auto item = dynamic_cast<DL_CompositePolylineData*>(entity.get());
            std::cout << item->number << item->elevation << std::endl;
            for (const auto& point: item->vertices) {
                std::cout << "point:" << point.x << point.y << point.z << point.bulge << std::endl;
            }
            break;
        }
        case DL_EntityType::TYPE_DL_ENTITY_CompositeSpline:
        {
            auto item = dynamic_cast<DL_CompositeSplineData*>(entity.get());
            std::cout << item->nFit << item->degree << item->nKnots << item->nControl << std::endl;
            for (const auto& point: item->controlPoints)  {
                std::cout << "control point:" << point.x << point.y << point.z << point.w << std::endl;
            }
            for (const auto& point: item->fitPoints)  {
                std::cout << "fit point:" << point.x << point.y << point.z << std::endl;
            }
            for (const auto& knot: item->knots)  {
                std::cout << "knot:" << knot.k << std::endl;
            }

            break;
        }
        // Other type...
        }
        std::cout << "--------------------" << std::endl;
    }

    return 0;
}
```

## 🖼️ 效果展示

| 原始DXF文件 | 解析渲染效果 |
|-------------|-------------|
| ![原图](https://img-blog.csdnimg.cn/direct/37826cdb9fc141a1b3b2803be55ca081.png) | ![解析后](https://img-blog.csdnimg.cn/direct/1512fb7adeb54fb59682212b0a2b3c7d.png) |

## 📦 项目集成

### 使用 CMake

```cmake
# 添加dxflib为子模块
add_subdirectory(dxflib)

# 链接到主项目
target_link_libraries(your_target PRIVATE dxflib)
```

## 📜 开源协议

本项目采用 Apache-2.0 许可证，允许自由使用于商业和非商业项目。
