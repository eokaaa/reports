#include <iostream>
#include <string>

std::string decode(std::string str, size_t len)
{
	for (int i = 0; i < len; ++i)
		str[i] ^= 0x55;

	return str;
}

void qw(std::string a1)
{
	int v4 = 1337 * a1.size();
	std::cout << "AORE-" << v4 << '\n';
}

int main()
{
	std::string qwe = "07 22<;2u;:!u499:\"01";
	std::cout << decode(qwe, qwe.size()) << '\n';

	std::string enterName = ";!0'u;480o";
	std::cout << decode(enterName, 11) << '\n';

	std::string enterKey = ";!0'u>0,o";
	std::cout << decode(enterKey, 10) << '\n';

	std::string deined = "660&&u10<;01u&";
	std::cout << decode(deined, 15) << '\n';

	std::string granted = "660&&u2'4;!01";
	std::cout << decode(granted, 15) << '\n';

	std::cout << "input name: ";
	std::string name;
	std::cin >> name;
	qw(name);

	system("pause");

	return 0;
}