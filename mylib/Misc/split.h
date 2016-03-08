#pragma once

inline size_t split(const std::string& str, char sepa, std::vector<std::string>& r)
{
	std::string::const_iterator it = str.begin(), start = it;
	for (; it != str.end(); it ++) {
		if (*it == sepa) {
			std::string item(start, it);
			r.push_back(item);

			start = it + 1;
		}
	}

	std::string item(start, it);
	r.push_back(item);

	return r.size();
}
