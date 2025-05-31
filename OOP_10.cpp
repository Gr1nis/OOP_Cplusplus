#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>

class User {
protected:
    std::string name;
    int id;
    int accessLevel;
public:
    User(std::string name, int id, int accessLevel) {
        if (id <= 0 || accessLevel < 0) {
            throw std::invalid_argument("Invalid id or access level");
        }
        this->name = name;
        this->id = id;
        this->accessLevel = accessLevel;
    }
    virtual void displayInfo() const {
        std::cout << "Name: " << name << ", ID: " << id << ", Access Level: " << accessLevel << std::endl;
    }
    virtual std::string getType() const = 0;
    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }
    virtual ~User() {}
};

class Student : public User {
public:
    Student(std::string name, int id) : User(name, id, 1) {}
    void displayInfo() const override {
        std::cout << "[Student] ";
        User::displayInfo();
    }
    std::string getType() const override {
        return "Student";
    }
};

class Teacher : public User {
public:
    Teacher(std::string name, int id) : User(name, id, 2) {}
    void displayInfo() const override {
        std::cout << "[Teacher] ";
        User::displayInfo();
    }
    std::string getType() const override {
        return "Teacher";
    }
};

class Administrator : public User {
public:
    Administrator(std::string name, int id) : User(name, id, 3) {}
    void displayInfo() const override {
        std::cout << "[Administrator] ";
        User::displayInfo();
    }
    std::string getType() const override {
        return "Administrator";
    }
};

// --- ДОБАВЛЕНИЕ: Класс Resource и метод checkAccess ---
class Resource {
private:
    std::string resourceName;
    int requiredAccessLevel;
public:
    Resource(const std::string& name, int level) : resourceName(name), requiredAccessLevel(level) {
        if (level < 0) {
            throw std::invalid_argument("Access level must be non-negative");
        }
    }

    std::string getName() const { return resourceName; }
    int getRequiredAccessLevel() const { return requiredAccessLevel; }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void displayResourceInfo() const {
        std::cout << "Resource: " << resourceName << ", Required Access Level: " << requiredAccessLevel << std::endl;
    }
};

void saveUsersToFile(const std::vector<std::shared_ptr<User>>& users, const std::string& filename) {
    std::ofstream out(filename);
    for (const auto& user : users) {
        out << user->getType() << "," << user->getName() << "," << user->getId() << "," << user->getAccessLevel() << "\n";
    }
}

void loadUsersFromFile(std::vector<std::shared_ptr<User>>& users, const std::string& filename) {
    std::ifstream in(filename);
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string type, name, idStr, accessStr;
        if (std::getline(ss, type, ',') && std::getline(ss, name, ',') && std::getline(ss, idStr, ',') && std::getline(ss, accessStr)) {
            int id = std::stoi(idStr);
            if (type == "Student") users.push_back(std::make_shared<Student>(name, id));
            else if (type == "Teacher") users.push_back(std::make_shared<Teacher>(name, id));
            else if (type == "Administrator") users.push_back(std::make_shared<Administrator>(name, id));
        }
    }
}

template<typename T>
std::shared_ptr<T> findUserById(const std::vector<std::shared_ptr<User>>& users, int id) {
    for (const auto& user : users) {
        if (user->getId() == id) {
            return std::dynamic_pointer_cast<T>(user);
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<User>> findUsersByName(const std::vector<std::shared_ptr<User>>& users, const std::string& name) {
    std::vector<std::shared_ptr<User>> results;
    for (const auto& user : users) {
        if (user->getName() == name) {
            results.push_back(user);
        }
    }
    return results;
}

void showMenu() {
    std::cout << "\n=== Access Control System Menu ===\n";
    std::cout << "1. Add Student\n";
    std::cout << "2. Add Teacher\n";
    std::cout << "3. Add Administrator\n";
    std::cout << "4. Find by ID\n";
    std::cout << "5. Find by Name\n";
    std::cout << "6. Display All\n";
    std::cout << "7. Save to File\n";
    std::cout << "8. Load from File\n";
    std::cout << "0. Exit\n";
}

int main() {
    std::vector<std::shared_ptr<User>> users;
    int choice;

    do {
        showMenu();
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::string name;
        int id;
        switch (choice) {
        case 1:
            std::cout << "Enter name and ID: ";
            std::cin >> name >> id;
            users.push_back(std::make_shared<Student>(name, id));
            break;
        case 2:
            std::cout << "Enter name and ID: ";
            std::cin >> name >> id;
            users.push_back(std::make_shared<Teacher>(name, id));
            break;
        case 3:
            std::cout << "Enter name and ID: ";
            std::cin >> name >> id;
            users.push_back(std::make_shared<Administrator>(name, id));
            break;
        case 4:
            std::cout << "Enter ID to search: ";
            std::cin >> id;
            {
                auto found = findUserById<User>(users, id);
                if (found) found->displayInfo();
                else std::cout << "User not found.\n";
            }
            break;
        case 5:
            std::cout << "Enter name to search: ";
            std::cin >> name;
            {
                auto found = findUsersByName(users, name);
                for (const auto& user : found) user->displayInfo();
                if (found.empty()) std::cout << "No users found.\n";
            }
            break;
        case 6:
            for (const auto& user : users) user->displayInfo();
            break;
        case 7:
            saveUsersToFile(users, "users.txt");
            std::cout << "Saved to users.txt\n";
            break;
        case 8:
            loadUsersFromFile(users, "users.txt");
            std::cout << "Loaded from users.txt\n";
            break;
        case 0:
            std::cout << "Exiting...\n";
            break;
        default:
            std::cout << "Invalid option.\n";
        }
    } while (choice != 0);

    return 0;
}
