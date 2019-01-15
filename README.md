<h1 align="center">
    RaZ
</h1>

<h4 align="center">Modern &amp; multiplatform 3D engine in C++14</h4>

---

| <img alt="Linux build" src="https://www.screenconnect.com/Images/LogoLinux.png" align="center" height="30" width="30" /><br />Linux | <img alt="Windows build" src="https://upload.wikimedia.org/wikipedia/commons/thumb/7/76/Windows_logo_-_2012_%28dark_blue%2C_lines_thinner%29.svg/414px-Windows_logo_-_2012_%28dark_blue%2C_lines_thinner%29.svg.png" align="center" height="30" width="30" /><br />Windows | <img alt="Windows build" src="https://upload.wikimedia.org/wikipedia/commons/thumb/f/fa/Apple_logo_black.svg/245px-Apple_logo_black.svg.png" align="center" height="30" width="30" /><br />macOS |
| :--------: | :--------: | :--------: |
| [![Linux build status](https://travis-ci.com/Razakhel/RaZ.svg?branch=master)](https://travis-ci.com/Razakhel/RaZ) | No CI yet | No CI yet |

## Summary

**RaZ** comes from, as you may have noticed, my username **Raz**akhel. This was also meant as a pun, since in french _RàZ_ is the acronym for "Remise à zéro", which basically means "reset". I didn't have any particular intent behind this double meaning. Except maybe hoping that using RaZ might reset your mind because of its awesomeness? Oh well.

Written in modern C++ (14), ECS driven, it is primarily developed for learning purposes. It is intended to be as optimized as possible, while trying to stay as modular as it can.

This library is available for Linux and Windows. It is also supposed to run on macOS, but since I don't own a Mac I can't say for sure.

If you want to get started, [head to the wiki](https://github.com/Razakhel/RaZ/wiki)!

### Gallery

| Crytek Sponza | Hylian shield (PBR) |
| :--------: | :---------: |
| [![Crytek Sponza](https://i.imgur.com/Tr1nnjV.jpg)](https://i.imgur.com/Tr1nnjV.jpg) | [![Hylian shield](https://i.imgur.com/UZ90KKJ.jpg)](https://i.imgur.com/UZ90KKJ.jpg) |

More examples are available [here](https://github.com/Razakhel/RaZ/wiki/Examples).

## Features

| Objectives | Implemented |
| :--------: | :---------: |
| Mesh loading (OBJ/FBX/OFF...) | **Yes** (OBJ/FBX/OFF) |
| Texture loading (JPEG/PNG/TGA/BMP/BPG) | **Yes** (PNG/TGA) |
| Vector: operations, dot, cross, normalization, length/magnitude calculation | **Yes** |
| Matrix: operations, transposition, determinant computation, inversion | **Yes** |
| Camera &amp; transformations | **Yes** (rotation with quaternions) |
| Lighting | **Yes** (point/directional lights) |
| Materials &amp; textures | **Yes** (multi-textures functional) |
| Deferred rendering | **Yes** (modularization in progress) |
| Screen-Space Ambient Occlusion (SSAO) | _On standby_ |
| Screen-Space Reflections (SSR) | _On standby_ |
| Normal mapping | **Yes** |
| PBR rendering | **Yes** (Cook-Torrance model) |
| Image Based Lighting (IBL) | No |
| Shadow mapping | No |
| Bloom | No |

## 

## In the future...

To be honest, keeping it as a "simple" 3D engine was what I expected, but I wouldn't say no to working on another features aside from the rendering. Thus, it is not excluded _at all_ to evolve into a game engine, when I'll have more time.
