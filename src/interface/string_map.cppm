// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <unordered_map>
#include <cctype>
#include <string>
#ifdef __linux__
#include <strings.h>
#endif

#ifdef _WIN32
#define STRICMP _stricmp
#else
#define STRICMP strcasecmp
#endif

export module pragma.string:string_map;

export import std.compat;

export namespace pragma::string {
	// See https://en.cppreference.com/w/cpp/container/unordered_map/find
	struct string_hash {
		using hash_type = std::hash<std::string_view>;
		using is_transparent = void;

		size_t operator()(const char *str) const { return hash_type {}(str); }
		size_t operator()(std::string_view str) const { return hash_type {}(str); }
		size_t operator()(std::string const &str) const { return hash_type {}(str); }
	};
	template<typename T>
	using StringMap = std::unordered_map<std::string, T, string_hash, std::equal_to<>>;
	struct string_hash_ci {
		using is_transparent = void;

		size_t operator()(const char *str) const { return operator()(std::string_view {str}); }
		size_t operator()(std::string_view str) const
		{
			// djb2 hash
			size_t hash = 5381;
			auto len = str.length();
			for(auto i = 0; i < str.length(); ++i) {
				auto idx = i;
				auto c = (unsigned char)str[i];
				if(c == '\\')
					c = '/';
				if(c == '/') {
					if(idx == 0 || idx == (len - 1))
						continue;
				}
				c = std::tolower(static_cast<unsigned char>(c));

				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
			}
			return hash;
		}
		size_t operator()(std::string const &str) const { return operator()(std::string_view {str}); }
	};
	struct CaseInsensitiveStringComparator {
		bool operator()(const std::string &lhs, const std::string &rhs) const
		{
			return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](char a, char b) { return tolower(a) == tolower(b); });
		}
	};
	template<typename T>
	using CIStringMap = std::unordered_map<std::string, T, string_hash_ci, std::equal_to<>>;
};
