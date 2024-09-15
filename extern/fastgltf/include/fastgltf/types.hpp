/*
 * Copyright (C) 2022 - 2024 spnda
 * This file is part of fastgltf <https://github.com/spnda/fastgltf>.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <cassert>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#endif

// Utils header already includes some headers, which we'll try and avoid including twice.
#include <fastgltf/util.hpp>
#include <fastgltf/math.hpp>

#if defined(_GLIBCXX_USE_CXX11_ABI) && !_GLIBCXX_USE_CXX11_ABI
// polymorphic allocators are only supported with the 'new' GCC ABI.
// Older compilers (older than GCC 5.1) default to the old ABI and sometimes the old ABI is
// explicitly selected on even the newest compilers, which we want to support.
#define FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL 1
#endif

#ifndef FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
#define FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL 0
#endif

#if __has_include(<memory_resource>)
#define FASTGLTF_MISSING_MEMORY_RESOURCE 0
#else
#define FASTGLTF_MISSING_MEMORY_RESOURCE 1
#if defined(FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL)
#undef FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
#endif
#define FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL 1
#endif

#if !FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <memory_resource>
#endif
#endif

#if FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
#define FASTGLTF_STD_PMR_NS ::std
#define FASTGLTF_FG_PMR_NS ::fastgltf

#define FASTGLTF_CONSTRUCT_PMR_RESOURCE(type, memoryResource, ...) type(__VA_ARGS__)
#define FASTGLTF_IF_PMR(expr)
#else
#define FASTGLTF_STD_PMR_NS ::std::pmr
#define FASTGLTF_FG_PMR_NS ::fastgltf::pmr

#define FASTGLTF_CONSTRUCT_PMR_RESOURCE(type, memoryResource, ...) type(__VA_ARGS__, memoryResource)
#define FASTGLTF_IF_PMR(expr) expr
#endif

#if FASTGLTF_CPP_20
#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <span>
#endif
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5030) // attribute 'x' is not recognized
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#endif

#define FASTGLTF_QUOTE_Q(x) #x
#define FASTGLTF_QUOTE(x) FASTGLTF_QUOTE_Q(x)

// fastgltf version string. Use FASTGLTF_QUOTE to stringify.
#define FASTGLTF_VERSION 0.8.0

namespace fastgltf {
#if defined(FASTGLTF_USE_64BIT_FLOAT) && FASTGLTF_USE_64BIT_FLOAT
	using num = double;
#else
	using num = float;
#endif

	namespace math {
		FASTGLTF_EXPORT using nvec2 = math::vec<num, 2>;
		FASTGLTF_EXPORT using nvec3 = math::vec<num, 3>;
		FASTGLTF_EXPORT using nvec4 = math::vec<num, 4>;
	}

#pragma region Enums
    // clang-format off
    FASTGLTF_EXPORT enum class PrimitiveType : std::uint8_t {
        Points = 0,
        Lines = 1,
        LineLoop = 2,
        LineStrip = 3,
        Triangles = 4,
        TriangleStrip = 5,
        TriangleFan = 6,
    };

    /**
     * Represents the type of element in the buffer pointed to by the accessor.
     *
     * We encode these values with the number of components in their top 8 bits for fast
     * access & storage. Therefore, use the fastgltf::getNumComponents and fastgltf::getElementByteSize
     * functions to extract data from this enum.
     *
     * 0aaaaabbb, where a is the component num, and b is the type index.
     */
    FASTGLTF_EXPORT enum class AccessorType : std::uint8_t {
        Invalid = 0,
        Scalar  = ( 1 << 3) | 1,
        Vec2    = ( 2 << 3) | 2,
        Vec3    = ( 3 << 3) | 3,
        Vec4    = ( 4 << 3) | 4,
        Mat2    = ( 4 << 3) | 5,
        Mat3    = ( 9 << 3) | 6,
        Mat4    = (16 << 3) | 7,
    };

    /**
     * Represents the various types of components an accessor could point at. This describes the
     * format each component of the structure, which in return is described by fastgltf::AccessorType, is in.
     *
     * As the constants used to identify component type in glTF fit within 13-bits, we store them in the lower 13 bits.
     * The remaining three bits are then used to store the byte width of the type, minus 1, as 8 is not representable
     * with just three bits.
     *
     * To get the byte or bit size of a component, use the fastgltf::getComponentByteSize or fastgltf::getComponentBitSize,
     * respectively. To get the OpenGL constant for the component type, use fastgltf::getGLComponentType.
     *
     * aaabbbbbbbbbbbbb, where a is the byte size, and b the OpenGL type enumeration.
     */
    FASTGLTF_EXPORT enum class ComponentType : std::uint16_t {
        Invalid         = 0,
        Byte            = (0 << 13) | 5120,
        UnsignedByte    = (0 << 13) | 5121,
        Short           = (1 << 13) | 5122,
        UnsignedShort   = (1 << 13) | 5123,
        /**
         * Signed integers are not officially allowed by the glTF spec, but are placed here for
         * the sake of completeness.
         */
        Int             = (3 << 13) | 5124,
        UnsignedInt     = (3 << 13) | 5125,
        Float           = (3 << 13) | 5126,
        /**
         * Doubles are only allowed through the KHR_accessor_float64 extension, but can also
         * be enabled by specifying the AllowDouble option.
         */
        Double          = (7 << 13) | 5130,
    };

    FASTGLTF_EXPORT enum class Filter : std::uint16_t {
        Nearest = 9728,
        Linear = 9729,
        NearestMipMapNearest = 9984,
        LinearMipMapNearest = 9985,
        NearestMipMapLinear = 9986,
        LinearMipMapLinear = 9987,
    };

    FASTGLTF_EXPORT enum class Wrap : std::uint16_t {
        ClampToEdge = 33071,
        MirroredRepeat = 33648,
        Repeat = 10497,
    };

    /**
     * Represents the intended OpenGL GPU buffer type to use with this buffer view.
     */
    FASTGLTF_EXPORT enum class BufferTarget : std::uint16_t {
        ArrayBuffer = 34962,
        ElementArrayBuffer = 34963,
    };

    FASTGLTF_EXPORT enum class MimeType : std::uint8_t {
        None = 0,
        JPEG = 1,
        PNG = 2,
        KTX2 = 3,
        DDS = 4,
        GltfBuffer = 5,
        OctetStream = 6,
    };

    FASTGLTF_EXPORT enum class AnimationInterpolation : std::uint8_t {
        /**
         * The animated values are linearly interpolated between keyframes. When targeting a
         * rotation, spherical linear interpolation (slerp) SHOULD be used to interpolate quaternions.
         */
        Linear = 0,
        /**
         * The animated values remain constant to the output of the first keyframe, until the next
         * keyframe.
         */
        Step = 1,
        /**
         * The animation’s interpolation is computed using a cubic spline with specified tangents.
         * The number of output elements MUST equal three times the number of input elements. For
         * each input element, the output stores three elements, an in-tangent, a spline vertex,
         * and an out-tangent.
         */
        CubicSpline = 2,
    };

    FASTGLTF_EXPORT enum class AnimationPath : std::uint8_t {
        /**
         * The values are the translation along the X, Y, and Z axes.
         */
        Translation = 1,
        /**
         * The values are a quaternion in the order x, y, z, w where w is the scalar.
         */
        Rotation = 2,
        /**
         * The values are scaling factors along the X, Y, and Z axes.
         */
        Scale = 3,
        Weights = 4,
    };

    FASTGLTF_EXPORT enum class AlphaMode : std::uint8_t {
        Opaque,
        Mask,
        Blend,
    };

    FASTGLTF_EXPORT enum class MeshoptCompressionMode : std::uint8_t {
        Attributes,
        Triangles,
        Indices,
    };

    FASTGLTF_EXPORT enum class MeshoptCompressionFilter : std::uint8_t {
        None = 0,
        Octahedral,
        Quaternion,
        Exponential,
    };

    FASTGLTF_EXPORT enum class LightType : std::uint8_t {
        Directional,
        Spot,
        Point,
    };

    FASTGLTF_EXPORT enum class Category : std::uint32_t {
        None        = 0,
        Buffers     = 1 <<  0,
        BufferViews = 1 <<  1,
        Accessors   = 1 <<  2,
        Images      = 1 <<  3,
        Samplers    = 1 <<  4,
        Textures    = 1 <<  5,
        Animations  = 1 <<  6,
        Cameras     = 1 <<  7,
        Materials   = 1 <<  8,
        Meshes      = 1 <<  9,
        Skins       = 1 << 10,
        Nodes       = 1 << 11,
        Scenes      = 1 << 12,
        Asset       = 1 << 13,

        All = ~(~0u << 14),
        // Includes everything needed for rendering but animations
        OnlyRenderable = All & ~(Animations) & ~(Skins),
        OnlyAnimations = Animations | Accessors | BufferViews | Buffers,
    };

    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Category, Category, |)
    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Category, Category, &)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Category, Category, |)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Category, Category, &)
    FASTGLTF_UNARY_OP_TEMPLATE_MACRO(Category, ~)
    // clang-format on
