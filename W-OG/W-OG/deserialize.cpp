#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "deserializer.h"

std::map<std::string, int> data;

std::string get_first_word(std::string s) {
	std::string word;
	
	for (int i = 0; i < s.size() && s[i] != ' '; ++i) {
		word += s[i];
	}

	return word;
}

inline int pow_of_10(int n) {
	int pow = 1;

	for (int i = 1; i <= n; ++i) {
		pow *= 10;
	}

	return pow;
}

int get_last_number(std::string s) {
	int number = 0;
	for (int i = s.size() - 1; i >= 0 && isdigit(s[i]); --i) {
		number += (s[i] - '0') * pow_of_10(s.size() - i - 1);
	}

	return number;
}

namespace deserializer {
	void check_for_missing_data() {
		int defaultValues[] = {
			800,
			600
		};

		std::vector<std::string> settingKeywords = { "window_width",
			"window_height",
			"wave_speed",
			"wave_max_size",
			"approach_speed",
			"fail_by_misses",
			"fail_by_mashing",
			"number_of_permitted_misses",
			"number_of_permitted_mashes",
			};

		for (int i = 0; i < settingKeywords.size(); ++i) {
			if (data[settingKeywords[i]] == -1) {
				std::cerr << "missing: " << settingKeywords[i] << ", using default value (" << defaultValues[i] << ")\n";
				data[settingKeywords[i]] = defaultValues[i];
			}
		}
	}

	int find_comment(std::string s) {
		for (int i = 0; i < s.size(); ++i) {
			if (s[i] == '#') {
				return i;
			}
		}
		return -1;
	}

	std::string delete_comments(std::string s) {
		int commentPos = find_comment(s);
		while (commentPos > 0 && s[commentPos - 1] == ' ') {
			--commentPos;
		}
		if (commentPos != -1) {
			s = s.substr(0, commentPos);
		}
		return s;
	}

	void deserialize() {
		std::ifstream file("data.cfg");
		std::string line, keyword;
		int settingNumber = -1;

		getline(file, line);

		std::cerr << "Deserializing...\n";
		while (!file.eof()) {
			line = delete_comments(line);
			if (line.size()) {
				keyword = get_first_word(line);

				if (line.find('=') < line.size() - 1 && isdigit(line[line.size() - 1])) {
					settingNumber = get_last_number(line);
				}

				std::cerr << keyword << " = " << settingNumber << '\n';
				data[keyword] = settingNumber;
			}
			getline(file, line);
		}
		check_for_missing_data();
		
		std::cerr << "Done.\n";
	}

	std::map<std::string, int> pass_data() {
		return data;
	}
}