// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#ifdef _WIN32
#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib") // Required for inet_addr / inet_ntoa
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

module pragma.string;

import std.compat;

#undef min
#undef max

#if defined(_UNICODE)
#define _T(x) L##x
#else
#define _T(x) x
#endif

/*
#ifdef __linux__
    typedef long LONG;
    typedef unsigned short WORD;
    typedef unsigned long DWORD;
    #define MAKELONG(a, b) \
        ((LONG) (((WORD) (a)) | ((DWORD) ((WORD) (b))) << 16))
    PCTSTR charUpper(PCTSTR text)
    {
        std::string t(text);
        std::transform(t.begin(),t.end(),t.begin(),::toupper);
        return t.c_str();
    }
#endif
*/
bool pragma::string::match(PCTSTR pszText, PCTSTR pszMatch, bool bMatchCase)
{
	// Loop over the string
	while(*pszText && *pszMatch != _T('*')) {
		// Found a match on a next normal character
		if((bMatchCase ? (*pszText != *pszMatch) : (toupper(*pszText) != toupper(*pszMatch))) && *pszMatch != _T('?'))
			return false; // TODO: toupper doesn't account for unicode

		// still a match
		pszMatch++;
		pszText++;
	}

	// Either we have a wildcard or or we are at end of the string to test
	if(!*pszText) {
		// There is a special case were there is only a wildcard char left
		// on the match string, so we just skip it
		while(*pszMatch == _T('*'))
			++pszMatch;
		// it would be a match if both strings reached the end of the string
		return !*pszText && !*pszMatch;
	}
	else {
		// We found a wildcard '*'. We have some chances now:
		// 1. we just ignore the wildcard and continue
		// 2. we just skip 1 character in the source and reuse the wildcard
		if(match(pszText, pszMatch + 1, bMatchCase))
			return true;
		else
			return match(pszText + 1, pszMatch, bMatchCase);
	}
}
bool pragma::string::match(const std::string_view &text, const std::string_view &strMatch, bool bMatchCase)
{
	// Loop over the string
	auto *pszText = text.data();
	auto *pszTextLast = text.data() + text.length();
	auto *pszMatch = strMatch.data();
	auto *pszMatchLast = strMatch.data() + strMatch.length();
	while(pszText != pszTextLast && *pszMatch != _T('*')) {
		// Found a match on a next normal character
		if((bMatchCase ? (*pszText != *pszMatch) : (toupper(*pszText) != toupper(*pszMatch))) && *pszMatch != _T('?'))
			return false; // TODO: toupper doesn't account for unicode

		// still a match
		pszMatch++;
		pszText++;
	}

	// Either we have a wildcard or or we are at end of the string to test
	if(pszText == pszTextLast) {
		// There is a special case were there is only a wildcard char left
		// on the match string, so we just skip it
		while(*pszMatch == _T('*'))
			++pszMatch;
		// it would be a match if both strings reached the end of the string
		return pszText == pszTextLast && pszMatch == pszMatchLast;
	}
	else {
		// We found a wildcard '*'. We have some chances now:
		// 1. we just ignore the wildcard and continue
		// 2. we just skip 1 character in the source and reuse the wildcard
		if(match(text.substr(pszText - text.data()), strMatch.substr((pszMatch + 1) - strMatch.data()), bMatchCase))
			return true;
		else
			return match(text.substr((pszText + 1) - text.data()), strMatch.substr(pszMatch - strMatch.data()), bMatchCase);
	}
}
void pragma::string::remove_whitespace(std::string_view &s)
{
	if(s.empty())
		return;
	uint32_t begin = s.find_first_not_of(WHITESPACE);
	uint32_t end = s.find_last_not_of(WHITESPACE);
	if(begin == uint32_t(-1) || end == uint32_t(-1)) {
		s = "";
		return;
	}
	s = s.substr(begin, (end - begin) + 1);
}
void pragma::string::remove_whitespace(std::string &s)
{
	if(s.empty())
		return;
	if(s.back() == '\0')
		s = std::string(s.c_str());
	uint32_t begin = s.find_first_not_of(WHITESPACE);
	uint32_t end = s.find_last_not_of(WHITESPACE);
	if(begin == uint32_t(-1) || end == uint32_t(-1)) {
		s = "";
		return;
	}
	s = s.substr(begin, (end - begin) + 1);
}

