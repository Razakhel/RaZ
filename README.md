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

Written in modern C++ (17), ECS driven, it is primarily developed for learning purposes. It is intended to be as optimized as possible, while trying to stay as modular as it can.

The engine is available under Windows, Linux & macOS; it can also run in a web browser through [WebAssembly](https://webassembly.org/) (using [Emscripten](https://emscripten.org/)).

📖 If you want to get started, [head to the wiki](https://github.com/Razakhel/RaZ/wiki). For the documentation, [look over here](http://razakhel.github.io/RaZ/doc/)!

💬 A Discord server dedicated to RaZ is [also available](https://discord.gg/25YGDED)! Feel free to come by to follow the development, ask any question, or just say hi 👋

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

|    Module     | Features                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
|:-------------:|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Animation** | - Skeleton data structure<br/>- Animation support _(in progress)_<br/>- BVH import _(in progress)_                                                                                                                                                                                                                                                                                                                                                                                              |
|   **Audio**   | - Playing/pausing/stopping/repeating sounds<br/>- Positional audio<br/>- WAV import                                                                                                                                                                                                                                                                                                                                                                                                             |
|   **Math**    | - Vectors, matrices & quaternions<br/>- Angles (degrees/radians)<br/>- Transformations (translation, rotation, scale)<br/>- Noise (Perlin)                                                                                                                                                                                                                                                                                                                                                      |
| **Rendering** | - OpenGL (4.6-3.3)<br/>- Vulkan _(in progress)_<br/>- Standard (Blinn-Phong) & PBR (Cook-Torrance) material models<br/>- Deferred rendering, using a custom render graph<br/>- Tessellation & compute shaders support<br/>- Camera (perspective/orthographic)<br/>- Light sources (point & directional)<br/>- Cubemap<br/>- Normal mapping                                                                                                                                                      |
|  **Physics**  | - Shapes (line, plane, sphere, triangle, quad, AABB, OBB)<br/>- Shape/shape collision checks _(in progress)_<br/>- Ray/shape intersection checks _(in progress)_<br/>- Rigid body simulation _(in progress)_                                                                                                                                                                                                                                                                                    |
|   **Misc**    | - Custom ECS (Entity Component System) implementation<br/>- Dynamic bitset<br/>- Uniformized platform-dependant path strings<br/>- Directed graph structure<br/>- Meshes (OBJ import/export, FBX import, OFF import)<br/>- Images (PNG import/export, TGA import, HDR import _(in progress)_)<br/>- Windowing (window, keyboard/mouse inputs with custom callbacks)<br/>- Overlay using [ImGui](https://github.com/ocornut/imgui)<br/>- Multithreading, compiler, enum, string & type utilities |
