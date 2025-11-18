// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#ifdef _WIN32
#include <Windows.h>
#endif

export module pragma.string;

export import :case_insensitive_hash;
export import std.compat;

export {
#ifdef __linux__
#if defined(_UNICODE)
	typedef wchar_t* PTSTR;
	typedef const wchar_t* PCTSTR;
#else
	typedef char* PTSTR;
	typedef const char* PCTSTR;
#endif
#endif

	namespace ustring {
		const std::string WHITESPACE = " \t\f\v\n\r";
		const size_t NOT_FOUND = std::string::npos;

		void remove_whitespace(std::string_view &s);
		void remove_whitespace(std::string &s);
		void remove_whitespace(std::vector<std::string> &ss);
		void remove_quotes(std::string &s);
		void remove_quotes(std::vector<std::string> &ss);
		void remove_comment(std::string &s);
		void to_lower(std::string &str);
		void to_upper(std::string &str);
		void split(const std::string &str, std::vector<std::string> &substrings);
		void explode(std::string str, const char *sep, std::vector<std::string> &substrings);
		std::string implode(const std::vector<std::string> &strs, const std::string &separator = " ");
		size_t get_parameters(const std::string &s, std::string &rname, std::vector<std::string> &args);
		size_t find_first_of_outside_quotes(const std::string &str, std::string tofind, uint32_t qPrev = 0);
		size_t find_first_of(FILE *f, const std::string &tofind, std::string *line = nullptr);
		std::string float_to_string(float f);
		std::string int_to_string(int32_t i);
		bool is_integer(const std::string &str);
		bool is_number(const std::string &str);
		void get_sequence_commands(const std::string &str, const std::function<void(std::string, std::vector<std::string> &)> &f);
		uint32_t get_args(const std::string &line, std::vector<std::string> &argv);
		std::vector<std::string> get_args(const std::string &line);
		uint32_t get_command_args(const std::string &line, std::string &cmd, std::vector<std::string> &argv);
		std::vector<std::string> get_command_args(const std::string &line, std::string &cmd);
		std::string fill_zeroes(const std::string &i, int32_t numDigits);
		bool get_key_value(const std::string &str, std::string &rkey, std::string &rval, const char *sep = "=");

		std::string read_until(const std::string &t, char c);
		std::string read_until_etx(const std::string &t);
		bool match(PCTSTR pszText, PCTSTR pszMatch, bool bMatchCase = false);
		bool match(const std::string_view &text, const std::string_view &match, bool bMatchCase = false);
		template<class type, class rtype>
		size_t string_to_array(const std::string &str, type *a, rtype (*atot)(const char *), uint32_t count);
		template<class type>
		size_t string_to_array(const std::string &str, type *a, type (*atot)(const char *), uint32_t count);
		std::wstring string_to_wstring(const std::string &str);
		std::string wstring_to_string(const std::wstring &str);
		void replace(std::string &str, const std::string &from, const std::string &to);
		template<typename TString>
		void replace(TString &str, const TString &from, const TString &to)
		{
			if(from.empty())
				return;
			size_t start_pos = 0;
			while((start_pos = str.find(from, start_pos)) != TString::npos) {
				str.replace(start_pos, from.length(), to);
				start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
			}
		}
		std::string get_lower(const std::string &str);
		std::string get_upper(const std::string &str);
		int32_t to_int(const std::string &str);
		float to_float(const std::string &str);
		double to_double(const std::string &str);
		void explode_whitespace(const std::string &str, std::vector<std::string> &substrings);
		uint32_t calc_levenshtein_distance(const std::string_view &s1, const std::string_view &s2);
		double calc_levenshtein_similarity(const std::string_view &s1, const std::string_view &s2);
		std::size_t longest_common_substring(const std::string &str1, const std::string &str2, std::size_t &startIdx1, std::size_t &startIdx2);
		std::size_t longest_common_substring(const std::string &str1, const std::string &str2, std::size_t &startIdx1);
		std::size_t longest_common_substring(const std::string &str1, const std::string &str2, std::string &subStr);
		std::size_t longest_common_substring(const std::string &str1, const std::string &str2);
		std::string substr(const std::string &str, std::size_t start, size_t len = std::string::npos);
		std::string_view substr(const std::string_view &str, std::size_t start, size_t len = std::string::npos);
		size_t find(const auto &strHaystack, const auto &strNeedle, bool caseSensitive = true);
		template<typename T>
			requires(std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
		bool compare(const T &a, const T &b, bool caseSensitive = true);
		bool compare(const char *a, const char *b, bool caseSensitive = true, size_t len = std::string::npos);
		std::string name_to_identifier(const std::string &name);
		std::string to_camel_case(const std::string &str);
		std::string to_snake_case(const std::string &str);

		// Calculates the similarity between the two specified strings. The result only makes sense in context,
		// lower values (<0) represent a higher similarity than higher values.
		// This function is VERY expensive (O(n^3)) and should be used sparingly.
		double calc_similarity(const std::string_view &inputWord, const std::string_view &checkWord);

		void gather_similar_elements(const std::string_view &baseElement, const std::vector<std::string> &elements, std::vector<size_t> &outElements, uint32_t limit, std::vector<float> *inOutSimilarities = nullptr);
		void gather_similar_elements(const std::string_view &baseElement, const std::function<std::optional<std::string_view>(void)> &f, std::vector<std::string_view> &outElements, uint32_t limit, std::vector<float> *inOutSimilarities = nullptr);

		uint32_t ip_to_int(const std::string_view &ip);
		std::string int_to_ip(uint32_t ip);
		void truncate_string(std::string &str, std::size_t maxLength);

		constexpr char char_to_lower(const char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; }
		constexpr char char_to_upper(const char c) { return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c; }

		constexpr bool iequals(const std::string_view &a, const std::string_view &b)
		{
			auto l0 = a.length();
			auto l1 = b.length();
			if(l0 != l1)
				return false;
			for(auto i = decltype(l0) {0u}; i < l0; ++i) {
				if(char_to_lower(a[i]) != char_to_lower(b[i]))
					return false;
			}
			return true;
			// TODO: Use this once C++20 is available
			/*return std::equal(a.begin(), a.end(),
				b.begin(), b.end(),
				[](char a, char b) {
					return char_to_lower(a) == char_to_lower(b);
			});*/
		}

		// See https://stackoverflow.com/a/26082447/2482983
		template<size_t N>
		consteval size_t length(char const (&)[N])
		{
			return N - 1;
		}

	#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-literal-operator"
	#endif
		namespace string_switch {
			// See https://stackoverflow.com/a/46711735/2482983
			constexpr uint32_t hash(const char *data, size_t const size) noexcept
			{
				uint32_t hash = 5381;

				for(const char *c = data; c < data + size; ++c)
					hash = ((hash << 5) + hash) + (unsigned char)*c;

				return hash;
			}

			constexpr uint32_t hash(const std::string_view &str) noexcept { return hash(str.data(), str.length()); }

			constexpr inline long long int operator"" _(char const *p, size_t) { return ustring::string_switch::hash(p, std::char_traits<char>::length(p)); }
		}

		namespace string_switch_ci {
			// Same as string_switch, but case-insensitive
			constexpr uint32_t hash(const char *data, size_t const size) noexcept
			{
				uint32_t hash = 5381;

				for(const char *c = data; c < data + size; ++c)
					hash = ((hash << 5) + hash) + char_to_lower((unsigned char)*c);

				return hash;
			}

			constexpr uint32_t hash(const std::string_view &str) noexcept { return hash(str.data(), str.length()); }

			constexpr inline long long int operator"" _(char const *p, size_t) { return ustring::string_switch_ci::hash(p, std::char_traits<char>::length(p)); }
		}
	#ifdef __clang__
	#pragma clang diagnostic pop
	#endif
	}

	size_t ustring::find(const auto &strHaystack, const auto &strNeedle, bool caseSensitive)
	{
		if(caseSensitive)
			return strHaystack.find(strNeedle);
		auto it = std::search(strHaystack.begin(), strHaystack.end(), strNeedle.begin(), strNeedle.end(), [](auto ch1, auto ch2) { return std::toupper(ch1) == std::toupper(ch2); });
		return (it != strHaystack.end()) ? std::distance(strHaystack.begin(), it) : std::string::npos;
	}

	template<class type, class rtype>
	size_t ustring::string_to_array(const std::string &str, type *a, rtype (*atot)(const char *), uint32_t count)
	{
		std::vector<std::string> vdat;
		explode(str, WHITESPACE.c_str(), vdat);
		size_t l = vdat.size();
		if(l > count)
			l = count;
		for(unsigned int i = 0; i < l; i++)
			a[i] = static_cast<type>(atot(vdat[i].c_str()));
		return l;
	}

	template<class type>
	size_t ustring::string_to_array(const std::string &str, type *a, type (*atot)(const char *), uint32_t count)
	{
		return string_to_array<type, type>(str, a, atot, count);
	}
}
