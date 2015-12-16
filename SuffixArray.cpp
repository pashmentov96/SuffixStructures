#include <iostream>
#include <cstring>
#include <vector>
#include <tuple>

typedef std::tuple <int, int, int> Triple;
typedef std::pair <int, int> Pair;

int GetSymbol(const std::vector <int> &str, int pos) {
	if (pos >= (int)str.size()) {
		return 0;
	}
	return str[pos];
}

void RadixSort(const std::vector <int> &array, std::vector <int> &answer, const std::vector <int> &str, int shift, int max_symbol) {
	std::vector <int> count(max_symbol + 1);
	int length = array.size();
	for (int i = 0; i < length; ++i) {
		++count[GetSymbol(str, array[i] + shift)];
	}
	int sum = 0;
	for (int i = 0; i <= max_symbol; ++i) {
		int temp = count[i];
		count[i] = sum;
		sum += temp;
	}
	for (int i = 0; i < length; ++i) {
		answer[count[GetSymbol(str, array[i] + shift)]++] = array[i];
	}
}

//Kärkkäinen-Sanders
void BuildSuffixArray(std::vector <int> &str, std::vector <int> &suffix_array, int max_symbol) {
	int length = str.size();
	int length_0 = (length + 2) / 3;
	int length_1 = (length + 1) / 3;
	int length_2 = length / 3;
	int fake = length_0 - length_1;
	int length_12 = length_0 + length_2;
	
	std::vector <int> str_12(length_12);
	std::vector <int> suffix_array_12(length_12);
	std::vector <int> str_0(length_0);
	std::vector <int> suffix_array_0(length_0);

	for (int i = 0, j = 0; i < length + fake; ++i) {
		if (i % 3 != 0) {
			str_12[j++] = i;
		}
	}

	RadixSort(str_12, suffix_array_12, str, 2, max_symbol);
	RadixSort(suffix_array_12, str_12, str, 1, max_symbol);  
	RadixSort(str_12, suffix_array_12, str, 0, max_symbol);
	
	int name = 0;
	Triple last_named(max_symbol + 1, max_symbol + 1, max_symbol + 1);
	for (int i = 0; i < length_12; ++i) {
		Triple current(GetSymbol(str, suffix_array_12[i]), GetSymbol(str, suffix_array_12[i] + 1), GetSymbol(str, suffix_array_12[i] + 2));
		if (current != last_named) {
			++name;
			last_named = current;
		}
		if (suffix_array_12[i] % 3 == 1) {
			str_12[suffix_array_12[i] / 3] = name;
		} else {
			str_12[suffix_array_12[i] / 3 + length_0] = name;
		}
	}
	
	if (name < length_12) {
		BuildSuffixArray(str_12, suffix_array_12, name);
		for (int i = 0; i < length_12; ++i) {
			str_12[suffix_array_12[i]] = i + 1;
		}
	} else {
		for (int i = 0; i < length_12; ++i) {
			suffix_array_12[str_12[i] - 1] = i;
		}
	}
	
	for (int i = 0, j = 0; i < length_12; ++i) {
		if (suffix_array_12[i] < length_0) {
			str_0[j++] = 3 * suffix_array_12[i];
		}
	}
	RadixSort(str_0, suffix_array_0, str, 0, max_symbol);
	
	for (int i_0 = 0, i_12 = fake, pos = 0; pos < length; ++pos) {
		int pos_12 = (GetSymbol(suffix_array_12, i_12) < length_0 ? GetSymbol(suffix_array_12, i_12) * 3 + 1 : (GetSymbol(suffix_array_12, i_12) - length_0) * 3 + 2);
		int pos_0 = GetSymbol(suffix_array_0, i_0);
		if (i_12 < length_12 && (suffix_array_12[i_12] < length_0 ? (Pair(GetSymbol(str, pos_12), GetSymbol(str_12, GetSymbol(suffix_array_12, i_12) + length_0)) <= Pair(GetSymbol(str, pos_0), GetSymbol(str_12, pos_0 / 3))) : 
											                        (Triple(GetSymbol(str, pos_12), GetSymbol(str, pos_12 + 1), GetSymbol(str_12, GetSymbol(suffix_array_12, i_12) - length_0 + 1)) <= Triple(GetSymbol(str, pos_0), GetSymbol(str, pos_0 + 1), GetSymbol(str_12, pos_0 / 3 + length_0)))))
		{
			suffix_array[pos] = pos_12;
			++i_12;
			if (i_12 == length_12) {
				++pos;
				while (i_0 < length_0) {
					suffix_array[pos++] = suffix_array_0[i_0++];
				}
			}
		} else { 
			suffix_array[pos] = pos_0;
			++i_0; 
			if (i_0 == length_0) {
				++pos;
				for (; i_12 < length_12; ++i_12) {
					suffix_array[pos++] = (GetSymbol(suffix_array_12, i_12) < length_0 ? GetSymbol(suffix_array_12, i_12) * 3 + 1 : (GetSymbol(suffix_array_12, i_12) - length_0) * 3 + 2); 
				}
			}  
		}
	}
}

//Kasai algorithm
std::vector <int> CalculateLCP(const std::vector <int> &str, const std::vector <int> &suffix_array) {
	int length = str.size();
	std::vector <int> position(length);
	for (int i = 0; i < length; ++i) {
		position[suffix_array[i]] = i;
	}
	std::vector <int> lcp(length);
	int result = 0;
	for (int cur = 0; cur < length; ++cur) {
		if (result > 0) {
			--result;
		}
		if (position[cur] == length - 1) {
			result = 0;
		} else {
			while (std::max(cur + result, suffix_array[position[cur] + 1] + result) < length && GetSymbol(str, cur + result) == GetSymbol(str, suffix_array[position[cur] + 1] + result)) {
				++result;
			}
			lcp[position[cur]] = result;
		}
	}
	return lcp;
}

int CountDifferentSubstrings(std::vector <int> &str, int max_symbol) {
	int length = str.size();
	std::vector <int> suffix_array(length);
	BuildSuffixArray(str, suffix_array, max_symbol);
	std::vector <int> lcp = CalculateLCP(str, suffix_array);
	int answer = length - suffix_array[0];
	for (int i = 1; i < (int)str.size(); ++i) {
		answer += (length - suffix_array[i] - lcp[i - 1]);
	}
	return answer;
}

const int MAX_SYMBOL = 30;

int main() {
	int k;
	std::string text;
	std::cin >> k >> text;
	int length = text.size();
	for (int i = 0; i < length; ++i) {
		std::vector <int> str(k);
		for (int j = 0; j < k; ++j) {
			str[j] = text[(i + j) % length] - 'a' + 1;
		}
		std::cout << CountDifferentSubstrings(str, MAX_SYMBOL) << " ";
	}
	return 0;
}
