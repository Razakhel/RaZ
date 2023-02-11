<h1 align="center">
    <img alt="RaZ logo" src="https://i.imgur.com/rwGGcpr.png" />
    <br/>
    RaZ
</h1>

<h4 align="center">Modern &amp; multiplatform 3D game engine in C++17</h4>

---

<table>
    <thead>
        <tr>
            <th></th>
            <th>
                <img alt="Linux build" src="https://www.screenconnect.com/Images/LogoLinux.png" align="center" height="30" width="30" /><br />
                Linux
            </th>
            <th>
                <img alt="Windows build" src="https://upload.wikimedia.org/wikipedia/commons/thumb/7/76/Windows_logo_-_2012_%28dark_blue%2C_lines_thinner%29.svg/414px-Windows_logo_-_2012_%28dark_blue%2C_lines_thinner%29.svg.png" align="center" height="30" width="30" /><br />
                Windows
            </th>
            <th>
                <img alt="macOS build" src="https://upload.wikimedia.org/wikipedia/commons/thumb/f/fa/Apple_logo_black.svg/245px-Apple_logo_black.svg.png" align="center" height="30" width="25" /><br />
                macOS
            </th>
            <th>
                <img alt="WASM build" src="https://upload.wikimedia.org/wikipedia/commons/thumb/3/30/WebAssembly_Logo.png/150px-WebAssembly_Logo.png" align="center" height="30" width="30" /><br />
                WebAssembly
            </th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <th align="center">Compilation</th>
            <td align="center"><a alt="Linux build status" href="https://github.com/Razakhel/RaZ/actions"><img src="https://github.com/Razakhel/RaZ/actions/workflows/RaZ.yml/badge.svg?branch=master" /></a></td>
            <td align="center"><a alt="Windows build status" href="https://github.com/Razakhel/RaZ/actions"><img src="https://github.com/Razakhel/RaZ/actions/workflows/RaZ.yml/badge.svg?branch=master" /></a></td>
            <td align="center"><a alt="macOS build status" href="https://github.com/Razakhel/RaZ/actions"><img src="https://github.com/Razakhel/RaZ/actions/workflows/RaZ.yml/badge.svg?branch=master" /></a></td>
            <td align="center"><a alt="WASM build status" href="https://github.com/Razakhel/RaZ/actions"><img src="https://github.com/Razakhel/RaZ/actions/workflows/RaZ.yml/badge.svg?branch=master" /></a></td>
        </tr>
        <tr>
            <th align="center">Unit tests</th>
            <td align="center"><a alt="Linux tests build status" href="https://github.com/Razakhel/RaZ/actions"><img src="https://github.com/Razakhel/RaZ/actions/workflows/RaZ.yml/badge.svg?branch=master" /></a></td>
            <td align="center">No CI yet</td>
            <td align="center">No CI yet</td>
            <td align="center">N/A</td>
        </tr>
    </tbody>
</table>

