#include "dxfreader.h"

// 检测是否支持 C++14 或更高版本
#if __cplusplus >= 201402L
    // 如果支持 C++14 或更高版本，直接使用 std::make_unique
    #define MAKE_UNIQUE std::make_unique
#else
    // 如果不支持 C++14，自定义 make_unique 函数
    namespace custom {
        template<typename T, typename... Args>
        std::unique_ptr<T> make_unique(Args&&... args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
    }
    #define MAKE_UNIQUE custom::make_unique
#endif

template<typename EntityType>
void addEntity(std::vector<std::shared_ptr<DL_EntityData>>& entities,
               const EntityType& data, const DL_Attributes& attribute) {
    auto entity = std::make_shared<EntityType>(data);
    entity->attribute = attribute;
    entities.push_back(std::move(entity));
}

DxfReader::DxfReader(const std::string &fileName)
{    
    // 读取 dxf 文件
    DL_Dxf *dxf = new DL_Dxf;
    if (!dxf->in(fileName.c_str(), this)) { // if file open failed
        std::cerr << fileName << " could not be opened.\n" << std::endl;
        return;
    }
    delete dxf;
    dxf = nullptr;
}

void DxfReader::addBlock(const DL_BlockData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    //std::cout << data.name.c_str() << std::endl;
}

void DxfReader::endBlock()
{
    //std::cout << Q_FUNC_INFO;
}

void DxfReader::addLayer(const DL_LayerData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    //std::cout << data.name.c_str() << std::endl;
}

void DxfReader::addLinetype(const DL_LinetypeData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    //std::cout << data.name.c_str() << data.description.c_str() << std::endl;
}

void DxfReader::addLinetypeDash(double length)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addText(const DL_TextData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addLine(const DL_LineData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addArc(const DL_ArcData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addCircle(const DL_CircleData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addEllipse(const DL_EllipseData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addPolyline(const DL_PolylineData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
//    addEntity(allEntities, data, attributes);

    // 如果当前有多段线未完成，先提交它
    if (currentPolyline) {
        finalizePolyline();
    }

    // 创建新的多段线对象
    currentPolyline = MAKE_UNIQUE<DL_CompositePolylineData>(
        data.number, data.flags, data.elevation);
    currentPolyline->attribute = attributes;
}

void DxfReader::addVertex(const DL_VertexData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
//    addEntity(allEntities, data, attributes);

    // 将顶点添加到当前多段线
    if (currentPolyline) {
        currentPolyline->vertices.push_back(data);

        // 检查是否已经完成当前多段线的解析
        if (currentPolyline->vertices.size() == currentPolyline->number) {
            finalizePolyline();
        }
    } else {
        std::cout << "Vertex without a polyline context!" << std::endl;
    }
}

void DxfReader::addPoint(const DL_PointData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addSpline(const DL_SplineData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
//    addEntity(allEntities, data, attributes);


    // 如果当前有样条曲线未完成，先提交它
    if (currentSpline) {
        finalizeSpline();
    }

    // 创建新的样条曲线对象
    currentSpline = MAKE_UNIQUE<DL_CompositeSplineData>(
        data.degree, data.nKnots, data.nControl, data.nFit, data.flags);
    currentSpline->attribute = attributes;
}

void DxfReader::addControlPoint(const DL_ControlPointData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;

    // 将控制点添加到当前样条曲线
    if (currentSpline) {
        currentSpline->controlPoints.push_back(data);

        // 检查是否已经完成所有控制点的解析
        if (currentSpline->controlPoints.size() == currentSpline->nControl &&
            currentSpline->knots.size() == currentSpline->nKnots &&
            currentSpline->fitPoints.size() == currentSpline->nFit) {
            finalizeSpline();
        }
    } else {
        std::cout << "Control point without a spline context!"  << std::endl;
    }
}

void DxfReader::addKnot(const DL_KnotData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;

    // 将节点添加到当前样条曲线
    if (currentSpline) {
        currentSpline->knots.push_back(data);

        // 检查是否已经完成所有节点的解析
        if (currentSpline->controlPoints.size() == currentSpline->nControl &&
            currentSpline->knots.size() == currentSpline->nKnots &&
            currentSpline->fitPoints.size() == currentSpline->nFit) {
            finalizeSpline();
        }
    } else {
        std::cout << "Knot without a spline context!" << std::endl;
    }
}

void DxfReader::addFitPoint(const DL_FitPointData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;

    // 将拟合点添加到当前样条曲线
    if (currentSpline) {
        currentSpline->fitPoints.push_back(data);

        // 检查是否已经完成所有拟合点的解析
        if (currentSpline->controlPoints.size() == currentSpline->nControl &&
            currentSpline->knots.size() == currentSpline->nKnots &&
            currentSpline->fitPoints.size() == currentSpline->nFit) {
            finalizeSpline();
        }
    } else {
        std::cout << "Fit point without a spline context!" << std::endl;
    }
}

void DxfReader::addXLine(const DL_XLineData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addRay(const DL_RayData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addInsert(const DL_InsertData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addSolid(const DL_SolidData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addTrace(const DL_TraceData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addTextStyle(const DL_StyleData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addMTextChunk(const std::string &text)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addMText(const DL_MTextData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addArcAlignedText(const DL_ArcAlignedTextData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addAttribute(const DL_AttributeData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addDimAlign(const DL_DimensionData &data, const DL_DimAlignedData &edata)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addDimLinear(const DL_DimensionData &data, const DL_DimLinearData &edata)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addDimRadial(const DL_DimensionData &data, const DL_DimRadialData &edata)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addDimDiametric(const DL_DimensionData &data, const DL_DimDiametricData &edata)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addDimAngular(const DL_DimensionData &data, const DL_DimAngularData &edata)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addDimAngular3P(const DL_DimensionData &data, const DL_DimAngular3PData &edata)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addDimOrdinate(const DL_DimensionData &data, const DL_DimOrdinateData &edata)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addLeader(const DL_LeaderData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addLeaderVertex(const DL_LeaderVertexData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addHatch(const DL_HatchData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::addHatchLoop(const DL_HatchLoopData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addHatchEdge(const DL_HatchEdgeData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addImage(const DL_ImageData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
    addEntity(allEntities, data, attributes);
}

void DxfReader::linkImage(const DL_ImageDefData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addXRecord(const std::string &handle)
{
}

void DxfReader::addXRecordString(int code, const std::string &value)
{
}

void DxfReader::addXRecordReal(int code, double value)
{
}

void DxfReader::addXRecordInt(int code, int value)
{
}

void DxfReader::addXRecordBool(int code, bool value)
{
}

void DxfReader::addXDataApp(const std::string &appId)
{
}

void DxfReader::addXDataString(int code, const std::string &value)
{
}

void DxfReader::addXDataReal(int code, double value)
{
}

void DxfReader::addXDataInt(int code, int value)
{
}

void DxfReader::addDictionary(const DL_DictionaryData &data)
{
}

void DxfReader::addDictionaryEntry(const DL_DictionaryEntryData &data)
{
}

void DxfReader::setVariableVector(const std::string &key, double v1, double v2, double v3, int code)
{
}

void DxfReader::setVariableString(const std::string &key, const std::string &value, int code)
{
}

void DxfReader::setVariableInt(const std::string &key, int value, int code)
{
}

void DxfReader::setVariableDouble(const std::string &key, double value, int code)
{
}

void DxfReader::add3dFace(const DL_3dFaceData &data)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::addComment(const std::string &comment)
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::endSequence()
{
    //std::cout << Q_FUNC_INFO << std::endl;
}

void DxfReader::printAttributes()
{
    //std::cout << "  Attributes: Layer: " << attributes.getLayer().c_str() << std::endl;
    //std::cout << " Color: " << std::endl;
    if (attributes.getColor()==256)	{
        //std::cout << "BYLAYER" << std::endl;
    } else if (attributes.getColor()==0) {
        //std::cout << "BYBLOCK" << std::endl;
    } else {
        //std::cout << attributes.getColor();
    }
    //std::cout << " Width: " << std::endl;
    if (attributes.getWidth()==-1) {
        //std::cout << "BYLAYER" << std::endl;
    } else if (attributes.getWidth()==-2) {
        //std::cout << "BYBLOCK" << std::endl;
    } else if (attributes.getWidth()==-3) {
        //std::cout << "DEFAULT" << std::endl;
    } else {
        //std::cout << attributes.getWidth() << std::endl;
    }
    //std::cout << " Type: " << attributes.getLinetype().c_str() << std::endl;
}


void DxfReader::finalizePolyline()
{
    if (!currentPolyline) {
        return;
    }

    // 将完整多段线添加到全局队列
    allEntities.push_back(std::move(currentPolyline));

    // 清除当前多段线上下文
    currentPolyline.reset();
}

void DxfReader::finalizeSpline()
{
    if (!currentSpline) {
        return;
    }

    // 确保所有组件都已解析
    if (currentSpline->controlPoints.size() != currentSpline->nControl ||
        currentSpline->knots.size() != currentSpline->nKnots ||
        currentSpline->fitPoints.size() != currentSpline->nFit) {
        std::cout << "Incomplete spline data!" << std::endl;
        return;
    }

    // 将完整样条曲线添加到全局队列
    allEntities.push_back(std::move(currentSpline));

    // 清除当前样条曲线上下文
    currentSpline.reset();
}
