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

#if !defined(__cplusplus) || (!defined(_MSVC_LANG) && __cplusplus < 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
#error "fastgltf requires C++17"
#endif

#include <fstream>
#include <functional>
#include <mutex>
#include <utility>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5030) // attribute 'x' is not recognized
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#pragma warning(disable : 4710) // function not inlined
#endif

#include <simdjson.h>

#ifdef SIMDJSON_TARGET_VERSION
// Make sure that SIMDJSON_TARGET_VERSION is equal to SIMDJSON_VERSION.
static_assert(std::string_view { SIMDJSON_TARGET_VERSION } == SIMDJSON_VERSION, "Outdated version of simdjson. Reconfigure project to update.");
#endif

#include <fastgltf/core.hpp>
#include <fastgltf/base64.hpp>

#if defined(FASTGLTF_IS_X86)
#include <nmmintrin.h> // SSE4.2 for the CRC-32C instructions
#elif defined(FASTGLTF_ENABLE_ARMV8_CRC)
// MSVC does not provide the arm crc32 intrinsics.
#include <arm_acle.h>
#endif

namespace fg = fastgltf;
namespace fs = std::filesystem;

namespace fastgltf {
    constexpr std::uint32_t binaryGltfHeaderMagic = 0x46546C67; // ASCII for "glTF".
    constexpr std::uint32_t binaryGltfJsonChunkMagic = 0x4E4F534A;
    constexpr std::uint32_t binaryGltfDataChunkMagic = 0x004E4942;

    struct BinaryGltfHeader {
        std::uint32_t magic;
        std::uint32_t version;
        std::uint32_t length;
    };
    static_assert(sizeof(BinaryGltfHeader) == 12, "Binary gltf header must be 12 bytes");
	static_assert(std::is_trivially_copyable_v<BinaryGltfHeader>);

	constexpr void readUint32LE(std::uint32_t& x, std::byte* bytes) noexcept {
		x = std::uint32_t(bytes[0])
				| (std::uint32_t(bytes[1]) << 8)
				| (std::uint32_t(bytes[2]) << 16)
				| (std::uint32_t(bytes[3]) << 24);
	}

	constexpr void writeUint32LE(std::uint32_t x, std::byte* buffer) noexcept {
		buffer[0] = static_cast<std::byte>(x);
		buffer[1] = static_cast<std::byte>(x >> 8);
		buffer[2] = static_cast<std::byte>(x >> 16);
		buffer[3] = static_cast<std::byte>(x >> 24);
	}

	/** GLBs are always little-endian, meaning we need to read the values accordingly */
	[[nodiscard, gnu::always_inline]] inline auto readBinaryHeader(GltfDataGetter& getter) noexcept {
		std::array<std::byte, sizeof(BinaryGltfHeader)> bytes {};
		getter.read(bytes.data(), bytes.size());

		BinaryGltfHeader header = {};
		readUint32LE(header.magic, &bytes[offsetof(BinaryGltfHeader, magic)]);
		readUint32LE(header.version, &bytes[offsetof(BinaryGltfHeader, version)]);
		readUint32LE(header.length, &bytes[offsetof(BinaryGltfHeader, length)]);
		return header;
	}

	[[gnu::always_inline]] inline auto writeBinaryHeader(const BinaryGltfHeader& header) noexcept {
		std::array<std::byte, sizeof(BinaryGltfHeader)> bytes {};
		writeUint32LE(header.magic, &bytes[offsetof(BinaryGltfHeader, magic)]);
		writeUint32LE(header.version, &bytes[offsetof(BinaryGltfHeader, version)]);
		writeUint32LE(header.length, &bytes[offsetof(BinaryGltfHeader, length)]);
		return bytes;
	}

    struct BinaryGltfChunk {
        std::uint32_t chunkLength;
        std::uint32_t chunkType;
    };
	static_assert(std::is_trivially_copyable_v<BinaryGltfChunk>);

	[[nodiscard, gnu::always_inline]] inline auto readBinaryChunk(GltfDataGetter& getter) noexcept {
		std::array<std::byte, sizeof(BinaryGltfChunk)> bytes {};
		getter.read(bytes.data(), bytes.size());

		BinaryGltfChunk chunk = {};
		readUint32LE(chunk.chunkLength, &bytes[offsetof(BinaryGltfChunk, chunkLength)]);
		readUint32LE(chunk.chunkType, &bytes[offsetof(BinaryGltfChunk, chunkType)]);
		return chunk;
	}

	[[gnu::always_inline]] inline auto writeBinaryChunk(const BinaryGltfChunk& chunk) noexcept {
		std::array<std::byte, sizeof(BinaryGltfChunk)> bytes {};
		writeUint32LE(chunk.chunkLength, &bytes[offsetof(BinaryGltfChunk, chunkLength)]);
		writeUint32LE(chunk.chunkType, &bytes[offsetof(BinaryGltfChunk, chunkType)]);
		return bytes;
	}

	using CRCStringFunction = std::uint32_t(*)(std::string_view str);

#if defined(FASTGLTF_IS_X86)
    [[gnu::hot, gnu::const, gnu::target("sse4.2")]] std::uint32_t sse_crc32c(std::string_view str) noexcept {
        return sse_crc32c(reinterpret_cast<const std::uint8_t*>(str.data()), str.size());
    }

    [[gnu::hot, gnu::const, gnu::target("sse4.2")]] std::uint32_t sse_crc32c(const std::uint8_t* d, std::size_t len) noexcept {
        std::uint32_t crc = 0;

        // Ddecode as much as possible using 4 byte steps.
        // We specifically don't use the 8 byte instruction here because it uses a 64-bit output integer.
        auto length = static_cast<std::int64_t>(len);
        while ((length -= sizeof(std::uint32_t)) >= 0) {
            std::uint32_t v;
            std::memcpy(&v, d, sizeof v);
            crc = _mm_crc32_u32(crc, v);
            d += sizeof v;
        }

        if (length & sizeof(std::uint16_t)) {
            std::uint16_t v;
            std::memcpy(&v, d, sizeof v);
            crc = _mm_crc32_u16(crc, v);
            d += sizeof v;
        }

        if (length & sizeof(std::uint8_t)) {
            crc = _mm_crc32_u8(crc, *d);
        }

        return crc;
    }
#elif defined(FASTGLTF_ENABLE_ARMV8_CRC)
	[[gnu::hot, gnu::const, gnu::target("+crc")]] std::uint32_t armv8_crc32c(std::string_view str) noexcept {
		return armv8_crc32c(reinterpret_cast<const std::uint8_t*>(str.data()), str.size());
	}

	[[gnu::hot, gnu::const, gnu::target("+crc")]] std::uint32_t armv8_crc32c(const std::uint8_t* d, std::size_t len) noexcept {
		std::uint32_t crc = 0;

		// Decrementing the length variable and incrementing the pointer directly has better codegen with Clang
		// than using a std::size_t i = 0.
		auto length = static_cast<std::int64_t>(len);
		while ((length -= sizeof(std::uint64_t)) >= 0) {
			std::uint64_t value;
			std::memcpy(&value, d, sizeof value);
			crc = __crc32cd(crc, value);
			d += sizeof value;
		}

		if (length & sizeof(std::uint32_t)) {
			std::uint32_t value;
			std::memcpy(&value, d, sizeof value);
			crc = __crc32cw(crc, value);
			d += sizeof value;
		}

		if (length & sizeof(std::uint16_t)) {
			std::uint16_t value;
			std::memcpy(&value, d, sizeof value);
			crc = __crc32ch(crc, value);
			d += sizeof value;
		}

		if (length & sizeof(std::uint8_t)) {
			crc = __crc32cb(crc, *d);
		}

		return crc;
	}
#endif

    /**
     * Points to the most 'optimal' CRC32-C encoding function. After initialiseCrc has been called,
     * this might also point to sse_crc32c or armv8_crc32c. We only use this for runtime evaluation of hashes, and is
     * intended to work for any length of data.
     */
    static CRCStringFunction crcStringFunction = crc32c;

    std::once_flag crcInitialisation;

    /**
     * Checks if SSE4.2 is available to try and use the hardware accelerated version.
     */
    void initialiseCrc() {
#if defined(FASTGLTF_IS_X86)
        const auto& impls = simdjson::get_available_implementations();
        if (const auto* sse4 = impls["westmere"]; sse4 != nullptr && sse4->supported_by_runtime_system()) {
            crcStringFunction = sse_crc32c;
        }
#elif defined(FASTGLTF_ENABLE_ARMV8_CRC)
		const auto& impls = simdjson::get_available_implementations();
		if (const auto* neon = impls["arm64"]; neon != nullptr && neon->supported_by_runtime_system()) {
			crcStringFunction = armv8_crc32c;
		}
#endif
    }

	[[nodiscard, gnu::always_inline]] inline bool getImageIndexForExtension(const simdjson::dom::element& element, Optional<std::size_t>& imageIndexOut) {
		using namespace simdjson;

		dom::object source;
		if (element.get(source) != simdjson::SUCCESS) FASTGLTF_UNLIKELY {
			return false;
		}
		std::uint64_t imageIndex;
		if (source["source"].get_uint64().get(imageIndex) != simdjson::SUCCESS) FASTGLTF_UNLIKELY {
			return false;
		}

		imageIndexOut = static_cast<std::size_t>(imageIndex);
		return true;
	}

	[[nodiscard, gnu::always_inline]] inline bool parseTextureExtensions(Texture& texture, simdjson::dom::object& extensions, Extensions extensionFlags) {
		for (auto extension : extensions) {
			auto hashedKey = crcStringFunction(extension.key);
			switch (hashedKey) {
				case force_consteval<crc32c(extensions::KHR_texture_basisu)>: {
					if (!hasBit(extensionFlags, Extensions::KHR_texture_basisu))
						break;
					if (!getImageIndexForExtension(extension.value, texture.basisuImageIndex))
						return false;
					break;
				}
				case force_consteval<crc32c(extensions::MSFT_texture_dds)>: {
					if (!hasBit(extensionFlags, Extensions::MSFT_texture_dds))
						break;
					if (!getImageIndexForExtension(extension.value, texture.ddsImageIndex))
						return false;
					break;
				}
				case force_consteval<crc32c(extensions::EXT_texture_webp)>: {
					if (!hasBit(extensionFlags, Extensions::EXT_texture_webp))
						break;
					if (!getImageIndexForExtension(extension.value, texture.webpImageIndex))
						return false;
					break;
				}
				default:
					break;
			}
		}

		return true;
	}

	[[nodiscard, gnu::always_inline]] inline Error getJsonArray(const simdjson::dom::object& parent, std::string_view arrayName, simdjson::dom::array* array) noexcept {
		using namespace simdjson;

		const auto error = parent[arrayName].get_array().get(*array);
		if (error == NO_SUCH_FIELD) {
			return Error::MissingField;
		}
		if (error == SUCCESS) FASTGLTF_LIKELY {
			return Error::None;
		}
		return Error::InvalidJson;
	}

	enum class TextureInfoType : std::uint_fast8_t {
		Standard = 0,
		NormalTexture = 1,
		OcclusionTexture = 2,
	};

	fg::Error parseTextureInfo(simdjson::dom::object& object, std::string_view key, TextureInfo* info, Extensions extensions, TextureInfoType type = TextureInfoType::Standard) noexcept {
		using namespace simdjson;

		dom::object child;
		if (auto childErr = object[key].get_object().get(child); childErr == NO_SUCH_FIELD) {
			return Error::MissingField;
		} else if (childErr != SUCCESS) FASTGLTF_UNLIKELY {
			return Error::InvalidGltf;
		}

		std::uint64_t index;
		if (child["index"].get_uint64().get(index) == SUCCESS) FASTGLTF_LIKELY {
			info->textureIndex = static_cast<std::size_t>(index);
		} else {
			return Error::InvalidGltf;
		}

		if (auto error = child["texCoord"].get_uint64().get(index); error == SUCCESS) FASTGLTF_LIKELY {
			info->texCoordIndex = static_cast<std::size_t>(index);
		} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidJson;
		}

		if (type == TextureInfoType::NormalTexture) {
            double scale;
			if (auto error = child["scale"].get_double().get(scale); error == SUCCESS) FASTGLTF_LIKELY {
				reinterpret_cast<NormalTextureInfo*>(info)->scale = static_cast<num>(scale);
			} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
			}
		} else if (type == TextureInfoType::OcclusionTexture) {
			double strength;
			if (auto error = child["strength"].get_double().get(strength); error == SUCCESS) FASTGLTF_LIKELY {
				reinterpret_cast<OcclusionTextureInfo*>(info)->strength = static_cast<num>(strength);
			} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
		}

		dom::object extensionsObject;
		if (child["extensions"].get_object().get(extensionsObject) == SUCCESS) FASTGLTF_LIKELY {
			dom::object textureTransform;
			if (hasBit(extensions, Extensions::KHR_texture_transform) && extensionsObject[extensions::KHR_texture_transform].get_object().get(textureTransform) == SUCCESS) FASTGLTF_LIKELY {
				auto transform = std::make_unique<TextureTransform>();
				transform->rotation = 0.0F;

				if (textureTransform["texCoord"].get_uint64().get(index) == SUCCESS) FASTGLTF_LIKELY {
					transform->texCoordIndex = index;
				}

				double rotation = 0.0F;
				if (textureTransform["rotation"].get_double().get(rotation) == SUCCESS) FASTGLTF_LIKELY {
					transform->rotation = static_cast<num>(rotation);
				}

				dom::array array;
				if (textureTransform["offset"].get_array().get(array) == SUCCESS) FASTGLTF_LIKELY {
					for (auto i = 0U; i < 2; ++i) {
						double val;
						if (array.at(i).get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
							return Error::InvalidGltf;
						}
						transform->uvOffset[i] = static_cast<num>(val);
					}
				}

				if (textureTransform["scale"].get_array().get(array) == SUCCESS) FASTGLTF_LIKELY {
					for (auto i = 0U; i < 2; ++i) {
						double val;
						if (array.at(i).get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
							return Error::InvalidGltf;
						}
						transform->uvScale[i] = static_cast<num>(val);
					}
				}

				info->transform = std::move(transform);
			}
		}

		return Error::None;
	}

	void writeTextureInfo(std::string& json, const TextureInfo* info, TextureInfoType type = TextureInfoType::Standard) {
		json += '{';
		json += "\"index\":" + std::to_string(info->textureIndex);
		if (info->texCoordIndex != 0) {
			json += ",\"texCoord\":" + std::to_string(info->texCoordIndex);
		}
		if (type == TextureInfoType::NormalTexture) {
			json += ",\"scale\":" + std::to_string(reinterpret_cast<const NormalTextureInfo*>(info)->scale);
		} else if (type == TextureInfoType::OcclusionTexture) {
			json += ",\"strength\":" + std::to_string(reinterpret_cast<const OcclusionTextureInfo*>(info)->strength);
		}

        if (info->transform != nullptr) {
            json += R"(,"extensions":{"KHR_texture_transform":{)";
            const auto& transform = *info->transform;
            if (transform.uvOffset[0] != 0.0 || transform.uvOffset[1] != 0.0) {
                json += "\"offset\":[" + std::to_string(transform.uvOffset[0]) + ',' + std::to_string(transform.uvOffset[1]) + ']';
            }
            if (transform.rotation != 0.0) {
                if (json.back() != '{') json += ',';
                json += "\"rotation\":" + std::to_string(transform.rotation);
            }
            if (transform.uvScale[0] != 1.0 || transform.uvScale[1] != 1.0) {
                if (json.back() != '{') json += ',';
                json += "\"scale\":[" + std::to_string(transform.uvScale[0]) + ',' + std::to_string(transform.uvScale[1]) + ']';
            }
            if (transform.texCoordIndex.has_value()) {
                if (json.back() != '{') json += ',';
                json += "\"texCoord\":" + std::to_string(transform.texCoordIndex.value());
            }
            json += "}}";
        }

		json += '}';
	}
} // namespace fastgltf

#pragma region URI
fg::URIView::URIView() noexcept = default;

fg::URIView::URIView(std::string_view uri) noexcept : view(uri) {
	parse();
}

fg::URIView::URIView(const URIView& other) noexcept {
	*this = other;
}

fg::URIView& fg::URIView::operator=(const URIView& other) {
	view = other.view;
	_scheme = other._scheme;
	_path = other._path;
	_userinfo = other._userinfo;
	_host = other._host;
	_port = other._port;
	_query = other._query;
	_fragment = other._fragment;
	return *this;
}

fg::URIView& fg::URIView::operator=(std::string_view other) {
	view = other;
	parse();
	return *this;
}

void fg::URIView::parse() {
	if (view.empty()) {
		_valid = false;
		return;
	}

	std::size_t idx = 0;
	auto firstColon = view.find(':');
	if (firstColon != std::string::npos) {
		// URI has a scheme.
		if (firstColon == 0) {
			// Empty scheme is invalid
			_valid = false;
			return;
		}
		_scheme = view.substr(0, firstColon);
		idx = firstColon + 1;
	}

	if (startsWith(view.substr(idx), "//")) {
		// URI has an authority part.
		idx += 2;
		auto nextSlash = view.find('/', idx);
		auto userInfo = view.find('@', idx);
		if (userInfo != std::string::npos && userInfo < nextSlash) {
			_userinfo = view.substr(idx, userInfo - idx);
			idx += _userinfo.size() + 1;
		}

		auto hostEnd = nextSlash - 1;
		std::size_t portColon;
		if (view[idx] == '[') {
			hostEnd = view.find(']', idx);
			if (hostEnd == std::string::npos) {
				_valid = false;
				return;
			}
			// IPv6 addresses are made up of colons, so we need to search after its address.
			// This will just be hostEnd + 1 or std::string::npos.
			portColon = view.find(':', hostEnd);
		} else {
			portColon = view.find(':', idx);
		}

		if (portColon != std::string::npos) {
			_host = view.substr(idx, portColon - idx);
			++portColon; // We don't want to include the colon in the port string.
			_port = view.substr(portColon, nextSlash - portColon);
		} else {
			++idx;
			_host = view.substr(idx, hostEnd - idx);
		}

		idx = nextSlash; // Path includes this slash
	}

	if (_scheme == "data") {
		// The data scheme is just followed by a mime and then bytes.
		// Also, let's avoid all the find and substr on very large data strings
		// which can be multiple MB.
		_path = view.substr(idx);
	} else {
		// Parse the path.
		auto questionIdx = view.find('?', idx);
		auto hashIdx = view.find('#', idx);
		if (questionIdx != std::string::npos) {
			_path = view.substr(idx, questionIdx - idx);

			if (hashIdx == std::string::npos) {
				_query = view.substr(++questionIdx);
			} else {
				++questionIdx;
				_query = view.substr(questionIdx, hashIdx - questionIdx);
				_fragment = view.substr(++hashIdx);
			}
		} else if (hashIdx != std::string::npos) {
			_path = view.substr(idx, hashIdx - idx);
			_fragment = view.substr(++hashIdx);
		} else {
			_path = view.substr(idx);
		}
	}
}

const char* fg::URIView::data() const noexcept {
	return view.data();
}

std::string_view fg::URIView::string() const noexcept { return view; }
std::string_view fg::URIView::scheme() const noexcept { return _scheme; }
std::string_view fg::URIView::userinfo() const noexcept { return _userinfo; }
std::string_view fg::URIView::host() const noexcept { return _host; }
std::string_view fg::URIView::port() const noexcept { return _port; }
std::string_view fg::URIView::path() const noexcept { return _path; }
std::string_view fg::URIView::query() const noexcept { return _query; }
std::string_view fg::URIView::fragment() const noexcept { return _fragment; }

fs::path fg::URIView::fspath() const {
	if (!isLocalPath())
		return {};
	return { path() };
}

bool fg::URIView::valid() const noexcept {
	return _valid;
}

bool fg::URIView::isLocalPath() const noexcept {
	return scheme().empty() || (scheme() == "file" && host().empty());
}

bool fg::URIView::isDataUri() const noexcept {
	return scheme() == "data";
}

fg::URI::URI() noexcept = default;

fg::URI::URI(std::string uri) noexcept : uri(std::move(uri)) {
	decodePercents(this->uri);
	view = this->uri; // Also parses.
}

fg::URI::URI(std::string_view uri) noexcept : uri(uri) {
	decodePercents(this->uri);
	view = this->uri; // Also parses.
}

fg::URI::URI(const URIView& view) noexcept : uri(view.view) {
	auto oldSize = uri.size();
	decodePercents(uri);
	if (uri.size() == oldSize) {
		readjustViews(view);
	} else {
		// Reparses the URI string
		this->view = this->uri;
	}
}

// Some C++ stdlib implementations copy in some cases when moving strings, which invalidates the
// views stored in the URI struct. This function adjusts the views from the old string to the new
// string for safe copying.
fg::URI::URI(const URI& other) {
	*this = other;
}

fg::URI::URI(URI&& other) noexcept {
	*this = other;
}

fg::URI& fg::URI::operator=(const URI& other) {
	uri = other.uri;
	// We'll assume that with copying the string will always have to reallocate.
	readjustViews(other.view);
	return *this;
}

fg::URI& fg::URI::operator=(const URIView& other) {
	uri = other.view;
	auto oldSize = uri.size();
	decodePercents(uri);
	if (uri.size() == oldSize) {
		readjustViews(other);
	} else {
		// We removed some encoded chars, which have now invalidated all the string views.
		// Therefore, the URI needs to be parsed again.
		view = this->uri;
	}
	return *this;
}

fg::URI& fg::URI::operator=(URI&& other) noexcept {
	auto* oldData = other.uri.data();
	uri = std::move(other.uri);

	// Invalidate the previous URI's view.
	view._valid = other.view._valid;
	other.view._valid = false;

	if (uri.data() != oldData) {
		// Allocation changed, we need to readjust views
		readjustViews(other.view);
	} else {
		// No reallocation happened, we can safely copy the view.
		view = other.view;
	}
	return *this;
}

fg::URI::operator fg::URIView() const noexcept {
	return view;
}

void fg::URI::readjustViews(const URIView& other) {
	if (!other._scheme.empty())   { view._scheme     = std::string_view(uri.data() + (other._scheme.data()     - other.view.data()), other._scheme.size()); }
	if (!other._path.empty())     { view._path       = std::string_view(uri.data() + (other._path.data()       - other.view.data()), other._path.size()); }
	if (!other._userinfo.empty()) { view._userinfo   = std::string_view(uri.data() + (other._userinfo.data()   - other.view.data()), other._userinfo.size()); }
	if (!other._host.empty())     { view._host       = std::string_view(uri.data() + (other._host.data()       - other.view.data()), other._host.size()); }
	if (!other._port.empty())     { view._port       = std::string_view(uri.data() + (other._port.data()       - other.view.data()), other._port.size()); }
	if (!other._query.empty())    { view._query      = std::string_view(uri.data() + (other._query.data()      - other.view.data()), other._query.size()); }
	if (!other._fragment.empty()) { view._fragment   = std::string_view(uri.data() + (other._fragment.data()   - other.view.data()), other._fragment.size()); }

	view.view = uri;
}

void fg::URI::decodePercents(std::string& x) noexcept {
	for (std::size_t i = 0; i < x.size(); ++i) {
		if (x[i] != '%')
			continue;

		// Read the next two chars and store them
		std::array<char, 3> chars = {x[i + 1], x[i + 2]};
		x[i] = static_cast<char>(std::strtoul(chars.data(), nullptr, 16));
		x.erase(i + 1, 2);
	}
}

std::string_view fg::URI::string() const noexcept { return uri; }
const char*      fg::URI::c_str() const noexcept { return uri.c_str(); }
std::string_view fg::URI::scheme() const noexcept { return view.scheme(); }
std::string_view fg::URI::userinfo() const noexcept { return view.userinfo(); }
std::string_view fg::URI::host() const noexcept { return view.host(); }
std::string_view fg::URI::port() const noexcept { return view.port(); }
std::string_view fg::URI::path() const noexcept { return view.path(); }
std::string_view fg::URI::query() const noexcept { return view.query(); }
std::string_view fg::URI::fragment() const noexcept { return view.fragment(); }

fs::path fg::URI::fspath() const {
	return view.fspath();
}

