#include <iostream>
#include <string>
#include <cmath>
#include <random>
#include <vector>

using namespace std;

// 随机数生成器
random_device rd;
mt19937 rng(rd());

class Character {
protected:
    string name;
    int level;
    int power;
    int knowledge;
    int luck;
    int exp;
    int hp;

    static const int EXP_LV = 10; // 每一级所需的经验值系数

public:
    Character(string n, int lv, int pow, int kno, int lu)
        : name(n), level(lv), power(pow), knowledge(kno), luck(lu), exp(0), hp(100) {}

    virtual ~Character() {}

    string getName() const { return name; }
    int getHP() const { return hp; }

    virtual void print() const {
        cout << "Name: " << name << ", Level: " << level
             << ", Power: " << power << ", Knowledge: " << knowledge
             << ", Luck: " << luck << ", Exp: " << exp
             << ", HP: " << hp << endl;
    }

    void levelUp(int poInc, int knInc, int luInc) {
        level++;
        power += poInc;
        knowledge += knInc;
        luck += luInc;
        cout << name << " leveled up to " << level << "!" << endl;
    }

    virtual void beatMonster(int expGained) {
        exp += expGained;
        while (exp >= pow(level, 2) * EXP_LV) {
            levelUp(5, 5, 5); // 默认升级参数
        }
    }

    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }

    void heal(int amount) {
        hp += amount;
        if (hp > 100) hp = 100; // 假设最大生命值为 100
    }
};

class Warrior : public Character {
private:
    static const int PO_LV = 10;
    static const int KN_LV = 5;
    static const int LU_LV = 5;

public:
    Warrior(string n, int lv = 1)
        : Character(n, lv, lv * PO_LV, lv * KN_LV, lv * LU_LV) {}

    void print() const override {
        cout << "Warrior ";
        Character::print();
    }

    void beatMonster(int expGained) override {
        exp += expGained;
        while (exp >= pow(level, 2) * EXP_LV) {
            levelUp(PO_LV, KN_LV, LU_LV);
        }
    }

    void attackEnemy(class Enemy& enemy);
};

class Wizard : public Character {
private:
    static const int PO_LV = 5;
    static const int KN_LV = 10;
    static const int LU_LV = 7;

public:
    Wizard(string n, int lv = 1)
        : Character(n, lv, lv * PO_LV, lv * KN_LV, lv * LU_LV) {}

    void print() const override {
        cout << "Wizard ";
        Character::print();
    }

    void beatMonster(int expGained) override {
        exp += expGained;
        while (exp >= pow(level, 2) * EXP_LV) {
            levelUp(PO_LV, KN_LV, LU_LV);
        }
    }
};

class Archer : public Character {
private:
    static const int PO_LV = 7;
    static const int KN_LV = 5;
    static const int LU_LV = 10;

public:
    Archer(string n, int lv = 1)
        : Character(n, lv, lv * PO_LV, lv * KN_LV, lv * LU_LV) {}

    void print() const override {
        cout << "Archer ";
        Character::print();
    }

    void beatMonster(int expGained) override {
        exp += expGained;
        while (exp >= pow(level, 2) * EXP_LV) {
            levelUp(PO_LV, KN_LV, LU_LV);
        }
    }

    void attackEnemy(class Enemy& enemy);
};

class Enemy {
private:
    string name;
    int hp;
    int attack;

public:
    Enemy(string n, int h, int a) : name(n), hp(h), attack(a) {}

    string getName() const { return name; }

    void attackCharacter(Character& character) {
        uniform_int_distribution<int> damageDist(attack - 5, attack + 5); // 随机波动
        int actualDamage = damageDist(rng);
        cout << "The " << name << " lunges at " << character.getName() << " and deals " << actualDamage << " damage!" << endl;
        character.takeDamage(actualDamage);
    }

    bool isDefeated() const { return hp <= 0; }

    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }

    void print() const {
        cout << "Enemy: " << name << ", HP: " << hp << ", Attack: " << attack << endl;
    }
};

void randomEvent(Character& character); // 提前声明


