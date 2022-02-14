#include <iostream>
#include <tuple>
#include <string>
#include <optional>
#include <utility>
#include <fstream>
#include <vector>
#include <algorithm>

struct Person {
	std::string surname;
	std::string name;
	std::optional<std::string> patronymic;
};

bool operator <(const Person& p1, const Person& p2) {
	return tie(p1.surname, p1.name, p1.patronymic) < tie(p2.surname, p2.name, p2.patronymic);
}

bool operator ==(const Person& p1, const Person& p2) {
	return tie(p1.surname, p1.name, p1.patronymic) == tie(p2.surname, p2.name, p2.patronymic);
}

std::ostream& operator<< (std::ostream& out, const Person& person) {
	out << person.surname << " " << person.name  <<  " " << person.patronymic.value() << " ";
	return out;
}

struct PhoneNumber {
	int country_code;
	int city_code;
	std::string number;
	std::optional<std::string> add_number;
};

bool operator <(const PhoneNumber& p1, const PhoneNumber& p2) {
	return tie(p1.country_code, p1.city_code, p1.number, p1.add_number) < tie(p2.country_code, p2.city_code, p2.number, p2.add_number);
}

bool operator ==(const PhoneNumber& p1, const PhoneNumber& p2) {
	return tie(p1.country_code, p1.city_code, p1.number, p1.add_number) == tie(p2.country_code, p2.city_code, p2.number, p2.add_number);
}

std::ostream& operator<< (std::ostream& out, const PhoneNumber& phone) {
	std::string add_number;
	if (phone.add_number.has_value())
	{
		add_number = phone.add_number.value();
	}
	else
	{
		add_number = " ";
	}
	out << "+" << phone.country_code << "(" << phone.city_code << ")" << phone.number << " " << add_number;
	return out;
}

class PhoneBook {
private:
	std::vector<std::pair <Person, PhoneNumber>> Person_Number;
public:
	PhoneBook() {}

	PhoneBook(std::ifstream &file1)
	{
		if (file1)
		{
			for (int i=0; !file1.eof(); i++)
			{
				Person p1;
				PhoneNumber p2;
				std::string optpatronymic;
				std::string optnumber;
				file1 >> p1.surname >> p1.name >> optpatronymic >> p2.country_code >> p2.city_code >> p2.number >> optnumber;
				if (optpatronymic == " " || optpatronymic == "-")
				{
					p1.patronymic = " ";
				}
				else
				{
					p1.patronymic = optpatronymic;
				}
				if (optnumber == " " || optnumber == "-")
				{
					p2.add_number = " ";
				}
				else
				{
					p2.add_number = optnumber;
				}
				Person_Number.push_back({ p1,p2 });
			}
		}
	}
	friend std::ostream& operator<< (std::ostream& out, const PhoneBook& phonebook);

	void SortByName() {
		sort(Person_Number.begin(), Person_Number.end(), [](std::pair <Person, PhoneNumber> pn1, std::pair <Person, PhoneNumber> pn2) { return pn1.first < pn2.first; });
	}
	
	void SortByPhone() {
		sort(Person_Number.begin(), Person_Number.end(), [](std::pair <Person, PhoneNumber> pn1, std::pair <Person, PhoneNumber> pn2) { return pn1.second < pn2.second; });
	}

	std::pair<std::string, PhoneNumber> GetPhoneNumber(std::string surname) {
		int count = 0;
		PhoneNumber p1;
		std::for_each(Person_Number.begin(), Person_Number.end(), [surname, &count, &p1] (const auto& Person_Number) mutable{
			if (Person_Number.first.surname == surname)
			{
				count++;
				p1.country_code = Person_Number.second.country_code;
				p1.city_code = Person_Number.second.city_code;
				p1.number = Person_Number.second.number;
				p1.add_number = Person_Number.second.add_number;
			};
			});
		if (count == 1)
		{
			return { "",p1 };

		}
		else if (count == 0)
		{
			return { "not found",p1 };

		}
		else if (true)
		{
			return { "found more than 1",p1 };
		}
	}

	void ChangePhoneNumber(Person p1, const PhoneNumber p2) {
		std::for_each(Person_Number.begin(), Person_Number.end(), [p1,p2]( auto& Person_Number) {
			if (Person_Number.first == p1)
			{
				Person_Number.second.country_code = p2.country_code;
				Person_Number.second.city_code = p2.city_code;
				Person_Number.second.number = p2.number;
				Person_Number.second.add_number = p2.add_number;
			}
			});
	}
};

std::ostream& operator<< (std::ostream& out, const PhoneBook& phonebook) {
	for (size_t i = 0; i < phonebook.Person_Number.size(); i++)
	{
		Person p1;
		PhoneNumber p2;
		std::tie(p1, p2) = phonebook.Person_Number[i];
		out << p1 << " " << p2 << std::endl;
	}
	return out;
}

int main()
{
	std::ifstream file ("phonebook.txt");

	PhoneBook book(file);
	std::cout << book;

	std::cout << "------SortByPhone-------" << std::endl;
	book.SortByPhone();
	std::cout << book;

	std::cout << "------SortByName--------" << std::endl;
	book.SortByName();
	std::cout << book;

	std::cout << "-----GetPhoneNumber-----" << std::endl;
	auto print_phone_number = [&book](const std::string& surname) {
		std::cout << surname << "\t";
		auto answer = book.GetPhoneNumber(surname);
		if (std::get<0>(answer).empty())
			std::cout << std::get<1>(answer);
		else
			std::cout << std::get<0>(answer);
		std::cout << std::endl;
	};

	// вызовы лямбды
	print_phone_number("Ivanov");
	print_phone_number("Petrov");

	std::cout << "----ChangePhoneNumber----" << std::endl;
	book.ChangePhoneNumber(Person{ "Kotov", "Vasilii", "Eliseevich" }, PhoneNumber{ 7, 123, "15344458", std::nullopt });
	book.ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" }, PhoneNumber{ 16, 465, "9155448", "13" });
	std::cout << book;

	file.close();

	system("pause");
	return 0;
}

