#include <iostream>
using namespace std;

const int MAX = 50;
const int LEN = 50;

struct Node {
    int step;
    Node* next;
};

struct Step {
    char name[LEN];
    int hours;
    int indegree;
    int start;
    int finish;
    int decidedBy;
    Node* next;
};

Step steps[MAX];
int n;

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

void addDependency(int a, int b) {
    Node* newNode = new Node;
    newNode->step = b;
    newNode->next = steps[a].next;
    steps[a].next = newNode;
    steps[b].indegree++;
}

void readInput() {
    n = readInt("Enter number of steps (1-50): ", 1, MAX);

    for (int i = 0; i < n; i++) {
        cout << "\nName of step " << i + 1 << ": ";
        readName(steps[i].name);
        steps[i].hours = readInt("Hours needed: ", 1, 10000);
        steps[i].indegree = 0;
        steps[i].start = 0;
        steps[i].finish = 0;
        steps[i].decidedBy = -1;
        steps[i].next = NULL;
    }

    cout << "\nSteps:\n";
    for (int i = 0; i < n; i++)
        cout << "  " << i + 1 << ". " << steps[i].name << " (" << steps[i].hours << " hours)\n";

    int m = readInt("\nEnter number of dependencies (0-500): ", 0, 500);
    cout << "Enter each as: a b (step a must finish before step b)\n";
    for (int i = 0; i < m; i++) {
        cout << "  Dependency " << i + 1 << ": ";
        int a = readInt("", 1, n) - 1;
        int b = readInt("", 1, n) - 1;
        addDependency(a, b);
    }
}

bool topologicalSort(int order[]) {
    int q[MAX];
    int front = 0, rear = 0;
    int indegree[MAX];
    int count = 0;

    for (int i = 0; i < n; i++) {
        indegree[i] = steps[i].indegree;
        if (indegree[i] == 0)
            q[rear++] = i;
    }

    while (front < rear) {
        int u = q[front++];
        order[count++] = u;

        for (Node* t = steps[u].next; t != NULL; t = t->next) {
            indegree[t->step]--;
            if (indegree[t->step] == 0)
                q[rear++] = t->step;
        }
    }

    return count == n;
}

void calculateTimes(int order[]) {
    for (int i = 0; i < n; i++) {
        int u = order[i];
        steps[u].finish = steps[u].start + steps[u].hours;

        for (Node* t = steps[u].next; t != NULL; t = t->next) {
            int v = t->step;
            if (steps[u].finish > steps[v].start) {
                steps[v].start = steps[u].finish;
                steps[v].decidedBy = u;
            }
        }
    }
}

void printTable(int order[]) {
    cout << "\n";
    cout.setf(ios::left, ios::adjustfield);
    cout.width(25);  cout << "Step";
    cout.width(8);   cout << "Hours";
    cout.width(16);  cout << "Earliest Start";
    cout.width(16);  cout << "Earliest Finish";
    cout << "\n-----------------------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        int u = order[i];
        cout.width(25);  cout << steps[u].name;
        cout.width(8);   cout << steps[u].hours;
        cout.width(16);  cout << steps[u].start;
        cout.width(16);  cout << steps[u].finish;
        cout << "\n";
    }
}

void printCriticalPath(int lastStep) {
    int path[MAX];
    int length = 0;

    for (int v = lastStep; v != -1; v = steps[v].decidedBy)
        path[length++] = v;

    cout << "Critical path: ";
    for (int i = length - 1; i >= 0; i--) {
        cout << steps[path[i]].name;
        if (i > 0)
            cout << " -> ";
    }
    cout << "\n";
}

void freeLists() {
    for (int i = 0; i < n; i++) {
        while (steps[i].next != NULL) {
            Node* temp = steps[i].next;
            steps[i].next = temp->next;
            delete temp;
        }
    }
}

int main() {
    readInput();

    int order[MAX];
    if (!topologicalSort(order)) {
        cout << "\nInvalid production plan\n";
        freeLists();
        return 0;
    }

    cout << "\nValid order of steps:\n";
    for (int i = 0; i < n; i++)
        cout << "  " << i + 1 << ". " << steps[order[i]].name << "\n";

    calculateTimes(order);
    printTable(order);

    int last = 0;
    for (int i = 1; i < n; i++) {
        if (steps[i].finish > steps[last].finish)
            last = i;
    }

    cout << "\nMinimum hours needed to build one car: " << steps[last].finish << "\n";
    printCriticalPath(last);

    freeLists();
    return 0;
}
