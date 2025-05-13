#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <string>

// Базовый класс User
class User {
private:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& name, int id, int accessLevel)
        : name(name), id(id), accessLevel(accessLevel) {
        if (name.empty() || id < 0 || accessLevel < 0)
            throw std::invalid_argument("Invalid user data");
    }

    virtual ~User() = default;

    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    virtual void displayInfo() const {
        std::cout << "User: " << name << ", ID: " << id << ", Access Level: " << accessLevel << "\n";
    }

    virtual std::string getType() const = 0;
};

// Студент
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& name, int id, int accessLevel, const std::string& group)
        : User(name, id, accessLevel), group(group) {}

    void displayInfo() const override {
        std::cout << "Student: " << getName() << ", ID: " << getId()
                  << ", Access Level: " << getAccessLevel()
                  << ", Group: " << group << "\n";
    }

    std::string getType() const override { return "Student"; }
};

// Преподаватель
class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& name, int id, int accessLevel, const std::string& department)
        : User(name, id, accessLevel), department(department) {}

    void displayInfo() const override {
        std::cout << "Teacher: " << getName() << ", ID: " << getId()
                  << ", Access Level: " << getAccessLevel()
                  << ", Department: " << department << "\n";
    }

    std::string getType() const override { return "Teacher"; }
};

// Администратор
class Administrator : public User {
private:
    std::string role;

public:
    Administrator(const std::string& name, int id, int accessLevel, const std::string& role)
        : User(name, id, accessLevel), role(role) {}

    void displayInfo() const override {
        std::cout << "Administrator: " << getName() << ", ID: " << getId()
                  << ", Access Level: " << getAccessLevel()
                  << ", Role: " << role << "\n";
    }

    std::string getType() const override { return "Administrator"; }
};

// Ресурс
class Resource {
private:
    std::string resourceName;
    int requiredAccessLevel;

public:
    Resource(const std::string& name, int level)
        : resourceName(name), requiredAccessLevel(level) {
        if (name.empty() || level < 0)
            throw std::invalid_argument("Invalid resource data");
    }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void display() const {
        std::cout << "Resource: " << resourceName
                  << ", Required Access Level: " << requiredAccessLevel << "\n";
    }

    std::string getName() const { return resourceName; }
};

// Шаблонная система контроля доступа
template<typename U, typename R>
class AccessControlSystem {
private:
    std::vector<std::shared_ptr<U>> users;
    std::vector<std::shared_ptr<R>> resources;

public:
    void addUser(std::shared_ptr<U> user) {
        users.push_back(user);
    }

    void addResource(std::shared_ptr<R> resource) {
        resources.push_back(resource);
    }

    void checkUserAccess(int userId, const std::string& resourceName) {
        auto user = std::find_if(users.begin(), users.end(), [&](auto& u) { return u->getId() == userId; });
        auto resource = std::find_if(resources.begin(), resources.end(), [&](auto& r) { return r->getName() == resourceName; });

        if (user != users.end() && resource != resources.end()) {
            std::cout << (*user)->getName() << ( (*resource)->checkAccess(**user) ? " has access to " : " does NOT have access to ") << resourceName << "\n";
        } else {
            std::cout << "User or resource not found.\n";
        }
    }

    void saveUsersToFile(const std::string& filename) {
        std::ofstream out(filename);
        for (const auto& u : users) {
            out << u->getType() << "," << u->getName() << "," << u->getId() << "," << u->getAccessLevel() << "\n";
        }
    }

    void listUsers() const {
        for (const auto& u : users) {
            u->displayInfo();
        }
    }

    void sortUsersByAccess() {
        std::sort(users.begin(), users.end(), [](auto& a, auto& b) {
            return a->getAccessLevel() > b->getAccessLevel();
        });
    }

    std::shared_ptr<U> findUserByName(const std::string& name) const {
        auto it = std::find_if(users.begin(), users.end(), [&](auto& u) {
            return u->getName() == name;
        });
        return (it != users.end()) ? *it : nullptr;
    }
};

int main() {
    try {
        AccessControlSystem<User, Resource> system;

        system.addUser(std::make_shared<Student>("Egor", 101, 1, "CSS_OOP"));
        system.addUser(std::make_shared<Teacher>("Anatoliy", 102, 3, "OOP"));
        system.addUser(std::make_shared<Administrator>("Ivan", 103, 5, "IT Support"));

        system.addResource(std::make_shared<Resource>("Library", 1));
        system.addResource(std::make_shared<Resource>("Lab", 3));
        system.addResource(std::make_shared<Resource>("Server Room", 5));

        system.listUsers();
        std::cout << "\nChecking Access:\n";
        system.checkUserAccess(101, "Library");
        system.checkUserAccess(101, "Lab");
        system.checkUserAccess(103, "Server Room");

        std::cout << "\nSorted Users by Access Level:\n";
        system.sortUsersByAccess();
        system.listUsers();

        system.saveUsersToFile("users.txt");

        auto found = system.findUserByName("Bob");
        if (found) {
            std::cout << "\nFound user:\n";
            found->displayInfo();
        }

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
    }

    return 0;
}
