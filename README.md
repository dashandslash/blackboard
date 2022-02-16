# [WIP] blackboard [![Build status](https://ci.appveyor.com/api/projects/status/27ciitc8s3abuk0m?svg=true)](https://ci.appveyor.com/project/dashandslash/blackboard)

imgui multiviewport + bgfx 

Support for:
 - MacOs Metal
 - Windows d3d11
 - WebAssemply (WIP)

## build
```
mkdir .build && cd .build
cmake .. -G Xcode
cmake --build . --target blackboard_example -j
```

![Alt Text](https://github.com/dashandslash/blackboard/blob/main/example/example.gif)
