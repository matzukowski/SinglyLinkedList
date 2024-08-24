#include <iostream>
#include <functional>

using namespace std;

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        unique_ptr<Node> next;

        Node(const T& value) : data(value), next(nullptr) {}
        Node(T&& value) : data(move(value)), next(nullptr) {}
    };

    unique_ptr<Node> first;
    size_t list_size;

public:
    LinkedList() : first(nullptr), list_size(0) {}

    // constructor with initializer_list
    LinkedList(initializer_list<T> init_list) : first(nullptr), list_size(0) {
        for (const T& value : init_list) {
            insert_front(value);
        }
    }

    // moving constructor
    LinkedList(LinkedList&& other) noexcept : first(move(other.first)), list_size(other.list_size) {
        other.list_size = 0;
    }

    // move operator
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            first = move(other.first);
            list_size = other.list_size;
            other.list_size = 0;
        }
        return *this;
    }

    // inserting at the front - lvalue reference
    void insert_front(const T& value) {
        auto new_node = make_unique<Node>(value);
        new_node->next = move(first);
        first = move(new_node);
        ++list_size;
    }

    // inserting at the front - rvalue reference (move)
    void insert_front(T&& value) {
        auto new_node = make_unique<Node>(move(value));
        new_node->next = move(first);
        first = move(new_node);
        ++list_size;
    }

    size_t size() const {
        return list_size;
    }

    void remove_front() {
        if (first) {
            first = move(first->next);
            --list_size;
        }
    }

    void iterate() const {
        Node* current = first.get();
        while (current) {
            cout << current->data << " ";
            current = current->next.get();
        }
        cout << endl;
    }

    LinkedList<T> split_at(size_t index) {
        if (index >= list_size || list_size == 0) {
            return LinkedList<T>();
        }

        LinkedList<T> new_list;
        Node* current = first.get();
        Node* prev = nullptr;
        size_t current_index = 0;

        while (current_index < index && current != nullptr) {
            prev = current;
            current = current->next.get();
            ++current_index;
        }

        if (prev != nullptr) {
            new_list.first = move(prev->next);
            prev->next = nullptr;

            new_list.list_size = list_size - current_index;
            list_size = current_index;
        }
        else {
            new_list.first = move(first);
            new_list.list_size = list_size;
            list_size = 0;
        }

        return new_list;
    }

    LinkedList<T> split_when(function<bool(const T&)> condition) {
        LinkedList<T> new_list;
        Node* current = first.get();
        Node* prev = nullptr;
        size_t new_list_size = 0;

        while (current) {
            if (condition(current->data)) {
                if (prev) {
                    new_list.first = move(prev->next);
                    prev->next = nullptr;

                    Node* temp = new_list.first.get();
                    while (temp) {
                        ++new_list_size;
                        temp = temp->next.get();
                    }

                    list_size -= new_list_size;
                    new_list.list_size = new_list_size;

                    break;
                }
                else {
                    new_list.first = move(first);
                    list_size = 0;

                    Node* temp = new_list.first.get();
                    while (temp) {
                        ++new_list_size;
                        temp = temp->next.get();
                    }
                    new_list.list_size = new_list_size;

                    break;
                }
            }
            prev = current;
            current = current->next.get();
        }

        return new_list;
    }

    ~LinkedList() {
        while (first) {
            first = move(first->next);
        }
    }
};

int main() {
    LinkedList<int> list = { 5, 4, 3, 2, 1 };

    cout << "original list: ";
    list.iterate();
    cout << "size of original list: " << list.size() << "\n\n";

    list.insert_front(0);
    cout << "after inserting 0 at the front: ";
    list.iterate();
    cout << "size of list after insertion: " << list.size() << "\n\n";

    list.remove_front();
    cout << "after removing from the front: ";
    list.iterate();
    cout << "size of list after removal: " << list.size() << "\n\n";

    auto splited = list.split_when([](const int& value) { return value == 4; });
    cout << "original list after split at element '4': ";
    list.iterate();
    cout << "size of original list after split: " << list.size() << "\n\n";

    cout << "new list after split: ";
    splited.iterate();
    cout << "size of new list after split: " << splited.size() << "\n\n";

    list.insert_front(0);
    list.insert_front(-1);
    list.insert_front(-2);
    cout << "original list after inserting 0, -1, -2 at the front: ";
    list.iterate();
    cout << "size of list after insertion: " << list.size() << "\n\n";

    auto splitedAt = list.split_at(3);
    cout << "original list after split at element with index '3': ";
    list.iterate();
    cout << "new list after split: ";
    splitedAt.iterate();

    return 0;
}