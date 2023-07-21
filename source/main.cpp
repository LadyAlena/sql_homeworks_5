#include <iostream>
#include <string>
#include <sstream>
#include <pqxx/pqxx>

#include "control_data_base/control_data_base.h"

int main() {

	try {
		control_data_base c_db (
			"localhost",
			"5432",
			"clients",
			"test_user",
			"test_password"
		);


		c_db.add_new_client("Федор", "Иванов", "fivanov9@yandex.ru");
		c_db.add_phone_for_client(1, "+7 (894) 345 12-12");

		c_db.add_new_client("Алексей", "Рыбаков", "alex_fish123@mail.ru");
		c_db.add_phone_for_client(2, "+7 (678) 910 11-12");
		c_db.add_phone_for_client(2, "+7 (123) 456 78-91");

		c_db.add_new_client("Марина", "Павлова", "parina@mail.ru");

		//c_db.update_client(2, "phone_number", "+7 (900) 900 00-00");

		//c_db.find_client("Марина", "Павлова", "parina@mail.ru");
		//c_db.find_client("Федор", "Иванов", "", "+7 (894) 345 12-12");

		//c_db.delete_phone_for_client(2);

		//c_db.delete_client(1);

		//c_db.update_client(3, "email", "marina2023@yandex.ru");
		
	}
	catch (const pqxx::sql_error& sql_ex) {
		std::cout << sql_ex.what() << std::endl;
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	catch (...) {
		std::cout << "Unknown error!" << std::endl;
	}

}