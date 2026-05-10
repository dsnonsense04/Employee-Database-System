#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <iterator>
#include <string>
#include <limits>
#include <vector>

struct Employee {
    std::string id;
    std::string name;
    std::string surname;
    std::string gender;
    std::string date_of_birth;
    bool is_deleted = false;
};

bool is_running = true;
int user_input;
Employee employee;

// -- Utility functions -- 

bool is_number(const std::string& str) {
    if (str.empty())
        return false;
    for (char c : str) {
        if (!std::isdigit(c))
            return false;
    }
    return true;
}

std::vector<std::string> read_file_to_vector(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::in);
    if (!file) {
        std::cerr << "Error opening file.\n";
        exit(EXIT_FAILURE);
    }
    std::string file_contents;
    std::string line;
    while (std::getline(file, line)) {
        file_contents += line + "\n";
    }
    std::vector<std::string> returnable;
    returnable.push_back(file_contents);
    return returnable;
}

std::string read_file_to_string(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::in);
    if (!file) {
        std::cerr << "Error opening file.\n";
        exit(EXIT_FAILURE);
    }
    std::string file_contents;
    std::string line;
    while (std::getline(file, line)) {
        file_contents += line + "\n";
    }
    return file_contents;
}

std::string get_id_from_csv(const std::string& line) {
    std::stringstream ss(line);
    std::string id;
    std::getline(ss, id, ',');
    return id;
}

std::string serialize(const Employee& emp) {
    return emp.id + "," +
            emp.name + "," +
            emp.surname + "," +
            emp.gender + "," +
            emp.date_of_birth + "," +
            (emp.is_deleted ? "1" : "0");
    // id,name,surname,gender,dob,is_deleted (CSV format)
}

Employee deserialize(const std::string& line) {
    std::stringstream ss(line);
    Employee emp;
    std::string deleted_flag;

    std::getline(ss, emp.id, ',');
    std::getline(ss, emp.name, ',');
    std::getline(ss, emp.surname, ',');
    std::getline(ss, emp.gender, ',');
    std::getline(ss, emp.date_of_birth, ',');
    std::getline(ss, deleted_flag);

    emp.is_deleted = (deleted_flag == "1");

    return emp;
}

void print_employee_details(const Employee& employee) {
    // Print in CSV format
    std::cout << employee.id << "," << employee.name << "," << employee.surname << "," << employee.gender << "," << employee.date_of_birth << "\n"; 
}

void add_record(std::string file_path, const Employee& emp) {
    std::ofstream file(file_path, std::ios::app);
    if (!file) {
        std::cerr << "Error opening file.\n";
        return;
    }
    file << serialize(emp) << "\n";
}

void read_records(const Employee& emp) {
    std::ifstream file("new_DB.txt");
    if (!file) {
        std::cerr << "Error opening file.\n";
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        Employee e = deserialize(line);
        if (!e.is_deleted)
            std::cout << line << "\n";
    }
}

void update_record(std::string file_path, const std::string& target_id, int field_choice, const std::string& new_value) {
    std::ifstream in(file_path);
    if (!in) {
        std::cerr << "Error opening file.\n";
        return;
    }
    std::vector<Employee> records;
    std::string line;
    bool found = false;
    while (std::getline(in, line)) {
        if (line.empty())
            continue;
        Employee emp = deserialize(line);
        if (emp.id == target_id && !emp.is_deleted) {
            found = true;
            switch (field_choice) {
                case 1:
                    emp.name = new_value;
                    break;
                case 2:
                    emp.surname = new_value;
                    break;
                case 3:
                    emp.gender = new_value;
                    break;
                case 4:
                    emp.date_of_birth = new_value;
                    break;
            }
        }
        records.push_back(emp);
    }
    in.close();
    if (!found) {
        std::cerr << "Error: Record not found.\n";
        return;
    }
    std::ofstream out(file_path, std::ios::trunc);
    for (const auto& e : records) {
        out << serialize(e) << "\n";
    }
}

// Delete CSV Employee in file (get by ID)
void delete_record(std::string file_path, const std::string& target_id) {
    std::ifstream in(file_path);
    if (!in) {
        std::cerr << "Error: cannot open file for reading.\n";
        return;
    }
    std::ofstream out("temp.txt");
    if (!out) {
        std::cerr << "Error: cannot open temp file.\n";
        return;
    }
    std::string line;
    bool found = false;
    while (std::getline(in, line)) {
        if (line.empty()) 
            continue;
        Employee e = deserialize(line);
        if (e.id == target_id && !e.is_deleted) {
            found = true;
            continue; // skip this record (this is the delete)
        }
        out << serialize(e) << '\n';
    }
    if (!found) {
        std::cerr << "Error: Record not found.\n";
        std::remove("temp.txt");
        return;
    }

    // Replace original file
    std::remove(file_path.c_str());
    std::rename("temp.txt", file_path.c_str());
    in.close();
    out.close();
}

/*
TODO: When adding employees, make sure no two employees can have the same ID.
      Implement editing employees properly. Original code broke.
*/