#pragma endregion

#pragma region ConversionFunctions
    /**
     * Gets the number of components for each element for the given accessor type. For example, with
     * a Vec3 accessor type this will return 3, as a Vec3 contains 3 components.
     */
    FASTGLTF_EXPORT constexpr auto getNumComponents(AccessorType type) noexcept {
    	static_assert(std::is_same_v<std::underlying_type_t<AccessorType>, std::uint8_t>);
        return static_cast<std::size_t>(to_underlying(type) >> 3U);
    }

    /**
     * Returns the number of rows in the given accessor type.
     */
    FASTGLTF_EXPORT constexpr auto getElementRowCount(AccessorType type) noexcept {
        switch (type) {
            case AccessorType::Mat2:
            case AccessorType::Vec2:
                return std::size_t(2U);
            case AccessorType::Mat3:
            case AccessorType::Vec3:
                return std::size_t(3U);
            case AccessorType::Mat4:
            case AccessorType::Vec4:
                return std::size_t(4U);
            default:
                return std::size_t(1U);
        }
    }

    FASTGLTF_EXPORT constexpr bool isMatrix(AccessorType type) noexcept {
        return type == AccessorType::Mat2 || type == AccessorType::Mat3 || type == AccessorType::Mat4;
    }

	FASTGLTF_EXPORT constexpr auto getComponentByteSize(ComponentType componentType) noexcept {
		static_assert(std::is_same_v<std::underlying_type_t<ComponentType>, std::uint16_t>);
		if (componentType == ComponentType::Invalid)
			return std::size_t(0U);
		return static_cast<std::size_t>(to_underlying(componentType) >> 13U) + 1;
	}

	FASTGLTF_EXPORT constexpr auto getComponentBitSize(ComponentType componentType) noexcept {
		return getComponentByteSize(componentType) * 8U;
	}

    FASTGLTF_EXPORT constexpr auto getElementByteSize(AccessorType type, ComponentType componentType) noexcept {
        const auto componentSize = getComponentByteSize(componentType);
        auto numComponents = getNumComponents(type);
        const auto rowCount = getElementRowCount(type);
        if (isMatrix(type) && (rowCount * componentSize) % 4 != 0) {
            // Matrices need extra padding per-column which affects their size.
            numComponents += rowCount * (4 - (rowCount % 4));
        }
        return numComponents * componentSize;
    }

    /**
     * Returns the OpenGL component type enumeration for the given component type.
     * All OpenGL enumerations use GLenum, which is a 32-bit wide integer, which is why
     * this function returns a std::uint32_t.
     *
     * For example, getGLComponentType(ComponentType::Float) will return GL_FLOAT (0x1406).
     */
    FASTGLTF_EXPORT constexpr auto getGLComponentType(ComponentType type) noexcept {
    	static_assert(std::is_same_v<std::underlying_type_t<ComponentType>, std::uint16_t>);
        return static_cast<std::uint32_t>(to_underlying(type) & 0x1FFF); // 2^13 - 1 in hex, to mask the lower 13 bits.
    }

    /**
     * Don't use this, use getComponentType instead.
     * This order matters as we assume that their glTF constant is ascending to index it.
     */
    static constexpr std::array<ComponentType, 11> components = {
        ComponentType::Byte,
        ComponentType::UnsignedByte,
        ComponentType::Short,
        ComponentType::UnsignedShort,
        ComponentType::Int,
        ComponentType::UnsignedInt,
        ComponentType::Float,
        ComponentType::Invalid,
        ComponentType::Invalid,
        ComponentType::Invalid,
        ComponentType::Double,
    };

    constexpr auto getComponentType(std::underlying_type_t<ComponentType> componentType) noexcept {
        const auto index = static_cast<std::size_t>(componentType - getGLComponentType(ComponentType::Byte));
        if (index >= components.size()) {
            return ComponentType::Invalid;
		}
        return components[index];
    }

    // This order matters as we assume that their glTF constant is ascending to index it.
    static constexpr std::array<AccessorType, 7> accessorTypes = {
        AccessorType::Scalar,
        AccessorType::Vec2,
        AccessorType::Vec3,
        AccessorType::Vec4,
        AccessorType::Mat2,
        AccessorType::Mat3,
        AccessorType::Mat4,
    };

    /**
     * Gets the AccessorType by its string representation found in glTF files.
     */
    constexpr auto getAccessorType(std::string_view accessorTypeName) noexcept {
        assert(!accessorTypeName.empty());
        switch (accessorTypeName[0]) {
            case 'S': return AccessorType::Scalar;
            case 'V': {
                const auto componentCount = static_cast<std::size_t>(accessorTypeName[3] - '2');
                if (componentCount + 1 >= accessorTypes.size()) {
                    return AccessorType::Invalid;
                }
                return accessorTypes[componentCount + 1];
            }
            case 'M': {
                const auto componentCount = static_cast<std::size_t>(accessorTypeName[3] - '2');
                if (componentCount + 4 >= accessorTypes.size()) {
                    return AccessorType::Invalid;
                }
                return accessorTypes[componentCount + 4];
            }
        	default:
        		return AccessorType::Invalid;
        }
    }

	static constexpr std::array<std::string_view, 7> accessorTypeNames = {
		"SCALAR",
		"VEC2",
		"VEC3",
		"VEC4",
		"MAT2",
		"MAT3",
		"MAT4"
	};

	constexpr std::string_view getAccessorTypeName(AccessorType type) noexcept {
    	static_assert(std::is_same_v<std::underlying_type_t<AccessorType>, std::uint8_t>);
		if (type == AccessorType::Invalid)
			return "";
		auto idx = to_underlying(type) & 0x7;
		return accessorTypeNames[idx - 1];
	}

	constexpr std::string_view mimeTypeJpeg = "image/jpeg";
	constexpr std::string_view mimeTypePng = "image/png";
	constexpr std::string_view mimeTypeKtx = "image/ktx2";
	constexpr std::string_view mimeTypeDds = "image/vnd-ms.dds";
	constexpr std::string_view mimeTypeGltfBuffer = "application/gltf-buffer";
	constexpr std::string_view mimeTypeOctetStream = "application/octet-stream";

	constexpr std::string_view getMimeTypeString(MimeType mimeType) noexcept {
		switch (mimeType) {
			case MimeType::JPEG:
				return mimeTypeJpeg;
			case MimeType::PNG:
				return mimeTypePng;
			case MimeType::KTX2:
				return mimeTypeKtx;
			case MimeType::DDS:
				return mimeTypeDds;
			case MimeType::GltfBuffer:
				return mimeTypeGltfBuffer;
			case MimeType::OctetStream:
				return mimeTypeOctetStream;
			default:
				return "";
		}
	}
#pragma endregion