void pragma::string::remove_whitespace(std::vector<std::string> &ss)
{
	for(int i = 0; i < ss.size(); i++)
		remove_whitespace(ss[i]);
}

void pragma::string::remove_quotes(std::string &s)
{
	if(s.empty() || s.front() != '\"' || s.back() != '\"')
		return;
	s = s.substr(1, s.length() - 2);
}

void pragma::string::remove_quotes(std::vector<std::string> &ss)
{
	for(int i = 0; i < ss.size(); i++)
		remove_quotes(ss[i]);
}

void pragma::string::remove_comment(std::string &s)
{
	auto cSt = s.find("//");
	if(cSt == NOT_FOUND)
		return;
	s = s.substr(0, cSt - 1);
}

void pragma::string::split(const std::string &str, std::vector<std::string> &substrings)
{
	std::string buf;
	std::stringstream ss(str);
	while(ss >> buf)
		substrings.push_back(buf);
}

std::string pragma::string::implode(const std::vector<std::string> &strs, const std::string &separator)
{
	std::string result {};
	auto first = true;
	for(auto &str : strs) {
		if(first)
			first = false;
		else
			result += separator;
		result += str;
	}
	return result;
}

void pragma::string::explode(std::string str, const char *sep, std::vector<std::string> &substrings)
{
	auto st = find_first_of_outside_quotes(str, sep);
	auto foundAtLeastOne = (st != NOT_FOUND);
	while(st != NOT_FOUND) {
		std::string sub = str.substr(0, st);
		str = str.substr(st + 1);
		remove_whitespace(sub);
		substrings.push_back(sub);
		st = find_first_of_outside_quotes(str, sep);
	}
	remove_whitespace(str);
	if(str.empty() == true && foundAtLeastOne == false)
		return;
	substrings.push_back(str);
}

void pragma::string::explode_whitespace(const std::string &str, std::vector<std::string> &substrings)
{
	auto estr = str;
	auto st = estr.find_first_not_of(WHITESPACE);
	st = find_first_of_outside_quotes(estr, WHITESPACE, st);
	while(st != NOT_FOUND) {
		auto sub = estr.substr(0, st);
		remove_whitespace(sub);
		substrings.push_back(sub);

		st = estr.find_first_not_of(WHITESPACE, st);
		if(st == NOT_FOUND)
			return;
		estr = estr.substr(st);
		st = find_first_of_outside_quotes(estr, WHITESPACE);
	}
	remove_whitespace(estr);
	if(estr.empty() == true)
		return;
	substrings.push_back(estr);
}

// See https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
uint32_t pragma::string::calc_levenshtein_distance(const std::string_view &s1, const std::string_view &s2)
{
	const auto len1 = s1.size();
	const auto len2 = s2.size();
	std::vector<std::vector<uint32_t>> d(len1 + 1, std::vector<uint32_t>(len2 + 1));

	d.at(0).at(0) = 0;
	for(auto i = decltype(len1) {1}; i <= len1; ++i)
		d.at(i).at(0) = i;
	for(auto i = decltype(len2) {1}; i <= len2; ++i)
		d.at(0).at(i) = i;

	for(auto i = decltype(len1) {1}; i <= len1; ++i) {
		for(auto j = decltype(len2) {1}; j <= len2; ++j)
			d.at(i).at(j) = std::min({d.at(i - 1).at(j) + 1, d.at(i).at(j - 1) + 1, d.at(i - 1).at(j - 1) + (s1.at(i - 1) == s2.at(j - 1) ? 0 : 1)});
	}
	return d.at(len1).at(len2);
}

// See http://stackoverflow.com/a/16018452/2482983
double pragma::string::calc_levenshtein_similarity(const std::string_view &s1, const std::string_view &s2)
{
	auto *longer = &s1;
	auto *shorter = &s2;
	if(s1.length() < s2.length()) {
		longer = &s2;
		shorter = &s1;
	}

	auto longerLength = longer->length();
	if(longerLength == 0)
		return 1.0;
	return (longerLength - calc_levenshtein_distance(*longer, *shorter)) / static_cast<double>(longerLength);
}