void Warrior::attackEnemy(Enemy& enemy) {
    uniform_int_distribution<int> critChance(1, 100);
    bool isCritical = critChance(rng) <= 20; // 20% 暴击几率
    int damage = power;
    if (isCritical) {
        damage *= 2;
        cout << "With a powerful swing, " << name << " lands a critical blow! " << endl;
    }
    enemy.takeDamage(damage);
    cout << name << " strikes " << enemy.getName() << " and inflicts " << damage << " damage!" << endl;
    randomEvent(*this);
}

void Archer::attackEnemy(Enemy& enemy) {
    uniform_int_distribution<int> hitChance(1, 100);
    if (hitChance(rng) <= 85) { // 85% 命中率
        int damage = power;
        cout << name << " skillfully aims an arrow and hits " << enemy.getName() << " for " << damage << " damage!" << endl;
        enemy.takeDamage(damage);
    } else {
        cout << name << " fires an arrow, but it misses its mark!" << endl;
    }
    randomEvent(*this);
}

class Item {
protected:
    string name;

public:
    Item(string n) : name(n) {}
    virtual ~Item() {}
    virtual void use(Character& character) = 0;
    virtual void print() const { cout << "Item: " << name << endl; }
};

class HealthPotion : public Item {
public:
    HealthPotion() : Item("Health Potion") {}

    void use(Character& character) override {
        character.heal(20); // 恢复20点生命值
        cout << "The potion glows as " << character.getName() << " drinks it, restoring 20 HP." << endl;
    }
};

Item* generateRandomItem() {
    uniform_int_distribution<int> itemType(1, 3); // 假设有3种不同物品
    int type = itemType(rng);
    if (type == 1) return new HealthPotion();
    // 添加更多物品类型
    return nullptr;
}

void dropItem() {
    Item* item = generateRandomItem();
    if (item) {
        cout << "As the dust settles, a shimmering item is revealed: ";
        item->print();
    } else {
        cout << "The battlefield is empty... No items were found." << endl;
    }
}

class Quest {
private:
    string description;
    bool isCompleted;

public:
    Quest(string desc) : description(desc), isCompleted(false) {}

    void complete() {
        uniform_int_distribution<int> rewardDist(50, 150); // 随机奖励范围
        int rewardExp = rewardDist(rng);
        isCompleted = true;
        cout << "The quest \"" << description << "\" is complete!" << endl;
        cout << "You receive " << rewardExp << " EXP as a reward!" << endl;
    }

    void print() const {
        cout << "Quest: " << description << (isCompleted ? " (Completed)" : " (Incomplete)") << endl;
    }
};

void randomEvent(Character& character) {
    uniform_int_distribution<int> eventChance(1, 100);
    int event = eventChance(rng);

    if (event <= 30) {
        cout << "As " << character.getName() << " moves forward, they discover a hidden treasure chest!" << endl;
        dropItem();
    } else if (event <= 60) {
        cout << "While exploring, " << character.getName() << " accidentally steps into a trap!" << endl;
        uniform_int_distribution<int> trapDamage(10, 30);
        int damage = trapDamage(rng);
        character.takeDamage(damage);
        cout << character.getName() << " suffers " << damage << " damage from the trap." << endl;
    } else {
        cout << character.getName() << " continues onward without incident." << endl;
    }
}

int main() {
    Warrior warrior("Alice", 10);
    Wizard wizard("Sophie", 8);
    Archer archer("Robin", 9);
    Enemy goblin("Goblin", 50, 15);

    cout << "Our heroes gather, their hearts resolute as they face their foe." << endl;
    warrior.print();
    wizard.print();
    archer.print();
    goblin.print();

    cout << "\n--- The Battle Begins ---" << endl;
    goblin.attackCharacter(warrior);
    randomEvent(warrior);
    warrior.attackEnemy(goblin);

    goblin.attackCharacter(wizard);
    randomEvent(wizard);

    goblin.attackCharacter(archer);
    randomEvent(archer);
    archer.attackEnemy(goblin);

    if (goblin.isDefeated()) {
        cout << "With a final blow, the Goblin falls!" << endl;
        warrior.beatMonster(200);
        wizard.beatMonster(150);
        archer.beatMonster(180);
        dropItem();
    } else {
        cout << "The Goblin, though wounded, retreats into the shadows." << endl;
    }

    cout << "\nThe battle is over, but the journey continues..." << endl;
    return 0;
}
