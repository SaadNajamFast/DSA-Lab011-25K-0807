#include <iostream>
using namespace std;

const int MAX = 20;
const int MAXLINKS = 200;
const int LEN = 50;
const int NOLINK = -1;
const int INF = 1000000000;

struct Link {
    int u, v, cost;
};

int n;
char names[MAX][LEN];

Link links[MAXLINKS];
int linkCount;
Link oldLinks[MAXLINKS];
int oldCount;
int budget;

int costMatrix[MAX][MAX];
bool isOld[MAX][MAX];

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

void readName(char name[]) {
    cin >> ws;
    cin.getline(name, LEN);
    if (cin.fail() && !cin.eof()) {
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

void readInput() {
    n = readInt("Enter number of villages (2-20): ", 2, MAX);

    cout << "Enter village names:\n";
    for (int i = 0; i < n; i++) {
        cout << "  Village " << i << ": ";
        readName(names[i]);
    }

    linkCount = readInt("\nNumber of possible cable links (0-200): ", 0, MAXLINKS);
    cout << "Enter each as: village village cost(lakh)\n";
    for (int i = 0; i < linkCount; i++) {
        cout << "  Link " << i + 1 << ": ";
        links[i].u = readInt("", 0, n - 1);
        links[i].v = readInt("", 0, n - 1);
        links[i].cost = readInt("", 0, 1000000);
    }

    oldCount = readInt("\nNumber of old links (0-200): ", 0, MAXLINKS);
    if (oldCount > 0)
        cout << "Enter each as: village village\n";
    for (int i = 0; i < oldCount; i++) {
        cout << "  Old link " << i + 1 << ": ";
        oldLinks[i].u = readInt("", 0, n - 1);
        oldLinks[i].v = readInt("", 0, n - 1);
        oldLinks[i].cost = 0;
    }

    budget = readInt("\nBudget (lakh rupees): ", 0, INF);
}

void buildMatrix() {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            costMatrix[i][j] = NOLINK;
            isOld[i][j] = false;
        }

    for (int i = 0; i < linkCount; i++) {
        int u = links[i].u, v = links[i].v;
        if (costMatrix[u][v] == NOLINK || links[i].cost < costMatrix[u][v])
            costMatrix[u][v] = costMatrix[v][u] = links[i].cost;
    }

    for (int i = 0; i < oldCount; i++) {
        int u = oldLinks[i].u, v = oldLinks[i].v;
        costMatrix[u][v] = costMatrix[v][u] = 0;
        isOld[u][v] = isOld[v][u] = true;
    }
}

int findRoot(int parent[], int x) {
    if (parent[x] != x)
        parent[x] = findRoot(parent, parent[x]);
    return parent[x];
}

bool join(int parent[], int a, int b) {
    int ra = findRoot(parent, a);
    int rb = findRoot(parent, b);
    if (ra == rb)
        return false;
    parent[rb] = ra;
    return true;
}

void sortLinks(Link arr[], int count) {
    for (int i = 0; i < count - 1; i++) {
        int smallest = i;
        for (int j = i + 1; j < count; j++)
            if (arr[j].cost < arr[smallest].cost)
                smallest = j;
        Link temp = arr[i];
        arr[i] = arr[smallest];
        arr[smallest] = temp;
    }
}

int kruskal(int parent[]) {
    for (int i = 0; i < n; i++)
        parent[i] = i;

    cout << "\n===== KRUSKAL =====\n";

    for (int i = 0; i < oldCount; i++)
        join(parent, oldLinks[i].u, oldLinks[i].v);

    Link sorted[MAXLINKS];
    for (int i = 0; i < linkCount; i++)
        sorted[i] = links[i];
    sortLinks(sorted, linkCount);

    int total = 0;
    int added = 0;
    cout << "New links to lay:\n";
    for (int i = 0; i < linkCount; i++) {
        if (join(parent, sorted[i].u, sorted[i].v)) {
            cout << "  " << names[sorted[i].u] << " - " << names[sorted[i].v]
                 << " : " << sorted[i].cost << " lakh\n";
            total += sorted[i].cost;
            added++;
        }
    }
    if (added == 0)
        cout << "  none\n";

    cout << "Total extra cost (Kruskal): " << total << " lakh\n";
    return total;
}

int prim(int group, int from[], int &parts) {
    int key[MAX];
    bool inTree[MAX] = {false};
    int size = 0;

    for (int i = 0; i < n; i++) {
        key[i] = INF;
        from[i] = -1;
        if (group & (1 << i))
            size++;
    }

    int total = 0;
    parts = 0;

    for (int step = 0; step < size; step++) {
        int u = -1;
        for (int i = 0; i < n; i++) {
            if ((group & (1 << i)) && !inTree[i] && (u == -1 || key[i] < key[u]))
                u = i;
        }

        if (key[u] == INF)
            parts++;
        else
            total += key[u];
        inTree[u] = true;

        for (int v = 0; v < n; v++) {
            if ((group & (1 << v)) && !inTree[v] &&
                costMatrix[u][v] != NOLINK && costMatrix[u][v] < key[v]) {
                key[v] = costMatrix[u][v];
                from[v] = u;
            }
        }
    }

    return total;
}

void printTreeLinks(int from[]) {
    for (int v = 0; v < n; v++) {
        if (from[v] == -1)
            continue;
        cout << "  " << names[from[v]] << " - " << names[v] << " : ";
        if (isOld[from[v]][v])
            cout << "old link (free)\n";
        else
            cout << costMatrix[from[v]][v] << " lakh\n";
    }
}

int countBits(int x) {
    int count = 0;
    while (x > 0) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}

void bestWithinBudget() {
    int bestGroup = 1;
    int bestSize = 1;
    int bestCost = 0;
    int from[MAX];
    int parts;

    for (int group = 1; group < (1 << n); group += 2) {
        int size = countBits(group);
        if (size < bestSize)
            continue;

        int cost = prim(group, from, parts);
        if (parts == 1 && cost <= budget && (size > bestSize || cost < bestCost)) {
            bestGroup = group;
            bestSize = size;
            bestCost = cost;
        }
    }

    cout << "Most villages that can be connected from " << names[0]
         << " within the budget: " << bestSize << "\n";
    cout << "Villages: ";
    bool first = true;
    for (int i = 0; i < n; i++) {
        if (bestGroup & (1 << i)) {
            if (!first)
                cout << ", ";
            cout << names[i];
            first = false;
        }
    }
    cout << "\nCost used: " << bestCost << " lakh\n";

    if (bestSize > 1) {
        cout << "Links used:\n";
        prim(bestGroup, from, parts);
        printTreeLinks(from);
    }
}

int main() {
    readInput();
    buildMatrix();

    int parent[MAX];
    int kruskalTotal = kruskal(parent);

    cout << "\n===== PRIM =====\n";
    int from[MAX];
    int parts;
    int primTotal = prim((1 << n) - 1, from, parts);
    cout << "Links in the tree:\n";
    printTreeLinks(from);
    cout << "Total extra cost (Prim): " << primTotal << " lakh\n";

    if (kruskalTotal == primTotal)
        cout << "Check passed: both totals are the same.\n";
    else
        cout << "Check failed: totals are different!\n";

    cout << "\n===== BUDGET =====\n";
    cout << "Budget: " << budget << " lakh\n";
    if (kruskalTotal <= budget)
        cout << "The total cost is within the budget.\n";
    else {
        cout << "The total cost is more than the budget.\n";
        bestWithinBudget();
    }

    cout << "\n===== UNREACHABLE VILLAGES =====\n";
    bool any = false;
    for (int i = 1; i < n; i++) {
        if (findRoot(parent, i) != findRoot(parent, 0)) {
            if (!any)
                cout << "These villages can never be connected to " << names[0] << ":\n";
            cout << "  " << names[i] << "\n";
            any = true;
        }
    }
    if (!any)
        cout << "All villages can be connected.\n";

    return 0;
}
