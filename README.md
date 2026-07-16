# 《从零打造渲染器》课程源码

本项目是 B 站《从零打造渲染器》课程的配套源码。课程视频请关注 UP 主 **发财学长** 的 B 站频道：

- 📺 课程合集：<https://space.bilibili.com/241656343/lists/8333706>

---

## 课程简介

**欢乐图形学 第一季 —— 《从零打造渲染器》**

本系列课程预计共 **25 讲**，将带你使用 **C++ 从零实现一个完整的离线渲染器**。

- 课程内容深度对标经典著作 **《Physically Based Rendering: From Theory to Implementation》(PBRT)**，但会按照更符合学习曲线的方式重新组织。
- 本教程 **不是** 那种照念 PPT 或 AI 生成的搬运课，而是一套完全 **原创的硬核实践教程**。
- 每一个数学公式、每一行代码，都会有 **详细讲解**。

---

## 目录结构

```
PrivateFortuneRenderer/
├── source/     # 各章节的 C++ 源码，按章节分目录（Chapter02、Chapter03 …）
├── scenes/     # 场景描述文件（XML 格式）
├── glm/        # 第三方数学库（GLM）
├── minifb/     # 第三方窗口/framebuffer 库（MiniFB）
├── CMakeLists.txt
├── build.bat   # Windows 一键构建脚本
└── build.sh    # Linux / macOS 一键构建脚本
```

---

## 项目构建

### Windows

1. 确保已安装 **Visual Studio** 和 **CMake**。
2. 下载/克隆本项目后，双击运行 `build.bat`。
3. 构建脚本会在 `build/` 目录下生成 `FortuneRenderer.sln`，用 Visual Studio 打开该解决方案即可进行编译、调试。

### Linux / macOS

1. 确保已安装 **CMake** 和 **GCC/G++**（或 Clang）。
2. 在项目根目录执行：

   ```bash
   ./build.sh
   cd build
   make
   ```

3. 编译完成后即可运行对应章节的可执行文件。

如果在 Linux 上遇到 **链接库缺失** 相关的编译错误，请先安装以下依赖（Ubuntu / Debian 系）：

```bash
sudo apt install -y \
    libx11-dev libxext-dev libxrandr-dev libxinerama-dev libxcursor-dev \
    mesa-common-dev libgl1-mesa-dev libxkbcommon-dev
```

---

## 已发布的课程章节

- <a href="https://www.bilibili.com/video/BV1BMEs6UEp7/" target="_blank" rel="noopener noreferrer">第 0 章：图形学导论</a>
- <a href="https://www.bilibili.com/video/BV1CZEW6xEng/" target="_blank" rel="noopener noreferrer">第 1 章（上）：3D 数学（上）</a>
- <a href="https://www.bilibili.com/video/BV1rcEs6WE2p/" target="_blank" rel="noopener noreferrer">第 1 章（下）：3D 数学（下）</a>
- <a href="https://www.bilibili.com/video/BV1ZuJJ6iEuu/" target="_blank" rel="noopener noreferrer">第 2 章：基础框架搭建</a>
- <a href="https://www.bilibili.com/video/BV1Q2JF6zEVQ/" target="_blank" rel="noopener noreferrer">第 3 章：摄像机</a>
- <a href="https://www.bilibili.com/video/BV1NFjq6dEXk/" target="_blank" rel="noopener noreferrer">第 4 章：基础几何体</a>
- <a href="https://www.bilibili.com/video/BV13fjk6DEh2/" target="_blank" rel="noopener noreferrer">第 5 章：3D 场景</a>
- <a href="https://www.bilibili.com/video/BV1J27E6vEYU/" target="_blank" rel="noopener noreferrer">第 6 章：辐射度量学</a>
- <a href="https://www.bilibili.com/video/BV1zpTK6TEr5/" target="_blank" rel="noopener noreferrer">第 7 章：光源</a>
- <a href="https://www.bilibili.com/video/BV1brMc6kEZi/" target="_blank" rel="noopener noreferrer">第 8 章：材质与直接光照</a>
- <a href="https://www.bilibili.com/video/BV1JyN46dEMg/" target="_blank" rel="noopener noreferrer">第 9 章（上）：蒙特卡洛积分与采样（上）</a>
- <a href="https://www.bilibili.com/video/BV1SANt6kEb4/" target="_blank" rel="noopener noreferrer">第 9 章（下）：蒙特卡洛积分与采样（下）</a>

> 更多章节持续更新中，欢迎关注 UP 主 **发财学长** 的 B 站频道，及时获取最新课程更新。

---

## 交流与反馈

- 🎬 B 站主页：<https://space.bilibili.com/241656343>
- 📚 课程合集：<https://space.bilibili.com/241656343/lists/8333706>

如果本课程对你有帮助，欢迎 **一键三连** 支持作者持续更新！
