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
            <td align="center"><a alt="Linux build status" href="https://travis-ci.com/Razakhel/RaZ"><img src="http://badges.herokuapp.com/travis/razakhel/raz?branch=master&label=build&env=BADGE=linux" /></a></td>
            <td align="center"><a alt="Windows build status" href="https://travis-ci.com/Razakhel/RaZ"><img src="http://badges.herokuapp.com/travis/razakhel/raz?branch=master&label=build&env=BADGE=windows" /></a></td>
            <td align="center"><a alt="macOS build status" href="https://travis-ci.com/Razakhel/RaZ"><img src="http://badges.herokuapp.com/travis/razakhel/raz?branch=master&label=build&env=BADGE=osx" /></a></td>
            <td align="center"><a alt="WASM build status" href="https://travis-ci.com/Razakhel/RaZ"><img src="http://badges.herokuapp.com/travis/razakhel/raz?branch=master&label=build&env=BADGE=wasm" /></a></td>
        </tr>
        <tr>
            <th align="center">Unit tests</th>
            <td align="center"><a alt="Linux tests build status" href="https://travis-ci.com/Razakhel/RaZ"><img src="http://badges.herokuapp.com/travis/razakhel/raz?branch=master&label=build&env=BADGE=linux_tests" /></a></td>
            <td align="center">No CI yet</td>
            <td align="center">No CI yet</td>
            <td align="center">N/A</td>
        </tr>
    </tbody>
</table>

| <img alt="Codacy" src="https://seeklogo.com/images/C/codacy-logo-1A40ABD314-seeklogo.com.png" align="center" height="30" width="30" /><br />Codacy | <img alt="Coveralls" src="https://avatars3.githubusercontent.com/ml/318?s=140&v=4" align="center" height="30" width="30" /><br />Coveralls |
| :------------------------------------------------------------------------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------------------------------------------------------------: |
| [![Codacy Badge](https://api.codacy.com/project/badge/Grade/2c8f744cfabc41ada9bd73f72ddd080f)](https://www.codacy.com/app/Razakhel/RaZ)            | [![Coverage Status](https://coveralls.io/repos/github/Razakhel/RaZ/badge.svg)](https://coveralls.io/github/Razakhel/RaZ)                   |

## Summary

**RaZ** comes from, as you may have noticed, my username **Raz**akhel. This was also meant as a pun, since in french _RàZ_ is the acronym for "Remise à zéro", which basically means "reset". I didn't have any particular intent behind this double meaning. Except maybe hoping that using RaZ might reset your mind because of its awesomeness? Oh well.

Written in modern C++ (17), ECS driven, it is primarily developed for learning purposes. It is intended to be as optimized as possible, while trying to stay as modular as it can.

The engine is available under Windows, Linux & macOS; it can also run in a web browser through [WebAssembly](https://webassembly.org/) (using [Emscripten](https://emscripten.org/)).

If you want to get started, [head to the wiki](https://github.com/Razakhel/RaZ/wiki). For the documentation, [look over here](http://razakhel.github.io/RaZ/doc/)!

### Gallery

| Crytek Sponza                                                                        | Hylian shield (PBR)                                                                  |
| :----------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------: |
| [![Crytek Sponza](https://i.imgur.com/Tr1nnjV.jpg)](https://i.imgur.com/Tr1nnjV.jpg) | [![Hylian shield](https://i.imgur.com/UZ90KKJ.jpg)](https://i.imgur.com/UZ90KKJ.jpg) |

More examples are available [here](https://github.com/Razakhel/RaZ/wiki/Examples).

## Features

| Module        | Features                                                                                                                                                                                                                                                                       |
| :-----------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Math**      | - Vectors<br/>- Matrices<br/>- Quaternions<br/>- Angles (degrees/radians)<br/>- Transformations (translation, rotation, scale)                                                                                                                                                 |
| **Rendering** | - OpenGL (3.3 or 4.6)<br/>- Vulkan _(in progress)_<br/>- Standard (Blinn-Phong) & PBR (Cook-Torrance) material models<br/>- Camera (perspective/orthographic)<br/>- Light sources (point & directional)<br/>- Cubemap<br/>- Normal mapping<br/>- Render passes _(in progress)_ |
| **Physics**   | - Shapes (line, plane, sphere, triangle, quad, AABB, OBB)<br/>- Shape/shape collision checks _(in progress)_<br/>- Ray/shape intersection checks _(in progress)_<br/>- Rigid body simulation _(in progress)_                                                                   |
| **Misc**      | - Meshes (OBJ import/export, FBX import, OFF import)<br/>- Images (PNG import/export, TGA import, HDR import _(in progress)_)<br/>- Windowing (window, overlay, keyboard/mouse inputs with custom callbacks)                                                                   |
