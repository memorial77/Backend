# 定义编译器
CXX = g++

# 定义编译选项
CXXFLAGS = -std=c++11 -pthread

# 定义源文件
SOURCES = UnitTests.cpp ThreadCache.cpp CentralCache.cpp PageCache.cpp ConCurrentAlloc.cpp

# 定义最终目标
TARGET = app

# 默认目标
all: $(TARGET)

# 链接目标，直接从源文件编译和链接
$(TARGET):
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

# 清理规则
clean:
	rm -f $(TARGET)