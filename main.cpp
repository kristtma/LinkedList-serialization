#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct ListNode {
    ListNode* prev = nullptr;
    ListNode* next = nullptr;
    ListNode* rand = nullptr;
    std::string data;
};

class List {
public:
    List() : head_(nullptr), tail_(nullptr), size_(0) {}

    // Деструктор для предотвращения утечек памяти
    ~List() {
        Clear();
    }
    std::string operator[](int index)

    {

        ListNode* node = head_;

        for (int i = 0; i < index; ++i) {

            node = node->next;

        }

        return node->data;

    }

    // Быстрое добавление в конец списка (O(1))
    void PushBack(const string& value) {
        ListNode* newNode = new ListNode;
        newNode->data = value;
        if (!head_) {
            head_ = tail_ = newNode;
        } else {
            tail_->next = newNode;
            newNode->prev = tail_;
            tail_ = newNode;
        }
        size_++;
    }

    void Serialize(const string& filename) {
        ofstream out(filename, ios::binary);
        if (!out.is_open()) return;

        // Создаем карту адресов в индексы
        unordered_map<ListNode*, int> nodeToIndex;
        ListNode* current = head_;
        for (int i = 0; i < size_; ++i) {
            nodeToIndex[current] = i;
            current = current->next;
        }

        // Записываем размер
        out.write(reinterpret_cast<char*>(&size_), sizeof(size_));

        // Записываем данные узлов
        current = head_;
        while (current) {
            int dataLength = current->data.length();
            out.write(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
            out.write(current->data.c_str(), dataLength);

            int randIndex = (current->rand) ? nodeToIndex[current->rand] : -1;
            out.write(reinterpret_cast<char*>(&randIndex), sizeof(randIndex));

            current = current->next;
        }
        out.close();
    }

    void Deserialize(const string& filename) {
        ifstream in(filename, ios::binary);
        if (!in.is_open()) return;

        Clear();

        int newSize;
        in.read(reinterpret_cast<char*>(&newSize), sizeof(newSize));
        if (newSize <= 0) return;

        vector<ListNode*> nodes(newSize);
        vector<int> randIndices(newSize);

        // Создаем узлы и читаем данные
        for (int i = 0; i < newSize; ++i) {
            nodes[i] = new ListNode;
            int len;
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            string buffer(len, '\0');
            in.read(&buffer[0], len);
            nodes[i]->data = buffer;
            in.read(reinterpret_cast<char*>(&randIndices[i]), sizeof(randIndices[i]));
        }

        // Восстанавливаем связи next/prev
        head_ = nodes[0];
        tail_ = nodes[newSize - 1];
        for (int i = 0; i < newSize; ++i) {
            if (i > 0) nodes[i]->prev = nodes[i - 1];
            if (i < newSize - 1) nodes[i]->next = nodes[i + 1];

            //  Восстанавливаем rand
            if (randIndices[i] != -1) {
                nodes[i]->rand = nodes[randIndices[i]];
            }
        }
        size_ = newSize;
        in.close();
    }

    void Clear() {
        ListNode* current = head_;
        while (current) {
            ListNode* next = current->next;
            delete current;
            current = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    ListNode* GetHead() { return head_; }

private:
    ListNode* head_;
    ListNode* tail_;
    int size_;
};

int main() {
    // Чтение из inlet.in и построение списка
    ifstream in("inlet.in");
    if (!in.is_open()) return 1;

    List list;
    string line;
    vector<int> rands;
    vector<ListNode*> nodes;

    // Сначала создаем все узлы 
    while (getline(in, line)) {
        size_t sep = line.find_last_of(';'); // Ищем последний разделитель
        if (sep != string::npos) {
            string data = line.substr(0, sep);
            int randIdx = stoi(line.substr(sep + 1));
            list.PushBack(data);
            rands.push_back(randIdx);
        }
    }
    in.close();

    // Заполняем rand связи 
    ListNode* curr = list.GetHead();
    // Собираем указатели в вектор для быстрого доступа по индексу
    while(curr) {
        nodes.push_back(curr);
        curr = curr->next;
    }
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (rands[i] != -1 && rands[i] < nodes.size()) {
            nodes[i]->rand = nodes[rands[i]];
        }
    }

    // Сериализация в бинарный файл
    list.Serialize("outlet.out");

    // (Опционально для теста) Десериализация
    List newList;
    newList.Deserialize("outlet.out");
    for(int i = 0 ; i < 3;++i){
        std::cout << newList[i] << std::endl;
    }

    return 0;
}
