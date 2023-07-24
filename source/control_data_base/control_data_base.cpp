#include "control_data_base.h"

#include <iostream>

control_data_base::control_data_base(std::string host,
	std::string port,
	std::string dbname,
	std::string user,
	std::string password)
	: connect("host=" + host +
		" port=" + port +
		" dbname=" + dbname +
		" user=" + user +
		" password=" + password) {

	if (!connect.is_open()) {
		throw pqxx::sql_error("Error connecting: ");
	}

	std::cout << "Connection completed successfully!" << std::endl;

	create_structure_data_base();
}

void control_data_base::create_structure_data_base() {

	pqxx::work tx{connect};

	tx.exec("CREATE TABLE IF NOT EXISTS client  ("
		"	client_id serial PRIMARY KEY,"
		"	first_name text NOT NULL,"
		"	last_name text NOT NULL,"
		"	email text UNIQUE);"

		"	CREATE TABLE IF NOT EXISTS phone("
		"	phone_id serial PRIMARY KEY,"
		"	client_id int REFERENCES client(client_id) NOT NULL,"
		"	phone_number text UNIQUE);"
	);

	tx.commit();

	std::cout << "The structure of the data base from 'client' and 'phone' tables created successfully!" << std::endl;
}

void control_data_base::add_new_client(std::string first_name, std::string last_name, std::string email) {
	pqxx::work  tx{connect};

	tx.exec("INSERT INTO client(first_name, last_name, email)"
		" VALUES('" + tx.esc(first_name) + "', '" + tx.esc(last_name) + "', '" + tx.esc(email) + "');");

	tx.commit();

	std::cout << "New client added successfully!" << std::endl;
}

void control_data_base::add_phone_for_client(int client_id, std::string phone) {

	check_valid_id(client_id);

	pqxx::work tx{connect};

	bool client_exists = false;

	for (const auto& [id] : tx.query<int>("SELECT client_id FROM client")) {
		if (id == client_id) client_exists = true;
	}

	if (!client_exists) {
		throw pqxx::sql_error("The client with ID = " + std::to_string(client_id) + " is not found.");
	}

	tx.exec("INSERT INTO phone(client_id, phone_number)"
		" VALUES(" + tx.esc(std::to_string(client_id)) + ", '" + tx.esc(phone) + "');");

	tx.commit();

	std::cout << "Number phone for the client with ID = " + std::to_string(client_id) << " added successfully!" << std::endl;
}

