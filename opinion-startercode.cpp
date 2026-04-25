#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
using namespace std;

/********************DO NOT EDIT**********************/
void read_opinions(string filename);
void read_edges(string filename);
void build_adj_matrix();

int total_nodes = 0;
/****************************************************************/

vector<int> opinions;

// adjacency list:
// adj[target] stores all source nodes that influence target
vector<vector<int>> adj;

void ensure_node_exists(int id)
{
    if (id >= total_nodes)
    {
        total_nodes = id + 1;
        opinions.resize(total_nodes, 0);
        adj.resize(total_nodes);
    }
}

void build_adj_matrix()
{
    // No longer needed.
    // Edges are inserted directly into adj when reading edge_list.txt.
}

double calculate_fraction_of_ones()
{
    int count_ones = 0;

    for (int i = 0; i < total_nodes; i++)
    {
        if (opinions[i] == 1)
            count_ones++;
    }

    if (total_nodes == 0)
        return 0.0;

    return (double)count_ones / total_nodes;
}

int get_majority_friend_opinions(int node)
{
    int count_zero = 0;
    int count_one = 0;

    for (int i = 0; i < (int)adj[node].size(); i++)
    {
        int neighbor = adj[node][i];

        if (opinions[neighbor] == 1)
            count_one++;
        else
            count_zero++;
    }

    if (count_one > count_zero)
        return 1;

    return 0;
}

bool update_opinions()
{
    vector<int> new_opinions(total_nodes);
    bool changed = false;

    for (int i = 0; i < total_nodes; i++)
    {
        new_opinions[i] = get_majority_friend_opinions(i);

        if (new_opinions[i] != opinions[i])
            changed = true;
    }

    opinions = new_opinions;
    return changed;
}

void print_fraction(int iteration)
{
    double frac = calculate_fraction_of_ones();

    cout << "Iteration " << iteration << ": fraction of 1's = ";

    if (frac == 0.0)
        cout << 0 << endl;
    else
        cout << fixed << setprecision(2) << frac << endl;
}

int main()
{
    read_opinions("opinions.txt");
    read_edges("edge_list.txt");

    build_adj_matrix();

    cout << "Total nodes: " << total_nodes << endl;

    int max_iterations = 30;
    int iteration = 0;

    print_fraction(iteration);

    while (iteration < max_iterations)
    {
        bool opinions_changed = update_opinions();

        if (!opinions_changed)
            break;

        iteration++;
        print_fraction(iteration);
    }

    double final_fraction = calculate_fraction_of_ones();

    if (final_fraction == 1.0)
        cout << "Consensus reached: all 1's" << endl;
    else if (final_fraction == 0.0)
        cout << "Consensus reached: all 0's" << endl;
    else
        cout << "No consensus reached after " << iteration << " iterations" << endl;

    return 0;
}

/*********** Functions to read files **************************/

void read_opinions(string filename)
{
    ifstream file(filename);
    int id, opinion;

    while (file >> id >> opinion)
    {
        ensure_node_exists(id);
        opinions[id] = opinion;
    }

    file.close();
}

void read_edges(string filename)
{
    ifstream file(filename);
    int source, target;

    while (file >> source >> target)
    {
        ensure_node_exists(source);
        ensure_node_exists(target);

        adj[target].push_back(source);
    }

    file.close();
}
