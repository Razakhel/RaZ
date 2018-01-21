<h1 align="center">
    RaZ
</h1>

<h4 align="center">Modern &amp; multiplatform 3D engine in C++14</h4>

---

## Summary

**RaZ** comes from, as you may have noticed, my username **Raz**akhel. This was also meant as a pun, since in french _RàZ_ is the acronym for "Remise à zéro", which basically means "reset". I didn't have any particular intent behind this double meaning. Except maybe hoping that using RaZ might reset your mind because of its awesomeness? Oh well.

Written in modern C++ (14), it is primarily developed for learning purposes. It is intended to be as optimized as possible, while trying to stay as modular as it can (which currently isn't quite the case).

This library is available for Linux and Windows. It is also supposed to run on macOS, but since I don't own a Mac I can't say for sure.

If you want to get started, [head to the wiki](https://github.com/Razakhel/RaZ/wiki)!

## Features

| Objectives | Implemented |
| :--------: | :---------: |
| Loading a mesh (OBJ/FBX/...) | **Yes** (OBJ) |
| Loading textures (JPEG/PNG/TGA/BMP/BPG) | **Yes** (PNG) |
| Base OpenGL classes | **Yes** |
| Base scene classes (model, mesh, light, ...) | **Yes** |
| Vector: operations, dot, cross, normalization, length/magnitude calculation | **Yes** |
| Matrix: operations, transposition, determinant computation, inversion | **Yes** |
| Camera &amp; transformations | **Yes** (rotation with quaternions) |
| Lighting (light types &amp; algorithms) | **Yes** (Lambert &amp; Blinn-Phong available) |
| Materials &amp; textures | **Yes** |
| Handling submeshes while importing | **Yes** |
| Deferred rendering | **Yes** (modularization in progress) |
| Screen-Space Ambiant Occlusion (SSAO) | _In progress_ |
| Screen-Space Reflections (SSR) | _In progress_ |

## In the future...

To be honest, keeping it as a "simple" 3D engine was what I expected, but I wouldn't say no to working on another features aside from the rendering. Thus, it is not excluded _at all_ to evolve into a game engine, when I'll have more time.