| <img alt="Codacy" src="https://seeklogo.com/images/C/codacy-logo-1A40ABD314-seeklogo.com.png" align="center" height="30" width="30" /><br />Codacy | <img alt="Coveralls" src="https://avatars3.githubusercontent.com/ml/318?s=140&v=4" align="center" height="30" width="30" /><br />Coveralls |
|:--------------------------------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------------------:|
|      [![Codacy status](https://api.codacy.com/project/badge/Grade/2c8f744cfabc41ada9bd73f72ddd080f)](https://app.codacy.com/gh/Razakhel/RaZ)       |          [![Coverage status](https://coveralls.io/repos/github/Razakhel/RaZ/badge.svg)](https://coveralls.io/github/Razakhel/RaZ)          |

## Summary

**RaZ** comes from, as you may have noticed, my username **Raz**akhel. This was also meant as a pun, since in french _RàZ_ is the acronym for "Remise à zéro", which basically means "reset". I didn't have any particular intent behind this double meaning. Except maybe hoping that using RaZ might reset your mind because of its awesomeness? Oh well.

Written in modern C++ (17), ECS driven, it is primarily developed for learning purposes. It is intended to be very intuitive & easy to use and tinker with, while trying to stay as optimized & modular as possible.

The engine is available under Windows, Linux & macOS; it can also run in a web browser through [WebAssembly](https://webassembly.org/) (using [Emscripten](https://emscripten.org/)).

<img alt="Lua logo" src="https://upload.wikimedia.org/wikipedia/commons/thumb/c/cf/Lua-Logo.svg/30px-Lua-Logo.svg.png" height="16" width="16" /> The engine can be used with [Lua](https://www.lua.org/about.html), a scripting language that is very simple to both learn & use. For more information, see the dedicated [wiki page](https://github.com/Razakhel/RaZ/wiki/Lua).

📖 If you want to get started with RaZ, [head to the wiki](https://github.com/Razakhel/RaZ/wiki). For the documentation, [look over here](http://razakhel.github.io/RaZ/doc/)!

🖥️ [RaZor](https://github.com/Razakhel/RaZor), an editor also multiplatform, is available to manipulate the engine through a graphical interface.

💬 A [Discord server](https://discord.gg/25YGDED) dedicated to RaZ & RaZor is also available! Feel free to come by to follow the development, ask any question, or just say hi 👋

### Gallery

|                                    Crytek Sponza                                     |                                 Hylian shield (PBR)                                  |
|:------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------:|
| [![Crytek Sponza](https://i.imgur.com/Tr1nnjV.jpg)](https://i.imgur.com/Tr1nnjV.jpg) | [![Hylian shield](https://i.imgur.com/UZ90KKJ.jpg)](https://i.imgur.com/UZ90KKJ.jpg) |

More examples are available [here](https://github.com/Razakhel/RaZ/wiki/Examples).

### Projects using RaZ

|                        Name                        |           Maintainer/creator            | Description                          |
|:--------------------------------------------------:|:---------------------------------------:|--------------------------------------|
|     [Atmos](https://github.com/Razakhel/Atmos)     | [Razakhel](https://github.com/Razakhel) | Atmospheric simulation               |
|   [Midgard](https://github.com/Razakhel/Midgard)   | [Razakhel](https://github.com/Razakhel) | Terrain procedural generation        |
| [Yggdrasil](https://github.com/Razakhel/Yggdrasil) | [Razakhel](https://github.com/Razakhel) | Tree & foliage procedural generation |

If you also are working on some on your own, feel free to get in touch so that I can add them to the list.

## Features

|    Module     | Features                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
|:-------------:|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Animation** | - Skeleton data structure<br/>- Animation support _(in progress)_                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
|   **Audio**   | - Using [OpenAL Soft](https://openal-soft.org/)<br/>- Playing/pausing/stopping/repeating sounds<br/>- Positional audio sources & listener                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
|   **Data**    | - [Bounding Volume Hierarchy (BVH)](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy) acceleration structure<br/>- [Directed graph](https://en.wikipedia.org/wiki/Directed_graph) structure<br/>- Dynamic bitset<br/>- File formats:<br/>&nbsp;&nbsp;&nbsp;&nbsp;- Meshes: [OBJ](https://en.wikipedia.org/wiki/Wavefront_.obj_file) import/export, [FBX](https://en.wikipedia.org/wiki/FBX) import (using the [FBX SDK](https://www.autodesk.com/developer-network/platform-technologies/fbx)), [OFF](https://en.wikipedia.org/wiki/OFF_(file_format)) import<br/>&nbsp;&nbsp;&nbsp;&nbsp;- Images: [PNG](https://en.wikipedia.org/wiki/Portable_Network_Graphics) import/export (using [libpng](http://www.libpng.org/pub/png/libpng.html)), [TGA](https://en.wikipedia.org/wiki/Truevision_TGA) import, [HDR](https://en.wikipedia.org/wiki/RGBE_image_format) import _(in progress)_<br/>&nbsp;&nbsp;&nbsp;&nbsp;- Audio: [WAV](https://en.wikipedia.org/wiki/WAV) import/export<br/>&nbsp;&nbsp;&nbsp;&nbsp;- Animation: [BVH](https://en.wikipedia.org/wiki/Biovision_Hierarchy) import _(in progress)_ |
|   **Math**    | - Vectors, matrices & quaternions<br/>- Angles (degrees/radians)<br/>- Transformations (translation, rotation, scale)<br/>- Noise ([Perlin](https://en.wikipedia.org/wiki/Perlin_noise), [Worley](https://en.wikipedia.org/wiki/Worley_noise))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
|  **Physics**  | - Shapes (line, plane, sphere, triangle, quad, AABB, OBB)<br/>- Shape/shape collision checks _(in progress)_<br/>- Ray/shape intersection checks _(in progress)_<br/>- Rigid body simulation _(in progress)_                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| **Rendering** | - OpenGL (4.6-3.3)<br/>- Vulkan _(in progress)_<br/>- [PBR](https://en.wikipedia.org/wiki/Physically_based_rendering) (Cook-Torrance) & legacy ([Blinn-Phong](https://en.wikipedia.org/wiki/Blinn–Phong_reflection_model)) material models<br/>- [Deferred rendering](https://en.wikipedia.org/wiki/Deferred_shading), using a custom render graph<br/>- Post effects: [bloom](https://en.wikipedia.org/wiki/Bloom_(shader_effect)), [tone mapping](https://en.wikipedia.org/wiki/Tone_mapping), SSR, [SSAO](https://en.wikipedia.org/wiki/Screen_space_ambient_occlusion), ... _(in progress)_<br/>- Tessellation & compute shaders support<br/>- Camera (perspective/orthographic)<br/>- Light sources (point & directional)<br/>- Windowing (window, keyboard/mouse inputs with custom callbacks), using [GLFW](https://www.glfw.org/)<br/>- Overlay, using [ImGui](https://github.com/ocornut/imgui)<br/>- [Cubemap](https://en.wikipedia.org/wiki/Cube_mapping)<br/>- [Normal mapping](https://en.wikipedia.org/wiki/Normal_mapping)                                                                           |
| **Scripting** | - [Lua](https://www.lua.org/about.html) scripting, using [Sol2](https://github.com/ThePhD/sol2)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
|   **Misc**    | - Custom [ECS (Entity Component System)](https://en.wikipedia.org/wiki/Entity_component_system) implementation<br/>- Uniformized platform-dependent path strings<br/>- Logging utilities<br/>- Multithreading utilities, thread pool implementation & parallelization functions<br/>- Plugin utilities, to load dynamic libraries<br/>- Compiler, enum, string, file, floating-point & type utilities                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
