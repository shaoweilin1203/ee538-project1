#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

// linked list node
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v) : val(v), next(nullptr) {}
};

// global data
vector<int> opinions;                  // opinions[i] = 0 or 1
vector<pair<int, int>> edge_list;      // (u, v): u influences v
vector<ListNode*> adj;                 // adj[v]: all nodes that influence v

// free linked lists
void free_adj() {
    for (int i = 0; i < (int)adj.size(); i++) {
        ListNode* cur = adj[i];
        while (cur != nullptr) {
            ListNode* tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }
    adj.clear();
}

// read opinions.txt
void load_opinions(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Failed to open " << filename << endl;
        exit(1);
    }

    vector<pair<int, int>> temp;
    string line;
    int max_id = -1;

    while (getline(fin, line)) {
        if (line.empty()) continue;
        for (char& c : line) {
            if (c == ',') c = ' ';
        }
        stringstream ss(line);
        int id, op;
        if (ss >> id >> op) {
            temp.push_back({id, op});
            if (id > max_id) max_id = id;
        }
    }

    opinions.assign(max_id + 1, 0);
    for (auto& p : temp) {
        opinions[p.first] = p.second;
    }

    fin.close();
}

// read edge_list.txt
void load_edges(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Failed to open " << filename << endl;
        exit(1);
    }

    edge_list.clear();
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        for (char& c : line) {
            if (c == ',') c = ' ';
        }
        stringstream ss(line);
        int u, v;
        if (ss >> u >> v) {
            edge_list.push_back({u, v});
        }
    }

    fin.close();
}

// (1)(2) allocate adjacency list and populate it
// For edge u -> v, voter u influences voter v,
// so u should be in v's friend list.
void build_adj_matrix() {
    free_adj();

    int n = opinions.size();
    adj.assign(n, nullptr);

    for (auto& e : edge_list) {
        int u = e.first;
        int v = e.second;

        if (u < 0 || u >= n || v < 0 || v >= n) continue;

        ListNode* node = new ListNode(u);
        node->next = adj[v];
        adj[v] = node;
    }
}

// (3) calculate fraction of nodes with opinion 1
double calculate_fraction_of_ones() {
    if (opinions.empty()) return 0.0;

    int count_one = 0;
    for (int op : opinions) {
        if (op == 1) count_one++;
    }

    return (double)count_one / (double)opinions.size();
}

// (4) majority opinion among node's neighbours; tie -> 0
int get_majority_friend_opinions(int node) {
    int count0 = 0;
    int count1 = 0;

    ListNode* cur = adj[node];
    while (cur != nullptr) {
        int neighbor = cur->val;
        if (opinions[neighbor] == 1) count1++;
        else count0++;
        cur = cur->next;
    }

    if (count1 > count0) return 1;
    return 0; // tie or 0 is majority
}

// (5) update all opinions simultaneously
// return true if any node changed
bool update_opinions() {
    int n = opinions.size();
    vector<int> new_opinions(n);

    for (int i = 0; i < n; i++) {
        new_opinions[i] = get_majority_friend_opinions(i);
    }

    bool changed = false;
    for (int i = 0; i < n; i++) {
        if (new_opinions[i] != opinions[i]) {
            changed = true;
            break;
        }
    }

    opinions = new_opinions;
    return changed;
}

int main() {
    load_opinions("opinions.txt");
    load_edges("edge_list.txt");
    build_adj_matrix();

    int max_iterations = 1000;
    int iteration = 0;

    cout << fixed << setprecision(2);

    // print iteration 0
    cout << "Iteration " << iteration
         << ": fraction of 1's = "
         << calculate_fraction_of_ones() << endl;

    // print every iteration from 1 until convergence / max_iterations
    while (iteration < max_iterations) {
        bool changed = update_opinions();
        iteration++;

        cout << "Iteration " << iteration
             << ": fraction of 1's = "
             << calculate_fraction_of_ones() << endl;

        if (!changed) break;
    }

    double final_fraction = calculate_fraction_of_ones();

    if (final_fraction == 0.0) {
        cout << "Consensus reached: all 0's" << endl;
    } else if (final_fraction == 1.0) {
        cout << "Consensus reached: all 1's" << endl;
    } else {
        cout << "No full consensus reached." << endl;
    }

    free_adj();
    return 0;
}