bool fg::URI::valid() const noexcept {
	return view.valid();
}

bool fg::URI::isLocalPath() const noexcept {
	return view.isLocalPath();
}

bool fg::URI::isDataUri() const noexcept {
	return view.isDataUri();
}
#pragma endregion

#pragma region glTF parsing
fg::Expected<fg::DataSource> fg::Parser::decodeDataUri(URIView& uri) const noexcept {
    auto path = uri.path();
    auto mimeEnd = path.find(';');
    auto mime = path.substr(0, mimeEnd);

    auto encodingEnd = path.find(',');
    auto encoding = path.substr(mimeEnd + 1, encodingEnd - mimeEnd - 1);
    if (encoding != "base64") {
		return Error::InvalidURI;
    }

    auto encodedData = path.substr(encodingEnd + 1);
    if (config.mapCallback != nullptr) {
        // If a map callback is specified, we use a pointer to memory specified by it.
        auto padding = base64::getPadding(encodedData);
        auto size = base64::getOutputSize(encodedData.size(), padding);
        auto info = config.mapCallback(size, config.userPointer);
        if (info.mappedMemory != nullptr) {
            if (config.decodeCallback != nullptr) {
                config.decodeCallback(encodedData, reinterpret_cast<std::uint8_t*>(info.mappedMemory), padding, size, config.userPointer);
            } else {
                base64::decode_inplace(encodedData, reinterpret_cast<std::uint8_t*>(info.mappedMemory), padding);
            }

            if (config.unmapCallback != nullptr) {
                config.unmapCallback(&info, config.userPointer);
            }

            sources::CustomBuffer source = {};
            source.id = info.customId;
            source.mimeType = getMimeTypeFromString(mime);
			return { source };
        }
    }

	// Decode the base64 data into a traditional vector
	auto padding = base64::getPadding(encodedData);
	fg::StaticVector<std::byte> uriData(base64::getOutputSize(encodedData.size(), padding));
	if (config.decodeCallback != nullptr) {
		config.decodeCallback(encodedData, reinterpret_cast<std::uint8_t*>(uriData.data()), padding, uriData.size(), config.userPointer);
	} else {
		base64::decode_inplace(encodedData, reinterpret_cast<std::uint8_t*>(uriData.data()), padding);
	}

	sources::Array source {
		std::move(uriData),
		getMimeTypeFromString(mime),
	};
	return { std::move(source) };
}

void fg::Parser::fillCategories(Category& inputCategories) noexcept {
    if (inputCategories == Category::All)
        return;

    // The Category enum used to already OR values together so that e.g. Scenes would also implicitly
    // have the Nodes bit set. This, however, caused some issues within the parse function as it tries
    // to bail out when all requested categories have been parsed, as now something that hasn't been
    // parsed could still be set. So, this has to exist...
    if (hasBit(inputCategories, Category::Scenes))
        inputCategories |= Category::Nodes;
    if (hasBit(inputCategories, Category::Nodes))
        inputCategories |= Category::Cameras | Category::Meshes | Category::Skins;
    if (hasBit(inputCategories, Category::Skins))
        // Skins needs nodes, nodes needs skins. To counter this circular dep we just redefine what we just wrote above.
        inputCategories |= Category::Accessors | (Category::Nodes | Category::Cameras | Category::Meshes | Category::Skins);
    if (hasBit(inputCategories, Category::Meshes))
        inputCategories |= Category::Accessors | Category::Materials;
    if (hasBit(inputCategories, Category::Materials))
        inputCategories |= Category::Textures;
    if (hasBit(inputCategories, Category::Animations))
        inputCategories |= Category::Accessors;
    if (hasBit(inputCategories, Category::Textures))
        inputCategories |= Category::Images | Category::Samplers;
    if (hasBit(inputCategories, Category::Images) || hasBit(inputCategories, Category::Accessors))
        inputCategories |= Category::BufferViews;
    if (hasBit(inputCategories, Category::BufferViews))
        inputCategories |= Category::Buffers;
}

fg::MimeType fg::Parser::getMimeTypeFromString(std::string_view mime) {
    const auto hash = crcStringFunction(mime);
    switch (hash) {
        case force_consteval<crc32c(mimeTypeJpeg)>: {
            return MimeType::JPEG;
        }
        case force_consteval<crc32c(mimeTypePng)>: {
            return MimeType::PNG;
        }
        case force_consteval<crc32c(mimeTypeKtx)>: {
            return MimeType::KTX2;
        }
        case force_consteval<crc32c(mimeTypeDds)>: {
            return MimeType::DDS;
        }
        case force_consteval<crc32c(mimeTypeGltfBuffer)>: {
            return MimeType::GltfBuffer;
        }
        case force_consteval<crc32c(mimeTypeOctetStream)>: {
            return MimeType::OctetStream;
        }
        default: {
            return MimeType::None;
        }
    }
}

template <typename T> fg::Error fg::Parser::parseAttributes(simdjson::dom::object& object, T& attributes) {
	using namespace simdjson;

	// We iterate through the JSON object and write each key/pair value into the
	// attribute map. The keys are only validated in the validate() method.
	attributes = FASTGLTF_CONSTRUCT_PMR_RESOURCE(std::remove_reference_t<decltype(attributes)>, resourceAllocator.get(), 0);
	attributes.reserve(object.size());
	for (const auto& field : object) {
		const auto key = field.key;

		std::uint64_t accessorIndex;
		if (field.value.get_uint64().get(accessorIndex) != SUCCESS) FASTGLTF_UNLIKELY {
			return Error::InvalidGltf;
		}
		attributes.emplace_back(Attribute {
			FASTGLTF_CONSTRUCT_PMR_RESOURCE(FASTGLTF_STD_PMR_NS::string, resourceAllocator.get(), key),
			static_cast<std::size_t>(accessorIndex),
		});
	}
	return Error::None;
}

// TODO: Is there some nicer way of declaring a templated version parseAttributes?
//       Currently, this exists because resourceAllocator is a optional field of Parser, which we can't unconditionally
//       pass as a parameter to a function, so parseAttributes needs to be a member function of Parser.
template fg::Error fg::Parser::parseAttributes(simdjson::dom::object&, FASTGLTF_STD_PMR_NS::vector<Attribute>&);
template fg::Error fg::Parser::parseAttributes(simdjson::dom::object&, decltype(fastgltf::Primitive::attributes)&);

namespace fastgltf {
	template<typename T>
	void writeIndices(PrimitiveType type, span<T> indices, std::size_t primitiveCount) {
		// Generate the correct indices for every primitive topology
		switch (type) {
			case PrimitiveType::Points: {
				for (std::size_t i = 0; i < primitiveCount; ++i)
					indices[i] = static_cast<T>(i);
				break;
			}
			case PrimitiveType::Lines:
			case PrimitiveType::LineLoop:
			case PrimitiveType::LineStrip: {
				for (std::size_t i = 0; i < primitiveCount; ++i) {
					indices[i * 2 + 0] = static_cast<T>(i * 2 + 0);
					indices[i * 2 + 1] = static_cast<T>(i * 2 + 1);
				}
				break;
			}
			case PrimitiveType::Triangles:
			case PrimitiveType::TriangleStrip:
			case PrimitiveType::TriangleFan: {
				for (std::size_t i = 0; i < primitiveCount; ++i) {
					indices[i * 3 + 0] = static_cast<T>(i * 3 + 0);
					indices[i * 3 + 1] = static_cast<T>(i * 3 + 1);
					indices[i * 3 + 2] = static_cast<T>(i * 3 + 2);
				}
				break;
			}
			default: FASTGLTF_UNREACHABLE
		}
	}

	std::pair<StaticVector<std::byte>, ComponentType> writeIndices(PrimitiveType type, std::size_t indexCount, std::size_t primitiveCount) {
		if (indexCount < 255) {
			StaticVector<std::byte> generatedIndices(indexCount * sizeof(std::uint8_t));
			span<std::uint8_t> indices(reinterpret_cast<std::uint8_t*>(generatedIndices.data()), generatedIndices.size() / sizeof(std::uint8_t));
			writeIndices(type, indices, primitiveCount);
			return std::make_pair(generatedIndices, ComponentType::UnsignedByte);
		} else if (indexCount < 65535) {
			StaticVector<std::byte> generatedIndices(indexCount * sizeof(std::uint16_t));
			span<std::uint16_t> indices(reinterpret_cast<std::uint16_t*>(generatedIndices.data()), generatedIndices.size() / sizeof(std::uint16_t));
			writeIndices(type, indices, primitiveCount);
			return std::make_pair(generatedIndices, ComponentType::UnsignedShort);
		} else {
			StaticVector<std::byte> generatedIndices(indexCount * sizeof(std::uint32_t));
			span<std::uint32_t> indices(reinterpret_cast<std::uint32_t*>(generatedIndices.data()), generatedIndices.size() / sizeof(std::uint32_t));
			writeIndices(type, indices, primitiveCount);
			return std::make_pair(generatedIndices, ComponentType::UnsignedInt);
		}
	}
}

fg::Error fg::Parser::generateMeshIndices(fastgltf::Asset& asset) const {
	for (auto& mesh : asset.meshes) {
		for (auto& primitive : mesh.primitives) {
			if (primitive.indicesAccessor.has_value())
				continue;

			auto* positionAttribute = primitive.findAttribute("POSITION");
			if (positionAttribute == primitive.attributes.end()) {
				return Error::InvalidGltf;
			}
			auto positionCount = asset.accessors[positionAttribute->accessorIndex].count;

			auto primitiveCount = [&]() -> std::size_t {
				switch (primitive.type) {
					case PrimitiveType::Points: return positionCount;
					case PrimitiveType::Lines: return positionCount / 2;
					case PrimitiveType::LineLoop:
					case PrimitiveType::LineStrip: return max<std::size_t>(0, positionCount - 1);
					case PrimitiveType::Triangles: return positionCount / 3;
					case PrimitiveType::TriangleStrip: return max<std::size_t>(0U, positionCount - 2);
					case PrimitiveType::TriangleFan: return max<std::size_t>(0U, positionCount - 2);
					default: FASTGLTF_UNREACHABLE
				}
			}();
			auto indexCount = [&]() -> std::size_t {
				switch (primitive.type) {
					case PrimitiveType::Points: return primitiveCount;
					case PrimitiveType::Lines:
					case PrimitiveType::LineLoop:
					case PrimitiveType::LineStrip: return primitiveCount * 2;
					case PrimitiveType::Triangles:
					case PrimitiveType::TriangleStrip:
					case PrimitiveType::TriangleFan: return primitiveCount * 3;
					default: FASTGLTF_UNREACHABLE
				}
			}();

			auto [generatedIndices, componentType] = writeIndices(primitive.type, indexCount, primitiveCount);

			auto bufferIdx = asset.buffers.size();
			auto& buffer = asset.buffers.emplace_back();
			buffer.byteLength = generatedIndices.size_bytes();
			sources::Array indicesArray {
				std::move(generatedIndices),
				MimeType::GltfBuffer,
			};
			buffer.data = std::move(indicesArray);

			auto bufferViewIdx = asset.bufferViews.size();
			auto& bufferView = asset.bufferViews.emplace_back();
			bufferView.byteLength = buffer.byteLength;
			bufferView.bufferIndex = bufferIdx;
			bufferView.byteOffset = 0;

			primitive.indicesAccessor = asset.accessors.size();
			auto& accessor = asset.accessors.emplace_back();
			accessor.byteOffset = 0;
			accessor.count = positionCount;
			accessor.type = AccessorType::Scalar;
			accessor.componentType = componentType;
			accessor.normalized = false;
			accessor.bufferViewIndex = bufferViewIdx;
		}
	}
	return Error::None;
}

fg::Error fg::validate(const fastgltf::Asset& asset) {
	auto isExtensionUsed = [&used = asset.extensionsUsed](std::string_view extension) {
		for (const auto& extensionUsed : used) {
			if (extension == extensionUsed) {
				return true;
			}
		}
		return false;
	};

	// From the spec: extensionsRequired is a subset of extensionsUsed. All values in extensionsRequired MUST also exist in extensionsUsed.
	if (asset.extensionsRequired.size() > asset.extensionsUsed.size()) {
		return Error::InvalidGltf;
	}
	for (const auto& required : asset.extensionsRequired) {
		bool found = false;
		for (const auto& used : asset.extensionsUsed) {
			if (required == used)
				found = true;
		}
		if (!found)
			return Error::InvalidGltf;
	}

	for (const auto& accessor : asset.accessors) {
		if (accessor.type == AccessorType::Invalid)
			return Error::InvalidGltf;
		if (accessor.componentType == ComponentType::Invalid)
			return Error::InvalidGltf;
		if (accessor.count < 1)
			return Error::InvalidGltf;
		if (accessor.bufferViewIndex.has_value() &&
		    accessor.bufferViewIndex.value() >= asset.bufferViews.size())
			return Error::InvalidGltf;
		if (accessor.byteOffset != 0) {
			// The offset of an accessor into a bufferView (i.e., accessor.byteOffset)
			// and the offset of an accessor into a buffer (i.e., accessor.byteOffset + bufferView.byteOffset)
			// MUST be a multiple of the size of the accessor’s component type.
			auto componentByteSize = getComponentByteSize(accessor.componentType);
			if (accessor.byteOffset % componentByteSize != 0)
				return Error::InvalidGltf;

			if (accessor.bufferViewIndex.has_value()) {
				const auto& bufferView = asset.bufferViews[accessor.bufferViewIndex.value()];
				if ((accessor.byteOffset + bufferView.byteOffset) % componentByteSize != 0)
					return Error::InvalidGltf;

				// When byteStride is defined, it MUST be a multiple of the size of the accessor’s component type.
				if (bufferView.byteStride.has_value() && bufferView.byteStride.value() % componentByteSize != 0)
					return Error::InvalidGltf;
			}
		}

		if (!std::holds_alternative<std::monostate>(accessor.max)) {
			if ((accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double)
			    && !std::holds_alternative<FASTGLTF_STD_PMR_NS::vector<double>>(accessor.max))
				return Error::InvalidGltf;
		}
		if (!std::holds_alternative<std::monostate>(accessor.min)) {
			if ((accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double)
			    && !std::holds_alternative<FASTGLTF_STD_PMR_NS::vector<double>>(accessor.min))
				return Error::InvalidGltf;
		}

		if (accessor.sparse) {
			const auto& indicesView = asset.bufferViews[accessor.sparse->indicesBufferView];
			if (indicesView.byteStride || indicesView.target)
				return Error::InvalidGltf;

			const auto& valueView = asset.bufferViews[accessor.sparse->valuesBufferView];
			if (valueView.byteStride || valueView.target)
				return Error::InvalidGltf;
		}
	}

	for (const auto& animation : asset.animations) {
		if (animation.channels.empty())
			return Error::InvalidGltf;
		for (const auto& channel1 : animation.channels) {
			for (const auto& channel2 : animation.channels) {
				if (&channel1 == &channel2)
					continue;
				if (channel1.nodeIndex == channel2.nodeIndex && channel1.path == channel2.path)
					return Error::InvalidGltf;
			}
		}

		if (animation.samplers.empty())
			return Error::InvalidGltf;
		for (const auto& channel : animation.channels) {
			const auto& sampler = animation.samplers[channel.samplerIndex];

			const auto& inputAccessor = asset.accessors[sampler.inputAccessor];
			// The accessor MUST be of scalar type with floating-point components
			if (inputAccessor.type != AccessorType::Scalar)
				return Error::InvalidGltf;
			if (inputAccessor.componentType != ComponentType::Float && inputAccessor.componentType != ComponentType::Double)
				return Error::InvalidGltf;
			if (inputAccessor.bufferViewIndex && asset.bufferViews[*inputAccessor.bufferViewIndex].meshoptCompression)
				continue;

			if (inputAccessor.count == 0)
				continue;

			if (channel.path == AnimationPath::Weights)
				continue; // TODO: For weights, the input count needs to be multiplied by the morph target count.

			const auto& outputAccessor = asset.accessors[sampler.outputAccessor];
			if (outputAccessor.bufferViewIndex && asset.bufferViews[*outputAccessor.bufferViewIndex].meshoptCompression)
				continue;

			switch (sampler.interpolation) {
				case AnimationInterpolation::Linear:
				case AnimationInterpolation::Step:
					if (inputAccessor.count != outputAccessor.count)
						return Error::InvalidGltf;
					break;
				case AnimationInterpolation::CubicSpline:
					if (inputAccessor.count < 2)
						return Error::InvalidGltf;
					if (inputAccessor.count * 3 != outputAccessor.count)
						return Error::InvalidGltf;
					break;
			}
		}
	}

	for (const auto& buffer : asset.buffers) {
		if (buffer.byteLength < 1)
			return Error::InvalidGltf;
	}

	for (const auto& bufferView : asset.bufferViews) {
		if (bufferView.byteLength < 1)
			return Error::InvalidGltf;
		if (bufferView.byteStride.has_value() && (*bufferView.byteStride < 4U || *bufferView.byteStride > 252U || *bufferView.byteStride % 4 != 0))
			return Error::InvalidGltf;
		if (bufferView.bufferIndex >= asset.buffers.size())
			return Error::InvalidGltf;

		if (bufferView.meshoptCompression != nullptr && !isExtensionUsed(extensions::EXT_meshopt_compression))
			return Error::InvalidGltf;

		if (bufferView.meshoptCompression) {
			const auto& compression = bufferView.meshoptCompression;
			switch (compression->mode) {
				case MeshoptCompressionMode::Attributes:
					if (compression->byteStride % 4 != 0 || compression->byteStride > 256)
						return Error::InvalidGltf;
					break;
				case MeshoptCompressionMode::Triangles:
					if (compression->count % 3 != 0)
						return Error::InvalidGltf;
					[[fallthrough]];
				case MeshoptCompressionMode::Indices:
					if (compression->byteStride != 2 && compression->byteStride != 4)
						return Error::InvalidGltf;
					break;
			}
		}
	}

	for (const auto& camera : asset.cameras) {
		if (const auto* pOrthographic = std::get_if<Camera::Orthographic>(&camera.camera)) {
			if (pOrthographic->zfar == 0)
				return Error::InvalidGltf;
		} else if (const auto* pPerspective = std::get_if<Camera::Perspective>(&camera.camera)) {
			if (pPerspective->aspectRatio.has_value() && pPerspective->aspectRatio == .0f)
				return Error::InvalidGltf;
			if (pPerspective->yfov == 0)
				return Error::InvalidGltf;
			if (pPerspective->zfar.has_value() && pPerspective->zfar == .0f)
				return Error::InvalidGltf;
			if (pPerspective->znear == 0.0F)
				return Error::InvalidGltf;
		}
	}

	for (const auto& image : asset.images) {
		if (const auto* view = std::get_if<sources::BufferView>(&image.data); view != nullptr) {
			if (view->bufferViewIndex >= asset.bufferViews.size()) {
				return Error::InvalidGltf;
			}
		}
	}

	for (const auto& light : asset.lights) {
		if (light.type == LightType::Directional && light.range.has_value())
			return Error::InvalidGltf;
		if (light.range.has_value() && light.range.value() <= 0)
			return Error::InvalidGltf;

		if (light.type != LightType::Spot) {
			if (light.innerConeAngle.has_value() || light.outerConeAngle.has_value()) {
				return Error::InvalidGltf;
			}
		} else {
			if (!light.innerConeAngle.has_value() || !light.outerConeAngle.has_value())
				return Error::InvalidGltf;
			if (light.innerConeAngle.value() < 0)
				return Error::InvalidGltf;
			if (light.innerConeAngle.value() > light.outerConeAngle.value())
				return Error::InvalidGltf;
			if (light.outerConeAngle.value() > math::pi / 2)
				return Error::InvalidGltf;
		}
	}

	for (const auto& material : asset.materials) {
		auto isInvalidTexture = [&textures = asset.textures](std::optional<std::size_t> textureIndex) {
			return textureIndex.has_value() && textureIndex.value() >= textures.size();
		};
		if (material.normalTexture.has_value() && isInvalidTexture(material.normalTexture->textureIndex))
			return Error::InvalidGltf;
		if (material.emissiveTexture.has_value() && isInvalidTexture(material.emissiveTexture->textureIndex))
			return Error::InvalidGltf;
		if (material.occlusionTexture.has_value() && isInvalidTexture(material.occlusionTexture->textureIndex))
			return Error::InvalidGltf;
		if (material.pbrData.baseColorTexture.has_value() &&
		    isInvalidTexture(material.pbrData.baseColorTexture->textureIndex))
			return Error::InvalidGltf;
		if (material.pbrData.metallicRoughnessTexture.has_value() &&
		    isInvalidTexture(material.pbrData.metallicRoughnessTexture->textureIndex))
			return Error::InvalidGltf;

		// Validate that for every additional material field from an extension the correct extension is marked as used by the asset.
		if (material.anisotropy && !isExtensionUsed(extensions::KHR_materials_anisotropy))
			return Error::InvalidGltf;
		if (material.clearcoat && !isExtensionUsed(extensions::KHR_materials_clearcoat))
			return Error::InvalidGltf;
		if (material.iridescence && !isExtensionUsed(extensions::KHR_materials_iridescence))
			return Error::InvalidGltf;
		if (material.sheen && !isExtensionUsed(extensions::KHR_materials_sheen))
			return Error::InvalidGltf;
		if (material.specular && !isExtensionUsed(extensions::KHR_materials_specular))
			return Error::InvalidGltf;
#if FASTGLTF_ENABLE_DEPRECATED_EXT
		if (material.specularGlossiness && !isExtensionUsed(extensions::KHR_materials_pbrSpecularGlossiness))
			return Error::InvalidGltf;
#endif
		if (material.transmission && !isExtensionUsed(extensions::KHR_materials_transmission))
			return Error::InvalidGltf;
		if (material.volume && !isExtensionUsed(extensions::KHR_materials_volume))
			return Error::InvalidGltf;
		if (material.emissiveStrength != 1.0f && !isExtensionUsed(extensions::KHR_materials_emissive_strength))
			return Error::InvalidGltf;
		if (material.ior != 1.5f && !isExtensionUsed(extensions::KHR_materials_ior))
			return Error::InvalidGltf;
		if (material.packedNormalMetallicRoughnessTexture && !isExtensionUsed(extensions::MSFT_packing_normalRoughnessMetallic))
			return Error::InvalidGltf;
		if (material.packedOcclusionRoughnessMetallicTextures && !isExtensionUsed(extensions::MSFT_packing_occlusionRoughnessMetallic))
			return Error::InvalidGltf;
	}

	for (const auto& mesh : asset.meshes) {
		for (const auto& primitive : mesh.primitives) {
			if (primitive.materialIndex.has_value() && *primitive.materialIndex >= asset.materials.size())
				return Error::InvalidGltf;

			if (!primitive.mappings.empty()) {
				if (!isExtensionUsed(fastgltf::extensions::KHR_materials_variants))
					return Error::InvalidGltf;
				if (primitive.mappings.size() != asset.materialVariants.size())
					return Error::InvalidGltf;
				for (const auto& mapping : primitive.mappings) {
					if (!mapping.has_value())
						continue;
					if (mapping.value() >= asset.materials.size())
						return Error::InvalidGltf;
				}
			}

			if (primitive.indicesAccessor.has_value()) {
				if (*primitive.indicesAccessor >= asset.accessors.size())
					return Error::InvalidGltf;
				const auto& accessor = asset.accessors[*primitive.indicesAccessor];
				if (accessor.bufferViewIndex.has_value()) {
					const auto& bufferView = asset.bufferViews[*accessor.bufferViewIndex];
					// The byteStride property must not be set on anything but vertex attributes.
					if (bufferView.byteStride.has_value())
						return Error::InvalidGltf;
				}
			}

			for (const auto& [name, index] : primitive.attributes) {
				if (asset.accessors.size() <= index)
					return Error::InvalidGltf;

				// The spec provides a list of attributes that it accepts and mentions that all
				// custom attributes have to start with an underscore. We'll enforce this.
				if (!startsWith(name, "_")) {
					if (name != "POSITION" && name != "NORMAL" && name != "TANGENT" &&
					    !startsWith(name, "TEXCOORD_") && !startsWith(name, "COLOR_") &&
					    !startsWith(name, "JOINTS_") && !startsWith(name, "WEIGHTS_")) {
						return Error::InvalidGltf;
					}
				}

				// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview
				const auto& accessor = asset.accessors[index];
				if (name == "POSITION") {
					// Animation input and vertex position attribute accessors MUST have accessor.min and accessor.max defined.
					if (std::holds_alternative<std::monostate>(accessor.max) || std::holds_alternative<std::monostate>(accessor.min))
						return Error::InvalidGltf;
					if (accessor.type != AccessorType::Vec3)
						return Error::InvalidGltf;
					if (!isExtensionUsed(extensions::KHR_mesh_quantization)) {
						if (accessor.componentType != ComponentType::Float)
							return Error::InvalidGltf;
					} else {
						if (accessor.componentType == ComponentType::Double || accessor.componentType == ComponentType::UnsignedInt)
							return Error::InvalidGltf;
					}
				} else if (name == "NORMAL") {
					if (accessor.type != AccessorType::Vec3)
						return Error::InvalidGltf;
					if (!isExtensionUsed(extensions::KHR_mesh_quantization)) {
						if (accessor.componentType != ComponentType::Float)
							return Error::InvalidGltf;
					} else {
						if (accessor.componentType != ComponentType::Float &&
						    accessor.componentType != ComponentType::Short &&
						    accessor.componentType != ComponentType::Byte)
							return Error::InvalidGltf;
					}
				} else if (name == "TANGENT") {
					if (accessor.type != AccessorType::Vec4)
						return Error::InvalidGltf;
					if (!isExtensionUsed(extensions::KHR_mesh_quantization)) {
						if (accessor.componentType != ComponentType::Float)
							return Error::InvalidGltf;
					} else {
						if (accessor.componentType != ComponentType::Float &&
						    accessor.componentType != ComponentType::Short &&
						    accessor.componentType != ComponentType::Byte)
							return Error::InvalidGltf;
					}
				} else if (startsWith(name, "TEXCOORD_")) {
					if (accessor.type != AccessorType::Vec2)
						return Error::InvalidGltf;
					if (!isExtensionUsed(extensions::KHR_mesh_quantization)) {
						if (accessor.componentType != ComponentType::Float &&
						    accessor.componentType != ComponentType::UnsignedByte &&
						    accessor.componentType != ComponentType::UnsignedShort) {
							return Error::InvalidGltf;
						}
					} else {
						if (accessor.componentType == ComponentType::Double ||
						    accessor.componentType == ComponentType::UnsignedInt) {
							return Error::InvalidGltf;
						}
					}
				} else if (startsWith(name, "COLOR_")) {
					if (accessor.type != AccessorType::Vec3 && accessor.type != AccessorType::Vec4)
						return Error::InvalidGltf;
					if (accessor.componentType != ComponentType::Float &&
					    accessor.componentType != ComponentType::UnsignedByte &&
					    accessor.componentType != ComponentType::UnsignedShort) {
						return Error::InvalidGltf;
					}
				} else if (startsWith(name, "JOINTS_")) {
					if (accessor.type != AccessorType::Vec4)
						return Error::InvalidGltf;
					if (accessor.componentType != ComponentType::UnsignedByte &&
					    accessor.componentType != ComponentType::UnsignedShort) {
						return Error::InvalidGltf;
					}
				} else if (startsWith(name, "WEIGHTS_")) {
					if (accessor.type != AccessorType::Vec4)
						return Error::InvalidGltf;
					if (accessor.componentType != ComponentType::Float &&
					    accessor.componentType != ComponentType::UnsignedByte &&
					    accessor.componentType != ComponentType::UnsignedShort) {
						return Error::InvalidGltf;
					}
				} else if (startsWith(name, "_")) {
					// Application-specific attribute semantics MUST start with an underscore, e.g., _TEMPERATURE.
					// Application-specific attribute semantics MUST NOT use unsigned int component type.
					if (accessor.componentType == ComponentType::UnsignedInt) {
						return Error::InvalidGltf;
					}
				}
			}
		}
	}

	for (const auto& node : asset.nodes) {
		if (node.cameraIndex.has_value() && asset.cameras.size() <= node.cameraIndex.value())
			return Error::InvalidGltf;
		if (node.skinIndex.has_value() && asset.skins.size() <= node.skinIndex.value())
			return Error::InvalidGltf;
		if (node.meshIndex.has_value() && asset.meshes.size() <= node.meshIndex.value())
			return Error::InvalidGltf;

		if (const auto* pTRS = std::get_if<TRS>(&node.transform)) {
			if (pTRS->rotation.x() > 1.0 || pTRS->rotation.x() < -1.0)
				return Error::InvalidGltf;
			if (pTRS->rotation.y() > 1.0 || pTRS->rotation.y() < -1.0)
				return Error::InvalidGltf;
			if (pTRS->rotation.z() > 1.0 || pTRS->rotation.z() < -1.0)
				return Error::InvalidGltf;
			if (pTRS->rotation.w() > 1.0 || pTRS->rotation.w() < -1.0)
				return Error::InvalidGltf;
		}

		if ((node.skinIndex.has_value() || !node.weights.empty()) && !node.meshIndex.has_value()) {
			return Error::InvalidGltf;
		}

		if (node.skinIndex.has_value()) {
			// "When the node contains skin, all mesh.primitives MUST contain JOINTS_0 and WEIGHTS_0 attributes."
			const auto& mesh = asset.meshes[node.meshIndex.value()];
			for (const auto& primitive : mesh.primitives) {
				const auto* joints0 = primitive.findAttribute("JOINTS_0");
				const auto* weights0 = primitive.findAttribute("WEIGHTS_0");
				if (joints0 == primitive.attributes.end() || weights0 == primitive.attributes.end())
					return Error::InvalidGltf;
			}
		}
	}

	for (const auto& sampler : asset.samplers) {
		if (sampler.magFilter.has_value() && (sampler.magFilter != Filter::Nearest && sampler.magFilter != Filter::Linear)) {
			return Error::InvalidGltf;
		}
	}

	for (const auto& scene : asset.scenes) {
		for (const auto& node : scene.nodeIndices) {
			if (node >= asset.nodes.size())
				return Error::InvalidGltf;
		}
	}

	for (const auto& skin : asset.skins) {
		if (skin.joints.empty())
			return Error::InvalidGltf;
		if (skin.skeleton.has_value() && skin.skeleton.value() >= asset.nodes.size())
			return Error::InvalidGltf;
		if (skin.inverseBindMatrices.has_value() && skin.inverseBindMatrices.value() >= asset.accessors.size())
			return Error::InvalidGltf;
	}

	for (const auto& texture : asset.textures) {
		if (texture.samplerIndex.has_value() && texture.samplerIndex.value() >= asset.samplers.size())
			return Error::InvalidGltf;
		// imageIndex needs to be defined, unless one of the texture extensions were enabled and define another image index.
		if (isExtensionUsed(extensions::KHR_texture_basisu) || isExtensionUsed(extensions::MSFT_texture_dds) || isExtensionUsed(extensions::EXT_texture_webp)) {
			if (!texture.imageIndex.has_value() && (!texture.basisuImageIndex.has_value() && !texture.ddsImageIndex.has_value() && !texture.webpImageIndex.has_value())) {
				return Error::InvalidGltf;
			}
		} else if (!texture.imageIndex.has_value()) {
			return Error::InvalidGltf;
		}
		if (texture.imageIndex.has_value() && texture.imageIndex.value() >= asset.images.size())
			return Error::InvalidGltf;
		if (texture.basisuImageIndex.has_value() && texture.basisuImageIndex.value() >= asset.images.size())
			return Error::InvalidGltf;
		if (texture.ddsImageIndex.has_value() && texture.ddsImageIndex.value() >= asset.images.size())
			return Error::InvalidGltf;
		if (texture.webpImageIndex.has_value() && texture.webpImageIndex.value() >= asset.images.size())
			return Error::InvalidGltf;
	}

	return Error::None;
}

