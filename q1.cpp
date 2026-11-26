#include <iostream>
using namespace std;

const int MAX = 20;
const int LEN = 50;

struct Node {
    int building;
    Node* next;
};

int n;
char names[MAX][LEN];
int matrix[MAX][MAX];
Node* adjList[MAX];

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

void freeList() {
    for (int i = 0; i < MAX; i++) {
        while (adjList[i] != NULL) {
            Node* temp = adjList[i];
            adjList[i] = adjList[i]->next;
            delete temp;
        }
    }
}

void buildList() {
    freeList();
    for (int i = 0; i < n; i++) {
        for (int j = n - 1; j >= 0; j--) {
            if (matrix[i][j] == 1) {
                Node* newNode = new Node;
                newNode->building = j;
                newNode->next = adjList[i];
                adjList[i] = newNode;
            }
        }
    }
}

void bfs(int start, int parent[], int level[]) {
    int q[MAX];
    int front = 0, rear = 0;

    for (int i = 0; i < n; i++) {
        parent[i] = -1;
        level[i] = -1;
    }

    level[start] = 0;
    q[rear++] = start;

    while (front < rear) {
        int u = q[front++];
        for (Node* t = adjList[u]; t != NULL; t = t->next) {
            int v = t->building;
            if (level[v] == -1) {
                level[v] = level[u] + 1;
                parent[v] = u;
                q[rear++] = v;
            }
        }
    }
}

void dfs(int u, bool visited[]) {
    visited[u] = true;
    for (Node* t = adjList[u]; t != NULL; t = t->next) {
        if (!visited[t->building])
            dfs(t->building, visited);
    }
}

void showBuildings() {
    cout << "\nBuildings:\n";
    for (int i = 0; i < n; i++)
        cout << "  " << i + 1 << ". " << names[i] << "\n";
}

void showLevels() {
    int parent[MAX], level[MAX];
    bfs(0, parent, level);

    cout << "\nDistance from main gate (" << names[0] << "):\n";
    for (int d = 1; d <= 3; d++) {
        cout << "  " << d << " walkway(s) away: ";
        int count = 0;
        for (int i = 0; i < n; i++) {
            if (level[i] == d) {
                if (count > 0)
                    cout << ", ";
                cout << names[i];
                count++;
            }
        }
        if (count == 0)
            cout << "none";
        cout << "\n";
    }
}

void showMap() {
    showBuildings();

    cout << "\nAdjacency Matrix:\n     ";
    for (int j = 0; j < n; j++) {
        cout.width(4);
        cout << j + 1;
    }
    cout << "\n";
    for (int i = 0; i < n; i++) {
        cout.width(4);
        cout << i + 1;
        cout << " ";
        for (int j = 0; j < n; j++) {
            cout.width(4);
            cout << matrix[i][j];
        }
        cout << "\n";
    }

    cout << "\nAdjacency List:\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << names[i] << ":";
        for (Node* t = adjList[i]; t != NULL; t = t->next)
            cout << " [" << names[t->building] << "]";
        cout << "\n";
    }

    showLevels();
}

void addWalkway() {
    showBuildings();
    int a = readInt("First building number: ", 1, n) - 1;
    int b = readInt("Second building number: ", 1, n) - 1;

    if (a == b)
        cout << "A walkway needs two different buildings.\n";
    else if (matrix[a][b] == 1)
        cout << "This walkway already exists.\n";
    else {
        matrix[a][b] = matrix[b][a] = 1;
        buildList();
        cout << "Walkway added: " << names[a] << " <-> " << names[b] << "\n";
    }
}

void closeWalkway() {
    showBuildings();
    int a = readInt("First building number: ", 1, n) - 1;
    int b = readInt("Second building number: ", 1, n) - 1;

    if (matrix[a][b] == 0) {
        cout << "There is no walkway between these buildings.\n";
        return;
    }

    bool before[MAX] = {false};
    dfs(0, before);

    matrix[a][b] = matrix[b][a] = 0;
    buildList();
    cout << "Walkway closed: " << names[a] << " <-> " << names[b] << "\n";

    bool after[MAX] = {false};
    dfs(0, after);

    bool problem = false;
    for (int i = 0; i < n; i++) {
        if (before[i] && !after[i]) {
            cout << "WARNING: " << names[i] << " is now unreachable from " << names[0] << "!\n";
            problem = true;
        }
    }
    if (!problem)
        cout << "No building has become unreachable.\n";
}

void findRoute() {
    showBuildings();
    int src = readInt("Start building number: ", 1, n) - 1;
    int dest = readInt("Destination building number: ", 1, n) - 1;

    int parent[MAX], level[MAX];
    bfs(src, parent, level);

    if (level[dest] == -1) {
        cout << "No route from " << names[src] << " to " << names[dest] << ".\n";
        return;
    }

    int path[MAX];
    int length = 0;
    for (int v = dest; v != -1; v = parent[v])
        path[length++] = v;

    cout << "Route: ";
    for (int i = length - 1; i >= 0; i--) {
        cout << names[path[i]];
        if (i > 0)
            cout << " -> ";
    }
    cout << "\nWalkways used: " << level[dest] << "\n";
}

int main() {
    n = readInt("Enter number of buildings (2-20): ", 2, MAX);

    cout << "Enter building names (the first one is the main gate):\n";
    for (int i = 0; i < n; i++) {
        cout << "  Building " << i + 1 << ": ";
        readName(names[i]);
    }

    showBuildings();
    int m = readInt("\nEnter number of walkways: ", 0, n * (n - 1) / 2);
    for (int i = 0; i < m && cin; i++) {
        cout << "Walkway " << i + 1 << " (two building numbers): ";
        int a = readInt("", 1, n) - 1;
        int b = readInt("", 1, n) - 1;
        if (a == b || matrix[a][b] == 1) {
            cout << "Same building or repeated walkway, enter it again.\n";
            i--;
        } else {
            matrix[a][b] = matrix[b][a] = 1;
        }
    }
    buildList();
    showLevels();

    int choice;
    do {
        cout << "\n===== CAMPUS MENU =====\n";
        cout << "1. Add a walkway\n";
        cout << "2. Close a walkway\n";
        cout << "3. Show the map\n";
        cout << "4. Find a route\n";
        cout << "5. Exit\n";
        choice = readInt("Choice: ", 1, 5);
        if (!cin)
            break;

        if (choice == 1)
            addWalkway();
        else if (choice == 2)
            closeWalkway();
        else if (choice == 3)
            showMap();
        else if (choice == 4)
            findRoute();
    } while (choice != 5);

    freeList();
    cout << "Goodbye!\n";
    return 0;
}
