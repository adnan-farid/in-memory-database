#include <iostream>
#include <unordered_map>
#include <optional>
#include <stdexcept>

using namespace std;

class InMemoryDB {
private:
    unordered_map<string, int> mainDatabase;
    //database before commit() is called
    unordered_map<string, int> tempDatabase;
    //if transaction is occuring
    bool stateTransaction = false;

public:
    optional<int> get(const string& key) {
        //used optional to return null instead of an error
        auto it = mainDatabase.find(key);
        if (it != mainDatabase.end()) {
            return it->second;
        }
        return nullopt;
    }

    void put(const string& key, int val) {
        if (!stateTransaction) {
            throw logic_error("transaction not in progress.");
        }
        tempDatabase[key] = val;
    }

    void begin_transaction() {
        if (stateTransaction) {
            throw logic_error("transaction already in progress");
        }
        stateTransaction = true;
        tempDatabase.clear();
    }

    void commit() {
        if (!stateTransaction) {
            throw logic_error("no transaction in progress to commit to the main database.");
        }
        for (const auto& pair : tempDatabase) {
            mainDatabase[pair.first] = pair.second;
        }
        stateTransaction = false;
        tempDatabase.clear();
    }

    void rollback() {
        if (!stateTransaction) {
            throw logic_error("no transaction to rollback");
        }
        stateTransaction = false;
        tempDatabase.clear();
    }
};

int main() {
    InMemoryDB db;

    auto print_result = [](const optional<int>& result) {
        if (result) cout << "Value: " << *result << endl;
        else cout << "Value not found in main database." << endl;
    };
    //test case 1
    print_result(db.get("A")); //return null
    //test case 2
    try {
        db.put("A", 5);
    } catch (const logic_error& e) {
        cout << "Error: " << e.what() << endl; //error
    }
    //test case 3
    db.begin_transaction(); //success
    //test case 4
    db.put("A", 5); //success
    //test case 5
    print_result(db.get("A")); // error
    //test case 6
    db.put("A", 6); //success
    //test case 7
    db.commit(); //success
    //test case 8
    print_result(db.get("A")); //return 6
    //test case 9
    try {
        db.commit();
    } catch (const logic_error& e) {
        cout << "Error: " << e.what() << endl; //error
    }
    //test case 10
    try {
        db.rollback(); //error
    } catch (const logic_error& e) {
        cout << "Error: " << e.what() << endl;
    }
    //test case 11
    print_result(db.get("B")); //error
    //test case 12
    db.begin_transaction(); //success
    //test case 13
    db.put("B", 10);//success
    //test case 14
    db.rollback(); //success
    //test case 15
    print_result(db.get("B")); //returns null
}