fg::Expected<fg::Asset> fg::Parser::parse(simdjson::dom::object root, Category categories) {
	using namespace simdjson;
	fillCategories(categories);

	Asset asset {};

#if !FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL
	// Create a new chunk memory resource for each asset we parse.
	asset.memoryResource = resourceAllocator = std::make_shared<ChunkMemoryResource>();
#endif

	if (!hasBit(options, Options::DontRequireValidAssetMember)) {
		dom::object assetInfo;
		AssetInfo info = {};
		auto error = root["asset"].get_object().get(assetInfo);
		if (error == NO_SUCH_FIELD) {
			return Error::InvalidOrMissingAssetField;
		}
		if (error != SUCCESS) FASTGLTF_UNLIKELY {
			return Error::InvalidJson;
		}

		std::string_view version;
		if (assetInfo["version"].get_string().get(version) != SUCCESS) FASTGLTF_UNLIKELY {
			return Error::InvalidOrMissingAssetField;
		}

		const auto major = static_cast<std::uint32_t>(version.substr(0, 1)[0] - '0');
		// std::uint32_t minor = version.substr(2, 3)[0] - '0';
		if (major != 2) {
			return Error::UnsupportedVersion;
		}
		info.gltfVersion = std::string { version };

		std::string_view copyright;
		if (assetInfo["copyright"].get_string().get(copyright) == SUCCESS) FASTGLTF_LIKELY {
			info.copyright = std::string { copyright };
		}

		std::string_view generator;
		if (assetInfo["generator"].get_string().get(generator) == SUCCESS) FASTGLTF_LIKELY {
			info.generator = std::string { generator };
		}

		asset.assetInfo = std::move(info);
	}

	dom::array extensionsRequired;
	if (root["extensionsRequired"].get_array().get(extensionsRequired) == SUCCESS) FASTGLTF_LIKELY {
		for (auto extension : extensionsRequired) {
			std::string_view string;
			if (extension.get_string().get(string) != SUCCESS) FASTGLTF_UNLIKELY {
				return Error::InvalidGltf;
			}

			bool known = false;
			for (const auto& [extensionString, extensionEnum] : extensionStrings) {
				if (extensionString == string) {
					known = true;
					if (!hasBit(config.extensions, extensionEnum)) {
						// The extension is required, but not enabled by the user.
						return Error::MissingExtensions;
					}
					break;
				}
			}
			if (!known) {
				return Error::UnknownRequiredExtension;
			}

			FASTGLTF_STD_PMR_NS::string FASTGLTF_CONSTRUCT_PMR_RESOURCE(requiredExtension, resourceAllocator.get(), string);
			asset.extensionsRequired.emplace_back(std::move(requiredExtension));
		}
	}

	Category readCategories = Category::None;
	for (const auto& object : root) {
		auto hashedKey = crcStringFunction(object.key);
		if (hashedKey == force_consteval<crc32c("scene")>) {
			std::uint64_t defaultScene;
			if (object.value.get_uint64().get(defaultScene) != SUCCESS) FASTGLTF_UNLIKELY {
				return Error::InvalidGltf;
			}
			asset.defaultScene = static_cast<std::size_t>(defaultScene);
			continue;
		}

		if (hashedKey == force_consteval<crc32c("extensions")>) {
			dom::object extensionsObject;
			if (object.value.get_object().get(extensionsObject) != SUCCESS) FASTGLTF_UNLIKELY {
				return Error::InvalidGltf;
			}

			if (auto error = parseExtensions(extensionsObject, asset); error != Error::None)
				return error;
			continue;
		}

		if (hashedKey == force_consteval<crc32c("asset")> || hashedKey == force_consteval<crc32c("extras")>) {
			continue;
		}

		dom::array array;
		if (object.value.get_array().get(array) != SUCCESS) FASTGLTF_UNLIKELY {
			return Error::InvalidGltf;
		}

#define KEY_SWITCH_CASE(name, id) case force_consteval<crc32c(FASTGLTF_QUOTE(id))>:       \
                if (hasBit(categories, Category::name))   \
                    error = parse##name(array, asset);                     \
                readCategories |= Category::name;         \
                break;

		Error error = Error::None;
		switch (hashedKey) {
			KEY_SWITCH_CASE(Accessors, accessors)
			KEY_SWITCH_CASE(Animations, animations)
			KEY_SWITCH_CASE(Buffers, buffers)
			KEY_SWITCH_CASE(BufferViews, bufferViews)
			KEY_SWITCH_CASE(Cameras, cameras)
			KEY_SWITCH_CASE(Images, images)
			KEY_SWITCH_CASE(Materials, materials)
			KEY_SWITCH_CASE(Meshes, meshes)
			KEY_SWITCH_CASE(Nodes, nodes)
			KEY_SWITCH_CASE(Samplers, samplers)
			KEY_SWITCH_CASE(Scenes, scenes)
			KEY_SWITCH_CASE(Skins, skins)
			KEY_SWITCH_CASE(Textures, textures)
			case force_consteval<crc32c("extensionsUsed")>: {
				for (auto usedValue : array) {
					std::string_view usedString;
					if (auto eError = usedValue.get_string().get(usedString); eError == SUCCESS) FASTGLTF_LIKELY {
						FASTGLTF_STD_PMR_NS::string FASTGLTF_CONSTRUCT_PMR_RESOURCE(string, resourceAllocator.get(), usedString);
						asset.extensionsUsed.emplace_back(std::move(string));
					} else {
						error = Error::InvalidGltf;
					}
				}
				break;
			}
			case force_consteval<crc32c("extensionsRequired")>: {
				// These are already parsed before this section.
				break;
			}
			default:
				break;
		}

		if (error != Error::None)
			return error;

#undef KEY_SWITCH_CASE
	}

	asset.availableCategories = readCategories;

	if (hasBit(options, Options::GenerateMeshIndices)) {
		if (auto error = generateMeshIndices(asset); error != Error::None) {
			return error;
		}
	}

	// Resize primitive mappings to match the global variant count
	if (hasBit(config.extensions, Extensions::KHR_materials_variants) && !asset.materialVariants.empty()) {
		const auto variantCount = asset.materialVariants.size();
		for (auto& mesh : asset.meshes) {
			for (auto& primitive : mesh.primitives) {
				if (primitive.mappings.empty() || primitive.mappings.size() == variantCount)
					continue;
				primitive.mappings.resize(variantCount);
			}
		}
	}

	return std::move(asset);
}

