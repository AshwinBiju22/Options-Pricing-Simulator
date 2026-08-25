set - # Stops the script 
g++ -O3 -Wall -shared -std=c++20 -fPIC \
    $(python3 -m pybind11 --includes) \
    src/cpp/bindings.cpp \
    -o src/python/engine$(python3-config --extension-suffix)
echo "Built src/python/engine module."