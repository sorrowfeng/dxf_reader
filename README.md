# dxf_reader

在qt中用于解析dxf文件

将dxflib中不同图形的实体抽离出同一个父类，方便多态使用

为父类添加Attibute信息，可通过不同实体的指针，得的各自对应的layer，linetype等信

将polyline及spline的顶点数据等，放在多段线同一个结构体中


# 使用示例
``` cpp
#include <iostream>
#include "dxfreader.h"

int main(int argc, char *argv[])
{
    DxfReader dxf("test.dxf");

    for (const auto& entity : dxf.getAllEntities()) {
        // 打印通用信息
        std::cout << "--------------------" << std::endl;
        std::cout << "Entity Type:" << entity->type() << std::endl;
        std::cout << "Layer:" << entity->attribute.getLayer() << std::endl;
        std::cout << "Linetype:" << entity->attribute.getLinetype() << std::endl;
        std::cout << "Color:" << entity->attribute.getColor() << std::endl;
        std::cout << "Width:" << entity->attribute.getWidth() << std::endl;

        // 根据实体类型进行处理
        switch (entity->type()) {
        case TYPE_DL_ENTITY_LINE: {
            auto item = dynamic_cast<DL_LineData*>(entity.get());
            std::cout << item->x1 << item->y1 << item->x2 << item->y2 << std::endl;
            break;
        }
        case TYPE_DL_ENTITY_CompositePolyline:
        {
            auto item = dynamic_cast<DL_CompositePolylineData*>(entity.get());
            std::cout << item->number << item->elevation << std::endl;
            for (const auto& point: item->vertices) {
                std::cout << "point:" << point.x << point.y << point.z << point.bulge << std::endl;
            }
            break;
        }
        case TYPE_DL_ENTITY_CompositeSpline:
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

# 解析效果
![原图](https://img-blog.csdnimg.cn/direct/37826cdb9fc141a1b3b2803be55ca081.png)
![解析后](https://img-blog.csdnimg.cn/direct/1512fb7adeb54fb59682212b0a2b3c7d.png)


