#include <iostream>
using namespace std;

const int MAX = 20;
const int LEN = 50;
const int RIDERS = 3;
const int INF = 1000000000;

int n;
char names[MAX][LEN];
int city[MAX][MAX];
int rush[MAX][MAX];

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

void showAreas() {
    cout << "\nAreas:\n";
    for (int i = 0; i < n; i++)
        cout << "  " << i + 1 << ". " << names[i] << "\n";
}

void dijkstra(int graph[MAX][MAX], int src, int dist[], int parent[]) {
    bool done[MAX] = {false};

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[src] = 0;

    for (int count = 0; count < n; count++) {
        int u = -1;
        for (int i = 0; i < n; i++) {
            if (!done[i] && dist[i] != INF && (u == -1 || dist[i] < dist[u]))
                u = i;
        }
        if (u == -1)
            break;
        done[u] = true;

        for (int v = 0; v < n; v++) {
            if (graph[u][v] > 0 && !done[v] && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
}

void printPath(int parent[], int v) {
    if (parent[v] != -1) {
        printPath(parent, parent[v]);
        cout << " -> ";
    }
    cout << names[v];
}

void printRest(int parent[], int v) {
    if (parent[v] == -1)
        return;
    printRest(parent, parent[v]);
    cout << " -> " << names[v];
}

int deliveryRoute(int graph[MAX][MAX], int restaurant, int customer) {
    int dist[MAX], parent[MAX];
    dijkstra(graph, restaurant, dist, parent);

    if (dist[customer] == INF) {
        cout << "Delivery not possible to this area\n";
        return INF;
    }

    cout << "Fastest route: ";
    printPath(parent, customer);
    cout << "\nTime: " << dist[customer] << " minutes\n";
    return dist[customer];
}

int fullTrip(int graph[MAX][MAX], int riders[], int restaurant, int customer) {
    int dist[MAX], parent[MAX];
    int best = -1;
    int bestTime = INF;

    cout << "\nTime for each rider to reach the restaurant:\n";
    for (int r = 0; r < RIDERS; r++) {
        dijkstra(graph, riders[r], dist, parent);
        cout << "  Rider " << r + 1 << " (" << names[riders[r]] << "): ";
        if (dist[restaurant] == INF)
            cout << "cannot reach\n";
        else {
            cout << dist[restaurant] << " minutes\n";
            if (dist[restaurant] < bestTime) {
                bestTime = dist[restaurant];
                best = r;
            }
        }
    }

    if (best == -1) {
        cout << "No rider can reach the restaurant.\n";
        return INF;
    }

    int riderParent[MAX];
    dijkstra(graph, riders[best], dist, riderParent);

    int foodDist[MAX], foodParent[MAX];
    dijkstra(graph, restaurant, foodDist, foodParent);

    int total = bestTime + foodDist[customer];

    cout << "Chosen: Rider " << best + 1 << "\n";
    cout << "Full trip: ";
    printPath(riderParent, restaurant);
    printRest(foodParent, customer);
    cout << "\n  Rider to restaurant: " << bestTime << " minutes\n";
    cout << "  Restaurant to customer: " << foodDist[customer] << " minutes\n";
    cout << "  Total time: " << total << " minutes\n";
    return total;
}

void readCity() {
    n = readInt("Enter number of areas (3-20): ", 3, MAX);

    cout << "Enter area names:\n";
    for (int i = 0; i < n; i++) {
        cout << "  Area " << i + 1 << ": ";
        readName(names[i]);
    }
    showAreas();

    int m = readInt("\nEnter number of roads: ", 0, n * (n - 1) / 2);
    cout << "Enter each road as: area area minutes\n";
    for (int i = 0; i < m && cin; i++) {
        cout << "  Road " << i + 1 << ": ";
        int a = readInt("", 1, n) - 1;
        int b = readInt("", 1, n) - 1;
        int t = readInt("", 1, 100000);
        if (a == b || city[a][b] != 0) {
            cout << "Same area or repeated road, enter it again.\n";
            i--;
        } else {
            city[a][b] = city[b][a] = t;
        }
    }
}

void rushHourChanges() {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            rush[i][j] = city[i][j];

    cout << "\n===== RUSH HOUR =====\n";
    while (true) {
        cout << "\n1. Double the time of a road\n";
        cout << "2. Block a road\n";
        cout << "0. Done, find the new route\n";
        int choice = readInt("Choice: ", 0, 2);
        if (choice == 0)
            break;

        showAreas();
        int a = readInt("First area of the road: ", 1, n) - 1;
        int b = readInt("Second area of the road: ", 1, n) - 1;

        if (city[a][b] == 0)
            cout << "There is no road between these areas.\n";
        else if (rush[a][b] == 0)
            cout << "This road is already blocked.\n";
        else if (choice == 1) {
            rush[a][b] = rush[b][a] = 2 * city[a][b];
            cout << "Road " << names[a] << " - " << names[b] << " now takes " << rush[a][b] << " minutes.\n";
        } else {
            rush[a][b] = rush[b][a] = 0;
            cout << "Road " << names[a] << " - " << names[b] << " is blocked.\n";
        }
    }
}

int main() {
    readCity();

    showAreas();
    int restaurant = readInt("\nRestaurant area number: ", 1, n) - 1;
    int customer = readInt("Customer area number: ", 1, n) - 1;

    cout << "\n===== DELIVERY ROUTE =====\n";
    int oldDelivery = deliveryRoute(city, restaurant, customer);
    if (oldDelivery == INF)
        return 0;

    int riders[RIDERS];
    cout << "\nEnter the areas of the 3 riders (must be different):\n";
    for (int r = 0; r < RIDERS && cin; r++) {
        cout << "Rider " << r + 1;
        riders[r] = readInt(" area number: ", 1, n) - 1;
        for (int k = 0; k < r; k++) {
            if (riders[k] == riders[r] && cin) {
                cout << "Another rider is already there.\n";
                r--;
                break;
            }
        }
    }
    if (!cin)
        return 0;

    int oldTotal = fullTrip(city, riders, restaurant, customer);

    rushHourChanges();

    cout << "\n===== NEW DELIVERY ROUTE (rush hour) =====\n";
    int newDelivery = deliveryRoute(rush, restaurant, customer);
    if (newDelivery == INF)
        return 0;
    cout << "Minutes added to delivery route: " << newDelivery - oldDelivery << "\n";

    int newTotal = fullTrip(rush, riders, restaurant, customer);
    if (newTotal != INF && oldTotal != INF)
        cout << "Minutes added to full trip: " << newTotal - oldTotal << "\n";

    return 0;
}
