#pragma once

#include <string>

#include <pqxx/pqxx>

class control_data_base {
public:
	control_data_base(std::string host,
		std::string port,
		std::string dbname,
		std::string user,
		std::string password);

	void create_structure_data_base();
	void add_new_client(std::string first_name, std::string last_name, std::string email);
	void add_phone_for_client(int client_id, std::string phone);
	void update_client(int client_id, std::string field, std::string update_data);
	void delete_phone_for_client(int client_id);
	void delete_client(int client_id);
	void find_client(std::string first_name, std::string last_name, std::string email, std::string phone);

private:
	pqxx::connection connect;
	void check_valid_id(int id);
};