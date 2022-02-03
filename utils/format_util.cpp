#include "format_util.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>

namespace pesieve {
	namespace util {

		bool is_hex(const char *buf, size_t len)
		{
			for (size_t i = 0; i < len; i++) {
				if (buf[i] >= '0' && buf[i] <= '9') continue;
				if (buf[i] >= 'A' && buf[i] <= 'F') continue;
				if (buf[i] >= 'a' && buf[i] <= 'f') continue;
				return false;
			}
			return true;
		}

		bool is_dec(const char *buf, size_t len)
		{
			for (size_t i = 0; i < len; i++) {
				if (buf[i] >= '0' && buf[i] <= '9') continue;
				return false;
			}
			return true;
		}

	};
};

long pesieve::util::get_number(const char *my_buf)
{
	const char hex_pattern[] = "0x";
	size_t hex_pattern_len = strlen(hex_pattern);

	const size_t len = strlen(my_buf);
	if (len == 0) return 0;

	long out = 0;
	const size_t min_length = 1; //tolerate number with at least 1 character
	if (len > hex_pattern_len) {
		if (is_cstr_equal(my_buf, hex_pattern, hex_pattern_len)) {
			if (!is_hex(my_buf + hex_pattern_len, min_length)) return 0;

			std::stringstream ss;
			ss << std::hex << my_buf;
			ss >> out;
			return out;
		}
	}
	if (!is_dec(my_buf, min_length)) return 0;

	std::stringstream ss;
	ss << std::dec << my_buf;
	ss >> out;
	return out;
}

bool pesieve::util::is_number(const char* my_buf)
{
	const char hex_pattern[] = "0x";
	size_t hex_pattern_len = strlen(hex_pattern);

	const size_t len = strlen(my_buf);
	if (len == 0) return false;

	if (len > hex_pattern_len) {
		if (is_cstr_equal(my_buf, hex_pattern, hex_pattern_len)) {
			if (!is_hex(my_buf + hex_pattern_len, len - hex_pattern_len)) return false;

			return true;
		}
	}
	if (!is_dec(my_buf, len)) return false;
	return true;
}

bool pesieve::util::is_in_list(const char *searched_str, std::set<std::string>& string_list)
{
	bool result = false;

	if (!searched_str || !searched_str[0]) {
		return false;
	}

	std::set<std::string>::iterator itr;
	for (itr = string_list.begin(); itr != string_list.end(); ++itr)
	{
		const std::string list_entry = *itr;
		if (!_stricmp(list_entry.c_str(), searched_str))
		{
			result = true;
			break;
		}
	}
	return result;
}

namespace pesieve {
	namespace util {

		std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			str.erase(0, str.find_first_not_of(chars));
			return str;
		}

		std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			str.erase(str.find_last_not_of(chars) + 1);
			return str;
		}

		std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			return ltrim(rtrim(str, chars), chars);
		}
	}
};

size_t pesieve::util::string_to_list(IN::std::string s, IN char _delim, OUT::std::set<::std::string>& elements_list, bool to_lower)
{
	std::string delim(std::string(1, _delim));
	size_t start = 0;
	size_t end = s.find(delim);
	while (end != std::string::npos)
	{
		std::string next_str = s.substr(start, end - start);
		trim(next_str);
		if (next_str.length() > 0) {
			if (to_lower) {
				std::transform(next_str.begin(), next_str.end(), next_str.begin(), std::tolower);
			}
			elements_list.insert(next_str);
		}
		start = end + delim.length();
		end = s.find(delim, start);
	}
	std::string next_str = s.substr(start, end);
	trim(next_str);
	if (next_str.length() > 0) {
		if (to_lower) {
			std::transform(next_str.begin(), next_str.end(), next_str.begin(), std::tolower);
		}
		elements_list.insert(next_str);
	}
	return elements_list.size();
}

size_t pesieve::util::load_list_from_file(IN::std::string filename, IN char _delim, OUT::std::set<::std::string>& elements_list, bool to_lower)
{
	std::ifstream infile(filename);
	std::string line;
	size_t total = 0;
	while (std::getline(infile, line)) {
		total += string_to_list(line, _delim, elements_list, to_lower);
	}
	return total;
}