#pragma region Containers
    /**
     * A static vector which cannot be resized freely. When constructed, the backing array is allocated once.
     */
    FASTGLTF_EXPORT template <typename T>
    class StaticVector final {
	public:
		using value_type = T;
		using size_type = std::size_t;
        using array_t = value_type[];

	private:
		size_type _size = 0;
        std::unique_ptr<array_t> _array;

		void copy(const T* first, size_type count, T* result) {
            if (count > 0) {
				if constexpr (std::is_trivially_copyable_v<T>) {
					std::memcpy(result, first, count * sizeof(T));
				} else {
					*result++ = *first;
					for (size_type i = 1; i < count; ++i) {
						*result++ = *++first;
					}
				}
            }
        }

    public:
		using reference = value_type&;
		using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = pointer;
        using const_iterator = const_pointer;

        explicit StaticVector(std::size_t size) : _size(size), _array(std::move(std::unique_ptr<array_t>(new std::remove_extent_t<array_t>[size]))) {}
		explicit StaticVector(std::size_t size, const T& initialValue) : _size(size), _array(std::move(std::unique_ptr<array_t>(new std::remove_extent_t<array_t>[size]))) {
			for (auto& value : *this) {
				value = initialValue;
			}
		}

        StaticVector(const StaticVector& other) {
            if (other.size() == 0) {
                _array.reset();
                _size = 0;
            } else {
                _array.reset(new std::remove_extent_t<array_t>[other.size()]);
                _size = other.size();
				copy(other.begin(), _size, begin());
            }
        }

        StaticVector(StaticVector&& other) noexcept {
            _array = std::move(other._array);
            _size = other.size();
        }

		StaticVector& operator=(StaticVector&& other) noexcept {
			_array = std::move(other._array);
			_size = other.size();
			return *this;
		}

		/**
		 * Copies the contents of the given vector into a new StaticVector.
		 */
		static StaticVector<T> fromVector(std::vector<T> vector) {
			StaticVector<T> staticVector(vector.size());
			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(staticVector.data(), vector.data(), vector.size());
			} else {
				for (auto it = vector.begin(); it != vector.end(); ++it) {
					staticVector[std::distance(vector.begin(), it)] = *it;
				}
			}
			return staticVector;
		}

        [[nodiscard]] pointer data() noexcept {
            return &_array.get()[0];
        }

        [[nodiscard]] const_pointer data() const noexcept {
            return &_array.get()[0];
        }

        [[nodiscard]] size_type size() const noexcept {
            return _size;
        }

        [[nodiscard]] size_type size_bytes() const noexcept {
            return _size * sizeof(value_type);
        }

        [[nodiscard]] bool empty() const noexcept {
            return _size == 0;
        }

        [[nodiscard]] iterator begin() noexcept { return data(); }
        [[nodiscard]] const_iterator begin() const noexcept { return data(); }
        [[nodiscard]] const_iterator cbegin() const noexcept { return data(); }
        [[nodiscard]] iterator end() noexcept { return begin() + size(); }
        [[nodiscard]] const_iterator end() const noexcept { return begin() + size(); }
        [[nodiscard]] const_iterator cend() const noexcept { return begin() + size(); }

		[[nodiscard]] T& operator[](std::size_t idx) {
            assert(idx < size());
            return begin()[idx];
        }
        [[nodiscard]] const T& operator[](std::size_t idx) const {
            assert(idx < size());
            return begin()[idx];
        }

        bool operator==(const StaticVector<value_type>& other) const {
            if (other.size() != size()) return false;
            return std::memcmp(data(), other.data(), size_bytes()) == 0;
        }

        // This is mostly just here for compatibility and the tests
        bool operator==(const std::vector<value_type>& other) const {
            if (other.size() != size()) return false;
            return std::memcmp(data(), other.data(), size_bytes()) == 0;
        }
    };

	/*
	 * The amount of items that the SmallVector can initially store in the storage
	 * allocated within the object itself.
	 */
	static constexpr auto initialSmallVectorStorage = 8;

    /**
     * A custom vector class for fastgltf, which can store up to N objects within itself.
     * This is useful for cases where the vector is expected to only ever hold a tiny amount of small objects,
     * such as a node's children.
     * SmallVector is also mostly conformant to C++17's std::vector, and can therefore be used as a drop-in replacement.
     * @note It is also available with polymorphic allocators in the fastgltf::pmr namespace.
     */
    FASTGLTF_EXPORT template <typename T, std::size_t N = initialSmallVectorStorage, typename Allocator = std::allocator<T>>
    class SmallVector final {
        static_assert(N != 0, "Cannot create a SmallVector with 0 initial capacity");

        alignas(T) std::array<std::byte, N * sizeof(T)> storage = {};

		Allocator allocator;

        T* _data;
        std::size_t _size = 0, _capacity = N;

        void copy(const T* first, std::size_t count, T* result) {
            if (count > 0) {
				if constexpr (std::is_trivially_copyable_v<T>) {
					std::memcpy(result, first, count * sizeof(T));
				} else {
					*result++ = *first;
					for (std::size_t i = 1; i < count; ++i) {
						*result++ = *++first;
					}
				}
            }
        }

    public:
		using iterator = T*;
		using const_iterator = const T*;

        SmallVector() : _data(reinterpret_cast<T*>(storage.data())) {}

		explicit SmallVector(const Allocator& allocator) noexcept : allocator(allocator), _data(reinterpret_cast<T*>(storage.data())) {}

        explicit SmallVector(std::size_t size, const Allocator& allocator = Allocator()) : allocator(allocator), _data(reinterpret_cast<T*>(storage.data())) {
            resize(size);
        }

        SmallVector(std::size_t size, const T& value, const Allocator& allocator = Allocator()) : allocator(allocator), _data(reinterpret_cast<T*>(storage.data())) {
            assign(size, value);
        }

        SmallVector(std::initializer_list<T> init, const Allocator& allocator = Allocator()) : allocator(allocator), _data(reinterpret_cast<T*>(storage.data())) {
            assign(init);
        }

        SmallVector(const SmallVector& other) noexcept : _data(reinterpret_cast<T*>(storage.data())) {
            resize(other.size());
            copy(other.begin(), other.size(), begin());
        }

        SmallVector(SmallVector&& other) noexcept : _data(reinterpret_cast<T*>(storage.data())) {
            if (other.isUsingStack()) {
                if (!other.empty()) {
                    resize(other.size());
                    copy(other.begin(), other.size(), begin());
                    other._data = reinterpret_cast<T*>(other.storage.data()); // Reset pointer
                    _size = std::exchange(other._size, 0);
                    _capacity = std::exchange(other._capacity, N);
                }
            } else {
                _data = std::exchange(other._data, nullptr);
                _size = std::exchange(other._size, 0);
                _capacity = std::exchange(other._capacity, N);
            }
        }

        SmallVector& operator=(const SmallVector& other) {
            if (std::addressof(other) != this) {
                if (!isUsingStack() && _data) {
	                std::destroy(begin(), end());
					allocator.deallocate(_data, _capacity);
                    _data = reinterpret_cast<T*>(storage.data());
                    _size = _capacity = 0;
                }

                resize(other.size());
                copy(other.begin(), other.size(), begin());
            }
            return *this;
        }

        SmallVector& operator=(SmallVector&& other) noexcept {
            if (std::addressof(other) != this) {
                if (other.isUsingStack()) {
                    if (!other.empty()) {
                        resize(other.size());
                        copy(other.begin(), other.size(), begin());
                        other._data = reinterpret_cast<T*>(other.storage.data()); // Reset pointer
                        _size = std::exchange(other._size, 0);
                        _capacity = std::exchange(other._capacity, N);
                    }
                } else {
                    _data = std::exchange(other._data, nullptr);
                    _size = std::exchange(other._size, 0);
                    _capacity = std::exchange(other._capacity, N);
                }
            }
            return *this;
        }

        ~SmallVector() {
			// As we use an array of std::byte for the stack storage, we have to destruct those manually too.
			std::destroy(begin(), end());

            if (!isUsingStack() && _data) {
                // Not using the stack, we'll have to free.
	            allocator.deallocate(_data, _capacity);
            }
        }

	    [[nodiscard]] iterator begin() noexcept { return _data; }
        [[nodiscard]] const_iterator begin() const noexcept { return _data; }
	    [[nodiscard]] const_iterator cbegin() const noexcept { return _data; }
	    [[nodiscard]] iterator end() noexcept { return begin() + size(); }
	    [[nodiscard]] const_iterator end() const noexcept { return begin() + size(); }
	    [[nodiscard]] const_iterator cend() const noexcept { return begin() + size(); }

        [[nodiscard]] std::reverse_iterator<T*> rbegin() { return end(); }
        [[nodiscard]] std::reverse_iterator<const T*> rbegin() const { return end(); }
        [[nodiscard]] std::reverse_iterator<const T*> crbegin() const { return end(); }
        [[nodiscard]] std::reverse_iterator<T*> rend() { return begin(); }
        [[nodiscard]] std::reverse_iterator<const T*> rend() const { return begin(); }
        [[nodiscard]] std::reverse_iterator<const T*> crend() const { return begin(); }

        [[nodiscard]] T* data() noexcept { return _data; }
        [[nodiscard]] const T* data() const noexcept { return _data; }
        [[nodiscard]] std::size_t size() const noexcept { return _size; }
        [[nodiscard]] std::size_t size_in_bytes() const noexcept { return _size * sizeof(T); }
        [[nodiscard]] std::size_t capacity() const noexcept { return _capacity; }

        [[nodiscard]] bool empty() const noexcept { return _size == 0; }
        [[nodiscard]] bool isUsingStack() const noexcept { return data() == reinterpret_cast<const T*>(storage.data()); }

        void reserve(std::size_t newCapacity) {
	        static_assert(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>, "T needs to be copy constructible.");

            // We don't want to reduce capacity with reserve, only with shrink_to_fit.
            if (newCapacity <= capacity()) {
                return;
            }

            // If the new capacity is lower than what we can hold on the stack, we ignore this request.
            if (newCapacity <= N && isUsingStack()) {
                _capacity = newCapacity;
                if (_size > _capacity) {
                    _size = _capacity;
                }
                return;
            }

            // We use geometric growth, similarly to std::vector.
            newCapacity = std::size_t(1) << (std::numeric_limits<decltype(newCapacity)>::digits - clz(newCapacity));

			T* alloc = allocator.allocate(newCapacity);

			// Copy/Move the old data into the new memory
			for (std::size_t i = 0; i < size(); ++i) {
				auto& x = (*this)[i];
				if constexpr (std::is_nothrow_move_constructible_v<T>) {
					new(alloc + i) T(std::move(x));
				} else if constexpr (std::is_copy_constructible_v<T>) {
					new(alloc + i) T(x);
				} else {
					new(alloc + i) T(std::move(x));
				}
			}

			// Destroy all objects in the old allocation
			std::destroy(begin(), end());

            if (!isUsingStack() && _data && size() != 0) {
				allocator.deallocate(_data, _capacity);
            }

            _data = alloc;
            _capacity = newCapacity;
        }

        void resize(std::size_t newSize) {
			static_assert(std::is_constructible_v<T>, "T has to be constructible");
			if (newSize == size()) {
				return;
			}

			if (newSize < size()) {
				// Just destroy the "overflowing" elements.
				std::destroy(begin() + newSize, end());
			} else {
                // Reserve enough capacity and copy the new value over.
                auto oldSize = _size;
                reserve(newSize);
                for (auto it = begin() + oldSize; it != begin() + newSize; ++it) {
                    new (it) T();
                }
            }

            _size = newSize;
        }

		void resize(std::size_t newSize, const T& value) {
			static_assert(std::is_copy_constructible_v<T>, "T needs to be copy constructible.");
			if (newSize == size()) {
				return;
			}

			if (newSize < size()) {
				// Just destroy the "overflowing" elements.
				std::destroy(begin() + newSize, end());
			} else {
				// Reserve enough capacity and copy the new value over.
				auto oldSize = _size;
				reserve(newSize);
				for (auto it = begin() + oldSize; it != begin() + newSize; ++it) {
					if (it == nullptr)
						break;

					if constexpr (std::is_move_constructible_v<T>) {
						new (it) T(std::move(value));
					} else if constexpr (std::is_trivially_copyable_v<T>) {
						std::memcpy(it, std::addressof(value), sizeof(T));
					} else {
						new (it) T(value);
					}
				}
			}

			_size = newSize;
		}

		void shrink_to_fit() {
			// Only have to shrink if there's any unused capacity.
			if (capacity() == size() || size() == 0) {
				return;
			}

			// If we can use the object's memory again, we'll copy everything over.
			if (size() <= N) {
				copy(begin(), size(), reinterpret_cast<T*>(storage.data()));
				_data = reinterpret_cast<T*>(storage.data());
			} else {
				// We have to use heap allocated memory.
				auto* alloc = allocator.allocate(size());
				for (std::size_t i = 0; i < size(); ++i) {
					new(alloc + i) T((*this)[i]);
				}

				if (_data && !isUsingStack()) {
					std::destroy(begin(), end());
					allocator.deallocate(_data, _capacity);
				}

				_data = alloc;
			}

			_capacity = _size;
		}

		void assign(std::size_t count, const T& value) {
			clear();
			resize(count, value);
		}

		void assign(std::initializer_list<T> init) {
			static_assert(std::is_trivially_copyable_v<T> || std::is_copy_constructible_v<T>, "T needs to be trivially copyable or be copy constructible");
			clear();
			reserve(init.size());
			_size = init.size();

			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(begin(), init.begin(), init.size() * sizeof(T));
			} else if constexpr (std::is_copy_constructible_v<T>) {
				for (auto it = init.begin(); it != init.end(); ++it) {
					new (_data + std::distance(init.begin(), it)) T(*it);
				}
			}
		}

		void clear() noexcept {
			std::destroy(begin(), end());

			if (!isUsingStack() && size() != 0) {
				allocator.deallocate(_data, _capacity);
				_data = reinterpret_cast<T*>(storage.data());
			}

			_size = 0;
		}

        template <typename... Args>
        decltype(auto) emplace_back(Args&&... args) {
            // We reserve enough capacity for the new element, and then just increment the size.
			reserve(_size + 1);
			++_size;
            new (std::addressof(back())) T(std::forward<Args>(args)...);
            return (back());
        }

        [[nodiscard]] T& at(std::size_t idx) {
            if (idx >= size()) {
                raise<std::out_of_range>("Index is out of range for SmallVector");
            }
            return begin()[idx];
        }
        [[nodiscard]] const T& at(std::size_t idx) const {
            if (idx >= size()) {
                raise<std::out_of_range>("Index is out of range for SmallVector");
            }
            return begin()[idx];
        }

        [[nodiscard]] T& operator[](std::size_t idx) {
            assert(idx < size());
            return begin()[idx];
        }
        [[nodiscard]] const T& operator[](std::size_t idx) const {
            assert(idx < size());
            return begin()[idx];
        }

        [[nodiscard]] T& front() {
            assert(!empty());
            return begin()[0];
        }
        [[nodiscard]] const T& front() const {
            assert(!empty());
            return begin()[0];
        }

        [[nodiscard]] T& back() {
            assert(!empty());
            return end()[-1];
        }
        [[nodiscard]] const T& back() const {
            assert(!empty());
            return end()[-1];
        }
    };

