﻿#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Класс Inventory для хранения предметов
class Inventory {
private:
    std::unique_ptr<std::string[]> items;  // Умный указатель на массив строк
    size_t capacity;  // Вместимость инвентаря
    size_t count;     // Текущее количество предметов

public:
    // Конструктор
    Inventory(size_t cap)
        : capacity(cap), count(0), items(std::make_unique<std::string[]>(cap)) {
    }

    // Метод для добавления предмета
    void addItem(const std::string& item) {
        if (count < capacity) {
            items[count++] = item;
            std::cout << "Added: " << item << std::endl;
        }
        else {
            std::cout << "Inventory full! Cannot add: " << item << std::endl;
        }
    }

    // Метод для отображения инвентаря
    void displayInventory() const {
        std::cout << "Inventory: ";
        if (count == 0) {
            std::cout << "Empty" << std::endl;
            return;
        }
        for (size_t i = 0; i < count; ++i) {
            std::cout << items[i];
            if (i < count - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
};

// Проверка в main()
int main() {
    Inventory inv(5); // Инвентарь на 5 предметов

    inv.displayInventory(); // Должен быть пустым

    inv.addItem("Sword");
    inv.addItem("Shield");
    inv.addItem("Health Potion");
    inv.addItem("Mana Potion");
    inv.addItem("Key");
    inv.addItem("Extra Item"); // Это не добавится

    inv.displayInventory(); // Показываем список предметов

    return 0;
}