// See https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Longest_common_substring#C.2B.2B_2
std::size_t pragma::string::longest_common_substring(const std::string &str1, const std::string &str2, std::size_t &startIdx1, std::size_t &startIdx2)
{
	if(str1.empty() == true || str2.empty() == true)
		return 0;

	std::vector<decltype(str2.size())> vcurr(str2.size());
	std::vector<decltype(str2.size())> vprev(str2.size());

	decltype(vcurr) *curr = &vcurr;
	decltype(vprev) *prev = &vprev;
	decltype(vcurr) *swap = nullptr;
	auto maxSubstr = decltype(str1.size()) {0};

	for(auto i = decltype(str1.size()) {0}; i < str1.size(); ++i) {
		for(auto j = decltype(str2.size()) {0}; j < str2.size(); ++j) {
			if(str1.at(i) != str2.at(j))
				curr->at(j) = 0;
			else {
				if(i == 0 || j == 0)
					curr->at(j) = 1;
				else
					curr->at(j) = 1 + prev->at(j - 1);
				if(maxSubstr < curr->at(j)) {
					maxSubstr = curr->at(j);
					startIdx1 = i - maxSubstr + 1;
					startIdx2 = j - maxSubstr + 1;
				}
			}
		}
		swap = curr;
		curr = prev;
		prev = swap;
	}
	return maxSubstr;
}

