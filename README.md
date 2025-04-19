<h1 align="center">🎮 JkrGUI</h1>
<p align="center"><i>A Cross-Platform Vulkan-Based Graphics Library for Lua & C++</i></p>
<p align="center"><b>Platform Support:</b> Windows ✅ | Android ✅ | macOS ✅</p>

<p align="center">
  <img src="https://img.shields.io/badge/build-chaotic-orange?style=flat-square&logo=github" />
  <img src="https://img.shields.io/badge/bugs-features-informational?style=flat-square&logo=bugatti" />
  <img src="https://img.shields.io/badge/api-none-critical?style=flat-square&logo=postman" />
  <img src="https://img.shields.io/badge/docs-wiki%20hunt-yellow?style=flat-square&logo=readthedocs" />
  <img src="https://img.shields.io/badge/memory%20leaks-none%20modern%20cpp20-red?style=flat-square&logo=leakcanary" />
  <img src="https://img.shields.io/badge/fps-uncapped-lightgrey?style=flat-square&logo=directx" />
</p>
---
## 🛠️ Building JkrGUI from Source

Follow these steps to build the JkrGUI engine on your platform. If it breaks, it’s probably your fault (or Vulkan’s).

### 🔧 Step 1: Install Vulkan SDK

1. Go to [https://vulkan.lunarg.com/sdk/home](https://vulkan.lunarg.com/sdk/home)
2. Download and install the Vulkan SDK for your OS (Windows/macOS/Linux)
3. Make sure the `VULKAN_SDK` environment variable is set correctly  
   (e.g., `C:/VulkanSDK/1.3.275.0`)

---

### 🌀 Step 2: Clone the Repo

```bash
git clone https://github.com/theanimatorspal/jkrgui.git
cd jkrgui
```

---

### 📄 Step 3: Configure with `CMakeConfig.cmake`

Create a file named `CMakeConfig.cmake` inside the `jkrgui/` folder with the following content:

```cmake
set(CMAKE_JKRGUI_DIR <path-to-jkrgui-folder>)
set(VULKAN_SDK <path-to-vulkan-sdk>)
set(VULKAN_VERSION <vulkan-version>)
```

**Example**:

```cmake
set(CMAKE_JKRGUI_DIR D:/Projects/jkrgui)
set(VULKAN_SDK C:/VulkanSDK/1.3.275.0)
set(VULKAN_VERSION 1.3.275.0)
```

---

### 🧱 Step 4: Build with CMake

#### 🪟 Windows (Visual Studio):

```bash
cmake -B out/build/VS -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
cmake --build out/build/VS --config Release
```

#### 🍎 macOS (Ninja):

```bash
brew install ninja
cmake -B out/build/Ninja -G Ninja -DCMAKE_BUILD_TYPE=Release
```

#### 🤖 Android (with NDK):

1. Install the Android NDK (via Android Studio or command line)
2. Set the `ANDROID_NDK_HOME` environment variable
3. Then run:

```bash
cmake -B out/build/Android \
  -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK_HOME%/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-24 \
  -DCMAKE_BUILD_TYPE=Release
```

---

### 🧩 Step 5: Final Setup

After compiling, find your built executable here:

```
out/build/<Preset>/jkrgui/jkrgui(.exe on Windows)
```

If you want to run `jkrgui` globally from any terminal:

- **Windows:** Add the path to your Environment Variables → `PATH`
- **macOS/Linux:** Add it to your `.bashrc`, `.zshrc`, or equivalent shell config

```bash
export PATH=$PATH:/your/path/to/jkrgui
```

> 💡 Pro Tip: If something goes wrong, blame the SDK, not the code.



## 🚀 Projects Built with JkrGUI

### 🔬 <a href="https://github.com/theanimatorspal/SANN">SANN – Simulated Annealing + Neural Network Visualizer</a>
A visualizer built using JkrGUI to demonstrate how simulated annealing can optimize a neural network's learning process.

<img src="https://github.com/user-attachments/assets/0ccc91be-2780-451a-bbf8-231a21313ca2" alt="SANN Visualizer" width="600"/>

---

### 🎯 <a href="https://github.com/theanimatorspal/SampraharReturns">Samprahar Returns – 3D Third-Person Shooter</a>  
<b>The first 3D game developed with JkrGUI!</b>  
A third-person air-gun shooting game where the player must push a cube off a platform using physics-based mechanics.

> 🕹️ <i>This game showcases JkrGUI's 3D rendering and physics capabilities.</i>

<b>🎥 Current Gameplay Preview:</b>  
<a href="https://github.com/theanimatorspal/SampraharReturns/assets/133136356/9ad9eeb9-43a8-4168-abcd-60f970d642e8">
  <img src="https://github.com/theanimatorspal/SampraharReturns/assets/133136356/9ad9eeb9-43a8-4168-abcd-60f970d642e8" alt="Gameplay Preview" width="600"/>
</a>

---

### ✨ <a href="https://github.com/theanimatorspal/JkrGUI-PBRTest">JkrGUI PBR Test</a>
A physically-based rendering (PBR) test showcasing realistic lighting and material effects powered by Vulkan.

<img src="https://github.com/user-attachments/assets/f66769ce-e3a7-4a18-b601-2b97e99c9055" alt="PBR Demo" width="600"/>

---

### 📦 <a href="https://github.com/theanimatorspal/PresentTest-for-JkrGUI">PresentTest-for-JkrGUI</a>
A simple test project used to evaluate frame presentation, rendering loop performance, and cross-platform windowing using the JkrGUI backend.

---

> 🧠 <b>Want to contribute or build your own game or visualizer?</b>  
Explore the source code, fork the repo, and start building with Vulkan-powered graphics for Lua and C++!

---