int main(void) {
    while (is_running) {
        prog_begin:
        std::cout << "RechnerWise Employee Database\n";
        std::cout << "\t1. Add employee\n";
        std::cout << "\t2. Edit employee by ID\n";
        std::cout << "\t3. Delete employee by ID\n";
        std::cout << "\t4. Exit program\n";

        std::cin >> user_input;
        std::string str_user_input = std::to_string(user_input);

        if (is_number(str_user_input) && (user_input <= 4 && user_input > 0)) {
            switch (user_input) {
                case 1:
                {
                    std::cout << "Adding employee.\n";
                    std::cout << "Enter employee ID: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::string input;
                    std::getline(std::cin, input);
                    if (!is_number(input)) {
                        std::cerr << "Error: ID must be numeric. Please try again.\n";
                        break;
                    } else {
                        // Check here if ID already exists; if so, throw an error and return
                        std::ifstream in_file("new_DB.txt");
                        if (!in_file) {
                            std::cerr << "Error opening file.\n";
                            return EXIT_FAILURE;
                        }
                        std::string line;
                        while (std::getline(in_file, line)) {
                            if (line.empty())
                                continue;
                            std::string id = get_id_from_csv(line);
                            if (input == id) {
                                std::cerr << "Error: Cannot add employee with same ID. Use a different ID instead, and try again.\n";
                                goto prog_begin;
                            } else {
                                employee.id = input;
                            }
                        }
                    }

                    std::cout << "Enter employee name: ";
                    std::string employee_name;
                    std::cin >> employee_name;
                    if (is_number(employee_name)) {
                        std::cerr << "Error: Name may not contain numbers. Please try again.\n";
                        break;
                    } else {
                        employee.name = employee_name;
                    }

                    std::cout << "Enter employee surname: ";
                    std::string employee_surname;
                    std::cin >> employee_surname;
                    if (is_number(employee_surname)) {
                        std::cerr << "Error: Surname may not contain numbers. Please try again.\n";
                        break;
                    } else {
                        employee.surname = employee_surname;
                    }

                    std::cout << "Enter employee gender: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::string gender;
                    std::getline(std::cin, gender);
                    if (gender != "M" && gender != "F") {
                        std::cerr << "Error: Gender must be either M or F. Please try again.\n";
                        break;
                    } else {
                        employee.gender = gender;
                    }

                    std::cout << "Enter employee date of birth: ";
                    std::string dob_input;
                    std::getline(std::cin, dob_input);
                    if (dob_input.size() != 8) {
                        std::cerr << "Error: Date of birth must be 8 characters long. Please try again.\n";
                        break;
                    } else {
                        employee.date_of_birth = dob_input;
                    }

                    print_employee_details(employee);
                    add_record("new_DB.txt", employee);
                    read_records(employee);

                } break;
                case 2:
                {
                    std::cout << "Editing employee.\n";
                    std::cout << "Choose an employee to edit: \n";
                    read_records(employee);
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::string editing_id;
                    std::getline(std::cin, editing_id);
                    if (!is_number(editing_id)) {
                        std::cerr << "Error: Must be a valid integer value based on the IDs shown. Please try again.\n";
                        break;
                    } else {
                        std::ifstream main_file("new_DB.txt");
                        if (!main_file) {
                            std::cerr << "Error opening file.\n";
                            return EXIT_FAILURE;
                        }
                        std::string line;
                        bool found = false;
                        while (std::getline(main_file, line)) {
                            if (line.empty()) {
                                std::cerr << "Error: No employees found in the database. Add to use the edit feature.\n";
                                break;
                            }
                            std::string id = get_id_from_csv(line);
                            if (editing_id == id) {
                                found = true;
                                break;
                            }
                        }
                        main_file.close();
                        if (!found) {
                            std::cerr << "Error: Employee " << editing_id << " does not exist. Cannot continue editing.\n";
                            break;
                        }
                        std::cout << "Employee " << editing_id << " exists. Continue editing.\n";
                        std::cout << "Which attribute of ID number " << editing_id << " do you want to edit? (Enter number as shown) \n";
                        std::cout << "\t1. Name\n";
                        std::cout << "\t2. Surname\n";
                        std::cout << "\t3. Gender\n";
                        std::cout << "\t4. Date of Birth\n";
                        int attribute_input;
                        std::cin >> attribute_input;
                        if (attribute_input < 1 || attribute_input > 4) {
                            std::cerr << "Error: Invalid attribute selection input. Please try again.\n";
                            break;
                        } else {
                            if (attribute_input == 1) {
                                std::cout << "Enter new name: ";
                            } else if (attribute_input == 2) {
                                std::cout << "Enter new surname: ";
                            } else if (attribute_input == 3) {
                                std::cout << "Update gender attribute: ";
                            } else if (attribute_input == 4) {
                                std::cout << "Enter new date of birth: ";
                            }
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::string new_value;
                            std::getline(std::cin, new_value);
                            update_record("new_DB.txt", editing_id, attribute_input, new_value);
                            std::cout << "Employee record updated successfully.\n";
                            read_records(employee);
                        }
                    }
                } break;
                case 3:
                {
                    std::cout << "Deleting employee.\n";
                    std::cout << "Choose an employee to delete: \n";
                    read_records(employee);
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::string deletion_id;
                    std::getline(std::cin, deletion_id);
                    if (!is_number(deletion_id)) {
                        std::cerr << "Error: Must be a valid integer value based on the IDs shown. Please try again.\n";
                        break;
                    } else {
                        delete_record("new_DB.txt", deletion_id);
                        read_records(employee);
                    }
                } break;
                case 4:
                    std::cout << "Exiting program...\n";
                    is_running = false;
                    return EXIT_SUCCESS;
                break;
                default:
                    std::cerr << "Error: Invalid option entered. Please try again.\n";
                    break;
            }
        } else {
            std::cerr << "Error: Invalid input; input must be number between 1-4. Please try again.\n";
            is_running = false;
        }
    }

    return EXIT_SUCCESS;
}