void control_data_base::update_client(int client_id, std::string field, std::string update_data) {

	check_valid_id(client_id);

	pqxx::work tx{connect};

	bool client_exists = false;

	for (const auto& [id] : tx.query<int>("SELECT client_id FROM client")) {
		if (id == client_id) client_exists = true;
	}

	if (!client_exists) {
		throw pqxx::sql_error("The client with ID = " + std::to_string(client_id) + " is not found.");
	}

	auto field_exists = [&field]() {
		std::vector<std::string> f = { "first_name", "last_name", "email", "phone_number" };

		auto yes = std::find(f.begin(), f.end(), field);

		if (yes != f.end()) {
			return true;
		}
		else {
			return false;
		}
	};

	if (!field_exists()) {
		throw pqxx::sql_error("The field named '" + field + "' does not exist");
	}

	if (field == "phone_number") {
		auto count_phone_numbers = tx.query_value<int>("SELECT COUNT(phone_id) FROM phone WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";");

		if (count_phone_numbers > 1) {

			auto counter{ 0 };
			std::vector<std::string> phone_numbers{};

			std::cout << "The client with ID = " << client_id << " has " << count_phone_numbers << " phone numbers" << std::endl;
			std::cout << "Which one do you want to upgrade?" << std::endl;

			for (const auto& [phone_number] : tx.query<std::string>("SELECT phone_number FROM phone WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";")) {
				std::cout << ++counter << ": " << phone_number << std::endl;
				phone_numbers.push_back(phone_number);
			}

			int sequence_number{};

			while (true) {

				std::string input_str{};

				std::cout << "Input sequence number: ";
				std::getline(std::cin, input_str);

				std::stringstream iss(input_str);

				if (!(iss >> sequence_number && iss.eof())) {
					std::cout << "Uncorrect input! Try again..." << std::endl;
				}
				else if (sequence_number > count_phone_numbers) {
					std::cout << "The value of the sequence number must not exceed " << counter << std::endl;
				}
				else break;
			}

			tx.exec("UPDATE phone SET phone_number = '" + tx.esc(update_data) + "' WHERE client_id = " + tx.esc(std::to_string(client_id)) + " AND phone_number  = '" + tx.esc(phone_numbers[sequence_number - 1]) + "';");

		}
		else if (count_phone_numbers == 1) {
			tx.exec("UPDATE phone SET phone_number = '" + tx.esc(update_data) + "' WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";");
		}
		else {
			throw pqxx::sql_error("The client with ID = " + std::to_string(client_id) + " has not a phone number");
		}
	}
	else {
		tx.exec("UPDATE client SET " + tx.esc(field) + " = '" + tx.esc(update_data) + "' WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";");
	}

	tx.commit();

	std::cout << "The client data with ID = " + std::to_string(client_id) << " updated successfully!" << std::endl;

}

void control_data_base::delete_phone_for_client(int client_id) {

	check_valid_id(client_id);

	pqxx::work tx{connect};

	bool client_exists = false;

	for (const auto& [id] : tx.query<int>("SELECT client_id FROM client")) {
		if (id == client_id) client_exists = true;
	}

	if (!client_exists) {
		throw pqxx::sql_error("The client with ID = " + std::to_string(client_id) + " is not found.");
	}

	auto count_phone_numbers = tx.query_value<int>("SELECT COUNT(phone_id) FROM phone WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";");

	if (count_phone_numbers > 1) {

		auto counter{ 0 };
		std::vector<std::string> phone_numbers{};

		std::cout << "The client with ID = " << client_id << " has " << count_phone_numbers << " phone numbers" << std::endl;
		std::cout << "Which one do you want to delete?" << std::endl;

		for (const auto& [phone_number] : tx.query<std::string>("SELECT phone_number FROM phone WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";")) {
			std::cout << ++counter << ": " << phone_number << std::endl;
			phone_numbers.push_back(phone_number);
		}

		int sequence_number{};

		while (true) {

			std::string input_str{};

			std::cout << "Input sequence number: ";
			std::getline(std::cin, input_str);

			std::stringstream iss(input_str);

			if (!(iss >> sequence_number && iss.eof())) {
				std::cout << "Uncorrect input! Try again..." << std::endl;
			}
			else if (sequence_number > count_phone_numbers) {
				std::cout << "The value of the sequence number must not exceed " << counter << std::endl;
			}
			else break;
		}

		tx.exec("DELETE FROM phone WHERE client_id = " + tx.esc(std::to_string(client_id)) + " AND phone_number  = '" + tx.esc(phone_numbers[sequence_number - 1]) + "';");

	}
	else if (count_phone_numbers == 1) {
		tx.exec("DELETE FROM phone WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";");
	}
	else {
		throw pqxx::sql_error("The client with ID = " + std::to_string(client_id) + " has not a phone number");
	}

	tx.commit();

	std::cout << "The phone for the client with ID =  " + std::to_string(client_id) << " deleted successfully!" << std::endl;
}

void control_data_base::delete_client(int client_id) {

	check_valid_id(client_id);

	pqxx::work tx{connect};

	bool client_exists = false;

	for (const auto& [id] : tx.query<int>("SELECT client_id FROM client")) {
		if (id == client_id) client_exists = true;
	}

	if (!client_exists) {
		throw pqxx::sql_error("The client with ID = " + std::to_string(client_id) + " is not found.");
	}

	auto phone_exists = tx.query_value<int>("SELECT COUNT(phone_id) FROM phone WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";");

	if (phone_exists) {
		tx.exec("DELETE FROM phone WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";");
	}

	tx.exec("DELETE FROM client WHERE client_id = " + tx.esc(std::to_string(client_id)) + ";");

	tx.commit();

	std::cout << "The client with ID = " + std::to_string(client_id) << " deleted successfully" << std::endl;

}

void control_data_base::find_client(std::string first_name, std::string last_name, std::string email = "", std::string phone = "") {
	pqxx::work tx(connect);

	int number_clients = tx.query_value<int>("SELECT COUNT(c.client_id)"
		" FROM client c"
		" LEFT JOIN phone p ON c.client_id = p.client_id"
		" WHERE c.first_name = '" + tx.esc(first_name) + "' AND c.last_name = '" + tx.esc(last_name) +
		+"' AND (c.email = '" + tx.esc(email) + "' OR p.phone_number = '" + tx.esc(phone) + "');");

	if (!number_clients) {
		throw pqxx::sql_error("Client is not found");
	}

	std::cout << "id" << "\t" << "first name" << "\t" << "last_name" << "\t" << "email" << "\t" << "phone" << std::endl;

	for (const auto& [id, first_name, last_name, email, phone] :
		tx.query<int, std::string, std::string, std::string, std::string>("SELECT c.client_id, c.first_name, c.last_name, c.email, p.phone_number"
			" FROM client c"
			" LEFT JOIN phone p ON c.client_id = p.client_id"
			" WHERE c.first_name = '" + tx.esc(first_name) + "' AND c.last_name = '" + tx.esc(last_name) +
			+"' AND (c.email = '" + tx.esc(email) + "' OR p.phone_number = '" + tx.esc(phone) + "');")) {
		std::cout << id << "\t" << first_name << "\t" << last_name << "\t" << email << "\t" << phone << std::endl;
	}

	tx.commit();

	std::cout << "The client found successfully" << std::endl;

}

void control_data_base::check_valid_id(int id) {
	if (id <= 0) {
		throw std::out_of_range("The client ID cannot be negative or equal zero");
	}
}