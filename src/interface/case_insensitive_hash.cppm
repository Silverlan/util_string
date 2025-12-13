// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.string:case_insensitive_hash;

export import std.compat;

export namespace pragma::string {
	struct CaseInsensitiveStringComparator {
		bool operator()(const std::string &lhs, const std::string &rhs) const;
	};
	struct CaseInsensitiveHashingFunc {
		unsigned long operator()(const std::string &key) const;
	};
	template<typename T>
	using CIMap = std::unordered_map<std::string, T, CaseInsensitiveHashingFunc, CaseInsensitiveStringComparator>;
	// Does not work correctly? Find out why
	//using KeyValueMap = std::unordered_map<std::string,std::string,util::CaseInsensitiveHashingFunc,util::CaseInsensitiveStringComparator>;
};