fg::Error fg::Parser::parseAccessors(simdjson::dom::array& accessors, Asset& asset) {
    using namespace simdjson;

	asset.accessors.reserve(accessors.size());
    for (auto accessorValue : accessors) {
        // Required fields: "componentType", "count"
        Accessor accessor = {};
        dom::object accessorObject;
        if (accessorValue.get_object().get(accessorObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::uint64_t componentType;
        if (accessorObject["componentType"].get_uint64().get(componentType) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
		accessor.componentType = getComponentType(static_cast<std::underlying_type_t<ComponentType>>(componentType));
        if (accessor.componentType == ComponentType::Double && (!hasBit(options, Options::AllowDouble) || !hasBit(config.extensions, Extensions::KHR_accessor_float64))) {
            return Error::InvalidGltf;
        }

        std::string_view accessorType;
        if (accessorObject["type"].get_string().get(accessorType) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
		accessor.type = getAccessorType(accessorType);

        std::uint64_t accessorCount;
        if (accessorObject["count"].get_uint64().get(accessorCount) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
		accessor.count = static_cast<std::size_t>(accessorCount);


        std::uint64_t bufferView;
        if (accessorObject["bufferView"].get_uint64().get(bufferView) == SUCCESS) FASTGLTF_LIKELY {
            accessor.bufferViewIndex = static_cast<std::size_t>(bufferView);
        }

        // byteOffset is optional, but defaults to 0
        std::uint64_t byteOffset;
        if (auto error = accessorObject["byteOffset"].get_uint64().get(byteOffset); error == SUCCESS) FASTGLTF_LIKELY {
            accessor.byteOffset = static_cast<std::size_t>(byteOffset);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        // Type of min and max should always be the same.
        auto parseMinMax = [&](std::string_view key, decltype(Accessor::max)& ref) -> fastgltf::Error {
            dom::array elements;
            if (accessorObject[key].get_array().get(elements) == SUCCESS) FASTGLTF_LIKELY {
                decltype(Accessor::max) variant;

				using double_vec = std::variant_alternative_t<1, decltype(Accessor::max)>;
				using int64_vec = std::variant_alternative_t<2, decltype(Accessor::max)>;

				auto num = getNumComponents(accessor.type);
                if (accessor.componentType == ComponentType::Float || accessor.componentType == ComponentType::Double) {
					variant = FASTGLTF_CONSTRUCT_PMR_RESOURCE(double_vec, resourceAllocator.get(), num);
                } else {
	                variant = FASTGLTF_CONSTRUCT_PMR_RESOURCE(int64_vec, resourceAllocator.get(), num);
                }

				std::size_t idx = 0;
                for (auto element : elements) {
                    auto type = element.type();
                    switch (type) {
                        case dom::element_type::DOUBLE: {
                            double value;
                            if (element.get_double().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
                                return Error::InvalidGltf;
                            }

                            if (auto* doubles = std::get_if<double_vec>(&variant); doubles != nullptr) {
                                (*doubles)[idx++] = value;
                            } else if (auto* ints = std::get_if<int64_vec>(&variant); ints != nullptr) {
                                (*ints)[idx++] = static_cast<std::int64_t>(value);
                            } else {
                                return Error::InvalidGltf;
                            }
                            break;
                        }
                        case dom::element_type::INT64: {
                            std::int64_t value;
                            if (element.get_int64().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
                                return Error::InvalidGltf;
                            }

							if (auto* doubles = std::get_if<double_vec>(&variant); doubles != nullptr) {
								(*doubles)[idx++] = static_cast<double>(value);
							} else if (auto* ints = std::get_if<int64_vec>(&variant); ints != nullptr) {
								(*ints)[idx++] = value;
							} else {
								return Error::InvalidGltf;
							}
                            break;
                        }
                        case dom::element_type::UINT64: {
                            // Note that the glTF spec doesn't care about any integer larger than 32-bits, so
                            // truncating uint64 to int64 wouldn't make any difference, as those large values
                            // aren't allowed anyway.
                            std::uint64_t value;
                            if (element.get_uint64().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
                                return Error::InvalidGltf;
                            }

							if (auto* doubles = std::get_if<double_vec>(&variant); doubles != nullptr) {
								(*doubles)[idx++] = static_cast<double>(value);
							} else if (auto* ints = std::get_if<int64_vec>(&variant); ints != nullptr) {
								(*ints)[idx++] = static_cast<std::int64_t>(value);
							} else {
								return Error::InvalidGltf;
							}
                            break;
                        }
                        default: return Error::InvalidGltf;
                    }
                }
                ref = std::move(variant);
            }
            return Error::None;
        };

        if (auto error = parseMinMax("max", accessor.max); error != Error::None) {
            return error;
        }
        if (auto error = parseMinMax("min", accessor.min); error != Error::None) {
            return error;
        }

        if (auto error = accessorObject["normalized"].get_bool().get(accessor.normalized); error != SUCCESS && error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

		// This property MUST NOT be set to true for accessors with FLOAT or UNSIGNED_INT component type.
		if (accessor.normalized && (accessor.componentType == ComponentType::UnsignedInt || accessor.componentType == ComponentType::Float)) {
			return Error::InvalidGltf;
		}

        dom::object sparseAccessorObject;
        if (accessorObject["sparse"].get_object().get(sparseAccessorObject) == SUCCESS) FASTGLTF_LIKELY {
            SparseAccessor sparse = {};
            std::uint64_t value;
            dom::object child;
            if (sparseAccessorObject["count"].get_uint64().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
            sparse.count = static_cast<std::size_t>(value);

            // Accessor Sparce Indices
            if (sparseAccessorObject["indices"].get_object().get(child) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            if (child["bufferView"].get_uint64().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
            sparse.indicesBufferView = static_cast<std::size_t>(value);

            if (auto error = child["byteOffset"].get_uint64().get(value); error == SUCCESS) FASTGLTF_LIKELY {
                sparse.indicesByteOffset = static_cast<std::size_t>(value);
            } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            if (child["componentType"].get_uint64().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
            sparse.indexComponentType = getComponentType(static_cast<std::underlying_type_t<ComponentType>>(value));

            // Accessor Sparse Values
            if (sparseAccessorObject["values"].get_object().get(child) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            if (child["bufferView"].get_uint64().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
            sparse.valuesBufferView = static_cast<std::size_t>(value);

            if (auto error = child["byteOffset"].get_uint64().get(value); error == SUCCESS) FASTGLTF_LIKELY {
                sparse.valuesByteOffset = static_cast<std::size_t>(value);
            } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            accessor.sparse = sparse;
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = accessorObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.accessors.size(), Category::Accessors, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		std::string_view name;
        if (accessorObject["name"].get_string().get(name) == SUCCESS) {
	        accessor.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(accessor.name), resourceAllocator.get(), name);
        }

	    asset.accessors.emplace_back(std::move(accessor));
    }

	return Error::None;
}

fg::Error fg::Parser::parseAnimations(simdjson::dom::array& animations, Asset& asset) {
    using namespace simdjson;

	asset.animations.reserve(animations.size());
    for (auto animationValue : animations) {
        dom::object animationObject;
        Animation animation = {};
        if (animationValue.get_object().get(animationObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        dom::array channels;
        auto channelError = getJsonArray(animationObject, "channels", &channels);
        if (channelError != Error::None) {
            return Error::InvalidGltf;
        }

	    animation.channels = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(animation.channels), resourceAllocator.get(), 0);
        animation.channels.reserve(channels.size());
        for (auto channelValue : channels) {
            dom::object channelObject;
            AnimationChannel channel = {};
            if (channelValue.get_object().get(channelObject) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            std::uint64_t sampler;
            if (channelObject["sampler"].get_uint64().get(sampler) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
            channel.samplerIndex = static_cast<std::size_t>(sampler);

            dom::object targetObject;
            if (channelObject["target"].get_object().get(targetObject) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            } else {
                std::uint64_t node;
				if (auto error = targetObject["node"].get_uint64().get(node); error == SUCCESS) FASTGLTF_LIKELY {
					channel.nodeIndex = static_cast<std::size_t>(node);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					// the glTF spec allows the node index of an animation channel to be absent, and requires
					// implementations to just ignore the animation if no extension exists to override it.
					// > When node isn't defined, channel SHOULD be ignored.
					return Error::InvalidGltf;
				}

                std::string_view path;
                if (targetObject["path"].get_string().get(path) != SUCCESS) FASTGLTF_UNLIKELY {
                    return Error::InvalidGltf;
                }

                if (path == "translation") {
                    channel.path = AnimationPath::Translation;
                } else if (path == "rotation") {
                    channel.path = AnimationPath::Rotation;
                } else if (path == "scale") {
                    channel.path = AnimationPath::Scale;
                } else if (path == "weights") {
                    channel.path = AnimationPath::Weights;
                }
            }

            animation.channels.emplace_back(channel);
        }

        dom::array samplers;
        auto samplerError = getJsonArray(animationObject, "samplers", &samplers);
        if (samplerError != Error::None) {
            return Error::InvalidGltf;
        }

	    animation.samplers = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(animation.samplers), resourceAllocator.get(), 0);
        animation.samplers.reserve(samplers.size());
        for (auto samplerValue : samplers) {
            dom::object samplerObject;
            AnimationSampler sampler = {};
            if (samplerValue.get_object().get(samplerObject) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            std::uint64_t input;
            if (samplerObject["input"].get_uint64().get(input) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
            sampler.inputAccessor = static_cast<std::size_t>(input);

            std::uint64_t output;
            if (samplerObject["output"].get_uint64().get(output) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
            sampler.outputAccessor = static_cast<std::size_t>(output);

            std::string_view interpolation;
            if (samplerObject["interpolation"].get_string().get(interpolation) != SUCCESS) FASTGLTF_UNLIKELY {
                sampler.interpolation = AnimationInterpolation::Linear;
            } else {
                if (interpolation == "LINEAR") {
                    sampler.interpolation = AnimationInterpolation::Linear;
                } else if (interpolation == "STEP") {
                    sampler.interpolation = AnimationInterpolation::Step;
                } else if (interpolation == "CUBICSPLINE") {
                    sampler.interpolation = AnimationInterpolation::CubicSpline;
                } else {
                    return Error::InvalidGltf;
                }
            }

            animation.samplers.emplace_back(sampler);
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = animationObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.animations.size(), Category::Animations, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}


		std::string_view name;
        if (animationObject["name"].get_string().get(name) == SUCCESS) {
	        animation.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(animation.name), resourceAllocator.get(), name);
        }

	    asset.animations.emplace_back(std::move(animation));
    }

	return Error::None;
}

fg::Error fg::Parser::parseBuffers(simdjson::dom::array& buffers, Asset& asset) {
    using namespace simdjson;

	asset.buffers.reserve(buffers.size());
    std::size_t bufferIndex = 0;
    for (auto bufferValue : buffers) {
        // Required fields: "byteLength"
        Buffer buffer = {};
        dom::object bufferObject;
        if (bufferValue.get_object().get(bufferObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::uint64_t byteLength;
        if (bufferObject["byteLength"].get_uint64().get(byteLength) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
		buffer.byteLength = static_cast<std::size_t>(byteLength);

		// The spec for EXT_meshopt_compression allows so-called 'fallback buffers' which only exist to
		// act as a valid fallback for compressed buffer views, but actually do not contain any data.
		// In these cases, there is either simply no URI, or a fallback boolean is added to the extensions'
		// extension field.
		// In these cases, fastgltf could just leave the std::monostate in the DataSource.
		// However, to make the actual use of these buffers clear, we'll use an empty fallback type.
		bool meshoptCompressionRequired = false;
		for (const auto& extension : asset.extensionsRequired) {
			if (extension == extensions::EXT_meshopt_compression) {
				meshoptCompressionRequired = true;
			}
		}

        // When parsing GLB, there's a buffer object that will point to the BUF chunk in the
        // file. Otherwise, data must be specified in the "uri" field.
        std::string_view uriString;
        if (bufferObject["uri"].get_string().get(uriString) == SUCCESS) FASTGLTF_LIKELY {
			URIView uriView(uriString);

            if (!uriView.valid()) {
                return Error::InvalidURI;
            }

            if (uriView.isDataUri()) {
                auto [error, source] = decodeDataUri(uriView);
                if (error != Error::None) {
                    return error;
                }

                buffer.data = std::move(source);
            } else if (uriView.isLocalPath() && hasBit(options, Options::LoadExternalBuffers)) {
	            auto [error, source] = loadFileFromUri(uriView);
                if (error != Error::None) {
                    return error;
                }

                buffer.data = std::move(source);
            } else {
                sources::URI filePath;
                filePath.fileByteOffset = 0;
                filePath.uri = uriView;
                buffer.data = std::move(filePath);
            }
        } else if (bufferIndex == 0 && !std::holds_alternative<std::monostate>(glbBuffer)) {
            buffer.data = std::move(glbBuffer);
        } else if (meshoptCompressionRequired) {
			// This buffer is not a GLB buffer and has no URI source and is therefore a fallback.
			buffer.data = sources::Fallback();
		} else {
            // All other buffers have to contain an uri field.
            return Error::InvalidGltf;
        }

        if (std::holds_alternative<std::monostate>(buffer.data)) {
            return Error::InvalidGltf;
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = bufferObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.buffers.size(), Category::Buffers, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		std::string_view name;
        if (bufferObject["name"].get_string().get(name) == SUCCESS) {
	        buffer.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(buffer.name), resourceAllocator.get(), name);
        }

        ++bufferIndex;
	    asset.buffers.emplace_back(std::move(buffer));
    }

	return Error::None;
}

fg::Error fg::Parser::parseBufferViews(simdjson::dom::array& bufferViews, Asset& asset) {
    using namespace simdjson;

	asset.bufferViews.reserve(bufferViews.size());
    for (auto bufferViewValue : bufferViews) {
        dom::object bufferViewObject;
        if (bufferViewValue.get_object().get(bufferViewObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::uint64_t number;
        BufferView view;
        if (auto error = bufferViewObject["buffer"].get_uint64().get(number); error != SUCCESS) FASTGLTF_UNLIKELY {
            return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
        }
        view.bufferIndex = static_cast<std::size_t>(number);

        if (auto error = bufferViewObject["byteOffset"].get_uint64().get(number); error == SUCCESS) FASTGLTF_LIKELY {
            view.byteOffset = static_cast<std::size_t>(number);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidJson;
        }

        if (auto error = bufferViewObject["byteLength"].get_uint64().get(number); error != SUCCESS) FASTGLTF_UNLIKELY {
            return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
        }
        view.byteLength = static_cast<std::size_t>(number);

        if (auto error = bufferViewObject["byteStride"].get_uint64().get(number); error == SUCCESS) FASTGLTF_LIKELY {
            view.byteStride = static_cast<std::size_t>(number);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidJson;
        }

        if (auto error = bufferViewObject["target"].get_uint64().get(number); error == SUCCESS) FASTGLTF_LIKELY {
            view.target = static_cast<BufferTarget>(number);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidJson;
        }

        std::string_view string;
        if (auto error = bufferViewObject["name"].get_string().get(string); error == SUCCESS) {
	        view.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(view.name), resourceAllocator.get(), string);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidJson;
        }

        dom::object extensionObject;
        if (bufferViewObject["extensions"].get_object().get(extensionObject) == SUCCESS) FASTGLTF_LIKELY {
            dom::object meshoptCompression;
            if (hasBit(config.extensions, Extensions::EXT_meshopt_compression) && extensionObject[extensions::EXT_meshopt_compression].get_object().get(meshoptCompression) == SUCCESS) FASTGLTF_LIKELY {
                auto compression = std::make_unique<CompressedBufferView>();

                if (auto error = meshoptCompression["buffer"].get_uint64().get(number); error != SUCCESS) FASTGLTF_UNLIKELY {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                compression->bufferIndex = static_cast<std::size_t>(number);

                if (auto error = meshoptCompression["byteOffset"].get_uint64().get(number); error == SUCCESS) FASTGLTF_LIKELY {
                    compression->byteOffset = static_cast<std::size_t>(number);
                } else if (error == NO_SUCH_FIELD) {
                    compression->byteOffset = 0;
                } else {
                    return Error::InvalidJson;
                }

                if (auto error = meshoptCompression["byteLength"].get_uint64().get(number); error != SUCCESS) FASTGLTF_UNLIKELY {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                compression->byteLength = static_cast<std::size_t>(number);

                if (auto error = meshoptCompression["byteStride"].get_uint64().get(number); error != SUCCESS) FASTGLTF_UNLIKELY {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                compression->byteStride = static_cast<std::size_t>(number);

                if (auto error = meshoptCompression["count"].get_uint64().get(number); error != SUCCESS) FASTGLTF_UNLIKELY {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                compression->count = number;

                if (auto error = meshoptCompression["mode"].get_string().get(string); error != SUCCESS) FASTGLTF_UNLIKELY {
                    return error == NO_SUCH_FIELD ? Error::InvalidGltf : Error::InvalidJson;
                }
                switch (crcStringFunction(string)) {
                    case force_consteval<crc32c("ATTRIBUTES")>: {
                        compression->mode = MeshoptCompressionMode::Attributes;
                        break;
                    }
                    case force_consteval<crc32c("TRIANGLES")>: {
                        compression->mode = MeshoptCompressionMode::Triangles;
                        break;
                    }
                    case force_consteval<crc32c("INDICES")>: {
                        compression->mode = MeshoptCompressionMode::Indices;
                        break;
                    }
                    default: {
                        return Error::InvalidGltf;
                    }
                }

                if (auto error = meshoptCompression["filter"].get_string().get(string); error == SUCCESS) FASTGLTF_LIKELY {
                    switch (crcStringFunction(string)) {
                        case force_consteval<crc32c("NONE")>: {
                            compression->filter = MeshoptCompressionFilter::None;
                            break;
                        }
                        case force_consteval<crc32c("OCTAHEDRAL")>: {
                            compression->filter = MeshoptCompressionFilter::Octahedral;
                            break;
                        }
                        case force_consteval<crc32c("QUATERNION")>: {
                            compression->filter = MeshoptCompressionFilter::Quaternion;
                            break;
                        }
                        case force_consteval<crc32c("EXPONENTIAL")>: {
                            compression->filter = MeshoptCompressionFilter::Exponential;
                            break;
                        }
                        default: {
                            return Error::InvalidGltf;
                        }
                    }
                } else if (error == NO_SUCH_FIELD) {
                    compression->filter = MeshoptCompressionFilter::None;
                } else {
                    return Error::InvalidJson;
                }

                view.meshoptCompression = std::move(compression);
            }
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = bufferViewObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.bufferViews.size(), Category::BufferViews, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		asset.bufferViews.emplace_back(std::move(view));
    }

	return Error::None;
}

fg::Error fg::Parser::parseCameras(simdjson::dom::array& cameras, Asset& asset) {
    using namespace simdjson;

	asset.cameras.reserve(cameras.size());
    for (auto cameraValue : cameras) {
        Camera camera = {};
        dom::object cameraObject;
        if (cameraValue.get_object().get(cameraObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::string_view name;
        if (cameraObject["name"].get_string().get(name) == SUCCESS) {
	        camera.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(camera.name), resourceAllocator.get(), name);
        }

        std::string_view type;
        if (cameraObject["type"].get_string().get(type) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        if (type == "perspective") {
            dom::object perspectiveCamera;
            if (cameraObject["perspective"].get_object().get(perspectiveCamera) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            Camera::Perspective perspective = {};
            double value;
            if (auto error = perspectiveCamera["aspectRatio"].get_double().get(value); error == SUCCESS) FASTGLTF_LIKELY {
                perspective.aspectRatio = static_cast<num>(value);
            } else if (error != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}

            if (auto error = perspectiveCamera["zfar"].get_double().get(value); error == SUCCESS) FASTGLTF_LIKELY {
                perspective.zfar = static_cast<num>(value);
            } else if (error != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}

            if (perspectiveCamera["yfov"].get_double().get(value) == SUCCESS) FASTGLTF_LIKELY {
                perspective.yfov = static_cast<num>(value);
            } else {
                return Error::InvalidGltf;
            }

            if (perspectiveCamera["znear"].get_double().get(value) == SUCCESS) FASTGLTF_LIKELY {
                perspective.znear = static_cast<num>(value);
            } else {
                return Error::InvalidGltf;
            }

            camera.camera = perspective;
        } else if (type == "orthographic") {
            dom::object orthographicCamera;
            if (cameraObject["orthographic"].get_object().get(orthographicCamera) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            Camera::Orthographic orthographic = {};
            double value;
            if (orthographicCamera["xmag"].get_double().get(value) == SUCCESS) FASTGLTF_LIKELY {
                orthographic.xmag = static_cast<num>(value);
            } else {
                return Error::InvalidGltf;
            }

            if (orthographicCamera["ymag"].get_double().get(value) == SUCCESS) FASTGLTF_LIKELY {
                orthographic.ymag = static_cast<num>(value);
            } else {
                return Error::InvalidGltf;
            }

            if (orthographicCamera["zfar"].get_double().get(value) == SUCCESS) FASTGLTF_LIKELY {
                orthographic.zfar = static_cast<num>(value);
            } else {
                return Error::InvalidGltf;
            }

            if (orthographicCamera["znear"].get_double().get(value) == SUCCESS) FASTGLTF_LIKELY {
                orthographic.znear = static_cast<num>(value);
            } else {
                return Error::InvalidGltf;
            }

            camera.camera = orthographic;
        } else {
            return Error::InvalidGltf;
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = cameraObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.cameras.size(), Category::Cameras, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		asset.cameras.emplace_back(std::move(camera));
    }

	return Error::None;
}

fg::Error fg::Parser::parseExtensions(simdjson::dom::object& extensionsObject, Asset& asset) {
    using namespace simdjson;

    for (auto extensionValue : extensionsObject) {
        dom::object extensionObject;
        if (auto error = extensionValue.value.get_object().get(extensionObject); error != SUCCESS) FASTGLTF_UNLIKELY {
            if (error == INCORRECT_TYPE) {
                continue; // We want to ignore
            }
            return Error::InvalidGltf;
        }

        auto hash = crcStringFunction(extensionValue.key);
        switch (hash) {
            case force_consteval<crc32c(extensions::KHR_lights_punctual)>: {
                if (!hasBit(config.extensions, Extensions::KHR_lights_punctual))
                    break;

                dom::array lightsArray;
                if (auto error = extensionObject["lights"].get_array().get(lightsArray); error == SUCCESS) FASTGLTF_LIKELY {
                    if (auto lightsError = parseLights(lightsArray, asset); lightsError != Error::None)
						return lightsError;
                } else if (error != NO_SUCH_FIELD) {
                    return Error::InvalidGltf;
                }
                break;
            }
			case force_consteval<crc32c(extensions::KHR_materials_variants)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_variants))
					break;

				dom::array variantsArray;
				if (auto arrayError = extensionObject["variants"].get_array().get(variantsArray); arrayError == SUCCESS) FASTGLTF_LIKELY {
					asset.materialVariants.reserve(variantsArray.size());
					for (auto variant : variantsArray) {
						dom::object variantObject;
						if (auto error = variant.get_object().get(variantObject); error == SUCCESS) {
							std::string_view name;
							if (variantObject["name"].get_string().get(name) != SUCCESS) {
								return Error::InvalidGltf;
							}
							asset.materialVariants.emplace_back(name);
						} else {
							return Error::InvalidGltf;
						}
					}
				} else {
					return Error::InvalidGltf;
				}
				break;
			}
        }
    }

	return Error::None;
}

fg::Error fg::Parser::parseImages(simdjson::dom::array& images, Asset& asset) {
    using namespace simdjson;

	asset.images.reserve(images.size());
    for (auto imageValue : images) {
        Image image = {};
        dom::object imageObject;
        if (imageValue.get_object().get(imageObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::string_view uriString;
        if (imageObject["uri"].get_string().get(uriString) == SUCCESS) FASTGLTF_LIKELY {
            if (imageObject["bufferView"].error() == SUCCESS) FASTGLTF_LIKELY {
                // If uri is declared, bufferView cannot be declared.
                return Error::InvalidGltf;
            }

            URIView uriView(uriString);
            if (!uriView.valid()) {
                return Error::InvalidURI;
            }

            if (uriView.isDataUri()) {
                auto [error, source] = decodeDataUri(uriView);
                if (error != Error::None) {
                    return error;
                }

                image.data = std::move(source);
            } else if (uriView.isLocalPath() && hasBit(options, Options::LoadExternalImages)) {
	            auto [error, source] = loadFileFromUri(uriView);
                if (error != Error::None) {
                    return error;
                }

                image.data = std::move(source);
            } else {
                sources::URI filePath;
                filePath.fileByteOffset = 0;
                filePath.uri = uriView;
                image.data = std::move(filePath);
            }

            std::string_view mimeType;
            if (imageObject["mimeType"].get_string().get(mimeType) == SUCCESS) FASTGLTF_LIKELY {
                std::visit([&](auto& arg) {
                    using T = std::decay_t<decltype(arg)>;

                    // This is kinda cursed
                    if constexpr (is_any<T, sources::CustomBuffer, sources::BufferView, sources::URI, sources::Array, sources::Vector>()) {
                        arg.mimeType = getMimeTypeFromString(mimeType);
                    }
                }, image.data);
            }
        }

        std::uint64_t bufferViewIndex;
        if (imageObject["bufferView"].get_uint64().get(bufferViewIndex) == SUCCESS) FASTGLTF_LIKELY {
            std::string_view mimeType;
            if (imageObject["mimeType"].get_string().get(mimeType) != SUCCESS) FASTGLTF_UNLIKELY {
                // If bufferView is defined, mimeType needs to also be defined.
                return Error::InvalidGltf;
            }

            image.data = sources::BufferView {
                static_cast<std::size_t>(bufferViewIndex),
                getMimeTypeFromString(mimeType),
            };
        }

        if (std::holds_alternative<std::monostate>(image.data)) {
            return Error::InvalidGltf;
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = imageObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.images.size(), Category::Images, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		// name is optional.
        std::string_view name;
        if (imageObject["name"].get_string().get(name) == SUCCESS) {
	        image.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(image.name), resourceAllocator.get(), name);
        }

        asset.images.emplace_back(std::move(image));
    }

	return Error::None;
}

fg::Error fg::Parser::parseLights(simdjson::dom::array& lights, Asset& asset) {
    using namespace simdjson;

    asset.lights.reserve(lights.size());
    for (auto lightValue : lights) {
        dom::object lightObject;
        if (lightValue.get_object().get(lightObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
        Light light = {};

        std::string_view type;
        if (lightObject["type"].get_string().get(type) == SUCCESS) FASTGLTF_LIKELY {
            switch (crcStringFunction(type.data())) {
                case force_consteval<crc32c("directional")>: {
                    light.type = LightType::Directional;
                    break;
                }
                case force_consteval<crc32c("spot")>: {
                    light.type = LightType::Spot;
                    break;
                }
                case force_consteval<crc32c("point")>: {
                    light.type = LightType::Point;
                    break;
                }
                default: {
                    return Error::InvalidGltf;
                }
            }
        } else {
            return Error::InvalidGltf;
        }

        if (light.type == LightType::Spot) {
            dom::object spotObject;
            if (lightObject["spot"].get_object().get(spotObject) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            double innerConeAngle;
            if (auto error = spotObject["innerConeAngle"].get_double().get(innerConeAngle); error == SUCCESS) FASTGLTF_LIKELY {
                light.innerConeAngle = static_cast<num>(innerConeAngle);
            } else if (error == NO_SUCH_FIELD) {
                light.innerConeAngle = 0.0f;
            } else {
                return Error::InvalidGltf;
            }

            double outerConeAngle;
            if (auto error = spotObject["outerConeAngle"].get_double().get(outerConeAngle); error == SUCCESS) FASTGLTF_LIKELY {
                light.outerConeAngle = static_cast<num>(outerConeAngle);
            } else if (error == NO_SUCH_FIELD) {
                light.outerConeAngle = static_cast<num>(math::pi / 4.0);
            } else {
                return Error::InvalidGltf;
            }
        }

        dom::array colorArray;
        if (auto error = lightObject["color"].get_array().get(colorArray); error == SUCCESS) FASTGLTF_LIKELY {
            if (colorArray.size() != 3U) {
                return Error::InvalidGltf;
            }
            for (std::size_t i = 0U; i < colorArray.size(); ++i) {
                double color;
                if (colorArray.at(i).get_double().get(color) == SUCCESS) FASTGLTF_LIKELY {
                    light.color[i] = static_cast<num>(color);
                } else {
                    return Error::InvalidGltf;
                }
            }
        } else if (error == NO_SUCH_FIELD) {
			light.color = math::nvec3(1);
        } else {
            return Error::InvalidGltf;
        }

        double intensity;
        if (lightObject["intensity"].get_double().get(intensity) == SUCCESS) FASTGLTF_LIKELY {
            light.intensity = static_cast<num>(intensity);
        } else {
            light.intensity = 0.0f;
        }

        double range;
        if (lightObject["range"].get_double().get(range) == SUCCESS) FASTGLTF_LIKELY {
            light.range = static_cast<num>(range);
        }

		std::string_view name;
        if (lightObject["name"].get_string().get(name) == SUCCESS) {
	        light.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(light.name), resourceAllocator.get(), name);
        }

        asset.lights.emplace_back(std::move(light));
    }

	return Error::None;
}

fg::Error fg::Parser::parseMaterialExtensions(simdjson::dom::object &object, fastgltf::Material &material) {
	using namespace simdjson;

	for (auto extensionField : object) {
		auto hashedKey = crcStringFunction(extensionField.key);

		switch (hashedKey) {
			case force_consteval<crc32c(extensions::KHR_materials_anisotropy)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_anisotropy))
					break;

				dom::object anisotropyObject;
				auto anisotropyError = extensionField.value.get_object().get(anisotropyObject);
				if (anisotropyError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				auto anisotropy = std::make_unique<MaterialAnisotropy>();

				double anisotropyStrength;
				if (auto error = anisotropyObject["anisotropyStrength"].get_double().get(anisotropyStrength);
						error == SUCCESS) FASTGLTF_LIKELY {
					anisotropy->anisotropyStrength = static_cast<num>(anisotropyStrength);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidJson;
				}

				double anisotropyRotation;
				if (auto error = anisotropyObject["anisotropyRotation"].get_double().get(anisotropyRotation);
						error == SUCCESS) FASTGLTF_LIKELY {
					anisotropy->anisotropyRotation = static_cast<num>(anisotropyRotation);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidJson;
				}

				TextureInfo anisotropyTexture;
				if (auto error = parseTextureInfo(anisotropyObject, "anisotropyTexture", &anisotropyTexture,
												  config.extensions); error == Error::None) FASTGLTF_LIKELY {
					anisotropy->anisotropyTexture = std::move(anisotropyTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				material.anisotropy = std::move(anisotropy);
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_clearcoat)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_clearcoat))
					break;

				dom::object clearcoatObject;
				auto clearcoatError = extensionField.value.get_object().get(clearcoatObject);
				if (clearcoatError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				auto clearcoat = std::make_unique<MaterialClearcoat>();

				double clearcoatFactor;
				if (auto error = clearcoatObject["clearcoatFactor"].get_double().get(clearcoatFactor); error ==
																									   SUCCESS) {
					clearcoat->clearcoatFactor = static_cast<num>(clearcoatFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidJson;
				}

				TextureInfo clearcoatTexture;
				if (auto error = parseTextureInfo(clearcoatObject, "clearcoatTexture", &clearcoatTexture,
												  config.extensions); error == Error::None) FASTGLTF_LIKELY {
					clearcoat->clearcoatTexture = std::move(clearcoatTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				double clearcoatRoughnessFactor;
				if (auto error = clearcoatObject["clearcoatRoughnessFactor"].get_double().get(
							clearcoatRoughnessFactor); error == SUCCESS) FASTGLTF_LIKELY {
					clearcoat->clearcoatRoughnessFactor = static_cast<num>(clearcoatRoughnessFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidJson;
				}

				TextureInfo clearcoatRoughnessTexture;
				if (auto error = parseTextureInfo(clearcoatObject, "clearcoatRoughnessTexture",
												  &clearcoatRoughnessTexture, config.extensions); error ==
																								  Error::None) {
					clearcoat->clearcoatRoughnessTexture = std::move(clearcoatRoughnessTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				TextureInfo clearcoatNormalTexture;
				if (auto error = parseTextureInfo(clearcoatObject, "clearcoatNormalTexture",
												  &clearcoatNormalTexture, config.extensions); error ==
																							   Error::None) {
					clearcoat->clearcoatNormalTexture = std::move(clearcoatNormalTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				material.clearcoat = std::move(clearcoat);
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_dispersion)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_dispersion))
					break;

				dom::object dispersionObject;
				auto dispersionError = extensionField.value.get_object().get(dispersionObject);
				if (dispersionError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				double dispersionFactor;
				auto error = dispersionObject["dispersion"].get_double().get(dispersionFactor);
				if (error == SUCCESS) FASTGLTF_LIKELY {
					material.dispersion = static_cast<num>(dispersionFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_emissive_strength)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_emissive_strength))
					break;

				dom::object emissiveObject;
				auto emissiveError = extensionField.value.get_object().get(emissiveObject);
				if (emissiveError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				double emissiveStrength;
				auto error = emissiveObject["emissiveStrength"].get_double().get(emissiveStrength);
				if (error == SUCCESS) FASTGLTF_LIKELY {
					material.emissiveStrength = static_cast<num>(emissiveStrength);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_ior)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_ior))
					break;

				dom::object iorObject;
				auto iorError = extensionField.value.get_object().get(iorObject);
				if (iorError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				double ior;
				auto error = iorObject["ior"].get_double().get(ior);
				if (error == SUCCESS) FASTGLTF_LIKELY {
					material.ior = static_cast<num>(ior);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidJson;
				}
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_iridescence)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_iridescence))
					break;

				dom::object iridescenceObject;
				auto iridescenceError = extensionField.value.get_object().get(iridescenceObject);
				if (iridescenceError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				auto iridescence = std::make_unique<MaterialIridescence>();

				double iridescenceFactor;
				if (auto error = iridescenceObject["iridescenceFactor"].get_double().get(iridescenceFactor);
						error == SUCCESS) FASTGLTF_LIKELY {
					iridescence->iridescenceFactor = static_cast<num>(iridescenceFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo iridescenceTexture;
				if (auto error = parseTextureInfo(iridescenceObject, "iridescenceTexture", &iridescenceTexture,
												  config.extensions); error == Error::None) FASTGLTF_LIKELY {
					iridescence->iridescenceTexture = std::move(iridescenceTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				double iridescenceIor;
				if (auto error = iridescenceObject["iridescenceIor"].get_double().get(iridescenceIor); error ==
																									   SUCCESS) {
					iridescence->iridescenceIor = static_cast<num>(iridescenceIor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				double iridescenceThicknessMinimum;
				if (auto error = iridescenceObject["iridescenceThicknessMinimum"].get_double().get(
							iridescenceThicknessMinimum); error == SUCCESS) FASTGLTF_LIKELY {
					iridescence->iridescenceThicknessMinimum = static_cast<num>(iridescenceThicknessMinimum);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				double iridescenceThicknessMaximum;
				if (auto error = iridescenceObject["iridescenceThicknessMaximum"].get_double().get(
							iridescenceThicknessMaximum); error == SUCCESS) FASTGLTF_LIKELY {
					iridescence->iridescenceThicknessMaximum = static_cast<num>(iridescenceThicknessMaximum);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo iridescenceThicknessTexture;
				if (auto error = parseTextureInfo(iridescenceObject, "iridescenceThicknessTexture",
												  &iridescenceThicknessTexture, config.extensions); error ==
																									Error::None) {
					iridescence->iridescenceThicknessTexture = std::move(iridescenceThicknessTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				material.iridescence = std::move(iridescence);
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_sheen)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_sheen))
					break;

				dom::object sheenObject;
				auto sheenError = extensionField.value.get_object().get(sheenObject);
				if (sheenError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				auto sheen = std::make_unique<MaterialSheen>();

				dom::array sheenColorFactor;
				if (auto error = sheenObject["sheenColorFactor"].get_array().get(sheenColorFactor); error ==
																									SUCCESS) {
					std::size_t i = 0;
					for (auto factor: sheenColorFactor) {
						if (i >= sheen->sheenColorFactor.size()) {
							return Error::InvalidGltf;
						}
						double value;
						if (factor.get_double().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
							return Error::InvalidGltf;
						}
						sheen->sheenColorFactor[i++] = static_cast<num>(value);
					}
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo sheenColorTexture;
				if (auto error = parseTextureInfo(sheenObject, "sheenColorTexture", &sheenColorTexture,
												  config.extensions); error == Error::None) FASTGLTF_LIKELY {
					sheen->sheenColorTexture = std::move(sheenColorTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				double sheenRoughnessFactor;
				if (auto error = sheenObject["sheenRoughnessFactor"].get_double().get(sheenRoughnessFactor);
						error == SUCCESS) FASTGLTF_LIKELY {
					sheen->sheenRoughnessFactor = static_cast<num>(sheenRoughnessFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo sheenRoughnessTexture;
				if (auto error = parseTextureInfo(sheenObject, "sheenRoughnessTexture", &sheenRoughnessTexture,
												  config.extensions); error == Error::None) FASTGLTF_LIKELY {
					sheen->sheenRoughnessTexture = std::move(sheenRoughnessTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				material.sheen = std::move(sheen);
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_specular)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_specular))
					break;

				dom::object specularObject;
				auto specularError = extensionField.value.get_object().get(specularObject);
				if (specularError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				auto specular = std::make_unique<MaterialSpecular>();

				double specularFactor;
				if (auto error = specularObject["specularFactor"].get_double().get(specularFactor); error ==
																									SUCCESS) {
					specular->specularFactor = static_cast<num>(specularFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo specularTexture;
				if (auto error = parseTextureInfo(specularObject, "specularTexture", &specularTexture,
												  config.extensions); error == Error::None) FASTGLTF_LIKELY {
					specular->specularTexture = std::move(specularTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				dom::array specularColorFactor;
				if (auto error = specularObject["specularColorFactor"].get_array().get(specularColorFactor);
						error == SUCCESS) FASTGLTF_LIKELY {
					std::size_t i = 0;
					for (auto factor: specularColorFactor) {
						if (i >= specular->specularColorFactor.size()) {
							return Error::InvalidGltf;
						}
						double value;
						if (factor.get_double().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
							return Error::InvalidGltf;
						}
						specular->specularColorFactor[i++] = static_cast<num>(value);
					}
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo specularColorTexture;
				if (auto error = parseTextureInfo(specularObject, "specularColorTexture", &specularColorTexture,
												  config.extensions); error == Error::None) FASTGLTF_LIKELY {
					specular->specularColorTexture = std::move(specularColorTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				material.specular = std::move(specular);
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_transmission)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_transmission))
					break;

				dom::object transmissionObject;
				auto transmissionError = extensionField.value.get_object().get(transmissionObject);
				if (transmissionError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				auto transmission = std::make_unique<MaterialTransmission>();

				double transmissionFactor;
				if (auto error = transmissionObject["transmissionFactor"].get_double().get(transmissionFactor);
						error == SUCCESS) FASTGLTF_LIKELY {
					transmission->transmissionFactor = static_cast<num>(transmissionFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo transmissionTexture;
				if (auto error = parseTextureInfo(transmissionObject, "transmissionTexture", &transmissionTexture,
												  config.extensions); error == Error::None) FASTGLTF_LIKELY {
					transmission->transmissionTexture = std::move(transmissionTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				material.transmission = std::move(transmission);
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_unlit)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_unlit))
					break;

				dom::object unlitObject;
				auto unlitError = extensionField.value.get_object().get(unlitObject);
				if (unlitError == SUCCESS) FASTGLTF_LIKELY {
					material.unlit = true;
				} else {
					return Error::InvalidGltf;
				}
				break;
			}
			case force_consteval<crc32c(extensions::KHR_materials_volume)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_volume))
					break;

				dom::object volumeObject;
				auto volumeError = extensionField.value.get_object().get(volumeObject);
				if (volumeError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				auto volume = std::make_unique<MaterialVolume>();

				double thicknessFactor;
				if (auto error = volumeObject["thicknessFactor"].get_double().get(thicknessFactor); error == SUCCESS) FASTGLTF_LIKELY {
					volume->thicknessFactor = static_cast<num>(thicknessFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo thicknessTexture;
				if (auto error = parseTextureInfo(volumeObject, "thicknessTexture", &thicknessTexture, config.extensions); error == Error::None) FASTGLTF_LIKELY {
					volume->thicknessTexture = std::move(thicknessTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				double attenuationDistance;
				if (auto error = volumeObject["attenuationDistance"].get_double().get(attenuationDistance); error == SUCCESS) FASTGLTF_LIKELY {
					volume->attenuationDistance = static_cast<num>(attenuationDistance);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				dom::array attenuationColor;
				if (auto error = volumeObject["attenuationColor"].get_array().get(attenuationColor); error == SUCCESS) FASTGLTF_LIKELY {
					std::size_t i = 0;
					for (auto factor : attenuationColor) {
						if (i >= volume->attenuationColor.size()) {
							return Error::InvalidGltf;
						}
						double value;
						if (factor.get_double().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
							return Error::InvalidGltf;
						}
						(volume->attenuationColor)[i++] = static_cast<num>(value);
					}
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				material.volume = std::move(volume);
				break;
			}
			case force_consteval<crc32c(extensions::MSFT_packing_normalRoughnessMetallic)>: {
				if (!hasBit(config.extensions, Extensions::MSFT_packing_normalRoughnessMetallic))
					break;

				dom::object normalRoughnessMetallic;
				if (extensionField.value.get_object().get(normalRoughnessMetallic) != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}
				TextureInfo textureInfo = {};
				if (auto error = parseTextureInfo(normalRoughnessMetallic, "normalRoughnessMetallicTexture", &textureInfo, config.extensions); error == Error::None) FASTGLTF_LIKELY {
					material.packedNormalMetallicRoughnessTexture = std::move(textureInfo);
				} else if (error != Error::MissingField) {
					return error;
				}
				break;
			}
			case force_consteval<crc32c(extensions::MSFT_packing_occlusionRoughnessMetallic)>: {
				if (!hasBit(config.extensions, Extensions::MSFT_packing_occlusionRoughnessMetallic))
					break;

				dom::object occlusionRoughnessMetallic;
				if (extensionField.value.get_object().get(occlusionRoughnessMetallic) != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}
				auto packedTextures = std::make_unique<MaterialPackedTextures>();
				TextureInfo textureInfo = {};
				if (auto error = parseTextureInfo(occlusionRoughnessMetallic, "occlusionRoughnessMetallicTexture", &textureInfo, config.extensions); error == Error::None) FASTGLTF_LIKELY {
					packedTextures->occlusionRoughnessMetallicTexture = std::move(textureInfo);
				} else if (error != Error::MissingField) {
					return error;
				}

				if (auto error = parseTextureInfo(occlusionRoughnessMetallic, "roughnessMetallicOcclusionTexture", &textureInfo, config.extensions); error == Error::None) FASTGLTF_LIKELY {
					packedTextures->roughnessMetallicOcclusionTexture = std::move(textureInfo);
				} else if (error != Error::MissingField) {
					return error;
				}

				if (auto error = parseTextureInfo(occlusionRoughnessMetallic, "normalTexture", &textureInfo, config.extensions); error == Error::None) FASTGLTF_LIKELY {
					packedTextures->normalTexture = std::move(textureInfo);
				} else if (error != Error::MissingField) {
					return error;
				}

				material.packedOcclusionRoughnessMetallicTextures = std::move(packedTextures);
				break;
			}
#if FASTGLTF_ENABLE_DEPRECATED_EXT
			case force_consteval<crc32c(extensions::KHR_materials_pbrSpecularGlossiness)>: {
				if (!hasBit(config.extensions, Extensions::KHR_materials_pbrSpecularGlossiness))
					break;

				dom::object specularGlossinessObject;
				auto specularGlossinessError = extensionField.value.get_object().get(specularGlossinessObject);
				if (specularGlossinessError != SUCCESS) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}
				auto specularGlossiness = std::make_unique<MaterialSpecularGlossiness>();

				dom::array diffuseFactor;
				if (auto error = specularGlossinessObject["diffuseFactor"].get_array().get(diffuseFactor); error == SUCCESS) FASTGLTF_LIKELY {
					std::size_t i = 0;
					for (auto factor : diffuseFactor) {
						if (i >= specularGlossiness->diffuseFactor.size()) {
							return Error::InvalidGltf;
						}
						double value;
						if (factor.get_double().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
							return Error::InvalidGltf;
						}
						specularGlossiness->diffuseFactor[i++] = static_cast<num>(value);
					}
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo diffuseTexture;
				if (auto error = parseTextureInfo(specularGlossinessObject, "diffuseTexture", &diffuseTexture, config.extensions); error == Error::None) FASTGLTF_LIKELY {
					specularGlossiness->diffuseTexture = std::move(diffuseTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				dom::array specularFactor;
				if (auto error = specularGlossinessObject["specularFactor"].get_array().get(specularFactor); error == SUCCESS) FASTGLTF_LIKELY {
					std::size_t i = 0;
					for (auto factor : specularFactor) {
						if (i >= specularGlossiness->specularFactor.size()) {
							return Error::InvalidGltf;
						}
						double value;
						if (factor.get_double().get(value) != SUCCESS) FASTGLTF_UNLIKELY {
							return Error::InvalidGltf;
						}
						specularGlossiness->specularFactor[i++] = static_cast<num>(value);
					}
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				double glossinessFactor;
				if (auto error = specularGlossinessObject["glossinessFactor"].get_double().get(glossinessFactor); error == SUCCESS) FASTGLTF_LIKELY {
					specularGlossiness->glossinessFactor = static_cast<num>(glossinessFactor);
				} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
					return Error::InvalidGltf;
				}

				TextureInfo specularGlossinessTexture;
				if (auto error = parseTextureInfo(specularGlossinessObject, "specularGlossinessTexture", &specularGlossinessTexture, config.extensions); error == Error::None) FASTGLTF_LIKELY {
					specularGlossiness->specularGlossinessTexture = std::move(specularGlossinessTexture);
				} else if (error != Error::MissingField) {
					return error;
				}

				material.specularGlossiness = std::move(specularGlossiness);
				break;
			}
#endif
			default:
				// Should we error on unknown extensions?
				break;
		}
	}

	return Error::None;
}

fg::Error fg::Parser::parseMaterials(simdjson::dom::array& materials, Asset& asset) {
    using namespace simdjson;

    asset.materials.reserve(materials.size());
    for (auto materialValue : materials) {
        dom::object materialObject;
        if (materialValue.get_object().get(materialObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
        Material material = {};

        dom::array emissiveFactor;
        if (auto error = materialObject["emissiveFactor"].get_array().get(emissiveFactor); error == SUCCESS) FASTGLTF_LIKELY {
            if (emissiveFactor.size() != 3) {
                return Error::InvalidGltf;
            }
            for (auto i = 0U; i < 3; ++i) {
                double val;
                if (emissiveFactor.at(i).get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
                    return Error::InvalidGltf;
                }
                material.emissiveFactor[i] = static_cast<num>(val);
            }
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

	    {
		    NormalTextureInfo normalTextureInfo = {};
		    if (auto error = parseTextureInfo(materialObject, "normalTexture", &normalTextureInfo, config.extensions, TextureInfoType::NormalTexture); error == Error::None) FASTGLTF_LIKELY {
			    material.normalTexture = std::move(normalTextureInfo);
		    } else if (error != Error::MissingField) {
			    return error;
		    }
	    }

	    {
			OcclusionTextureInfo occlusionTextureInfo = {};
	        if (auto error = parseTextureInfo(materialObject, "occlusionTexture", &occlusionTextureInfo, config.extensions, TextureInfoType::OcclusionTexture); error == Error::None) FASTGLTF_LIKELY {
	            material.occlusionTexture = std::move(occlusionTextureInfo);
	        } else if (error != Error::MissingField) {
	            return error;
	        }
	    }

	    {
		    TextureInfo textureInfo = {};
	        if (auto error = parseTextureInfo(materialObject, "emissiveTexture", &textureInfo, config.extensions); error == Error::None) FASTGLTF_LIKELY {
	            material.emissiveTexture = std::move(textureInfo);
	        } else if (error != Error::MissingField) {
	            return error;
	        }
	    }

        dom::object pbrMetallicRoughness;
        if (materialObject["pbrMetallicRoughness"].get_object().get(pbrMetallicRoughness) == SUCCESS) FASTGLTF_LIKELY {
            PBRData pbr = {};

            dom::array baseColorFactor;
            if (pbrMetallicRoughness["baseColorFactor"].get_array().get(baseColorFactor) == SUCCESS) FASTGLTF_LIKELY {
                for (auto i = 0U; i < 4; ++i) {
                    double val;
                    if (baseColorFactor.at(i).get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
                        return Error::InvalidGltf;
                    }
                    pbr.baseColorFactor[i] = static_cast<num>(val);
                }
            }

            double factor;
            if (auto error = pbrMetallicRoughness["metallicFactor"].get_double().get(factor); error == SUCCESS) FASTGLTF_LIKELY {
                pbr.metallicFactor = static_cast<num>(factor);
            } else if (error != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
            if (auto error = pbrMetallicRoughness["roughnessFactor"].get_double().get(factor); error == SUCCESS) FASTGLTF_LIKELY {
                pbr.roughnessFactor = static_cast<num>(factor);
            } else if (error != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}

	        TextureInfo textureInfo;
            if (auto error = parseTextureInfo(pbrMetallicRoughness, "baseColorTexture", &textureInfo, config.extensions); error == Error::None) FASTGLTF_LIKELY {
                pbr.baseColorTexture = std::move(textureInfo);
            } else if (error != Error::MissingField) {
                return error;
            }

            if (auto error = parseTextureInfo(pbrMetallicRoughness, "metallicRoughnessTexture", &textureInfo, config.extensions); error == Error::None) FASTGLTF_LIKELY {
                pbr.metallicRoughnessTexture = std::move(textureInfo);
            } else if (error != Error::MissingField) {
                return error;
            }

            material.pbrData = std::move(pbr);
        }

        std::string_view alphaMode;
        if (auto error = materialObject["alphaMode"].get_string().get(alphaMode); error == SUCCESS) FASTGLTF_LIKELY {
            if (alphaMode == "OPAQUE") {
                material.alphaMode = AlphaMode::Opaque;
            } else if (alphaMode == "MASK") {
                material.alphaMode = AlphaMode::Mask;
            } else if (alphaMode == "BLEND") {
                material.alphaMode = AlphaMode::Blend;
            } else {
                return Error::InvalidGltf;
            }
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        double alphaCutoff;
        if (auto error = materialObject["alphaCutoff"].get_double().get(alphaCutoff); error == SUCCESS) FASTGLTF_LIKELY {
            material.alphaCutoff = static_cast<num>(alphaCutoff);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        bool doubleSided;
        if (auto error = materialObject["doubleSided"].get_bool().get(doubleSided); error == SUCCESS) FASTGLTF_LIKELY {
            material.doubleSided = doubleSided;
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::string_view name;
        if (materialObject["name"].get_string().get(name) == SUCCESS) {
	        material.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(material.name), resourceAllocator.get(), name);
        }

        dom::object extensionsObject;
        if (auto extensionError = materialObject["extensions"].get_object().get(extensionsObject); extensionError == SUCCESS) {
			parseMaterialExtensions(extensionsObject, material);
        } else if (extensionError != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidJson;
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = materialObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.materials.size(), Category::Materials, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		asset.materials.emplace_back(std::move(material));
    }

	return Error::None;
}

fg::Error fg::Parser::parseMeshes(simdjson::dom::array& meshes, Asset& asset) {
    using namespace simdjson;

    asset.meshes.reserve(meshes.size());
    for (auto meshValue : meshes) {
        // Required fields: "primitives"
        dom::object meshObject;
        if (meshValue.get_object().get(meshObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
        Mesh mesh = {};

        dom::array array;
        auto meshError = getJsonArray(meshObject, "primitives", &array);
		if (meshError != Error::None) {
			return meshError == Error::MissingField ? Error::InvalidGltf : meshError;
		}

		mesh.primitives = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(mesh.primitives), resourceAllocator.get(), 0);
		mesh.primitives.reserve(array.size());
		for (auto primitiveValue : array) {
			// Required fields: "attributes"
			Primitive primitive = {};
			dom::object primitiveObject;
			if (primitiveValue.get_object().get(primitiveObject) != SUCCESS) FASTGLTF_UNLIKELY {
				return Error::InvalidGltf;
			}

			dom::object attributesObject;
			if (primitiveObject["attributes"].get_object().get(attributesObject) != SUCCESS) FASTGLTF_UNLIKELY {
				return Error::InvalidGltf;
			}
			if (auto attributesError = parseAttributes(attributesObject, primitive.attributes); attributesError != Error::None) {
				return attributesError;
			}

			dom::array targets;
			if (primitiveObject["targets"].get_array().get(targets) == SUCCESS) FASTGLTF_LIKELY {
				primitive.targets = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(primitive.targets), resourceAllocator.get(), 0);
				primitive.targets.reserve(targets.size());
				for (auto targetValue : targets) {
					if (targetValue.get_object().get(attributesObject) != SUCCESS) FASTGLTF_UNLIKELY {
						return Error::InvalidGltf;
					}
					auto& map = primitive.targets.emplace_back();
					if (auto attributesError = parseAttributes(attributesObject, map); attributesError != Error::None) {
						return attributesError;
					}
				}
			}

			std::uint64_t value;
			if (auto error = primitiveObject["mode"].get_uint64().get(value); error == SUCCESS) FASTGLTF_LIKELY {
				primitive.type = static_cast<PrimitiveType>(value);
			} else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
				return Error::InvalidGltf;
			}

			if (auto error = primitiveObject["indices"].get_uint64().get(value); error == SUCCESS) FASTGLTF_LIKELY {
				primitive.indicesAccessor = static_cast<std::size_t>(value);
			} else if (error != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}

			if (auto error = primitiveObject["material"].get_uint64().get(value); error == SUCCESS) FASTGLTF_LIKELY {
				primitive.materialIndex = static_cast<std::size_t>(value);
			} else if (error != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}

			if (hasBit(config.extensions, Extensions::KHR_materials_variants)) {
				dom::object extensionsObject;
				if (auto error = primitiveObject["extensions"].get_object().get(extensionsObject); error == SUCCESS) {
					dom::object extensionObject;
					if (auto variantsError = extensionsObject[extensions::KHR_materials_variants].get_object().get(extensionObject); variantsError == SUCCESS) {
						dom::array mappingsArray;
						if (extensionObject["mappings"].get_array().get(mappingsArray) != SUCCESS) FASTGLTF_UNLIKELY {
							return Error::InvalidGltf;
						}

						for (auto mapping : mappingsArray) {
							dom::object mappingObject;
							if (mapping.get_object().get(mappingObject) != SUCCESS) FASTGLTF_UNLIKELY {
								return Error::InvalidGltf;
							}

							std::uint64_t materialIndex;
							if (mappingObject["material"].get_uint64().get(materialIndex) != SUCCESS) FASTGLTF_UNLIKELY {
								return Error::InvalidGltf;
							}

							dom::array variantsArray;
							if (mappingObject["variants"].get_array().get(variantsArray) != SUCCESS) FASTGLTF_UNLIKELY {
								return Error::InvalidGltf;
							}
							for (auto variant : variantsArray) {
								std::uint64_t variantIndex;
								if (variant.get_uint64().get(variantIndex) != SUCCESS) FASTGLTF_UNLIKELY {
									return Error::InvalidGltf;
								}

								primitive.mappings.resize(max(primitive.mappings.size(),
															  static_cast<std::size_t>(variantIndex + 1)));
								primitive.mappings[std::size_t(variantIndex)] = materialIndex;
							}
						}
					} else if (variantsError != NO_SUCH_FIELD) {
						return Error::InvalidGltf;
					}
				} else if (error != NO_SUCH_FIELD) {
					return Error::InvalidGltf;
				}
			}

			mesh.primitives.emplace_back(std::move(primitive));
		}

        if (meshError = getJsonArray(meshObject, "weights", &array); meshError == Error::None) FASTGLTF_LIKELY {
	        mesh.weights = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(mesh.weights), resourceAllocator.get(), 0);
            mesh.weights.reserve(array.size());
            for (auto weightValue : array) {
                double val;
                if (weightValue.get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
                    return Error::InvalidGltf;
                }
                mesh.weights.emplace_back(static_cast<num>(val));
            }
        } else if (meshError != Error::MissingField && meshError != Error::None) {
            return Error::InvalidGltf;
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = meshObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.meshes.size(), Category::Meshes, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		std::string_view name;
        if (meshObject["name"].get_string().get(name) == SUCCESS) {
	        mesh.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(mesh.name), resourceAllocator.get(), name);
        }

        asset.meshes.emplace_back(std::move(mesh));
    }

	return Error::None;
}

fg::Error fg::Parser::parseNodes(simdjson::dom::array& nodes, Asset& asset) {
    using namespace simdjson;

    asset.nodes.reserve(nodes.size());
    for (auto nodeValue : nodes) {
        Node node = {};
        dom::object nodeObject;
        if (nodeValue.get_object().get(nodeObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::uint64_t index;
        if (auto error = nodeObject["mesh"].get_uint64().get(index); error == SUCCESS) {
            node.meshIndex = static_cast<std::size_t>(index);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
			return Error::InvalidGltf;
		}
        if (auto error = nodeObject["skin"].get_uint64().get(index); error == SUCCESS) {
            node.skinIndex = static_cast<std::size_t>(index);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
			return Error::InvalidGltf;
		}
        if (auto error = nodeObject["camera"].get_uint64().get(index); error == SUCCESS) {
            node.cameraIndex = static_cast<std::size_t>(index);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
			return Error::InvalidGltf;
		}

        dom::array array;
        auto childError = getJsonArray(nodeObject, "children", &array);
        if (childError == Error::None) FASTGLTF_LIKELY {
	        node.children = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(node.children), resourceAllocator.get(), 0);
			node.children.reserve(array.size());
            for (auto childValue : array) {
                if (childValue.get_uint64().get(index) != SUCCESS) FASTGLTF_UNLIKELY {
                    return Error::InvalidGltf;
                }

                node.children.emplace_back(static_cast<std::size_t>(index));
            }
        } else if (childError != Error::MissingField) {
            return childError;
        }

        auto weightsError = getJsonArray(nodeObject, "weights", &array);
        if (weightsError != Error::MissingField) {
            if (weightsError != Error::None) {
	            node.weights = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(node.weights), resourceAllocator.get(), 0);
                node.weights.reserve(array.size());
                for (auto weightValue : array) {
                    double val;
                    if (weightValue.get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
                        return Error::InvalidGltf;
                    }
                    node.weights.emplace_back(static_cast<num>(val));
                }
            } else {
                return Error::InvalidGltf;
            }
        }

        auto error = nodeObject["matrix"].get_array().get(array);
        if (error == SUCCESS) FASTGLTF_LIKELY {
            math::fmat4x4 transformMatrix;
            std::size_t i = 0, j = 0;
			for (auto num : array) {
                double val;
                if (num.get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
                    break;
                }
				transformMatrix.col(i)[j++] = static_cast<fastgltf::num>(val);
				if (j == 4) {
					j = 0;
					++i;
				}
            }

            if (hasBit(options, Options::DecomposeNodeMatrices)) {
                TRS trs = {};
                math::decomposeTransformMatrix(transformMatrix, trs.scale, trs.rotation, trs.translation);
                node.transform = trs;
            } else {
                node.transform = transformMatrix;
            }
        } else if (error == NO_SUCH_FIELD) {
            TRS trs = {};

            // There's no matrix, let's see if there's scale, rotation, or rotation fields.
            if (auto scaleError = nodeObject["scale"].get_array().get(array); scaleError == SUCCESS) FASTGLTF_LIKELY {
                auto i = 0U;
                for (auto num : array) {
                    double val;
                    if (num.get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
                        return Error::InvalidGltf;
                    }
                    trs.scale[i] = static_cast<fastgltf::num>(val);
                    ++i;
                }
            } else if (scaleError != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
                return Error::InvalidJson;
            }

            if (auto translationError = nodeObject["translation"].get_array().get(array); translationError == SUCCESS) FASTGLTF_LIKELY {
                auto i = 0U;
                for (auto num : array) {
                    double val;
                    if (num.get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
                        return Error::InvalidGltf;
                    }
                    trs.translation[i] = static_cast<fastgltf::num>(val);
                    ++i;
                }
            } else if (translationError != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            if (auto rotationError = nodeObject["rotation"].get_array().get(array); rotationError == SUCCESS) FASTGLTF_LIKELY {
                auto i = 0U;
                for (auto num : array) {
                    double val;
                    if (num.get_double().get(val) != SUCCESS) FASTGLTF_UNLIKELY {
                        return Error::InvalidGltf;
                    }
                    trs.rotation[i] = static_cast<fastgltf::num>(val);
                    ++i;
                }
            } else if (rotationError != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }

            node.transform = trs;
        }

        dom::object extensionsObject;
        if (nodeObject["extensions"].get_object().get(extensionsObject) == SUCCESS) FASTGLTF_LIKELY {
			if (hasBit(config.extensions, Extensions::KHR_lights_punctual)) {
				dom::object lightsObject;
				if (extensionsObject[extensions::KHR_lights_punctual].get_object().get(lightsObject) == SUCCESS) FASTGLTF_LIKELY {
					std::uint64_t light;
					if (auto lightError = lightsObject["light"].get_uint64().get(light); lightError == SUCCESS) FASTGLTF_LIKELY {
						node.lightIndex = static_cast<std::size_t>(light);
					} else {
						return lightError == NO_SUCH_FIELD || lightError == INCORRECT_TYPE ? Error::InvalidGltf : Error::InvalidJson;
					}
				} else if (error != NO_SUCH_FIELD) {
					return Error::InvalidGltf;
				}
			}

			if (hasBit(config.extensions, Extensions::EXT_mesh_gpu_instancing)) {
				dom::object gpuInstancingObject;
				if (auto instancingError = extensionsObject[extensions::EXT_mesh_gpu_instancing].get_object().get(gpuInstancingObject); instancingError == SUCCESS) FASTGLTF_LIKELY {
					dom::object attributesObject;

					if (gpuInstancingObject["attributes"].get_object().get(attributesObject) != SUCCESS) FASTGLTF_UNLIKELY {
						return Error::InvalidGltf;
					}

					if (auto attributesError = parseAttributes(attributesObject, node.instancingAttributes); attributesError != Error::None) {
						return attributesError;
					}
				} else if (instancingError != NO_SUCH_FIELD) {
					return Error::InvalidGltf;
				}
			}
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = nodeObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.nodes.size(), Category::Nodes, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

        std::string_view name;
        if (nodeObject["name"].get_string().get(name) == SUCCESS) {
	        node.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(node.name), resourceAllocator.get(), name);
        }

        asset.nodes.emplace_back(std::move(node));
    }

	return Error::None;
}

fg::Error fg::Parser::parseSamplers(simdjson::dom::array& samplers, Asset& asset) {
    using namespace simdjson;

    std::uint64_t number;
    asset.samplers.reserve(samplers.size());
    for (auto samplerValue : samplers) {
        Sampler sampler = {};
        dom::object samplerObject;
        if (samplerValue.get_object().get(samplerObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        if (auto error = samplerObject["magFilter"].get_uint64().get(number); error == SUCCESS) FASTGLTF_LIKELY {
            sampler.magFilter = static_cast<Filter>(number);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
			return Error::InvalidGltf;
		}
        if (auto error = samplerObject["minFilter"].get_uint64().get(number); error == SUCCESS) FASTGLTF_LIKELY {
            sampler.minFilter = static_cast<Filter>(number);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
			return Error::InvalidGltf;
		}

        if (auto error = samplerObject["wrapS"].get_uint64().get(number); error == SUCCESS) FASTGLTF_LIKELY {
            sampler.wrapS = static_cast<Wrap>(number);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
        if (auto error = samplerObject["wrapT"].get_uint64().get(number); error == SUCCESS) FASTGLTF_LIKELY {
            sampler.wrapT = static_cast<Wrap>(number);
        } else if (error != NO_SUCH_FIELD) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = samplerObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.samplers.size(), Category::Samplers, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		std::string_view name;
		if (samplerObject["name"].get_string().get(name) == SUCCESS) {
			sampler.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(sampler.name), resourceAllocator.get(), name);
		}

		asset.samplers.emplace_back(std::move(sampler));
    }

	return Error::None;
}

fg::Error fg::Parser::parseScenes(simdjson::dom::array& scenes, Asset& asset) {
    using namespace simdjson;

    asset.scenes.reserve(scenes.size());
    for (auto sceneValue : scenes) {
        // The scene object can be completely empty
        Scene scene = {};
        dom::object sceneObject;
        if (sceneValue.get_object().get(sceneObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::string_view name;
        if (sceneObject["name"].get_string().get(name) == SUCCESS) {
	        scene.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(scene.name), resourceAllocator.get(), name);
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = sceneObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.scenes.size(), Category::Scenes, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		// Parse the array of nodes.
        dom::array nodes;
        auto nodeError = getJsonArray(sceneObject, "nodes", &nodes);
        if (nodeError == Error::None) FASTGLTF_LIKELY {
	        scene.nodeIndices = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(scene.nodeIndices), resourceAllocator.get(), 0);
			scene.nodeIndices.reserve(nodes.size());
            for (auto nodeValue : nodes) {
                std::uint64_t index;
                if (nodeValue.get_uint64().get(index) != SUCCESS) FASTGLTF_UNLIKELY {
                    return Error::InvalidGltf;
                }

                scene.nodeIndices.emplace_back(static_cast<std::size_t>(index));
            }

            asset.scenes.emplace_back(std::move(scene));
        } else if (nodeError != Error::MissingField) {
            return nodeError;
        }
    }

	return Error::None;
}

fg::Error fg::Parser::parseSkins(simdjson::dom::array& skins, Asset& asset) {
    using namespace simdjson;

    asset.skins.reserve(skins.size());
    for (auto skinValue : skins) {
        Skin skin = {};
        dom::object skinObject;
        if (skinValue.get_object().get(skinObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::uint64_t index;
        if (auto error = skinObject["inverseBindMatrices"].get_uint64().get(index); error == SUCCESS) FASTGLTF_LIKELY {
            skin.inverseBindMatrices = static_cast<std::size_t>(index);
        } else if (error != NO_SUCH_FIELD) {
			return Error::InvalidGltf;
		}
        if (auto error = skinObject["skeleton"].get_uint64().get(index); error == SUCCESS) FASTGLTF_LIKELY {
            skin.skeleton = static_cast<std::size_t>(index);
        } else if (error != NO_SUCH_FIELD) {
			return Error::InvalidGltf;
		}

        dom::array jointsArray;
        if (skinObject["joints"].get_array().get(jointsArray) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }
		skin.joints = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(skin.joints), resourceAllocator.get(), 0);
        skin.joints.reserve(jointsArray.size());
        for (auto jointValue : jointsArray) {
            if (jointValue.get_uint64().get(index) != SUCCESS) FASTGLTF_UNLIKELY {
                return Error::InvalidGltf;
            }
            skin.joints.emplace_back(index);
        }

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = skinObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.skins.size(), Category::Skins, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		std::string_view name;
        if (skinObject["name"].get_string().get(name) == SUCCESS) {
	        skin.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(skin.name), resourceAllocator.get(), name);
        }
        asset.skins.emplace_back(std::move(skin));
    }

	return Error::None;
}

fg::Error fg::Parser::parseTextures(simdjson::dom::array& textures, Asset& asset) {
    using namespace simdjson;

    asset.textures.reserve(textures.size());
    for (auto textureValue : textures) {
        Texture texture;
        dom::object textureObject;
        if (textureValue.get_object().get(textureObject) != SUCCESS) FASTGLTF_UNLIKELY {
            return Error::InvalidGltf;
        }

        std::uint64_t sourceIndex;
        if (auto error = textureObject["source"].get_uint64().get(sourceIndex); error == SUCCESS) FASTGLTF_LIKELY {
            texture.imageIndex = static_cast<std::size_t>(sourceIndex);
        } else if (error != NO_SUCH_FIELD) {
			return Error::InvalidGltf;
		}

        dom::object extensionsObject;
        if (auto error = textureObject["extensions"].get_object().get(extensionsObject); error == SUCCESS) FASTGLTF_LIKELY {
			if (!parseTextureExtensions(texture, extensionsObject, config.extensions)) {
				return Error::InvalidGltf;
			}
        } else if (error != NO_SUCH_FIELD) {
			return Error::InvalidGltf;
		}

        // The index of the sampler used by this texture. When undefined, a sampler with
        // repeat wrapping and auto filtering SHOULD be used.
        std::uint64_t samplerIndex;
        if (auto error = textureObject["sampler"].get_uint64().get(samplerIndex); error == SUCCESS) FASTGLTF_LIKELY {
            texture.samplerIndex = static_cast<std::size_t>(samplerIndex);
        } else if (error != NO_SUCH_FIELD) {
			return Error::InvalidGltf;
		}

		if (config.extrasCallback != nullptr) {
			dom::object extrasObject;
			if (auto extrasError = textureObject["extras"].get_object().get(extrasObject); extrasError == SUCCESS) {
				config.extrasCallback(&extrasObject, asset.textures.size(), Category::Textures, config.userPointer);
			} else if (extrasError != NO_SUCH_FIELD) {
				return Error::InvalidGltf;
			}
		}

		std::string_view name;
        if (textureObject["name"].get_string().get(name) == SUCCESS) {
			texture.name = FASTGLTF_CONSTRUCT_PMR_RESOURCE(decltype(texture.name), resourceAllocator.get(), name);
        }

        asset.textures.emplace_back(std::move(texture));
    }

	return Error::None;
}

#pragma endregion

#pragma region Parser
fastgltf::GltfType fg::determineGltfFileType(GltfDataGetter& data) {
	// We'll try and read a BinaryGltfHeader from the buffer to see if the magic is correct.
	auto header = readBinaryHeader(data);
	data.reset();
	if (header.magic == binaryGltfHeaderMagic) {
		return GltfType::GLB;
	}

	// First, check if any of the first four characters is a '{'.
	std::array<std::uint8_t, 4> begin = {};
	data.read(begin.data(), begin.size());
	data.reset();
	for (const auto& i : begin) {
		if ((char)i == ' ')
			continue;
		if ((char)i == '{')
			return GltfType::glTF;
	}

	return GltfType::Invalid;
}

fg::Parser::Parser(Extensions extensionsToLoad) noexcept {
    std::call_once(crcInitialisation, initialiseCrc);
    jsonParser = std::make_unique<simdjson::dom::parser>();
    config.extensions = extensionsToLoad;
}

fg::Parser::Parser(Parser&& other) noexcept : jsonParser(std::move(other.jsonParser)), config(other.config) {}

fg::Parser& fg::Parser::operator=(Parser&& other) noexcept {
    jsonParser = std::move(other.jsonParser);
    config = other.config;
    return *this;
}

fg::Parser::~Parser() = default;

fg::Expected<fg::Asset> fg::Parser::loadGltf(GltfDataGetter& data, fs::path _directory, Options _options, Category categories) {
    auto type = fastgltf::determineGltfFileType(data);

    if (type == fastgltf::GltfType::glTF) {
        return loadGltfJson(data, std::move(_directory), _options, categories);
    }

    if (type == fastgltf::GltfType::GLB) {
        return loadGltfBinary(data, std::move(_directory), _options, categories);
    }

    return Error::InvalidFileData;
}

fg::Expected<fg::Asset> fg::Parser::loadGltfJson(GltfDataGetter& data, fs::path _directory, Options _options, Category categories) {
    using namespace simdjson;

	options = _options;
	directory = std::move(_directory);

#if !defined(__ANDROID__)
    // If we never have to load the files ourselves, we're fine with the directory being invalid/blank.
    if (std::error_code ec; hasBit(_options, Options::LoadExternalBuffers) && (!fs::is_directory(directory, ec) || ec)) {
        return Error::InvalidPath;
    }
#endif

	data.reset();
	auto jsonSpan = data.read(data.totalSize(), SIMDJSON_PADDING);
	padded_string_view view(reinterpret_cast<const std::uint8_t*>(jsonSpan.data()),
									  data.totalSize(),
									  data.totalSize() + SIMDJSON_PADDING);
	dom::object root;
    if (auto error = jsonParser->parse(view).get(root); error != SUCCESS) FASTGLTF_UNLIKELY {
	    return Error::InvalidJson;
    }

	return parse(root, categories);
}

fg::Expected<fg::Asset> fg::Parser::loadGltfBinary(GltfDataGetter& data, fs::path _directory, Options _options, Category categories) {
    using namespace simdjson;

	options = _options;
	directory = std::move(_directory);

    // If we never have to load the files ourselves, we're fine with the directory being invalid/blank.
    if (std::error_code ec; hasBit(options, Options::LoadExternalBuffers) && (!fs::is_directory(directory, ec) || ec)) {
	    return Error::InvalidPath;
    }

	data.reset();

    auto header = readBinaryHeader(data);
    if (header.magic != binaryGltfHeaderMagic) {
	    return Error::InvalidGLB;
    }
	if (header.version != 2) {
		return Error::UnsupportedVersion;
	}
    if (header.length > data.totalSize()) {
	    return Error::InvalidGLB;
    }

    // The glTF 2 spec specifies that in GLB files the order of chunks is predefined. Specifically,
    //  1. JSON chunk
    //  2. BIN chunk (optional)
    auto jsonChunk = readBinaryChunk(data);
    if (jsonChunk.chunkType != binaryGltfJsonChunkMagic) {
	    return Error::InvalidGLB;
    }

    // Create a string view of the JSON chunk in the GLB data buffer. The documentation of parse()
    // says the padding can be initialised to anything, apparently. Therefore, this should work.
	auto jsonSpan = data.read(jsonChunk.chunkLength, SIMDJSON_PADDING);
    simdjson::padded_string_view jsonChunkView(reinterpret_cast<const std::uint8_t*>(jsonSpan.data()),
                                               jsonChunk.chunkLength,
                                               jsonChunk.chunkLength + SIMDJSON_PADDING);

	simdjson::dom::object root;
    if (jsonParser->parse(jsonChunkView).get(root) != SUCCESS) FASTGLTF_UNLIKELY {
	    return Error::InvalidJson;
    }

    // Is there enough room for another chunk header?
    if (header.length > (data.bytesRead() + sizeof(BinaryGltfChunk))) {
        auto binaryChunk = readBinaryChunk(data);

        if (binaryChunk.chunkType != binaryGltfDataChunkMagic) {
	        return Error::InvalidGLB;
        }

		// TODO: Somehow allow skipping the binary part in the future?
		if (binaryChunk.chunkLength != 0) {
			if (config.mapCallback != nullptr) {
				auto info = config.mapCallback(binaryChunk.chunkLength, config.userPointer);
				if (info.mappedMemory != nullptr) {
					data.read(info.mappedMemory, binaryChunk.chunkLength);
					if (config.unmapCallback != nullptr) {
						config.unmapCallback(&info, config.userPointer);
					}
					glbBuffer = sources::CustomBuffer{info.customId, MimeType::None};
				}
			} else {
				StaticVector<std::byte> binaryData(binaryChunk.chunkLength);
				data.read(binaryData.data(), binaryChunk.chunkLength);

				sources::Array vectorData = {
					std::move(binaryData),
					MimeType::GltfBuffer,
				};
				glbBuffer = std::move(vectorData);
			}
		}
    }

	return parse(root, categories);
}

void fg::Parser::setBufferAllocationCallback(BufferMapCallback* mapCallback, BufferUnmapCallback* unmapCallback) noexcept {
	if (mapCallback == nullptr)
		unmapCallback = nullptr;
	config.mapCallback = mapCallback;
	config.unmapCallback = unmapCallback;
}

void fg::Parser::setBase64DecodeCallback(Base64DecodeCallback* decodeCallback) noexcept {
    config.decodeCallback = decodeCallback;
}

void fg::Parser::setExtrasParseCallback(ExtrasParseCallback *extrasCallback) noexcept {
	config.extrasCallback = extrasCallback;
}

void fg::Parser::setUserPointer(void* pointer) noexcept {
    config.userPointer = pointer;
}
#pragma endregion

#pragma region Exporter
void fg::prettyPrintJson(std::string& json) {
    std::size_t i = 0;
    std::size_t depth = 0;
    auto insertNewline = [&i, &depth, &json]() {
        json.insert(i, 1, '\n');
        json.insert(i + 1, depth, '\t');
        i += 1 + depth;
    };

    while (i < json.size()) {
        if (json[i] == '"') {
            // Skip to the end of the string
            do {
                ++i;
                if (json[i] == '"' && json[i - 1] != '\\') {
                    break;
                }
            } while (true);
            ++i; // Skip over the last "
        }

        switch (json[i]) {
            case '{': case '[':
                ++depth;
                ++i; // Insert \n after the character
                insertNewline();
                break;
            case '}': case ']':
                --depth;
                insertNewline();
                ++i; // Insert \n before the character
                break;
            case ',':
                ++i;  // Insert \n after the character
                insertNewline();
                break;
            default:
                ++i;
                break;
        }
    }
}

namespace fastgltf {
	static void escapeString(std::string& string) {
		std::size_t i = 0;
		do {
			switch (string[i]) {
				case '\"': {
					const std::string_view s = "\\\"";
					string.replace(i, 1, s);
					i += s.size();
					break;
				}
				case '\\': {
					const std::string_view s = "\\\\";
					string.replace(i, 1, s);
					i += s.size();
					break;
				}
			}
			++i;
		} while (i < string.size());
	}

	/**
	 * Normalizes the path using lexically_normal, calls generic_string to always use forward slashes,
	 * and escapes any necessary characters.
	 */
	static std::string normalizeAndFormatPath(fs::path& path) {
		auto string = path.lexically_normal().generic_string();
		escapeString(string);
		return string;
	}
} // namespace fastgltf

std::string fg::escapeString(std::string_view string) {
    std::string ret(string);
	escapeString(ret);
    return ret;
}

void fg::Exporter::setBufferPath(fs::path folder) {
    if (!folder.is_relative()) {
        return;
    }
    bufferFolder = std::move(folder);
}

void fg::Exporter::setImagePath(fs::path folder) {
    if (!folder.is_relative()) {
        return;
    }
    imageFolder = std::move(folder);
}

void fg::Exporter::setExtrasWriteCallback(ExtrasWriteCallback* callback) noexcept {
	extrasWriteCallback = callback;
}

void fg::Exporter::setUserPointer(void* pointer) noexcept {
	userPointer = pointer;
}

void fg::Exporter::writeAccessors(const Asset& asset, std::string& json) {
	if (asset.accessors.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += R"("accessors":[)";
	for (auto it = asset.accessors.begin(); it != asset.accessors.end(); ++it) {
		json += '{';

		if (it->byteOffset != 0) {
			json += "\"byteOffset\":" + std::to_string(it->byteOffset) + ',';
		}

		json += "\"count\":" + std::to_string(it->count) + ',';
		json += R"("type":")" + std::string(getAccessorTypeName(it->type)) + "\",";
		json += "\"componentType\":" + std::to_string(getGLComponentType(it->componentType));

		if (it->normalized) {
			json += ",\"normalized\":true";
		}

		if (it->bufferViewIndex.has_value()) {
			json += ",\"bufferView\":" + std::to_string(it->bufferViewIndex.value());
		}

		auto writeMinMax = [&](const decltype(Accessor::max)& ref, std::string_view name) {
			if (std::holds_alternative<std::monostate>(ref))
				return;
			json += ",\"" + std::string(name) + "\":[";
			std::visit(visitor {
				[](std::monostate) {},
				[&](const auto& arg) {
					for (auto it = arg.begin(); it != arg.end(); ++it) {
						json += std::to_string(*it);
						if (uabs(std::distance(arg.begin(), it)) + 1 < arg.size())
							json += ',';
					}
				}
			}, ref);
			json += ']';
		};
		writeMinMax(it->max, "max");
		writeMinMax(it->min, "min");

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.accessors.begin(), it)), fastgltf::Category::Accessors, userPointer);
			if (extras.has_value()) {
				json += std::string(",\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';

		json += '}';
		if (uabs(std::distance(asset.accessors.begin(), it)) + 1 <asset.accessors.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeAnimations(const Asset& asset, std::string& json)
{
	if (asset.animations.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += R"("animations":[)";
	for (auto it = asset.animations.begin(); it != asset.animations.end(); ++it) {
		json += '{';

		json += R"("channels":[)";
		for (auto ci = it->channels.begin(); ci != it->channels.end(); ++ci) {
			json += "{";
			json += R"("sampler":)" + std::to_string(ci->samplerIndex) + ",";
			json += R"("target":{)";
			if (ci->nodeIndex.has_value()) {
				json += R"("node":)" + std::to_string(ci->nodeIndex.value()) + ",";
			}
			json += R"("path":")";
			switch (ci->path) {
			case fg::AnimationPath::Translation:
				json += "translation";
				break;
			case fg::AnimationPath::Rotation:
				json += "rotation";
				break;
			case fg::AnimationPath::Scale:
				json += "scale";
				break;
			case fg::AnimationPath::Weights:
				json += "weights";
				break;
			}
			json += "\"}}";

			if (uabs(std::distance(it->channels.begin(), ci)) + 1 < it->channels.size())
				json += ',';
		}
		json += "],";

		json += R"("samplers":[)";
		for (auto si = it->samplers.begin(); si != it->samplers.end(); ++si) {
			json += '{';
			json += R"("input":)" + std::to_string(si->inputAccessor) + ',';

			if (si->interpolation != fg::AnimationInterpolation::Linear) {
				json += R"("interpolation":")";
				if (si->interpolation == fg::AnimationInterpolation::Step) {
					json += "STEP\",";
				} else {
					json += "CUBICSPLINE\",";
				}
			}

			json += R"("output":)" + std::to_string(si->outputAccessor);
			json += '}';

			if (uabs(std::distance(it->samplers.begin(), si)) + 1 < it->samplers.size())
				json += ',';
		}
		json += ']';

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.animations.begin(), it)), fastgltf::Category::Animations, userPointer);
			if (extras.has_value()) {
				json += std::string(",\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';

		json += '}';
		if (uabs(std::distance(asset.animations.begin(), it)) + 1 < asset.animations.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeBuffers(const Asset& asset, std::string& json) {
	if (asset.buffers.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"buffers\":[";
	for (auto it = asset.buffers.begin(); it != asset.buffers.end(); ++it) {
		json += '{';

        auto bufferIdx = uabs(std::distance(asset.buffers.begin(), it));
		std::visit(visitor {
			[&](auto&) {
				// Covers BufferView and CustomBuffer.
				errorCode = Error::InvalidGltf;
			},
			[&]([[maybe_unused]] const sources::Array& vector) {
                if (bufferIdx == 0 && exportingBinary) {
                    bufferPaths.emplace_back(std::nullopt);
                    return;
                }
                auto path = getBufferFilePath(asset, bufferIdx);
                json += std::string(R"("uri":")") + fg::normalizeAndFormatPath(path) + '"' + ',';
                bufferPaths.emplace_back(path);
			},
			[&]([[maybe_unused]] const sources::Vector& vector) {
				if (bufferIdx == 0 && exportingBinary) {
					bufferPaths.emplace_back(std::nullopt);
					return;
				}
				auto path = getBufferFilePath(asset, bufferIdx);
				json += std::string(R"("uri":")") + fg::normalizeAndFormatPath(path) + '"' + ',';
				bufferPaths.emplace_back(path);
			},
			[&]([[maybe_unused]] const sources::ByteView& view) {
				if (bufferIdx == 0 && exportingBinary) {
					bufferPaths.emplace_back(std::nullopt);
					return;
				}
                auto path = getBufferFilePath(asset, bufferIdx);
                json += std::string(R"("uri":")") + fg::normalizeAndFormatPath(path) + '"' + ',';
                bufferPaths.emplace_back(path);
			},
			[&](const sources::URI& uri) {
				json += std::string(R"("uri":")") + fg::escapeString(uri.uri.string()) + '"' + ',';
                bufferPaths.emplace_back(std::nullopt);
			},
			[&]([[maybe_unused]] const sources::Fallback& fallback) {
				json += R"("extensions":{"EXT_meshopt_compression":{"fallback":true}},)";
				bufferPaths.emplace_back(std::nullopt);
			},
		}, it->data);

		json += "\"byteLength\":" + std::to_string(it->byteLength);

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.buffers.begin(), it)), fastgltf::Category::Buffers, userPointer);
			if (extras.has_value()) {
				json += std::string(",\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';
		json += '}';
		if (uabs(std::distance(asset.buffers.begin(), it)) + 1 <asset.buffers.size())
			json += ',';
	}
	json += "]";
}

void fg::Exporter::writeBufferViews(const Asset& asset, std::string& json) {
	if (asset.bufferViews.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"bufferViews\":[";
	for (auto it = asset.bufferViews.begin(); it != asset.bufferViews.end(); ++it) {
		json += '{';

		json += "\"buffer\":" + std::to_string(it->bufferIndex) + ',';
		json += "\"byteLength\":" + std::to_string(it->byteLength);

		if (it->byteOffset != 0) {
			json += ",\"byteOffset\":" + std::to_string(it->byteOffset);
		}

		if (it->byteStride.has_value()) {
			json += ",\"byteStride\":" + std::to_string(it->byteStride.value());
		}

		if (it->target.has_value()) {
			json += ",\"target\":" + std::to_string(to_underlying(it->target.value()));
		}

        if (it->meshoptCompression != nullptr) {
            json += R"(,"extensions":{"EXT_meshopt_compression":{)";
            const auto& meshopt = *it->meshoptCompression;
            json += "\"buffer\":" + std::to_string(meshopt.bufferIndex);
            if (meshopt.byteOffset != 0) {
                json += ",\"byteOffset\":" + std::to_string(meshopt.byteOffset);
            }
            json += ",\"byteLength\":" + std::to_string(meshopt.byteLength);
            json += ",\"byteStride\":" + std::to_string(meshopt.byteStride);
            json += ",\"count\":" + std::to_string(meshopt.count);

            json += ",\"mode\":";
            if (meshopt.mode == MeshoptCompressionMode::Attributes) {
                json += "\"ATTRIBUTES\"";
            } else if (meshopt.mode == MeshoptCompressionMode::Triangles) {
                json += "\"TRIANGLES\"";
            } else if (meshopt.mode == MeshoptCompressionMode::Indices) {
                json += "\"INDICES\"";
            }
            if (meshopt.filter != MeshoptCompressionFilter::None) {
                json += ",\"filter\":";
                if (meshopt.filter == MeshoptCompressionFilter::Exponential) {
                    json += "\"EXPONENTIAL\"";
                } else if (meshopt.filter == MeshoptCompressionFilter::Quaternion) {
                    json += "\"QUATERNION\"";
                } else if (meshopt.filter == MeshoptCompressionFilter::Octahedral) {
                    json += "\"OCTAHEDRAL\"";
                }
            }
            json += "}}";
        }

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.bufferViews.begin(), it)), fastgltf::Category::BufferViews, userPointer);
			if (extras.has_value()) {
				json += std::string(",\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';

		json += '}';
		if (uabs(std::distance(asset.bufferViews.begin(), it)) + 1 <asset.bufferViews.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeCameras(const Asset& asset, std::string& json) {
	if (asset.cameras.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"cameras\":[";
	for (auto it = asset.cameras.begin(); it != asset.cameras.end(); ++it) {
		json += '{';

		std::visit(visitor {
			[](auto&) {},
			[&](const Camera::Perspective& perspective) {
				json += "\"perspective\":{";

				if (perspective.aspectRatio.has_value()) {
					json += "\"aspectRatio\":" + std::to_string(perspective.aspectRatio.value()) + ',';
				}

				json += "\"yfov\":" + std::to_string(perspective.yfov) + ',';

				if (perspective.zfar.has_value()) {
					json += "\"zfar\":" + std::to_string(perspective.zfar.value()) + ',';
				}

				json += "\"znear\":" + std::to_string(perspective.znear);

				json += R"(},"type":"perspective")";
			},
			[&](const Camera::Orthographic& orthographic) {
				json += "\"orthographic\":{";
				json += "\"xmag\":" + std::to_string(orthographic.xmag) + ',';
				json += "\"ymag\":" + std::to_string(orthographic.ymag) + ',';
				json += "\"zfar\":" + std::to_string(orthographic.zfar) + ',';
				json += "\"znear\":" + std::to_string(orthographic.znear);
				json += R"(},"type":"orthographic")";
			}
		}, it->camera);

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.cameras.begin(), it)), fastgltf::Category::Cameras, userPointer);
			if (extras.has_value()) {
				json += std::string(",\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';

		json += '}';
		if (uabs(std::distance(asset.cameras.begin(), it)) + 1 <asset.cameras.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeImages(const Asset& asset, std::string& json) {
	if (asset.images.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"images\":[";
	for (auto it = asset.images.begin(); it != asset.images.end(); ++it) {
		json += '{';

        auto imageIdx = uabs(std::distance(asset.images.begin(), it));
		std::visit(visitor {
			[&](auto&) {
				errorCode = Error::InvalidGltf;
			},
            [&](const sources::BufferView& bufferView) {
                json += std::string(R"("bufferView":)") + std::to_string(bufferView.bufferViewIndex) + ',';
				json += std::string(R"("mimeType":")") + std::string(getMimeTypeString(bufferView.mimeType)) + '"';
                imagePaths.emplace_back(std::nullopt);
            },
            [&](const sources::Array& vector) {
                auto path = getImageFilePath(asset, imageIdx, vector.mimeType);
                json += std::string(R"("uri":")") + fg::normalizeAndFormatPath(path) + '"';
				if (vector.mimeType != MimeType::None) {
					json += std::string(R"(,"mimeType":")") + std::string(getMimeTypeString(vector.mimeType)) + '"';
				}
                imagePaths.emplace_back(path);
            },
			[&](const sources::Vector& vector) {
				auto path = getImageFilePath(asset, imageIdx, vector.mimeType);
				json += std::string(R"("uri":")") + fg::normalizeAndFormatPath(path) + '"';
				if (vector.mimeType != MimeType::None) {
					json += std::string(R"(,"mimeType":")") + std::string(getMimeTypeString(vector.mimeType)) + '"';
				}
				imagePaths.emplace_back(path);
			},
			[&](const sources::URI& uri) {
				json += std::string(R"("uri":")") + fg::escapeString(uri.uri.string()) + '"';
                imagePaths.emplace_back(std::nullopt);
			},
		}, it->data);
		if (errorCode != Error::None)
			return;

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.images.begin(), it)), fastgltf::Category::Images, userPointer);
			if (extras.has_value()) {
				json += std::string(",\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';
		json += '}';
		if (uabs(std::distance(asset.images.begin(), it)) + 1 <asset.images.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeLights(const Asset& asset, std::string& json) {
	if (asset.lights.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += R"("KHR_lights_punctual":{"lights":[)";
	for (auto it = asset.lights.begin(); it != asset.lights.end(); ++it) {
		json += '{';

		// [1.0f, 1.0f, 1.0f] is the default.
		if (it->color[0] != 1.0f && it->color[1] != 1.0f && it->color[2] != 1.0f) {
			json += R"("color":[)";
			json += std::to_string(it->color[0]) + ',' + std::to_string(it->color[1]) + ',' + std::to_string(it->color[2]);
			json += "],";
		}

		if (it->intensity != 1.0f) {
			json += R"("intensity":)" + std::to_string(it->intensity) + ',';
		}

		switch (it->type) {
			case LightType::Directional: {
				json += R"("type":"directional")";
				break;
			}
			case LightType::Spot: {
				json += R"("type":"spot")";
				break;
			}
			case LightType::Point: {
				json += R"("type":"point")";
				break;
			}
		}

		if (it->range.has_value()) {
			json += R"(,"range":)" + std::to_string(it->range.value());
		}

		if (it->type == LightType::Spot) {
			if (it->innerConeAngle.has_value())
				json += R"("innerConeAngle":)" + std::to_string(it->innerConeAngle.value()) + ',';

			if (it->outerConeAngle.has_value())
				json += R"("outerConeAngle":)" + std::to_string(it->outerConeAngle.value()) + ',';
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';
		json += '}';
		if (uabs(std::distance(asset.lights.begin(), it)) + 1 <asset.lights.size())
			json += ',';
	}
	json += "]}";
}

void fg::Exporter::writeMaterials(const Asset& asset, std::string& json) {
	if (asset.materials.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"materials\":[";
	for (auto it = asset.materials.begin(); it != asset.materials.end(); ++it) {
		json += '{';

		json += "\"pbrMetallicRoughness\":{";
		if (it->pbrData.baseColorFactor != math::nvec4(1)) {
			json += R"("baseColorFactor":[)";
			json += std::to_string(it->pbrData.baseColorFactor[0]) + ',' + std::to_string(it->pbrData.baseColorFactor[1]) + ',' +
				std::to_string(it->pbrData.baseColorFactor[2]) + ',' + std::to_string(it->pbrData.baseColorFactor[3]);
			json += "]";
		}

		if (it->pbrData.baseColorTexture.has_value()) {
			if (json.back() != '{') json += ',';
			json += "\"baseColorTexture\":";
			writeTextureInfo(json, &it->pbrData.baseColorTexture.value());
		}

		if (it->pbrData.metallicFactor != 1.0f) {
			if (json.back() != '{') json += ',';
			json += "\"metallicFactor\":" + std::to_string(it->pbrData.metallicFactor);
		}

		if (it->pbrData.roughnessFactor != 1.0f) {
			if (json.back() != '{') json += ',';
			json += "\"roughnessFactor\":" + std::to_string(it->pbrData.roughnessFactor);
		}

		if (it->pbrData.metallicRoughnessTexture.has_value()) {
			if (json.back() != '{') json += ',';
			json += "\"metallicRoughnessTexture\":";
			writeTextureInfo(json, &it->pbrData.metallicRoughnessTexture.value());
		}

		json += '}';

		if (it->normalTexture.has_value()) {
			if (json.back() != ',') json += ',';
			json += "\"normalTexture\":";
			writeTextureInfo(json, &it->normalTexture.value(), TextureInfoType::NormalTexture);
		}

		if (it->occlusionTexture.has_value()) {
			if (json.back() != ',') json += ',';
			json += "\"occlusionTexture\":";
			writeTextureInfo(json, &it->occlusionTexture.value(), TextureInfoType::OcclusionTexture);
		}

		if (it->emissiveTexture.has_value()) {
			if (json.back() != ',') json += ',';
			json += "\"emissiveTexture\":";
			writeTextureInfo(json, &it->emissiveTexture.value());
		}

		if (it->emissiveFactor != math::nvec3(0)) {
			if (json.back() != ',') json += ',';
			json += R"("emissiveFactor":[)";
			json += std::to_string(it->emissiveFactor[0]) + ',' + std::to_string(it->emissiveFactor[1]) + ',' + std::to_string(it->emissiveFactor[2]);
			json += "],";
		}

		if (it->alphaMode != AlphaMode::Opaque) {
			if (json.back() != ',') json += ',';
			json += R"("alphaMode":)";
			if (it->alphaMode == AlphaMode::Blend) {
				json += "\"BLEND\"";
			} else if (it->alphaMode == AlphaMode::Mask) {
				json += "\"MASK\"";
			}
		}

		if (it->alphaMode == AlphaMode::Mask && it->alphaCutoff != 0.5f) {
			if (json.back() != ',') json += ',';
			json += R"("alphaCutoff":)" + std::to_string(it->alphaCutoff);
		}

		if (it->doubleSided) {
			if (json.back() != ',') json += ',';
			json += R"("doubleSided":true)";
		}

		if (json.back() != ',') json += ',';
		json += R"("extensions":{)";

		if (it->anisotropy) {
			json += R"("KHR_materials_anisotropy":{)";
			if (it->anisotropy->anisotropyStrength != 0.0f) {
				json += R"("anisotropyStrength":)" + std::to_string(it->anisotropy->anisotropyStrength);
			}
			if (it->anisotropy->anisotropyRotation != 0.0f) {
				if (json.back() != '{') json += ',';
				json += R"("anisotropyRotation":)" + std::to_string(it->anisotropy->anisotropyRotation);
			}
			if (it->anisotropy->anisotropyTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"anisotropyTexture\":";
				writeTextureInfo(json, &it->anisotropy->anisotropyTexture.value());
			}
			json += '}';
		}

		if (it->clearcoat) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_clearcoat":{)";
			if (it->clearcoat->clearcoatFactor != 0.0f) {
				json += R"("clearcoatFactor":)" + std::to_string(it->clearcoat->clearcoatFactor);
			}
			if (it->clearcoat->clearcoatTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"clearcoatTexture\":";
				writeTextureInfo(json, &it->clearcoat->clearcoatTexture.value());
			}
			if (it->clearcoat->clearcoatRoughnessFactor != 0.0f) {
				if (json.back() != '{') json += ',';
				json += R"("clearcoatRoughnessFactor":)" + std::to_string(it->clearcoat->clearcoatRoughnessFactor);
			}
			if (it->clearcoat->clearcoatRoughnessTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"clearcoatRoughnessTexture\":";
				writeTextureInfo(json, &it->clearcoat->clearcoatRoughnessTexture.value());
			}
			if (it->clearcoat->clearcoatNormalTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"clearcoatNormalTexture\":";
				writeTextureInfo(json, &it->clearcoat->clearcoatNormalTexture.value());
			}
			json += '}';
		}

		if (it->dispersion != 0.0f) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_dispersion":{"dispersion":)" + std::to_string(it->dispersion) + '}';
		}

		if (it->emissiveStrength != 1.0f) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_emissive_strength":{"emissiveStrength":)" + std::to_string(it->emissiveStrength) + '}';
		}

		if (it->ior != 1.5f) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_ior":{"ior":)" + std::to_string(it->ior) + '}';
		}

		if (it->iridescence) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_iridescence":{)";
			if (it->iridescence->iridescenceFactor != 0.0f) {
				json += R"("iridescenceFactor":)" + std::to_string(it->iridescence->iridescenceFactor);
			}
			if (it->iridescence->iridescenceTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"iridescenceTexture\":";
				writeTextureInfo(json, &it->iridescence->iridescenceTexture.value());
			}
			if (it->iridescence->iridescenceIor != 1.3f) {
				if (json.back() != '{') json += ',';
				json += R"("iridescenceIor":)" + std::to_string(it->iridescence->iridescenceIor);
			}
			if (it->iridescence->iridescenceThicknessMinimum != 100.0f) {
				if (json.back() != '{') json += ',';
				json += R"("iridescenceThicknessMinimum":)" + std::to_string(it->iridescence->iridescenceThicknessMinimum);
			}
			if (it->iridescence->iridescenceThicknessMaximum != 400.0f) {
				if (json.back() != '{') json += ',';
				json += R"("iridescenceThicknessMaximum":)" + std::to_string(it->iridescence->iridescenceThicknessMaximum);
			}
			if (it->iridescence->iridescenceThicknessTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"iridescenceThicknessTexture\":";
				writeTextureInfo(json, &it->iridescence->iridescenceThicknessTexture.value());
			}
			json += '}';
		}

		if (it->sheen) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_sheen":{)";
			if (it->sheen->sheenColorFactor != math::nvec3(0)) {
				json += R"("sheenColorFactor":[)" +
					std::to_string(it->sheen->sheenColorFactor[0]) + ',' +
					std::to_string(it->sheen->sheenColorFactor[1]) + ',' +
					std::to_string(it->sheen->sheenColorFactor[2]) + ']';
			}
			if (it->sheen->sheenColorTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"sheenColorTexture\":";
				writeTextureInfo(json, &it->sheen->sheenColorTexture.value());
			}
			if (it->sheen->sheenRoughnessFactor != 0.0f) {
				if (json.back() != '{') json += ',';
				json += R"("sheenRoughnessFactor":)" + std::to_string(it->sheen->sheenRoughnessFactor);
			}
			if (it->sheen->sheenRoughnessTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"sheenRoughnessTexture\":";
				writeTextureInfo(json, &it->sheen->sheenRoughnessTexture.value());
			}
			json += '}';
		}

		if (it->specular) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_specular":{)";
			if (it->specular->specularFactor != 1.0f) {
				json += R"("specularFactor":)" + std::to_string(it->specular->specularFactor);
			}
			if (it->specular->specularTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"specularTexture\":";
				writeTextureInfo(json, &it->specular->specularTexture.value());
			}
			if (it->specular->specularColorFactor != math::nvec3(1)) {
				if (json.back() != '{') json += ',';
				json += R"("specularColorFactor":[)" +
						std::to_string(it->specular->specularColorFactor[0]) + ',' +
						std::to_string(it->specular->specularColorFactor[1]) + ',' +
						std::to_string(it->specular->specularColorFactor[2]) + ']';
			}
			if (it->specular->specularColorTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"specularColorTexture\":";
				writeTextureInfo(json, &it->specular->specularColorTexture.value());
			}
			json += '}';
		}

		if (it->transmission) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_transmission":{)";
			if (it->transmission->transmissionFactor != 0.0f) {
				json += R"("transmissionFactor":)" + std::to_string(it->transmission->transmissionFactor);
			}
			if (it->transmission->transmissionTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"transmissionTexture\":";
				writeTextureInfo(json, &it->transmission->transmissionTexture.value());
			}
			json += '}';
		}

		if (it->unlit) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_unlit":{})";
		}

		if (it->volume) {
			if (json.back() == '}') json += ',';
			json += R"("KHR_materials_volume":{)";
			if (it->volume->thicknessFactor != 0.0f) {
				json += R"("thicknessFactor":)" + std::to_string(it->volume->thicknessFactor);
			}
			if (it->volume->thicknessTexture.has_value()) {
				if (json.back() != '{') json += ',';
				json += "\"thicknessTexture\":";
				writeTextureInfo(json, &it->volume->thicknessTexture.value());
			}
			if (it->volume->attenuationDistance != std::numeric_limits<num>::infinity()) {
				if (json.back() != '{') json += ',';
				json += R"("attenuationDistance":)" + std::to_string(it->volume->attenuationDistance);
			}
			if (it->volume->attenuationColor != math::nvec3(1)) {
				if (json.back() != '{') json += ',';
				json += R"("attenuationColor":[)" +
						std::to_string(it->volume->attenuationColor[0]) + ',' +
						std::to_string(it->volume->attenuationColor[1]) + ',' +
						std::to_string(it->volume->attenuationColor[2]) + ']';
			}
			json += '}';
		}

		if (it->packedNormalMetallicRoughnessTexture.has_value()) {
			if (json.back() == '}') json += ',';
			json += R"("MSFT_packing_normalRoughnessMetallic":{"normalRoughnessMetallicTexture":)";
			writeTextureInfo(json, &it->packedNormalMetallicRoughnessTexture.value());
			json += '}';
		}

		if (it->packedOcclusionRoughnessMetallicTextures) {
			if (json.back() == '}') json += ',';
			json += R"("MSFT_packing_normalRoughnessMetallic":{)";
			if (it->packedOcclusionRoughnessMetallicTextures->occlusionRoughnessMetallicTexture.has_value()) {
				json += R"("occlusionRoughnessMetallicTexture":)";
				writeTextureInfo(json, &it->packedOcclusionRoughnessMetallicTextures->occlusionRoughnessMetallicTexture.value());
			}
			if (it->packedOcclusionRoughnessMetallicTextures->roughnessMetallicOcclusionTexture.has_value()) {
				json += R"("roughnessMetallicOcclusionTexture":)";
				writeTextureInfo(json, &it->packedOcclusionRoughnessMetallicTextures->roughnessMetallicOcclusionTexture.value());
			}
			if (it->packedOcclusionRoughnessMetallicTextures->normalTexture.has_value()) {
				json += R"("normalTexture":)";
				writeTextureInfo(json, &it->packedOcclusionRoughnessMetallicTextures->normalTexture.value());
			}
			json += '}';
		}

		json += '}';

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.materials.begin(), it)), fastgltf::Category::Materials, userPointer);
			if (extras.has_value()) {
				if (json.back() != '{')
					json += ',';
				json += std::string("\"extras\":") + *extras;
			}
		}

		if (!it->name.empty()) {
			if (json.back() != ',') json += ',';
			json += R"("name":")" + fg::escapeString(it->name) + '"';
		}
		json += '}';
		if (uabs(std::distance(asset.materials.begin(), it)) + 1 <asset.materials.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeMeshes(const Asset& asset, std::string& json) {
	if (asset.meshes.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"meshes\":[";
	for (auto it = asset.meshes.begin(); it != asset.meshes.end(); ++it) {
		json += '{';

        if (!it->primitives.empty()) {
            json += R"("primitives":[)";
            auto itp = it->primitives.begin();
            while (itp != it->primitives.end()) {
                json += '{';

                {
                    json += R"("attributes":{)";
                    for (auto ita = itp->attributes.begin(); ita != itp->attributes.end(); ++ita) {
                        json += '"' + std::string(ita->name) + "\":" + std::to_string(ita->accessorIndex);
                        if (uabs(std::distance(itp->attributes.begin(), ita)) + 1 <itp->attributes.size())
                            json += ',';
                    }
                    json += '}';
                }

                if (itp->indicesAccessor.has_value()) {
                    json += R"(,"indices":)" + std::to_string(itp->indicesAccessor.value());
                }

                if (itp->materialIndex.has_value()) {
                    json += R"(,"material":)" + std::to_string(itp->materialIndex.value());
                }

                if (itp->type != PrimitiveType::Triangles) {
                    json += R"(,"mode":)" + std::to_string(to_underlying(itp->type));
                }

				if (!itp->mappings.empty()) {
					json += R"(,"extensions":{"KHR_materials_variants":{"mappings":[)";
					// TODO: We should optimise to avoid writing multiple objects for the same material index
					for (std::size_t i = 0; i < asset.materialVariants.size(); ++i) {
						if (!itp->mappings[i].has_value())
							continue;
						if (json.back() == '}')
							json += ',';
						json += "{\"material\":" + std::to_string(itp->mappings[i].value()) + ",\"variants\":[" + std::to_string(i) + "]}";
					}
					json += "]}}";
				}

                json += '}';
                ++itp;
                if (uabs(std::distance(it->primitives.begin(), itp)) < it->primitives.size())
                    json += ',';
            }
            json += ']';
        }

		if (!it->weights.empty()) {
			if (json.back() != '{')
				json += ',';
			json += R"("weights":[)";
			auto itw = it->weights.begin();
			while (itw != it->weights.end()) {
				json += std::to_string(*itw);
				++itw;
				if (uabs(std::distance(it->weights.begin(), itw)) < it->weights.size())
					json += ',';
			}
			json += ']';
		}

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.meshes.begin(), it)), fastgltf::Category::Meshes, userPointer);
			if (extras.has_value()) {
				if (json.back() != '{')
					json += ',';
				json += std::string("\"extras\":") + *extras;
			}
		}

		if (!it->name.empty()) {
            if (json.back() != '{')
                json += ',';
            json += R"("name":")" + fg::escapeString(it->name) + '"';
        }
		json += '}';
		if (uabs(std::distance(asset.meshes.begin(), it)) + 1 <asset.meshes.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeNodes(const Asset& asset, std::string& json) {
	if (asset.nodes.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"nodes\":[";
	for (auto it = asset.nodes.begin(); it != asset.nodes.end(); ++it) {
		json += '{';

		if (it->meshIndex.has_value()) {
			json += R"("mesh":)" + std::to_string(it->meshIndex.value());
		}
		if (it->cameraIndex.has_value()) {
			if (json.back() != '{')
				json += ',';
			json += R"("camera":)" + std::to_string(it->cameraIndex.value());
		}
		if (it->skinIndex.has_value()) {
			if (json.back() != '{')
				json += ',';
			json += R"("skin":)" + std::to_string(it->skinIndex.value());
		}

		if (!it->children.empty()) {
            if (json.back() != '{')
                json += ',';
			json += R"("children":[)";
			auto itc = it->children.begin();
			while (itc != it->children.end()) {
				json += std::to_string(*itc);
				++itc;
				if (uabs(std::distance(it->children.begin(), itc)) < it->children.size())
					json += ',';
			}
			json += ']';
		}

		if (!it->weights.empty()) {
            if (json.back() != '{')
                json += ',';
			json += R"("weights":[)";
			auto itw = it->weights.begin();
			while (itw != it->weights.end()) {
				json += std::to_string(*itw);
				++itw;
				if (uabs(std::distance(it->weights.begin(), itw)) < it->weights.size())
					json += ',';
			}
			json += ']';
		}

		std::visit(visitor {
			[&](const TRS& trs) {
				if (trs.rotation != math::fquat(0.f, 0.f, 0.f, 1.f)) {
                    if (json.back() != '{')
                        json += ',';
					json += R"("rotation":[)";
					json += std::to_string(trs.rotation[0]) + ',' + std::to_string(trs.rotation[1]) + ',' + std::to_string(trs.rotation[2]) + ',' + std::to_string(trs.rotation[3]);
					json += "]";
				}

				if (trs.scale != math::fvec3(1.f)) {
                    if (json.back() != '{')
                        json += ',';
					json += R"("scale":[)";
					json += std::to_string(trs.scale[0]) + ',' + std::to_string(trs.scale[1]) + ',' + std::to_string(trs.scale[2]);
					json += "]";
				}

				if (trs.translation != math::fvec3(0.f)) {
                    if (json.back() != '{')
                        json += ',';
					json += R"("translation":[)";
					json += std::to_string(trs.translation[0]) + ',' + std::to_string(trs.translation[1]) + ',' + std::to_string(trs.translation[2]);
					json += "]";
				}
			},
			[&](const math::fmat4x4& matrix) {
				if (json.back() != '{')
					json += ',';
				json += R"("matrix":[)";
				for (std::size_t i = 0; i < matrix.columns(); ++i) {
					for (std::size_t j = 0; j < matrix.rows(); ++j) {
						json += std::to_string(matrix.col(i)[j]);
						if (i * matrix.columns() + j + 1 < matrix.columns() * matrix.rows()) {
							json += ',';
						}
					}
				}
				json += ']';
			},
		}, it->transform);

        if (!it->instancingAttributes.empty() || it->lightIndex.has_value()) {
			if (json.back() != '{') json += ',';
			json += R"("extensions":{)";
			if (!it->instancingAttributes.empty()) {
				json += R"("EXT_mesh_gpu_instancing":{"attributes":{)";
				for (auto ait = it->instancingAttributes.begin(); ait != it->instancingAttributes.end(); ++ait) {
					json += '"' + std::string(ait->name) + "\":" + std::to_string(ait->accessorIndex);
					if (uabs(std::distance(it->instancingAttributes.begin(), ait)) + 1 <
						it->instancingAttributes.size())
						json += ',';
				}
				json += "}}";
			}
			if (it->lightIndex.has_value()) {
				if (json.back() != '{') json += ',';
				json += R"("KHR_lights_punctual":{"light":)" + std::to_string(it->lightIndex.value()) + "}";
			}
            json += "}";
        }

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.nodes.begin(), it)), fastgltf::Category::Nodes, userPointer);
			if (extras.has_value()) {
				if (json.back() != '{')
					json += ',';
				json += std::string("\"extras\":") + *extras;
			}
		}

		if (!it->name.empty()) {
            if (json.back() != '{')
                json += ',';
			json += R"("name":")" + fg::escapeString(it->name) + '"';
		}
		json += '}';
		if (uabs(std::distance(asset.nodes.begin(), it)) + 1 <asset.nodes.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeSamplers(const Asset& asset, std::string& json) {
	if (asset.samplers.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"samplers\":[";
	for (auto it = asset.samplers.begin(); it != asset.samplers.end(); ++it) {
		json += '{';

		if (it->magFilter.has_value()) {
			json += R"("magFilter":)" + std::to_string(to_underlying(it->magFilter.value()));
		}
		if (it->minFilter.has_value()) {
			if (json.back() != '{') json += ',';
			json += R"("minFilter":)" + std::to_string(to_underlying(it->minFilter.value()));
		}
		if (it->wrapS != Wrap::Repeat) {
			if (json.back() != '{') json += ',';
			json += R"("wrapS":)" + std::to_string(to_underlying(it->wrapS));
		}
		if (it->wrapT != Wrap::Repeat) {
			if (json.back() != '{') json += ',';
			json += R"("wrapT":)" + std::to_string(to_underlying(it->wrapT));
		}

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.samplers.begin(), it)), fastgltf::Category::Samplers, userPointer);
			if (extras.has_value()) {
				if (json.back() != '{')
					json += ',';
				json += std::string("\"extras\":") + *extras;
			}
		}

		if (!it->name.empty()) {
			if (json.back() != '{') json += ',';
			json += R"("name":")" + fg::escapeString(it->name) + '"';
		}
		json += '}';
		if (uabs(std::distance(asset.samplers.begin(), it)) + 1 <asset.samplers.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeScenes(const Asset& asset, std::string& json) {
	if (asset.scenes.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	if (asset.defaultScene.has_value()) {
		json += "\"scene\":" + std::to_string(asset.defaultScene.value()) + ',';
	}

	json += "\"scenes\":[";
	for (auto it = asset.scenes.begin(); it != asset.scenes.end(); ++it) {
		json += '{';

		json += R"("nodes":[)";
		auto itn = it->nodeIndices.begin();
		while (itn != it->nodeIndices.end()) {
			json += std::to_string(*itn);
			++itn;
			if (uabs(std::distance(it->nodeIndices.begin(), itn)) < it->nodeIndices.size())
				json += ',';
		}
		json += ']';

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.scenes.begin(), it)), fastgltf::Category::Scenes, userPointer);
			if (extras.has_value()) {
				if (json.back() != '{')
					json += ',';
				json += std::string("\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';
		json += '}';
		if (uabs(std::distance(asset.scenes.begin(), it)) + 1 <asset.scenes.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeSkins(const Asset& asset, std::string& json) {
	if (asset.skins.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"skins\":[";
	for (auto it = asset.skins.begin(); it != asset.skins.end(); ++it) {
		json += '{';

		if (it->inverseBindMatrices.has_value())
			json += R"("inverseBindMatrices":)" + std::to_string(it->inverseBindMatrices.value()) + ',';

		if (it->skeleton.has_value())
			json += R"("skeleton":)" + std::to_string(it->skeleton.value()) + ',';

		json += R"("joints":[)";
		auto itj = it->joints.begin();
		while (itj != it->joints.end()) {
			json += std::to_string(*itj);
			++itj;
			if (uabs(std::distance(it->joints.begin(), itj)) < it->joints.size())
				json += ',';
		}
		json += ']';

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.skins.begin(), it)), fastgltf::Category::Skins, userPointer);
			if (extras.has_value()) {
				if (json.back() != '{')
					json += ',';
				json += std::string("\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';
		json += '}';
		if (uabs(std::distance(asset.skins.begin(), it)) + 1 <asset.skins.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeTextures(const Asset& asset, std::string& json) {
	if (asset.textures.empty())
		return;
	if (json.back() == ']' || json.back() == '}')
		json += ',';

	json += "\"textures\":[";
	for (auto it = asset.textures.begin(); it != asset.textures.end(); ++it) {
		json += '{';

		if (it->samplerIndex.has_value())
			json += R"("sampler":)" + std::to_string(it->samplerIndex.value());

		if (it->imageIndex.has_value()) {
			if (json.back() != '{') json += ',';
			json += R"("source":)" + std::to_string(it->imageIndex.value());
		}

		if (it->basisuImageIndex.has_value() || it->ddsImageIndex.has_value() || it->webpImageIndex.has_value()) {
			if (json.back() != '{') json += ',';
			json += R"("extensions":{)";
			if (it->basisuImageIndex.has_value()) {
				json += R"("KHR_texture_basisu":{"source":)" + std::to_string(it->basisuImageIndex.value()) + '}';
			}
			if (it->ddsImageIndex.has_value()) {
				if (json.back() == '}') json += ',';
				json += R"("MSFT_texture_dds":{"source":)" + std::to_string(it->ddsImageIndex.value()) + '}';
			}
			if (it->webpImageIndex.has_value()) {
				if (json.back() == '}') json += ',';
				json += R"("EXT_texture_webp":{"source":)" + std::to_string(it->webpImageIndex.value()) + '}';
			}
			json += "}";
		}

		if (extrasWriteCallback != nullptr) {
			auto extras = extrasWriteCallback(uabs(std::distance(asset.textures.begin(), it)), fastgltf::Category::Textures, userPointer);
			if (extras.has_value()) {
				if (json.back() != '{')
					json += ',';
				json += std::string("\"extras\":") + *extras;
			}
		}

		if (!it->name.empty())
			json += R"(,"name":")" + fg::escapeString(it->name) + '"';
		json += '}';
		if (uabs(std::distance(asset.textures.begin(), it)) + 1 <asset.textures.size())
			json += ',';
	}
	json += ']';
}

void fg::Exporter::writeExtensions(const fastgltf::Asset& asset, std::string& json) {
	if (json.back() == ']' || json.back() == '}')
		json += ',';
    json += "\"extensions\":{";

    writeLights(asset, json);

	if (!asset.materialVariants.empty()) {
		if (json.back() != '{')
			json += ',';
		json += R"("KHR_materials_variants":{"variants":[)";
		for (const auto& variant : asset.materialVariants) {
			if (json.back() == '}')
				json += ',';
			json += R"({"name":")" + variant + "\"}";
		}
		json += "]}";
	}

    json += '}';
}

fs::path fg::Exporter::getBufferFilePath(const Asset& asset, std::size_t index) {
    const auto& bufferName = asset.buffers[index].name;
    if (bufferName.empty()) {
        return bufferFolder / ("buffer" + std::to_string(index) + ".bin");
    }
	return bufferFolder / (bufferName + ".bin");
}

fs::path fg::Exporter::getImageFilePath(const Asset& asset, std::size_t index, MimeType mimeType) {
    std::string_view extension;
    switch (mimeType) {
        case MimeType::JPEG:
            extension = ".jpeg";
            break;
        case MimeType::PNG:
            extension = ".png";
            break;
		case MimeType::KTX2:
			extension = ".ktx2";
			break;
		case MimeType::DDS:
			extension = ".dds";
			break;
		default:
		case MimeType::None:
		case MimeType::GltfBuffer:
		case MimeType::OctetStream:
			extension = ".bin";
			break;
    }

    const auto& imageName = asset.images[index].name;
	if (imageName.empty()) {
		return imageFolder / ("image" + std::to_string(index) + std::string(extension));
	}
	return imageFolder / (std::string(imageName) + std::string(extension));
}

std::string fg::Exporter::writeJson(const fastgltf::Asset &asset) {
    // Fairly rudimentary approach of just composing the JSON string using a std::string.
    std::string outputString;

    outputString += "{";

    // Write asset info
    outputString += "\"asset\":{";
    if (asset.assetInfo.has_value()) {
        if (!asset.assetInfo->copyright.empty())
            outputString += R"("copyright":")" + fg::escapeString(asset.assetInfo->copyright) + "\",";
        if (!asset.assetInfo->generator.empty())
            outputString += R"("generator":")" + fg::escapeString(asset.assetInfo->generator) + "\",";
        outputString += R"("version":")" + asset.assetInfo->gltfVersion + '"';
    } else {
        outputString += R"("generator":"fastgltf",)";
        outputString += R"("version":"2.0")";
    }
    outputString += '}';

	// Write extension usage info
	if (!asset.extensionsUsed.empty()) {
		if (outputString.back() != '{') outputString += ',';
		outputString += "\"extensionsUsed\":[";
		for (auto it = asset.extensionsUsed.begin(); it != asset.extensionsUsed.end(); ++it) {
			outputString += '\"' + *it + '\"';
			if (uabs(std::distance(asset.extensionsUsed.begin(), it)) + 1 <asset.extensionsUsed.size())
				outputString += ',';
		}
		outputString += ']';
	}
	if (!asset.extensionsRequired.empty()) {
		if (outputString.back() != '{') outputString += ',';
		outputString += "\"extensionsRequired\":[";
		for (auto it = asset.extensionsRequired.begin(); it != asset.extensionsRequired.end(); ++it) {
			outputString += '\"' + *it + '\"';
			if (uabs(std::distance(asset.extensionsRequired.begin(), it)) + 1 <asset.extensionsRequired.size())
				outputString += ',';
		}
		outputString += ']';
	}

    writeAccessors(asset, outputString);
    writeAnimations(asset, outputString);
    writeBuffers(asset, outputString);
    writeBufferViews(asset, outputString);
    writeCameras(asset, outputString);
    writeImages(asset, outputString);
    writeMaterials(asset, outputString);
    writeMeshes(asset, outputString);
    writeNodes(asset, outputString);
    writeSamplers(asset, outputString);
    writeScenes(asset, outputString);
    writeSkins(asset, outputString);
    writeTextures(asset, outputString);
    writeExtensions(asset, outputString);

    outputString += "}";

    if (hasBit(options, ExportOptions::PrettyPrintJson)) {
        prettyPrintJson(outputString);
    }

    return outputString;
}

fg::Expected<fg::ExportResult<std::string>> fg::Exporter::writeGltfJson(const Asset& asset, ExportOptions _options) {
    bufferPaths.clear();
    imagePaths.clear();
    options = _options;
	exportingBinary = false;

    if (hasBit(options, ExportOptions::ValidateAsset)) {
        auto validation = validate(asset);
        if (validation != Error::None) {
            return validation;
        }
    }

    // Fairly rudimentary approach of just composing the JSON string using a std::string.
    std::string outputString = writeJson(asset);
    if (errorCode != Error::None) {
		return errorCode;
    }

    ExportResult<std::string> result;
    result.output = std::move(outputString);
    result.bufferPaths = std::move(bufferPaths);
    result.imagePaths = std::move(imagePaths);
    return std::move(result);
}

fg::Expected<fg::ExportResult<std::vector<std::byte>>> fg::Exporter::writeGltfBinary(const Asset& asset, ExportOptions _options) {
    bufferPaths.clear();
    imagePaths.clear();
    options = _options;
	exportingBinary = true;

    options &= (~ExportOptions::PrettyPrintJson);

    ExportResult<std::vector<std::byte>> result;
    auto json = writeJson(asset);
    if (errorCode != Error::None) {
		return errorCode;
    }

    result.bufferPaths = std::move(bufferPaths);
    result.imagePaths = std::move(imagePaths);

	// TODO: Add ExportOption enumeration for disabling this?
    const bool withEmbeddedBuffer = !asset.buffers.empty()
			// We only support writing Vectors and ByteViews as embedded buffers
			&& (std::holds_alternative<sources::Array>(asset.buffers.front().data) || std::holds_alternative<sources::ByteView>(asset.buffers.front().data) || std::holds_alternative<sources::Vector>(asset.buffers.front().data))
			&& asset.buffers.front().byteLength < std::numeric_limits<decltype(BinaryGltfChunk::chunkLength)>::max();

    std::size_t binarySize = 0;
    binarySize += sizeof(BinaryGltfHeader); // glTF header
    binarySize += sizeof(BinaryGltfChunk) + alignUp(json.size(), 4); // JSON chunk
    if (withEmbeddedBuffer) {
        binarySize += sizeof(BinaryGltfChunk) + alignUp(asset.buffers.front().byteLength, 4); // BIN chunk
    }

	// A GLB is limited to 2^32 bytes since the length field in the file header is a 32-bit integer.
	if (binarySize >= static_cast<std::size_t>(std::numeric_limits<decltype(BinaryGltfHeader::length)>::max())) {
		return Error::InvalidGLB;
	}

    result.output.resize(binarySize);
    auto write = [output = result.output.data()](const void* data, std::size_t size) mutable {
        std::memcpy(output, data, size);
        output += size;
    };

	// Write glTF header
	BinaryGltfHeader header {};
	header.magic = binaryGltfHeaderMagic;
	header.version = 2;
	header.length = static_cast<std::uint32_t>(binarySize);
	auto headerBytes = writeBinaryHeader(header);
	write(headerBytes.data(), headerBytes.size());

	// Write JSON chunk
	BinaryGltfChunk jsonChunk {};
	jsonChunk.chunkType = binaryGltfJsonChunkMagic;
	jsonChunk.chunkLength = static_cast<std::uint32_t>(alignUp(json.size(), 4));
	auto chunkBytes = writeBinaryChunk(jsonChunk);
	write(chunkBytes.data(), chunkBytes.size());

    write(json.data(), json.size());

    // 4 bytes padding with space character (0x20)
    for (std::size_t i = json.size(); i % 4 != 0; ++i) {
        static constexpr std::uint8_t space = 0x20U;
        write(&space, sizeof space);
    }

    if (withEmbeddedBuffer) {
        const auto& buffer = asset.buffers.front();

        // Write BIN chunk
        BinaryGltfChunk dataChunk {};
        dataChunk.chunkType = binaryGltfDataChunkMagic;
        dataChunk.chunkLength = static_cast<std::uint32_t>(alignUp(buffer.byteLength, 4));
		chunkBytes = writeBinaryChunk(dataChunk);
		write(chunkBytes.data(), chunkBytes.size());

		std::visit(visitor {
			[](auto&) {},
			[&](const sources::Array& vector) {
				write(vector.bytes.data(), buffer.byteLength);
			},
			[&](const sources::Vector& vector) {
				write(vector.bytes.data(), buffer.byteLength);
			},
			[&](const sources::ByteView& byteView) {
				write(byteView.bytes.data(), buffer.byteLength);
			},
		}, buffer.data);

        // 4 bytes padding with zeros
        for (std::size_t i = buffer.byteLength; i % 4 != 0; ++i) {
            static constexpr std::uint8_t zero = 0x0U;
            write(&zero, sizeof zero);
        }
    }

    return std::move(result);
}

namespace fastgltf {
	bool writeFile(const DataSource& dataSource, const fs::path& baseFolder, const fs::path& filePath) {
		// Get the final normalized path. TODO: Perhaps move these filesystem checks to the parent function?
		auto finalPath = (baseFolder / filePath).lexically_normal();
		if (std::error_code ec; !fs::exists(finalPath.parent_path(), ec) || ec) {
			// If the parent folder of the destination file does not exist, we'll create it.
			fs::create_directory(finalPath.parent_path(), ec);
			if (ec) {
				return false;
			}
		}

		return std::visit(visitor {
			[](auto&) {
				return false;
			},
			[&](const sources::Array& vector) {
				std::ofstream file(finalPath, std::ios::out | std::ios::binary);
				if (!file.is_open())
					return false;
				file.write(reinterpret_cast<const char *>(vector.bytes.data()),
						   static_cast<std::streamsize>(vector.bytes.size()));
				file.close();
				return file.good();
			},
			[&](const sources::Vector& vector) {
				std::ofstream file(finalPath, std::ios::out | std::ios::binary);
				if (!file.is_open())
					return false;
				file.write(reinterpret_cast<const char *>(vector.bytes.data()),
						   static_cast<std::streamsize>(vector.bytes.size()));
				file.close();
				return file.good();
			},
			[&](const sources::ByteView& view) {
				std::ofstream file(finalPath, std::ios::out | std::ios::binary);
				if (!file.is_open())
					return false;
				file.write(reinterpret_cast<const char *>(view.bytes.data()),
						   static_cast<std::streamsize>(view.bytes.size()));
				file.close();
				return file.good();
			},
		}, dataSource);
	}

	template<typename T>
	bool writeFiles(const Asset& asset, ExportResult<T> &result, fs::path baseFolder) {
		for (std::size_t i = 0; i < asset.buffers.size(); ++i) {
			auto& path = result.bufferPaths[i];
			if (path.has_value()) {
				if (!writeFile(asset.buffers[i].data, baseFolder, path.value()))
					return false;
			}
		}

		for (std::size_t i = 0; i < asset.images.size(); ++i) {
			auto& path = result.imagePaths[i];
			if (path.has_value()) {
				if (!writeFile(asset.images[i].data, baseFolder, path.value()))
					return false;
			}
		}
		return true;
	}
} // namespace fastgltf

fg::Error fg::FileExporter::writeGltfJson(const Asset& asset, fs::path target, ExportOptions _options) {
	if (std::error_code ec; !fs::exists(target.parent_path(), ec) || ec) {
		fs::create_directory(target.parent_path(), ec);
		if (ec) {
			return Error::InvalidPath;
		}
	}

	auto expected = Exporter::writeGltfJson(asset, _options);

    if (!expected) {
        return expected.error();
    }
    auto& result = expected.get();

    std::ofstream file(target, std::ios::out);
    if (!file.is_open()) {
        return fg::Error::InvalidPath;
    }

    file << result.output;
    file.close();

	if (!writeFiles(asset, result, target.parent_path())) {
		return Error::FailedWritingFiles;
	}
    return Error::None;
}

fg::Error fg::FileExporter::writeGltfBinary(const Asset& asset, fs::path target, ExportOptions _options) {
	if (std::error_code ec; !fs::exists(target.parent_path(), ec) || ec) {
		fs::create_directory(target.parent_path(), ec);
		if (ec) {
			return Error::InvalidPath;
		}
	}

    auto expected = Exporter::writeGltfBinary(asset, _options);

    if (!expected) {
        return expected.error();
    }
    auto& result = expected.get();

    std::ofstream file(target, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        return fg::Error::InvalidPath;
    }

    file.write(reinterpret_cast<const char*>(result.output.data()),
               static_cast<std::streamsize>(result.output.size()));

	if (!writeFiles(asset, result, target.parent_path())) {
		return Error::FailedWritingFiles;
	}
    return Error::None;
}
#pragma endregion

#ifdef _MSC_VER
#pragma warning(pop)
#endif