#if !FASTGLTF_MISSING_MEMORY_RESOURCE
	namespace pmr {
		FASTGLTF_EXPORT template<typename T, std::size_t N>
		using SmallVector = SmallVector<T, N, std::pmr::polymorphic_allocator<T>>;
	} // namespace pmr
#endif

#ifndef FASTGLTF_USE_CUSTOM_SMALLVECTOR
#define FASTGLTF_USE_CUSTOM_SMALLVECTOR 0
#endif

#if FASTGLTF_USE_CUSTOM_SMALLVECTOR
	FASTGLTF_EXPORT template <typename T, std::size_t N = initialSmallVectorStorage>
	using MaybeSmallVector = SmallVector<T, N>;
#else
	FASTGLTF_EXPORT template <typename T, std::size_t N = 0>
	using MaybeSmallVector = std::vector<T>;
#endif

#if !FASTGLTF_MISSING_MEMORY_RESOURCE
	namespace pmr {
#if FASTGLTF_USE_CUSTOM_SMALLVECTOR
		FASTGLTF_EXPORT template <typename T, std::size_t N = initialSmallVectorStorage>
		using MaybeSmallVector = pmr::SmallVector<T, N>;
#else
		FASTGLTF_EXPORT template <typename T, std::size_t N = 0>
		using MaybeSmallVector = std::pmr::vector<T>;
#endif
	} // namespace pmr
