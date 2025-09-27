cmake_minimum_required(VERSION 3.16)

# 项目名称和版本
project(svgRenderer
    VERSION 1.0.0
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找Qt 6组件
find_package(Qt6 6.2 COMPONENTS
    Core
    Gui
    Widgets
    Xml
    REQUIRED
)

# 自动处理Qt的元对象编译
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 源文件列表
set(SOURCES
    src/SvgElement.cpp
    src/SvgStyle.cpp
    src/SvgPen.cpp
    src/SvgBrush.cpp
    src/SvgTransform.cpp
    src/SvgDocument.cpp
    src/SvgRenderer.cpp
    src/main.cpp
)

# 头文件列表
set(HEADERS
    include/SvgElement.h
    include/SvgStyle.h
    include/SvgPen.h
    include/SvgBrush.h
    include/SvgTransform.h
    include/SvgDocument.h
    include/SvgRenderer.h
)

# 创建可执行文件
qt_add_executable(SvgRenderer
    ${SOURCES}
    ${HEADERS}
    include/SvgElementFactory.h
    include/SvgRect.h
    include/SvgCircle.h
    include/SvgText.h
    include/SvgGroup.h
    src/SvgElementFactory.cpp
    src/SvgRect.cpp
    src/SvgCircle.cpp
    src/Svgtext.cpp
    src/SvgGroup.cpp
)

# 包含头文件目录
target_include_directories(SvgRenderer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接Qt库
target_link_libraries(SvgRenderer
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Xml
)

# 安装配置（可选）
install(TARGETS SvgRenderer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