std::size_t pragma::string::longest_common_substring(const std::string &str1, const std::string &str2, std::size_t &startIdx1)
{
	std::size_t startIdx2 = 0;
	return longest_common_substring(str1, str2, startIdx1, startIdx2);
}
std::size_t pragma::string::longest_common_substring(const std::string &str1, const std::string &str2, std::string &subStr)
{
	std::size_t startIdx1 = 0;
	auto len = longest_common_substring(str1, str2, startIdx1);
	if(len == 0) {
		subStr.clear();
		return len;
	}
	subStr = str1.substr(startIdx1, len);
	return len;
}
std::size_t pragma::string::longest_common_substring(const std::string &str1, const std::string &str2)
{
	std::size_t startIdx = 0;
	return longest_common_substring(str1, str2, startIdx);
}
std::string pragma::string::substr(const std::string &str, std::size_t start, size_t len) { return (start < str.length()) ? str.substr(start, len) : ""; }
std::string_view pragma::string::substr(const std::string_view &str, std::size_t start, size_t len) { return (start < str.length()) ? str.substr(start, len) : std::string_view {}; }
template<typename T>
    requires(std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
bool pragma::string::compare(const T &a, const T &b, bool caseSensitive)
{
	if(caseSensitive == true)
		return a == b;
	if(a.length() != b.length())
		return false;
	return std::equal(b.begin(), b.end(), a.begin(), [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
}
template bool pragma::string::compare<std::string>(const std::string &a, const std::string &b, bool caseSensitive);
template bool pragma::string::compare<std::string_view>(const std::string_view &a, const std::string_view &b, bool caseSensitive);
bool pragma::string::compare(const char *a, const char *b, bool caseSensitive, size_t len)
{
	if(caseSensitive == true) {
		if(len == std::string::npos)
			return strcmp(a, b) == 0;
		return strncmp(a, b, len) == 0;
	}
	if(len == std::string::npos) {
		auto len0 = strlen(a);
		auto len1 = strlen(b);
		if(len0 != len1)
			return false;
		len = len0;
	}
	for(; len > 0; ++a, ++b, --len) {
		auto d = tolower(*a) - tolower(*b);
		if(d != 0 || !*a)
			return false;
	}
	return true;
}

size_t pragma::string::get_parameters(const std::string &s, std::string &rname, std::vector<std::string> &args)
{
	auto arSt = s.find('(');
	auto arEn = s.find(')');
	if(arSt == NOT_FOUND || arEn == NOT_FOUND)
		return arEn;
	std::string name = s.substr(0, arSt);
	rname = name;
	size_t arNext = 0;
	int it = 0;
	do {
		arNext = s.find_first_of(",)", arNext + 1);
		std::string arg = s.substr(arSt + 1, arNext - arSt - 1);
		remove_whitespace(arg);
		if(arg.length() == 0 && it == 0)
			return arEn;
		args.push_back(arg);
		arSt = arNext;
		it++;
	} while(s[arNext] != ')');
	return arEn;
}

size_t pragma::string::find_first_of_outside_quotes(const std::string &str, std::string tofind, uint32_t qPrev)
{
	size_t qStart = 0;
	size_t qEnd;
	qPrev--;
	for(;;) {
		qStart = str.find_first_of('\"', qPrev + 1);
		qEnd = str.find_first_of('\"', qStart + 1);
		auto f = str.find_first_of(tofind, qPrev + 1);
		if(qStart == NOT_FOUND || qEnd == NOT_FOUND || f < qStart)
			return f;
		qPrev = qEnd;
	}
	//return NOT_FOUND;
}

size_t pragma::string::find_first_of(FILE *f, const std::string &tofind, std::string *line)
{
	char buf[4096];
	while(fgets(buf, 4096, f)) {
		std::string sbuf(buf);
		remove_comment(sbuf);
		auto brSt = sbuf.find_first_of(tofind);
		if(brSt != NOT_FOUND) {
			if(line != nullptr)
				*line = sbuf;
			return brSt;
		}
	}
	return NOT_FOUND;
}

std::string pragma::string::float_to_string(float f) { return std::to_string(f); }
std::string pragma::string::int_to_string(int i) { return std::to_string(i); }

bool pragma::string::is_integer(const std::string &str)
{
	char *p;
	long converted = strtol(str.c_str(), &p, 10);
	return !*p;
}
bool pragma::string::is_number(const std::string &str)
{
	char *p;
	long converted = strtod(str.c_str(), &p);
	return !*p;
}

uint32_t pragma::string::get_args(const std::string &line, std::vector<std::string> &argv)
{
	uint32_t i = 0;
	while(i != uint32_t(-1)) {
		uint32_t n = line.find_first_not_of(WHITESPACE, i);
		if(n == uint32_t(-1))
			break;
		bool isstr = false;
		if(line[n] == '\"') {
			i = line.find_first_of('\"', n + 1);
			if(i == uint32_t(-1))
				i = line.find_first_of(WHITESPACE, n);
			else {
				n = n + 1;
				isstr = true;
			}
		}
		else
			i = line.find_first_of(WHITESPACE, n);
		argv.push_back(line.substr(n, i - n));
		if(isstr)
			i++;
	}
	return argv.size();
}
std::vector<std::string> pragma::string::get_args(const std::string &line)
{
	std::vector<std::string> argv;
	get_args(line, argv);
	return argv;
}

uint32_t pragma::string::get_command_args(const std::string &line, std::string &cmd, std::vector<std::string> &argv)
{
	argv = get_args(line);
	if(argv.empty())
		cmd = "";
	else {
		cmd = argv.front();
		to_lower(cmd);
		argv.erase(argv.begin());
	}
	return argv.size();
}
std::vector<std::string> pragma::string::get_command_args(const std::string &line, std::string &cmd)
{
	std::vector<std::string> argv;
	get_command_args(line, cmd, argv);
	return argv;
}

void pragma::string::get_sequence_commands(const std::string &line, const std::function<void(std::string, std::vector<std::string> &)> &f)
{
	std::string substr(line);
	auto st = substr.find_first_not_of(WHITESPACE);
	if(st == NOT_FOUND)
		return;
	substr = substr.substr(st, substr.length());
	auto s = find_first_of_outside_quotes(substr, ";");
	std::string next;
	bool bNext = false;
	if(s != NOT_FOUND) {
		bNext = true;
		next = substr.substr(s + 1, substr.length());
		substr = substr.substr(0, s);
	}
	std::string cmd;
	std::vector<std::string> argv = get_command_args(substr, cmd);
	f(cmd, argv);
	if(bNext)
		get_sequence_commands(next.c_str(), f);
}

std::string pragma::string::read_until(const std::string &t, char c)
{
	std::string str;
	unsigned int i = 0;
	while(t[i] != c) {
		str += t[i];
		i++;
	}
	return str;
}

std::string pragma::string::read_until_etx(const std::string &t) { return read_until(t, 3); }

std::string pragma::string::fill_zeroes(const std::string &i, int numDigits)
{
	if(i.length() >= numDigits)
		return i;
	auto l = static_cast<uint32_t>(numDigits) - i.length();
	std::stringstream si;
	for(int j = 0; j < l; j++)
		si << "0";
	si << i;
	return si.str();
}

void pragma::string::to_lower(std::string &str) { std::transform(str.begin(), str.end(), str.begin(), static_cast<int (*)(int)>(std::tolower)); }
void pragma::string::to_upper(std::string &str) { std::transform(str.begin(), str.end(), str.begin(), static_cast<int (*)(int)>(std::toupper)); }

bool pragma::string::get_key_value(const std::string &str, std::string &rkey, std::string &rval, const char *sep)
{
	std::vector<std::string> sub;
	string::explode(str, sep, sub);
	if(sub.size() < 2)
		return false;
	rkey = sub[0];
	remove_whitespace(rkey);
	remove_quotes(rkey);
	to_lower(rkey);
	rval = sub[1];
	remove_whitespace(rval);
	remove_quotes(rval);
	return true;
}

std::wstring pragma::string::string_to_wstring(const std::string &str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}
std::string pragma::string::wstring_to_string(const std::wstring &wstr)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.to_bytes(wstr);
}

void pragma::string::replace(std::string &str, const std::string &from, const std::string &to) { replace<std::string>(str, from, to); }

std::string pragma::string::get_lower(const std::string &str)
{
	auto out = str;
	to_lower(out);
	return out;
}
std::string pragma::string::get_upper(const std::string &str)
{
	auto out = str;
	to_upper(out);
	return out;
}
int32_t pragma::string::to_int(const std::string &str) {
	int32_t value = 0;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return value;
}
float pragma::string::to_float(const std::string &str) {
	float value = 0.f;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return value;
}
double pragma::string::to_double(const std::string &str) {
	double value = 0.0;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return value;
}
std::string pragma::string::name_to_identifier(const std::string &name)
{
	auto r = name;
	to_lower(r);
	for(auto &c : r) {
		if(c <= 47 || (c >= 58 && c <= 64) || (c >= 91 && c <= 96) || c >= 123)
			c = '_';
	}
	return r;
}
std::string pragma::string::to_camel_case(const std::string &str)
{
	std::string camelCase;
	bool capitalizeNext = false;

	for(char ch : str) {
		if(ch == '_')
			capitalizeNext = true; // Set flag to capitalize the next character
		else {
			if(capitalizeNext) {
				camelCase += std::toupper(ch); // Capitalize current character
				capitalizeNext = false;
			}
			else {
				camelCase += ch; // Add character as is
			}
		}
	}

	return camelCase;
}
std::string pragma::string::to_snake_case(const std::string &str)
{
	std::string snake_case;

	auto isFirstchar = true;
	for(char ch : str) {
		if(std::isupper(ch)) {
			if(!isFirstchar)
				snake_case += '_';          // Add underscore before uppercase letter
			snake_case += std::tolower(ch); // Convert uppercase to lowercase
		}
		else {
			snake_case += ch; // Add character as is
		}
		isFirstchar = false;
	}

	return snake_case;
}

uint32_t pragma::string::ip_to_int(const std::string_view &ip) { return inet_addr(ip.data()); }
std::string pragma::string::int_to_ip(uint32_t ip)
{
	in_addr paddr;
#ifdef _WIN32
	paddr.S_un.S_addr = ip;
#else
	paddr.s_addr = ip;
#endif
	return inet_ntoa(paddr);
}

void pragma::string::truncate_string(std::string &str, std::size_t maxLength)
{
	if(str.length() > maxLength)
		str = str.substr(0, maxLength - 3) + "...";
}
