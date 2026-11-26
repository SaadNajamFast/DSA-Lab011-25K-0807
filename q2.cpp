#include <iostream>
using namespace std;

const int MAX = 50;
const int MAXPIPES = 500;

struct Network {
    int n;
    int pipes[MAX + 1][MAX + 1];
};

struct Pipe {
    int from, to;
    bool oneWay;
};

Network oldNet;
Network newNet;

Pipe removed[2 * MAXPIPES];
int removedCount = 0;

int readInt(const char* message, int low, int high) {
    int x;
    while (true) {
        cout << message;
        if (cin >> x && x >= low && x <= high)
            return x;
        if (cin.eof())
            return low;
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Enter a number from " << low << " to " << high << ".\n";
    }
}

void readNetwork(Network &g, bool oneWay) {
    g.n = readInt("Number of junctions (1-50): ", 1, MAX);
    int m = readInt("Number of pipes (0-500): ", 0, MAXPIPES);

    if (oneWay)
        cout << "Enter each pipe as: from to\n";
    else
        cout << "Enter each pipe as: junction junction\n";

    for (int i = 0; i < m; i++) {
        cout << "  Pipe " << i + 1 << ": ";
        int u = readInt("", 1, g.n);
        int v = readInt("", 1, g.n);
        g.pipes[u][v]++;
        if (!oneWay && u != v)
            g.pipes[v][u]++;
    }
}

bool dfsOld(int u, bool visited[], int parent[], int &loopStart, int &loopEnd) {
    visited[u] = true;

    for (int v = 1; v <= oldNet.n; v++) {
        if (oldNet.pipes[u][v] == 0)
            continue;

        if (!visited[v]) {
            parent[v] = u;
            if (dfsOld(v, visited, parent, loopStart, loopEnd))
                return true;
        } else if (v != parent[u] || oldNet.pipes[u][v] > 1) {
            loopStart = v;
            loopEnd = u;
            return true;
        }
    }
    return false;
}

bool dfsNew(int u, int state[], int parent[], int &loopStart, int &loopEnd) {
    state[u] = 1;

    for (int v = 1; v <= newNet.n; v++) {
        if (newNet.pipes[u][v] == 0)
            continue;

        if (state[v] == 0) {
            parent[v] = u;
            if (dfsNew(v, state, parent, loopStart, loopEnd))
                return true;
        } else if (state[v] == 1) {
            loopStart = v;
            loopEnd = u;
            return true;
        }
    }

    state[u] = 2;
    return false;
}

bool findOldLoop(int parent[], int &loopStart, int &loopEnd) {
    bool visited[MAX + 1] = {false};
    for (int i = 1; i <= oldNet.n; i++)
        parent[i] = 0;

    for (int i = 1; i <= oldNet.n; i++) {
        if (!visited[i] && dfsOld(i, visited, parent, loopStart, loopEnd))
            return true;
    }
    return false;
}

bool findNewLoop(int parent[], int &loopStart, int &loopEnd) {
    int state[MAX + 1] = {0};
    for (int i = 1; i <= newNet.n; i++)
        parent[i] = 0;

    for (int i = 1; i <= newNet.n; i++) {
        if (state[i] == 0 && dfsNew(i, state, parent, loopStart, loopEnd))
            return true;
    }
    return false;
}

void printLoop(int parent[], int loopStart, int loopEnd) {
    int path[MAX + 1];
    int length = 0;

    for (int v = loopEnd; v != loopStart; v = parent[v])
        path[length++] = v;
    path[length++] = loopStart;

    for (int i = length - 1; i >= 0; i--)
        cout << path[i] << " -> ";
    cout << loopStart << "\n";
}

void saveRemoved(int from, int to, bool oneWay) {
    removed[removedCount].from = from;
    removed[removedCount].to = to;
    removed[removedCount].oneWay = oneWay;
    removedCount++;
}

void fixOldNetwork() {
    cout << "\n===== OLD NETWORK (two-way pipes) =====\n";
    int parent[MAX + 1];
    int s, e;
    int loops = 0;

    while (findOldLoop(parent, s, e)) {
        loops++;
        cout << "Loop " << loops << ": ";
        printLoop(parent, s, e);

        cout << "  Suggested removal: pipe " << e << " - " << s << " (removed, checking again)\n";
        oldNet.pipes[e][s]--;
        if (e != s)
            oldNet.pipes[s][e]--;
        saveRemoved(e, s, false);
    }

    if (loops == 0)
        cout << "No loop found.\n";
    else
        cout << "No loop left in the old network.\n";
}

void fixNewNetwork() {
    cout << "\n===== NEW NETWORK (one-way pipes) =====\n";
    int parent[MAX + 1];
    int s, e;
    int loops = 0;

    while (findNewLoop(parent, s, e)) {
        loops++;
        cout << "Loop " << loops << ": ";
        printLoop(parent, s, e);

        cout << "  Suggested removal: pipe " << e << " -> " << s << " (removed, checking again)\n";
        newNet.pipes[e][s]--;
        saveRemoved(e, s, true);
    }

    if (loops == 0)
        cout << "No loop found.\n";
    else
        cout << "No loop left in the new network.\n";
}

int main() {
    cout << "--- Old network (two-way pipes) ---\n";
    readNetwork(oldNet, false);

    cout << "\n--- New network (one-way pipes) ---\n";
    readNetwork(newNet, true);

    fixOldNetwork();
    fixNewNetwork();

    cout << "\n===== REPORT =====\n";
    if (removedCount > 0) {
        cout << "Pipes removed:\n";
        for (int i = 0; i < removedCount; i++) {
            cout << "  " << i + 1 << ". " << removed[i].from;
            if (removed[i].oneWay)
                cout << " -> " << removed[i].to << " (new network)\n";
            else
                cout << " - " << removed[i].to << " (old network)\n";
        }
    }
    cout << "Network is safe\n";
    cout << "Total pipes removed: " << removedCount << "\n";
    return 0;
}
