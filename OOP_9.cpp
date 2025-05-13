#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

// ========== Шаблонный логгер ==========
template<typename T>
class Logger {
public:
    static void log(const T& message) {
        std::ofstream file("log.txt", std::ios::app);
        if (file.is_open()) {
            file << message << std::endl;
        }
    }
};

// ========== Класс Inventory ==========
class Inventory {
private:
    std::vector<std::string> items;
public:
    void addItem(const std::string& item) {
        items.push_back(item);
        std::cout << "Added item: " << item << std::endl;
        Logger<std::string>::log("Item added: " + item);
    }

    void removeItem(const std::string& item) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (*it == item) {
                items.erase(it);
                std::cout << "Removed item: " << item << std::endl;
                Logger<std::string>::log("Item removed: " + item);
                return;
            }
        }
        std::cout << "Item not found: " << item << std::endl;
    }

    void displayInventory() const {
        std::cout << "Inventory: ";
        for (const auto& item : items) {
            std::cout << item << ", ";
        }
        std::cout << std::endl;
    }

    const std::vector<std::string>& getItems() const {
        return items;
    }

    void setItems(const std::vector<std::string>& newItems) {
        items = newItems;
    }
};

// ========== Класс Character ==========
class Character {
private:
    std::string name;
    int health;
    int attack;
    int defense;
    int level;
    int experience;

public:
    Inventory inventory;

    Character(const std::string& n = "Hero", int h = 100, int a = 15, int d = 5)
        : name(n), health(h), attack(a), defense(d), level(1), experience(0) {
    }

    void attackEnemy(class Monster& enemy);

    void takeDamage(int dmg) {
        health -= dmg;
        if (health <= 0) {
            health = 0;
            throw std::runtime_error(name + " has died!");
        }
    }

    void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
        std::cout << name << " heals for " << amount << " HP!" << std::endl;
        Logger<std::string>::log(name + " heals for " + std::to_string(amount));
    }

    void gainExperience(int exp) {
        experience += exp;
        if (experience >= 100) {
            level++;
            experience -= 100;
            std::cout << name << " leveled up to level " << level << "!" << std::endl;
            Logger<std::string>::log(name + " leveled up to level " + std::to_string(level));
        }
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense
            << ", Level: " << level << ", Experience: " << experience << std::endl;
    }

    void save(const std::string& filename) const {
        std::ofstream out(filename);
        out << name << '\n' << health << '\n' << attack << '\n'
            << defense << '\n' << level << '\n' << experience << '\n';

        const auto& items = inventory.getItems();
        out << items.size() << '\n';
        for (const auto& item : items) {
            out << item << '\n';
        }
    }

    void load(const std::string& filename) {
        std::ifstream in(filename);
        if (!in.is_open()) return;
        size_t itemCount;
        in >> name >> health >> attack >> defense >> level >> experience;
        in >> itemCount;
        std::string item;
        std::vector<std::string> loadedItems;
        for (size_t i = 0; i < itemCount; ++i) {
            in >> item;
            loadedItems.push_back(item);
        }
        inventory.setItems(loadedItems);
    }

    int getHealth() const { return health; }
    int getDefense() const { return defense; }
    std::string getName() const { return name; }
};

// ========== Класс Monster ==========
class Monster {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Monster(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
    }

    virtual ~Monster() {}

    virtual void displayInfo() const {
        std::cout << "Monster: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    virtual bool isAlive() const { return health > 0; }

    virtual void takeDamage(int dmg) {
        health -= dmg;
        if (health < 0) health = 0;
    }

    virtual int getAttack() const { return attack; }
    virtual int getDefense() const { return defense; }
    virtual std::string getName() const { return name; }

    friend class Character;
};

void Character::attackEnemy(Monster& enemy) {
    int damage = attack - enemy.getDefense();
    if (damage < 0) damage = 0;
    enemy.takeDamage(damage);
    std::cout << name << " attacks " << enemy.getName() << " for " << damage << " damage!" << std::endl;
    Logger<std::string>::log(name + " attacks " + enemy.getName() + " for " + std::to_string(damage));
}

// ========== Разные типы монстров ==========
class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 10, 2) {}
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 100, 25, 10) {}
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Skeleton", 40, 15, 3) {}
};

// ========== Класс Game ==========
class Game {
private:
    Character player;

    void battle(Monster& monster) {
        std::cout << "\nBattle started with " << monster.getName() << "!\n";
        monster.displayInfo();

        try {
            while (monster.isAlive() && player.getHealth() > 0) {
                player.attackEnemy(monster);
                if (monster.isAlive()) {
                    int dmg = monster.getAttack() - player.getDefense();
                    if (dmg < 0) dmg = 0;
                    player.takeDamage(dmg);
                    std::cout << monster.getName() << " attacks " << player.getName() << " for " << dmg << " damage!\n";
                    Logger<std::string>::log(monster.getName() + " attacks " + player.getName() + " for " + std::to_string(dmg));
                }
            }

            if (!monster.isAlive()) {
                std::cout << monster.getName() << " defeated!\n";
                player.gainExperience(50);
                player.inventory.addItem("Loot from " + monster.getName());
            }

        }
        catch (const std::exception& e) {
            std::cerr << "\nGame Over: " << e.what() << std::endl;
        }
    }

public:
    void start() {
        std::cout << "Welcome to the RPG Game!\n";
        player.load("save.txt");
        player.displayInfo();

        while (true) {
            std::cout << "\n1. Fight Goblin\n2. Fight Dragon\n3. Fight Skeleton\n4. Heal\n5. Inventory\n6. Save & Exit\nChoose: ";
            int choice;
            std::cin >> choice;
            if (choice == 1) {
                Goblin goblin;
                battle(goblin);
            }
            else if (choice == 2) {
                Dragon dragon;
                battle(dragon);
            }
            else if (choice == 3) {
                Skeleton skeleton;
                battle(skeleton);
            }
            else if (choice == 4) {
                player.heal(20);
            }
            else if (choice == 5) {
                player.inventory.displayInventory();
            }
            else if (choice == 6) {
                player.save("save.txt");
                std::cout << "Game saved. Bye!\n";
                break;
            }
        }
    }
};

// ========== main ==========
int main() {
    Game game;
    game.start();
    return 0;
}