#endif

	FASTGLTF_EXPORT template<typename, typename = void>
	struct OptionalFlagValue {
		static constexpr std::nullopt_t missing_value = std::nullopt;
	};

	template<>
	struct OptionalFlagValue<std::size_t> {
		static constexpr auto missing_value = std::numeric_limits<std::size_t>::max();
	};

	template<>
	struct OptionalFlagValue<float, std::enable_if_t<std::numeric_limits<float>::is_iec559>> {
		// This float is a quiet NaN with a specific bit pattern to be able to differentiate
		// between this flag value and any result from FP operations.
		static constexpr auto missing_value = static_cast<float>(0x7fedb6db);
	};

	template<>
	struct OptionalFlagValue<double, std::enable_if_t<std::numeric_limits<double>::is_iec559>> {
		static constexpr auto missing_value = static_cast<double>(0x7ffdb6db6db6db6d);
	};

	template<>
	struct OptionalFlagValue<Filter> {
		static constexpr auto missing_value = static_cast<Filter>(std::numeric_limits<std::underlying_type_t<Filter>>::max());
	};

	template<>
	struct OptionalFlagValue<BufferTarget> {
		static constexpr auto missing_value = static_cast<BufferTarget>(std::numeric_limits<std::underlying_type_t<BufferTarget>>::max());
	};

	/**
	 * A custom optional class for fastgltf,
	 * which uses so-called "flag values" which are specific values of T that will never be present as an actual value.
	 * We can therefore use those values as flags for whether there is an actual value stored,
	 * instead of the additional bool used by std::optional.
	 *
	 * These flag values are obtained from the specializations of OptionalFlagValue.
	 * If no specialization for T of OptionalFlagValue is provided, a static assert will be triggered.
	 * In those cases, use std::optional or fastgltf::Optional instead.
	 */
	FASTGLTF_EXPORT template<typename T>
	class OptionalWithFlagValue final {
		static_assert(!std::is_same_v<std::nullopt_t, std::remove_const_t<decltype(OptionalFlagValue<T>::missing_value)>>,
			"OptionalWithFlagValue can only be used when there is an appropriate specialization of OptionalFlagValue<T>.");

		struct NonTrivialDummy {
			constexpr NonTrivialDummy() noexcept {}
		};

		union {
			NonTrivialDummy dummy;
			std::remove_const_t<T> _value;
		};

	public:
		OptionalWithFlagValue() noexcept {
			reset();
		}

		OptionalWithFlagValue(std::nullopt_t) noexcept {
			reset();
		}

		template <typename U = T, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
		OptionalWithFlagValue(const OptionalWithFlagValue<U>& other) {
			if (other.has_value()) {
				new (std::addressof(_value)) T(*other);
			} else {
				reset();
			}
		}

		template <typename U = T, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
		OptionalWithFlagValue(OptionalWithFlagValue<U>&& other) {
			if (other.has_value()) {
				new (std::addressof(_value)) T(std::move(*other));
			} else {
				reset();
			}
		}

		template<typename... Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
		explicit OptionalWithFlagValue(std::in_place_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
			: _value(std::forward<Args>(args)...) {}

		template <typename U = T, std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
		OptionalWithFlagValue(U&& _new) noexcept(std::is_nothrow_assignable_v<T&, U> && std::is_nothrow_constructible_v<T, U>) {
			new (std::addressof(_value)) T(std::forward<U>(_new));
		}

		~OptionalWithFlagValue() {
			reset();
		}

		OptionalWithFlagValue& operator=(std::nullopt_t) noexcept {
			reset();
			return *this;
		}

		template<typename U = T, std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
		OptionalWithFlagValue& operator=(U&& _new) noexcept(std::is_nothrow_assignable_v<T&, U> && std::is_nothrow_constructible_v<T, U>) {
			if (has_value()) {
				_value = std::forward<U>(_new);
			} else {
				new(std::addressof(_value)) T(std::forward<U>(_new));
			}
			return *this;
		}

		template <typename U, std::enable_if_t<std::conjunction_v<std::is_constructible<T, const U&>, std::is_assignable<T&, const U&>>, int> = 0>
		OptionalWithFlagValue& operator=(const OptionalWithFlagValue<U>& other) {
			if (other.has_value()) {
				if (has_value()) {
					_value = other._value;
				} else {
					new(std::addressof(_value)) T(other._value);
				}
			} else {
				reset();
			}
			return *this;
		}

		template <typename U, std::enable_if_t<std::conjunction_v<std::is_constructible<T, U>, std::is_assignable<T&, U>>, int> = 0>
		OptionalWithFlagValue& operator=(OptionalWithFlagValue<U>&& other) noexcept(std::is_nothrow_assignable_v<T&, T> && std::is_nothrow_constructible_v<T, T>) {
			if (other.has_value()) {
				if (has_value()) {
					_value = std::move(other._value);
				} else {
					new(std::addressof(_value)) T(other._value);
				}
			} else {
				reset();
			}
			return *this;
		}

		[[nodiscard]] bool has_value() const {
			return this->_value != OptionalFlagValue<T>::missing_value;
		}

		[[nodiscard]] T& value()& {
			if (!has_value()) {
                raise<std::bad_optional_access>();
			}
			return _value;
		}

		[[nodiscard]] const T& value() const& {
			if (!has_value()) {
				raise<std::bad_optional_access>();
			}
			return _value;
		}

		[[nodiscard]] T&& value()&& {
			if (!has_value()) {
				raise<std::bad_optional_access>();
			}
			return std::move(_value);
		}

		[[nodiscard]] const T&& value() const&& {
			if (!has_value()) {
				raise<std::bad_optional_access>();
			}
			return std::move(_value);
		}

		template<typename U>
		[[nodiscard]] T value_or(U&& default_value) const& {
			return has_value() ? **this : static_cast<T>(std::forward<U>(default_value));
		}

		template<typename U>
		[[nodiscard]] T value_or(U&& default_value)&& {
			return has_value() ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
		}

		template <typename F>
		[[nodiscard]] OptionalWithFlagValue<T> and_then(F&& func)& {
			if (!has_value())
				return std::nullopt;
			return func(value());
		}

		template <typename F>
		[[nodiscard]] OptionalWithFlagValue<T> and_then(F&& func)&& {
			if (!has_value())
				return std::nullopt;
			return func(std::move(value()));
		}

		template <typename F>
		[[nodiscard]] OptionalWithFlagValue<T> transform(F&& func)& {
			if (!has_value())
				return std::nullopt;
			return OptionalWithFlagValue<T>(func(value()));
		}

		template <typename F>
		[[nodiscard]] OptionalWithFlagValue<T> transform(F&& func)&& {
			if (!has_value())
				return std::nullopt;
			return OptionalWithFlagValue<T>(func(std::move(value())));
		}

		template <typename F>
		[[nodiscard]] T or_else(F&& func) const& {
			return *this ? *this : std::forward<F>(func)();
		}

		template <typename F>
		[[nodiscard]] T or_else(F&& func)&& {
			return *this ? std::move(*this) : std::forward<F>(func)();
		}

		void swap(OptionalWithFlagValue<T>& other) noexcept(std::is_nothrow_move_constructible_v<T> &&
		                                                    std::is_nothrow_swappable_v<T>) {
			static_assert(std::is_move_constructible_v<T>);
			if (has_value() && other.has_value()) {
				std::swap(_value, other._value);
			} else if (has_value() && !other.has_value()) {
				other._value = std::move(_value);
			} else if (!has_value() && other.has_value()) {
				_value = std::move(other._value);
			}
		}

		void reset() noexcept {
			this->_value = OptionalFlagValue<T>::missing_value;
		}

		template <typename... Args>
		T& emplace(Args&&... args) {
			new (std::addressof(_value)) T(std::forward<Args>(args)...);
			return _value;
		}

		template <typename U, typename... Args>
		T& emplace(std::initializer_list<U> list, Args&&... args) {
			static_assert(std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>);
			new (std::addressof(_value)) T(list, std::forward<Args>(args)...);
			return _value;
		}

		explicit operator bool() const noexcept {
			return has_value();
		}

		T* operator->() noexcept {
			return std::addressof(_value);
		}

		const T* operator->() const noexcept {
			return std::addressof(_value);
		}

		T& operator*()& noexcept {
			return _value;
		}

		const T& operator*() const& noexcept {
			return _value;
		}

		T&& operator*()&& noexcept {
			return std::move(_value);
		}

		const T&& operator*() const&& noexcept {
			return std::move(_value);
		}
	};

	FASTGLTF_EXPORT template <typename T, typename U>
	bool operator==(const OptionalWithFlagValue<T>& lhs, const OptionalWithFlagValue<U>& rhs) {
		return bool(lhs) == bool(rhs) && !bool(lhs) && *lhs == *rhs;
	}

	FASTGLTF_EXPORT template <typename T, typename U>
	bool operator!=(const OptionalWithFlagValue<T>& lhs, const OptionalWithFlagValue<U>& rhs) {
		return !(lhs == rhs);
	}

	FASTGLTF_EXPORT template <typename T, typename U>
	bool operator==(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && (*opt) == value;
	}

	FASTGLTF_EXPORT template <typename T, typename U>
	bool operator!=(const OptionalWithFlagValue<T>& opt, const U& value) {
		return !(opt == value);
	}

	FASTGLTF_EXPORT template <typename T, typename U>
	bool operator<(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && *opt < value;
	}

	FASTGLTF_EXPORT template <typename T, typename U>
	bool operator<=(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && *opt <= value;
	}

	FASTGLTF_EXPORT template <typename T, typename U>
	bool operator>(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && *opt > value;
	}

	FASTGLTF_EXPORT template <typename T, typename U>
	bool operator>=(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && *opt >= value;
	}

	/**
	 * A type alias which checks if there is a specialization of OptionalFlagValue for T and "switches"
	 * between fastgltf::OptionalWithFlagValue and std::optional.
	 */
	FASTGLTF_EXPORT template <typename T>
	using Optional = std::conditional_t<
		!std::is_same_v<std::nullopt_t, std::remove_const_t<decltype(OptionalFlagValue<T>::missing_value)>>,
		OptionalWithFlagValue<T>,
		std::optional<T>>;
#pragma endregion

#pragma region Structs
	FASTGLTF_EXPORT class URI;

	/**
	 * Custom URI class for fastgltf's needs. glTF 2.0 only allows two types of URIs:
	 *  (1) Data URIs as specified in RFC 2397.
	 *  (2) Relative paths as specified in RFC 3986.
	 *
	 * See https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#uris for details.
	 * However, the glTF spec allows more broader URIs in client implementations. Therefore,
	 * this supports all types of URIs as defined in RFC 3986.
	 *
	 * This class, unlike fastgltf::URI, only holds a std::string_view to the URI and therefore
	 * doesn't own the allocation.
	 */
	FASTGLTF_EXPORT class URIView {
		friend class URI;

		std::string_view view;

		std::string_view _scheme;
		std::string_view _path;

		std::string_view _userinfo;
		std::string_view _host;
		std::string_view _port;

		std::string_view _query;
		std::string_view _fragment;

		bool _valid = true;

		void parse();

		[[nodiscard]] auto data() const noexcept -> const char*;

	public:
		explicit URIView() noexcept;
		explicit URIView(std::string_view uri) noexcept;
		URIView(const URIView& other) noexcept;

		URIView& operator=(const URIView& other);
		URIView& operator=(std::string_view other);

		[[nodiscard]] auto string() const noexcept -> std::string_view;

		[[nodiscard]] auto scheme() const noexcept -> std::string_view;
		[[nodiscard]] auto userinfo() const noexcept -> std::string_view;
		[[nodiscard]] auto host() const noexcept -> std::string_view;
		[[nodiscard]] auto port() const noexcept -> std::string_view;
		[[nodiscard]] auto path() const noexcept -> std::string_view;
		[[nodiscard]] auto query() const noexcept -> std::string_view;
		[[nodiscard]] auto fragment() const noexcept -> std::string_view;

		[[nodiscard]] auto fspath() const -> std::filesystem::path;
		[[nodiscard]] bool valid() const noexcept;
		[[nodiscard]] bool isLocalPath() const noexcept;
		[[nodiscard]] bool isDataUri() const noexcept;
	};

	/**
	 * Custom URI class for fastgltf's needs. glTF 2.0 only allows two types of URIs:
	 *  (1) Data URIs as specified in RFC 2397.
	 *  (2) Relative paths as specified in RFC 3986.
	 *
	 * See https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#uris for details.
	 * However, the glTF spec allows more broader URIs in client implementations. Therefore,
	 * this supports all types of URIs as defined in RFC 3986.
	 *
	 * This class, unlike fastgltf::URIView, holds a std::string which contains the URI. It
	 * also decodes any percent-encoded characters.
	 */
	class URI {
		std::string uri;
		URIView view;

		void readjustViews(const URIView& other);

	public:
		explicit URI() noexcept;

		explicit URI(std::string uri) noexcept;
		explicit URI(std::string_view uri) noexcept;
		explicit URI(const URIView& view) noexcept;

		URI(const URI& other);
		URI(URI&& other) noexcept;

		URI& operator=(const URI& other);
		URI& operator=(const URIView& other);
		URI& operator=(URI&& other) noexcept;

		operator URIView() const noexcept;

		static void decodePercents(std::string& x) noexcept;

		[[nodiscard]] auto string() const noexcept -> std::string_view;
		[[nodiscard]] auto c_str() const noexcept -> const char*;

		[[nodiscard]] auto scheme() const noexcept -> std::string_view;
		[[nodiscard]] auto userinfo() const noexcept -> std::string_view;
		[[nodiscard]] auto host() const noexcept -> std::string_view;
		[[nodiscard]] auto port() const noexcept -> std::string_view;
		[[nodiscard]] auto path() const noexcept -> std::string_view;
		[[nodiscard]] auto query() const noexcept -> std::string_view;
		[[nodiscard]] auto fragment() const noexcept -> std::string_view;

		[[nodiscard]] auto fspath() const -> std::filesystem::path;
		[[nodiscard]] bool valid() const noexcept;
		[[nodiscard]] bool isLocalPath() const noexcept;
		[[nodiscard]] bool isDataUri() const noexcept;
    };

    FASTGLTF_EXPORT inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

    /**
     * Custom span class imitating C++20's std::span for referencing bytes without owning the
     * allocation. Can also directly be converted to a std::span or used by itself.
     */
    FASTGLTF_EXPORT template <typename T, std::size_t Extent = dynamic_extent>
    class span {
        using element_type = T;
        using value_type = std::remove_cv_t<T>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;

        pointer _ptr = nullptr;
        size_type _size = 0;

    public:
        constexpr span() noexcept = default;

        template <typename Iterator>
        explicit constexpr span(Iterator first, size_type count) : _ptr(first), _size(count) {}

#if FASTGLTF_CPP_20
        constexpr span(std::span<T> data) : _ptr(data.data()), _size(data.size()) {}
#endif

        constexpr span(const span& other) noexcept = default;
        constexpr span& operator=(const span& other) noexcept = default;

        [[nodiscard]] constexpr reference operator[](size_type idx) const {
            return data()[idx];
        }

		[[nodiscard]] constexpr reference at(size_type idx) const {
			if (idx >= size()) {
                raise<std::out_of_range>("Index is out of range for span");
			}
			return data()[idx];
		}

        [[nodiscard]] constexpr pointer data() const noexcept {
            return _ptr;
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return _size;
        }

        [[nodiscard]] constexpr size_type size_bytes() const noexcept {
            return size() * sizeof(element_type);
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return size() == 0;
        }

        [[nodiscard]] constexpr span<T, Extent> first(size_type count) const {
            return span(_ptr, count);
        }

        [[nodiscard]] constexpr span<T, Extent> last(size_type count) const {
            return span(&data()[size() - count], count);
        }

        [[nodiscard]] constexpr span<T, Extent> subspan(size_type offset, size_type count = dynamic_extent) const {
            return span(&data()[offset], count == dynamic_extent ? size() - offset : count);
        }

#if FASTGLTF_CPP_20
        operator std::span<T>() const {
            return std::span(data(), size());
        }
#endif
    };

	// Deduction guide for easily instantiating spans
	FASTGLTF_EXPORT template <typename T>
	span(const T* data, std::size_t size) -> span<const T>;

    FASTGLTF_EXPORT using CustomBufferId = std::uint64_t;

    /**
     * Namespace for structs that describe individual sources of data for images and/or buffers.
     */
    namespace sources {
        FASTGLTF_EXPORT struct BufferView {
            std::size_t bufferViewIndex;
            MimeType mimeType = MimeType::None;
        };

        FASTGLTF_EXPORT struct URI {
            std::size_t fileByteOffset;
            fastgltf::URI uri;
            MimeType mimeType = MimeType::None;
        };

        FASTGLTF_EXPORT struct Array {
            StaticVector<std::byte> bytes;
            MimeType mimeType = MimeType::None;
        };

		/** @note This type is not used by the fastgltf parser and is only used for exporting. Use sources::Array instead when importing intead. */
		FASTGLTF_EXPORT struct Vector {
			std::vector<std::byte> bytes;
			MimeType mimeType = MimeType::None;
		};

        FASTGLTF_EXPORT struct CustomBuffer {
            CustomBufferId id;
            MimeType mimeType = MimeType::None;
        };

        FASTGLTF_EXPORT struct ByteView {
            span<const std::byte> bytes;
            MimeType mimeType = MimeType::None;
        };

		FASTGLTF_EXPORT struct Fallback {};
    } // namespace sources

    /**
     * Represents the data source of a buffer or image. These could be a buffer view, a file path
     * (including offsets), a StaticVector (if #Options::LoadExternalBuffers or #Options::LoadGLBBuffers
     * was specified), or the ID of a custom buffer.
     *
     * @note As a user, you should never encounter this variant holding the std::monostate, as that would be an ill-formed glTF,
     * which fastgltf already checks for while parsing.
     *
     * @note For buffers, this variant will never hold a sources::BufferView, as only images are able to reference buffer views as a source.
     */
    FASTGLTF_EXPORT using DataSource = std::variant<std::monostate, sources::BufferView, sources::URI, sources::Array, sources::Vector, sources::CustomBuffer, sources::ByteView, sources::Fallback>;

    FASTGLTF_EXPORT struct AnimationChannel {
        std::size_t samplerIndex;
        Optional<std::size_t> nodeIndex;
        AnimationPath path;
    };

    FASTGLTF_EXPORT struct AnimationSampler {
        std::size_t inputAccessor;
        std::size_t outputAccessor;
        AnimationInterpolation interpolation = AnimationInterpolation::Linear;
    };

    FASTGLTF_EXPORT struct Animation {
	    FASTGLTF_FG_PMR_NS::MaybeSmallVector<AnimationChannel> channels;
	    FASTGLTF_FG_PMR_NS::MaybeSmallVector<AnimationSampler> samplers;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct AssetInfo {
        FASTGLTF_STD_PMR_NS::string gltfVersion;
        FASTGLTF_STD_PMR_NS::string copyright;
        FASTGLTF_STD_PMR_NS::string generator;
    };

    FASTGLTF_EXPORT struct Camera {
        struct Orthographic {
            num xmag;
            num ymag;
            num zfar;
            num znear;
        };
        struct Perspective {
            Optional<num> aspectRatio;
            num yfov;
            // If omitted, use an infinite projection matrix.
            Optional<num> zfar;
            num znear;
        };

        /**
         * Variant holding either a perspective or a orthographic camera. Use std::holds_alternative
         * and/or std::get_if to figure out which camera type is being used.
         */
        std::variant<Perspective, Orthographic> camera;
        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct Skin {
	    Optional<std::size_t> inverseBindMatrices;
        Optional<std::size_t> skeleton;
        FASTGLTF_FG_PMR_NS::MaybeSmallVector<std::size_t> joints;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct Sampler {
	    Optional<Filter> magFilter;
	    Optional<Filter> minFilter;
        Wrap wrapS = Wrap::Repeat;
        Wrap wrapT = Wrap::Repeat;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct Scene {
	    FASTGLTF_FG_PMR_NS::MaybeSmallVector<std::size_t> nodeIndices;

        FASTGLTF_STD_PMR_NS::string name;
    };

	FASTGLTF_EXPORT struct TRS {
		math::fvec3 translation = math::fvec3(0.f);
		math::fquat rotation = math::fquat(0.f, 0.f, 0.f, 1.f);
		math::fvec3 scale = math::fvec3(1.f);
	};

	FASTGLTF_EXPORT struct Attribute {
		FASTGLTF_STD_PMR_NS::string name;
		std::size_t accessorIndex;
	};

    FASTGLTF_EXPORT struct Node {
        Optional<std::size_t> meshIndex;
	    Optional<std::size_t> skinIndex;
	    Optional<std::size_t> cameraIndex;

        /**
         * Only ever non-empty when KHR_lights_punctual is enabled and used by the asset.
         */
        Optional<std::size_t> lightIndex;

	    FASTGLTF_FG_PMR_NS::MaybeSmallVector<std::size_t> children;
	    FASTGLTF_FG_PMR_NS::MaybeSmallVector<num> weights;

        /**
         * Variant holding either the three TRS components; transform, rotation, and scale, or a
         * transformation matrix, which cannot skew or shear. The latter can be decomposed into
         * the TRS components by specifying Options::DecomposeNodeMatrices.
         */
        std::variant<TRS, math::fmat4x4> transform;

        /**
         * Only ever non-empty when EXT_mesh_gpu_instancing is enabled and used by the asset.
         */
        FASTGLTF_STD_PMR_NS::vector<Attribute> instancingAttributes;

        FASTGLTF_STD_PMR_NS::string name;

        [[nodiscard]] auto findInstancingAttribute(std::string_view attributeName) noexcept {
            for (auto it = instancingAttributes.begin(); it != instancingAttributes.end(); ++it) {
                if (it->name == attributeName)
                    return it;
            }
            return instancingAttributes.end();
        }

        [[nodiscard]] auto findInstancingAttribute(std::string_view attributeName) const noexcept {
            for (auto it = instancingAttributes.cbegin(); it != instancingAttributes.cend(); ++it) {
                if (it->name == attributeName)
                    return it;
            }
            return instancingAttributes.cend();
        }
    };

    FASTGLTF_EXPORT struct Primitive {
		// Instead of a map, we have a list of attributes here. Each pair contains
		// the name of the attribute and the corresponding accessor index.
		FASTGLTF_FG_PMR_NS::SmallVector<Attribute, 4> attributes;
        PrimitiveType type = PrimitiveType::Triangles;

        FASTGLTF_STD_PMR_NS::vector<FASTGLTF_FG_PMR_NS::SmallVector<Attribute, 4>> targets;

        Optional<std::size_t> indicesAccessor;
        Optional<std::size_t> materialIndex;

		/**
		 * Represents the mappings data from KHR_material_variants.
		 * Use the variant index to index into this array to get the corresponding material index to use.
		 * If the optional has no value, the normal materialIndex should be used as a fallback.
		 */
		std::vector<Optional<std::size_t>> mappings;

		[[nodiscard]] auto findAttribute(std::string_view name) noexcept {
			for (auto* it = attributes.begin(); it != attributes.end(); ++it) {
				if (it->name == name)
					return it;
			}
			return attributes.end();
		}

		[[nodiscard]] auto findAttribute(std::string_view name) const noexcept {
			for (const auto* it = attributes.cbegin(); it != attributes.cend(); ++it) {
				if (it->name == name)
					return it;
			}
			return attributes.cend();
		}

		[[nodiscard]] auto findTargetAttribute(std::size_t targetIndex, std::string_view name) noexcept {
			auto& targetAttributes = targets[targetIndex];
			for (auto* it = targetAttributes.begin(); it != targetAttributes.end(); ++it) {
				if (it->name == name)
					return it;
			}
			return targetAttributes.end();
		}

		[[nodiscard]] auto findTargetAttribute(std::size_t targetIndex, std::string_view name) const noexcept {
			const auto& targetAttributes = targets[targetIndex];
			for (const auto* it = targetAttributes.cbegin(); it != targetAttributes.cend(); ++it) {
				if (it->name == name)
					return it;
			}
			return targetAttributes.cend();
		}
	};

    FASTGLTF_EXPORT struct Mesh {
		FASTGLTF_FG_PMR_NS::MaybeSmallVector<Primitive, 2> primitives;
	    FASTGLTF_FG_PMR_NS::MaybeSmallVector<num> weights;

        FASTGLTF_STD_PMR_NS::string name;
    };

    /**
     * Texture transform information as per KHR_texture_transform.
     */
    FASTGLTF_EXPORT struct TextureTransform {
        /**
         * Rotate the UVs by this many radians counter-clockwise around the origin. This is equivalent to a similar rotation of the image clockwise.
         */
        num rotation;

		/**
		 * The offset of the UV coordinate origin as a factor of the texture dimensions.
		 */
		math::nvec2 uvOffset = math::nvec2(0);

		/**
		 * The scale factor applied to the components of the UV coordinates.
		 */
		math::nvec2 uvScale = math::nvec2(1);

        /**
         * Overrides the textureInfo texCoord value if supplied.
         */
        Optional<std::size_t> texCoordIndex;
    };

    FASTGLTF_EXPORT struct TextureInfo {
        std::size_t textureIndex;
        std::size_t texCoordIndex = 0;

        /**
         * Data from KHR_texture_transform, and nullptr if the extension wasn't enabled or used.
         */
        std::unique_ptr<TextureTransform> transform;
    };

	FASTGLTF_EXPORT struct NormalTextureInfo : TextureInfo {
		num scale = 1.f;
	};

	FASTGLTF_EXPORT struct OcclusionTextureInfo : TextureInfo {
		num strength = 1.f;
	};

    FASTGLTF_EXPORT struct PBRData {
		/**
		 * The factors for the base color of then material.
		 */
		math::nvec4 baseColorFactor = math::nvec4(1);

        /**
         * The factor for the metalness of the material.
         */
        num metallicFactor = 1.0f;

        /**
         * The factor for the roughness of the material.
         */
        num roughnessFactor = 1.0f;

        Optional<TextureInfo> baseColorTexture;
        Optional<TextureInfo> metallicRoughnessTexture;
    };

	FASTGLTF_EXPORT struct MaterialAnisotropy {
		num anisotropyStrength = 0.0f;
		num anisotropyRotation = 0.0f;
		Optional<TextureInfo> anisotropyTexture;
	};

    /**
     * Specular information from KHR_materials_specular.
     */
    FASTGLTF_EXPORT struct MaterialSpecular {
        num specularFactor = 1.0f;
        Optional<TextureInfo> specularTexture;
		math::nvec3 specularColorFactor = math::nvec3(1);
        Optional<TextureInfo> specularColorTexture;
    };

    /**
     * Iridescence information from KHR_materials_iridescence
     */
    FASTGLTF_EXPORT struct MaterialIridescence {
        num iridescenceFactor = 0.0f;
        Optional<TextureInfo> iridescenceTexture;
        num iridescenceIor = 1.3f;
        num iridescenceThicknessMinimum = 100.0f;
        num iridescenceThicknessMaximum = 400.0f;
        Optional<TextureInfo> iridescenceThicknessTexture;
    };

    /**
     * Volume information from KHR_materials_volume
     */
    FASTGLTF_EXPORT struct MaterialVolume {
        num thicknessFactor = 0.0f;
        Optional<TextureInfo> thicknessTexture;
        num attenuationDistance = std::numeric_limits<num>::infinity();
		math::nvec3 attenuationColor = math::nvec3(1);
    };

    FASTGLTF_EXPORT struct MaterialTransmission {
        num transmissionFactor = 0.0f;
        Optional<TextureInfo> transmissionTexture;
    };

    FASTGLTF_EXPORT struct MaterialClearcoat {
        num clearcoatFactor = 0.0f;
        Optional<TextureInfo> clearcoatTexture;
        num clearcoatRoughnessFactor = 0.0f;
        Optional<TextureInfo> clearcoatRoughnessTexture;
        Optional<TextureInfo> clearcoatNormalTexture;
    };

    FASTGLTF_EXPORT struct MaterialSheen {
		math::nvec3 sheenColorFactor = math::nvec3(0);
        Optional<TextureInfo> sheenColorTexture;
        num sheenRoughnessFactor = 0.0f;
        Optional<TextureInfo> sheenRoughnessTexture;
    };

#if FASTGLTF_ENABLE_DEPRECATED_EXT
    /**
     * Specular/Glossiness information from KHR_materials_pbrSpecularGlossiness.
     */
    FASTGLTF_EXPORT struct MaterialSpecularGlossiness {
		math::nvec4 diffuseFactor = math::nvec4(1);
        Optional<TextureInfo> diffuseTexture;
		math::nvec3 specularFactor = math::nvec3(1);
        num glossinessFactor = 1.0f;
        Optional<TextureInfo> specularGlossinessTexture;
    };
#endif

	FASTGLTF_EXPORT struct MaterialPackedTextures {
		Optional<TextureInfo> occlusionRoughnessMetallicTexture;
		Optional<TextureInfo> roughnessMetallicOcclusionTexture;
		Optional<TextureInfo> normalTexture;
	};

    FASTGLTF_EXPORT struct Material {
        /**
         * A set of parameter values that are used to define the metallic-roughness material model
         * from Physically Based Rendering (PBR) methodology.
         */
        PBRData pbrData;

        /**
         * The tangent space normal texture.
         */
        Optional<NormalTextureInfo> normalTexture;
        Optional<OcclusionTextureInfo> occlusionTexture;
        Optional<TextureInfo> emissiveTexture;

		/**
		 * The factors for the emissive color of the material.
		 */
		math::nvec3 emissiveFactor = math::nvec3(0);

        /**
         * The values used to determine the transparency of the material.
         */
        AlphaMode alphaMode = AlphaMode::Opaque;

		/**
		 * Determines whether back-face culling should be disabled when using this material.
		 */
		bool doubleSided = false;

		/**
		 * Only true when KHR_materials_unlit is enabled and specified for this material.
		 */
		bool unlit = false;

		/**
		 * The alpha value that determines the upper limit for fragments that
		 * should be discarded for transparency.
		 */
        num alphaCutoff = 0.5f;

		/**
		 * The emissive strength from the KHR_materials_emissive_strength extension.
		 */
		num emissiveStrength = 1.0f;

		/**
		 * The index of refraction as specified through KHR_materials_ior.
		 */
		num ior = 1.5f;

		/**
		 * The dispersion factor from KHR_materials_dispersion, specifies as 20/Abbe number (20/V).
		 */
		num dispersion = 0.0f;

		std::unique_ptr<MaterialAnisotropy> anisotropy;

        std::unique_ptr<MaterialClearcoat> clearcoat;

        /**
         * Iridescence information from KHR_materials_iridescence.
         */
        std::unique_ptr<MaterialIridescence> iridescence;

        std::unique_ptr<MaterialSheen> sheen;

        /**
         * Specular information from KHR_materials_specular.
         */
        std::unique_ptr<MaterialSpecular> specular;

#if FASTGLTF_ENABLE_DEPRECATED_EXT
        /**
         * Specular/Glossiness information from KHR_materials_pbrSpecularGlossiness.
         */
        std::unique_ptr<MaterialSpecularGlossiness> specularGlossiness;
#endif

        /**
         * Specular information from KHR_materials_transmission.
         */
        std::unique_ptr<MaterialTransmission> transmission;

        /**
         * Volume information from KHR_materials_volume
         */
        std::unique_ptr<MaterialVolume> volume;

		/**
		 * The index of a packed texture from the MSFT_packing_normalRoughnessMetallic extension,
		 * providing normal, roughness and metallic data.
		 */
		Optional<TextureInfo> packedNormalMetallicRoughnessTexture;

		std::unique_ptr<MaterialPackedTextures> packedOcclusionRoughnessMetallicTextures;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct Texture {
		/**
		 * If no sampler is specified, use a default sampler with repeat wrap and auto filter.
		 */
		Optional<std::size_t> samplerIndex;

		/**
		 * The index of the image used by this texture. Either this will have a value,
		 * or one of the following extensions will define a texture index. If no extensions
		 * were enabled while parsing, this will always have a value.
		 */
		Optional<std::size_t> imageIndex;

		/**
		 * An optional texture index from the KHR_texture_basisu extension.
		 */
		Optional<std::size_t> basisuImageIndex;

		/**
		 * An optional texture index from the MSFT_texture_dds extension.
		 */
		Optional<std::size_t> ddsImageIndex;

		/**
		 * An optional texture index from the EXT_texture_webp extension.
		 */
		Optional<std::size_t> webpImageIndex;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct Image {
        DataSource data;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct SparseAccessor {
        std::size_t count;
        std::size_t indicesBufferView;
        std::size_t indicesByteOffset = 0;
        std::size_t valuesBufferView;
        std::size_t valuesByteOffset = 0;
        ComponentType indexComponentType;
    };

    FASTGLTF_EXPORT struct Accessor {
        std::size_t byteOffset = 0;
        std::size_t count;
        AccessorType type;
        ComponentType componentType;
        bool normalized = false;

        std::variant<std::monostate, FASTGLTF_STD_PMR_NS::vector<double>, FASTGLTF_STD_PMR_NS::vector<std::int64_t>> max;
        std::variant<std::monostate, FASTGLTF_STD_PMR_NS::vector<double>, FASTGLTF_STD_PMR_NS::vector<std::int64_t>> min;

        // Could have no value for sparse morph targets
        Optional<std::size_t> bufferViewIndex;

        Optional<SparseAccessor> sparse;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct CompressedBufferView {
        std::size_t bufferIndex;
        std::size_t byteOffset;
        std::size_t byteLength;
        std::size_t count;
        MeshoptCompressionMode mode;
        MeshoptCompressionFilter filter;

        std::size_t byteStride;
    };

    FASTGLTF_EXPORT struct BufferView {
        std::size_t bufferIndex;
        std::size_t byteOffset = 0;
        std::size_t byteLength;

        Optional<std::size_t> byteStride;
        Optional<BufferTarget> target;

        /**
         * Data from EXT_meshopt_compression, and nullptr if the extension was not enabled or used.
         */
        std::unique_ptr<CompressedBufferView> meshoptCompression;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct Buffer {
        std::size_t byteLength;

        DataSource data;

        FASTGLTF_STD_PMR_NS::string name;
    };

    FASTGLTF_EXPORT struct Light {
        LightType type;
        /** RGB light color in linear space. */
        math::nvec3 color;

        /** Point and spot lights use candela (lm/sr) while directional use lux (lm/m^2) */
        num intensity;
        /** Range for point and spot lights. If not present, range is infinite. */
        Optional<num> range;

		/** The inner and outer cone angles only apply to spot lights */
        Optional<num> innerConeAngle;
        Optional<num> outerConeAngle;

        FASTGLTF_STD_PMR_NS::string name;
    };

	class ChunkMemoryResource;
	FASTGLTF_EXPORT class Parser;

	FASTGLTF_EXPORT class Asset {
		friend class Parser;

#if !FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
		// This has to be first in this struct so that it gets destroyed last, leaving all allocations
		// alive until the end.
		std::shared_ptr<ChunkMemoryResource> memoryResource;
#endif

	public:
        /**
         * This will only ever have no value if #Options::DontRequireValidAssetMember was specified.
         */
        Optional<AssetInfo> assetInfo;
		FASTGLTF_STD_PMR_NS::vector<FASTGLTF_STD_PMR_NS::string> extensionsUsed;
		FASTGLTF_STD_PMR_NS::vector<FASTGLTF_STD_PMR_NS::string> extensionsRequired;

        Optional<std::size_t> defaultScene;
        std::vector<Accessor> accessors;
        std::vector<Animation> animations;
        std::vector<Buffer> buffers;
        std::vector<BufferView> bufferViews;
        std::vector<Camera> cameras;
        std::vector<Image> images;
        std::vector<Light> lights;
        std::vector<Material> materials;
        std::vector<Mesh> meshes;
        std::vector<Node> nodes;
        std::vector<Sampler> samplers;
        std::vector<Scene> scenes;
        std::vector<Skin> skins;
        std::vector<Texture> textures;

		std::vector<std::string> materialVariants;

        // Keeps tracked of categories that were actually parsed.
        Category availableCategories = Category::None;

        explicit Asset() = default;
        explicit Asset(const Asset& other) = delete;
        Asset(Asset&& other) noexcept :
#if !FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
				memoryResource(std::move(other.memoryResource)),
#endif
				assetInfo(std::move(other.assetInfo)),
				extensionsUsed(std::move(other.extensionsUsed)),
				extensionsRequired(std::move(other.extensionsRequired)),
				defaultScene(other.defaultScene),
				accessors(std::move(other.accessors)),
				animations(std::move(other.animations)),
				buffers(std::move(other.buffers)),
				bufferViews(std::move(other.bufferViews)),
				cameras(std::move(other.cameras)),
				images(std::move(other.images)),
				lights(std::move(other.lights)),
				materials(std::move(other.materials)),
				meshes(std::move(other.meshes)),
				nodes(std::move(other.nodes)),
				samplers(std::move(other.samplers)),
				scenes(std::move(other.scenes)),
				skins(std::move(other.skins)),
				textures(std::move(other.textures)),
				materialVariants(std::move(other.materialVariants)),
				availableCategories(other.availableCategories) {}

		Asset& operator=(const Asset& other) = delete;
		Asset& operator=(Asset&& other) noexcept {
			assetInfo = std::move(other.assetInfo);
			extensionsUsed = std::move(other.extensionsUsed);
			extensionsRequired = std::move(other.extensionsRequired);
			defaultScene = other.defaultScene;
			accessors = std::move(other.accessors);
			animations = std::move(other.animations);
			buffers = std::move(other.buffers);
			bufferViews = std::move(other.bufferViews);
			cameras = std::move(other.cameras);
			images = std::move(other.images);
			lights = std::move(other.lights);
			materials = std::move(other.materials);
			meshes = std::move(other.meshes);
			nodes = std::move(other.nodes);
			samplers = std::move(other.samplers);
			scenes = std::move(other.scenes);
			skins = std::move(other.skins);
			textures = std::move(other.textures);
			materialVariants = std::move(other.materialVariants);
			availableCategories = other.availableCategories;
#if !FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
			// This needs to be last to not destroy the old memoryResource for the current data.
			memoryResource = std::move(other.memoryResource);
#endif
			return *this;
		}
    };
#pragma endregion
} // namespace fastgltf

#ifdef _MSC_VER
#pragma warning(pop)
#endif
