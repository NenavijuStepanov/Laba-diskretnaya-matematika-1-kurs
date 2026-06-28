#include <iostream>
#include <array>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <stack>
#include <queue>
using namespace std;

//"C:\Users\user\Desktop\vhod\3.txt"

string path_save = "[net puti]";
string path_read = "[net puti]";
int begin_vertex = 1;
int end_vertex = 1;
int promejutoc_number, vertexCount, type;
//type=1 directed type=2 undirected
int method = 4;
int file_data_mat(), first_line(), vertex_test(), number_finder(string path), path_test(string path), marshrut(), file_data_spisok(), file_data_mat(), file_data_edges(), vector_stepeney();
int dfs(int v, const vector<vector<int>>& current_adj, vector<int>& component), fillOrder(int v, stack<int>& Stack), svyznosti_components(), floyd_warshall(), deikstera();
string path_finder(string path);
array<string, 2> obrabot(string vhod);

struct edge_data {
	int from;
	int to;
	int weight;
};

vector<edge_data> edges;

vector<vector<int>> adj;
vector<vector<int>> rev_adj;
vector<vector<int>> undir_adj;
vector<bool> visited;

const int INF = 1e9; 


// Вспомогательные функции для Крускала
int dsu_find(int v, vector<int>& parent) {    // Disjoint Set Union (Система непересекающихся множеств) Тут сразу ставит батьку у вершины
	if (v == parent[v]) return v;
	return parent[v] = dsu_find(parent[v], parent);
}

void dsu_union(int a, int b, vector<int>& parent) {
	a = dsu_find(a, parent);
	b = dsu_find(b, parent);
	if (a != b) parent[b] = a;
}

int kruskal_mst() {    // Kruskal's Minimum Spanning Tree
	ofstream outFile;
	if (path_save != "[net puti]") outFile.open(path_save, ios::app);
	vector<edge_data> mst_edges = edges;


	sort(mst_edges.begin(), mst_edges.end(), [](const edge_data& a, const edge_data& b) {
		return a.weight < b.weight;
		});

	vector<int> parent(vertexCount + 1);
	for (int i = 1; i <= vertexCount; i++) parent[i] = i; 

	vector<edge_data> result_tree;
	int total_weight = 0;

	for (const auto& e : mst_edges) {

		if (dsu_find(e.from, parent) != dsu_find(e.to, parent)) {
			result_tree.push_back(e);
			total_weight += e.weight;
			dsu_union(e.from, e.to, parent);
		}
	}

	if (result_tree.size() < vertexCount - 1) { // количество вершин - 1 будет мин дерево
		cout << "\nGraf ne svyazen, nevozmojno postroit polnoe ostovnoe derevo." << endl;
		if (outFile.is_open()) outFile << "\nGraf ne svyazen, nevozmojno postroit polnoe ostovnoe derevo." << endl;
	}

	cout << "\nMinimalnoe ostovnoe derevo (Kruskal):" << endl;
	if (outFile.is_open()) outFile << "\nMinimalnoe ostovnoe derevo (Kruskal):" << endl;
	for (const auto& e : result_tree) {
		cout << e.from << " - " << e.to << " (ves: " << e.weight << ")" << endl;
		if (outFile.is_open()) outFile << e.from << " - " << e.to << " (ves: " << e.weight << ")" << endl;
	}
	cout << "Obshiy ves ostovnogo dereva: " << total_weight << endl;
	if (outFile.is_open()) outFile << "Obsniy ves ostovnogo dereva: " << total_weight << endl;
	outFile.close();
	return 0;
}

int deikstera() {
	ofstream outFile;
	if (path_save != "[net puti]") outFile.open(path_save, ios::app);
	vector<vector<pair<int, int>>> adj_weight(vertexCount + 1); 	// adj_weight[from] = {to, weight}
	for (const auto& e : edges) {
		adj_weight[e.from].push_back({ e.to, e.weight });
		if (type == 2) adj_weight[e.to].push_back({ e.from, e.weight });
	}

	vector<int> dist(vertexCount + 1, INF);
	vector<int> parent(vertexCount + 1, -1); 

	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; 	// очередь с приоритетом: хранит пары {расстояние, вершина}, сортирует по возрастанию расстояния, поэтому оно 1-ое

	dist[begin_vertex] = 0;
	pq.push({ 0, begin_vertex });  

	while (!pq.empty()) {
		int current_dist = pq.top().first;  
		int current_vertex = pq.top().second;
		pq.pop();

		if (current_dist > dist[current_vertex]) continue; // пропускаем устаревшие записи

		for (auto& edge : adj_weight[current_vertex]) {
			int neighbor_vertex = edge.first;
			int weight = edge.second;

			if (dist[current_vertex] + weight < dist[neighbor_vertex]) {
				dist[neighbor_vertex] = dist[current_vertex] + weight;
				parent[neighbor_vertex] = current_vertex; 
				pq.push({ dist[neighbor_vertex], neighbor_vertex });
			}
		}
	}
	cout << "\nPoisk kratchayshego puti iz " << begin_vertex << " v " << end_vertex << " :" << endl;
	if (outFile.is_open()) outFile << "\nPoisk kratchayshego puti iz " << begin_vertex << " v " << end_vertex << " :" << endl;
	if (dist[end_vertex] == INF) {
		cout << "Put ne nayden" << endl;
		if (outFile.is_open()) outFile << "Put ne nayden" << endl;
	}
	else {
		cout << "Rasstoyanie: " << dist[end_vertex] << endl;
		if (outFile.is_open()) outFile << "Rasstoyanie: " << dist[end_vertex] << endl;

		vector<int> path;
		for (int v = end_vertex; v != -1; v = parent[v]) path.push_back(v);     // построение путя
		reverse(path.begin(), path.end());

		cout << "Marshrut: ";
		if (outFile.is_open()) outFile << "Marshrut: ";
		for (int v : path) {
			cout << v << " ";
			if (outFile.is_open()) outFile << v << " ";
		}
		cout << endl;
		if (outFile.is_open()) outFile << endl;
	}
	outFile.close();
	return 0;
}

int floyd_warshall() {   // матрица расстояний, радиус, диаметр, цент. и периф. вершины
	ofstream outFile;
	if (path_save != "[net puti]") outFile.open(path_save, ios::app);
	vector<vector<int>> dist(vertexCount + 1, vector<int>(vertexCount + 1, INF));
	for (int i = 1; i <= vertexCount; i++) dist[i][i] = 0;
	for (const auto& e : edges) {
		dist[e.from][e.to] = min(dist[e.from][e.to], e.weight); 
		if (type == 2) {
			dist[e.to][e.from] = min(dist[e.to][e.from], e.weight);
		}
	}
	for (int k = 1; k <= vertexCount; k++) {
		for (int i = 1; i <= vertexCount; i++) {
			for (int j = 1; j <= vertexCount; j++) {
				if (dist[i][k] < INF && dist[k][j] < INF) {
					dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
				}
			}
		}
	}
	cout << "\nMatrica kratchayshih rasstoyaniy:" << endl;
	if (outFile.is_open()) outFile << "\nMatrica kratchayshih rasstoyaniy:" << endl;
	for (int i = 1; i <= vertexCount; i++) {
		for (int j = 1; j <= vertexCount; j++) {
			if (dist[i][j] == INF) {
				cout << "INF\t";
				if (outFile.is_open()) outFile << "INF\t";
			}
			else {
				cout << dist[i][j] << "\t";
				if (outFile.is_open()) outFile << dist[i][j] << "\t";
			}
		}
		cout << endl;
		if (outFile.is_open()) outFile << endl;
	}

	vector<int> ecc(vertexCount + 1, 0);
	int radius = INF;
	int diameter = 0;

	for (int i = 1; i <= vertexCount; i++) {
		for (int j = 1; j <= vertexCount; j++) {
			if (dist[i][j] != INF) {
				ecc[i] = max(ecc[i], dist[i][j]);
			}
		}
		if (ecc[i] < radius && ecc[i] > 0) radius = ecc[i]; 
		if (ecc[i] > diameter) diameter = ecc[i];           
	}

	cout << "\nRadius grafa: " << (radius == INF ? -1 : radius) << endl;
	if (outFile.is_open()) outFile << "\nRadius grafa: " << (radius == INF ? -1 : radius) << endl;
	cout << "Diametr grafa: " << diameter << endl;
	if (outFile.is_open()) outFile << "Diametr grafa: " << diameter << endl;

	cout << "Centralnie vershini: ";
	if (outFile.is_open()) outFile << "Centralnie vershini: ";
	for (int i = 1; i <= vertexCount; i++) {
		if (ecc[i] == radius) {
			cout << i << " ";
			if (outFile.is_open()) outFile << i << " ";
		}
	}

	cout << "\nPeriferiynie vershini: ";
	if (outFile.is_open()) outFile << "\nPeriferiynie vershini: ";
	for (int i = 1; i <= vertexCount; i++) {
		if (ecc[i] == diameter) {
			cout << i << " ";
			if (outFile.is_open()) outFile << i << " ";
		}
	}
	cout << endl;
	if (outFile.is_open()) outFile << endl;
	outFile.close();
	return 0;
}

int dfs(int v, const vector<vector<int>>& current_adj, vector<int>& component) { // поиск в глубину
	visited[v] = true;
	component.push_back(v);
	for (int neighbor : current_adj[v]) {
		if (!visited[neighbor]) {
			dfs(neighbor, current_adj, component);
		}
	}
	return 0;
}

int fillOrder(int v, stack<int>& Stack) { // DFS для заполнения порядка выхода (алгоритм Косарайю)
	visited[v] = true;
	for (int neighbor : rev_adj[v]) {
		if (!visited[neighbor]) fillOrder(neighbor, Stack);
	}
	Stack.push(v);
	return 0;
}

int svyznosti_components() { // нахождение компонентов связности 
	ofstream outFile;
	if (path_save != "[net puti]") outFile.open(path_save, ios::app);
	adj.assign(vertexCount + 1, vector<int>());
	rev_adj.assign(vertexCount + 1, vector<int>());
	undir_adj.assign(vertexCount + 1, vector<int>());

	for (const auto& e : edges) {

		adj[e.from].push_back(e.to);
		rev_adj[e.to].push_back(e.from);

		undir_adj[e.from].push_back(e.to);
		undir_adj[e.to].push_back(e.from);
	}

	if (type == 2) {
		visited.assign(vertexCount + 1, false);
		cout << "\nKomponenti svyaznosti:" << endl;
		if (outFile.is_open()) outFile << "\nKomponenti svyaznosti:" << endl;
		int count1 = 0;
		for (int i = 1; i <= vertexCount; i++) {
			if (!visited[i]) {
				vector<int> component;
				dfs(i, undir_adj, component);
				cout << "Komponenta " << ++count1 << ": ";
				if (outFile.is_open()) outFile << "Komponenta " << count1 << ": ";
				for (int v : component) {
					cout << v << " ";
					if (outFile.is_open()) outFile << v << " ";
				}
				cout << endl;
				if (outFile.is_open()) outFile << endl;
			}
		}
	}
	else if (type == 1) {
		// А. Слабая связность (как неориентированный)
		visited.assign(vertexCount + 1, false);
		cout << "\nSlabo svyaznie komponenti:" << endl;
		if (outFile.is_open()) outFile << "\nSlabo svyaznie komponenti:" << endl;
		int slab_count = 0;
		for (int i = 1; i <= vertexCount; i++) {
			if (!visited[i]) {
				vector<int> component;
				dfs(i, undir_adj, component);
				cout << "Slabay-Komponenta " << ++slab_count << ": ";
				if (outFile.is_open()) outFile << "Slabay-Komponenta " << slab_count << ": ";
				for (int v : component) {
					cout << v << " ";
					if (outFile.is_open()) outFile << v << " ";
				}
				cout << endl;
				if (outFile.is_open()) outFile << endl;
			}
		}

		// Б. Сильная связность (Алгоритм Косарайю)
		stack<int> Stack;
		visited.assign(vertexCount + 1, false);
		for (int i = 1; i <= vertexCount; i++) {
			if (!visited[i]) fillOrder(i, Stack);
		}

		visited.assign(vertexCount + 1, false);
		cout << "\nSilno svyaznie komponenti:" << endl;
		if (outFile.is_open()) outFile << "\nSilno svyaznie komponenti:" << endl;
		int sil_count = 0;
		while (!Stack.empty()) {
			int v = Stack.top();
			Stack.pop();
			if (!visited[v]) {
				vector<int> component;
				dfs(v, adj, component);
				cout << "Silnaya-Komponenta " << ++sil_count << ": ";
				if (outFile.is_open()) outFile << "Silnaya-Komponenta " << sil_count << ": ";
				for (int node : component) {
					cout << node << " ";
					if (outFile.is_open()) outFile << node << " ";
				}
				cout << endl;
				if (outFile.is_open()) outFile << endl;
			}
		}
	}
	outFile.close();
	return 0;
}

int vector_stepeney() { // нахождение вектора степеней вершин  
	ofstream outFile;
	if (path_save != "[net puti]") outFile.open(path_save, ios::app);
	vector<int> out_degrees(vertexCount + 1, 0);
	vector<int> in_degrees(vertexCount + 1, 0);
	if (path_save != "[net puti]") ofstream outFile(path_save, ios::app);

	for (int i = 0; i < edges.size(); i++) {
		int u = edges[i].from;
		int v = edges[i].to;

		if (u >= 1 && u <= vertexCount) {
			out_degrees[u]++;
		}
		if (v >= 1 && v <= vertexCount) {
			in_degrees[v]++;
		}
	}
	
	cout << "\nStepeni vershin:" << endl;
	if (outFile.is_open()) outFile << "\nStepeni vershin:" << endl;
	for (int i = 1; i <= vertexCount; i++) {
		if (type == 1) {
			
			cout << "Vershina " << i << ": "
				<< "Ishodyashaya = " << out_degrees[i]
				<< ", Vhodyashaya = " << in_degrees[i] << endl;
			if (outFile.is_open()) {
				outFile << "Vershina " << i << ": "
					<< "Ishodyashaya = " << out_degrees[i]
					<< ", Vhodyashaya = " << in_degrees[i] << endl;
			}
		}
		else {
			cout << "Vershina " << i << ": Stepen = " << (out_degrees[i]) << endl;
			if (outFile.is_open()) outFile << "Vershina " << i << ": Stepen = " << (out_degrees[i]) << endl;
		}
	}
	outFile.close();
	return 0;
}



int file_data_mat() {      // считывание файла без первой строки для матрицы смежности
	edges.clear();
	ifstream file(path_read);

	if (file.is_open()) {
		string line;
		getline(file, line);
		int currentRow = 1;

		while (getline(file, line)) {
			if (line.empty()) continue;

			stringstream ss(line);
			int weight_loc;
			int currentCol = 1;

			while (ss >> weight_loc) {

				if (weight_loc > 0) {

					edges.push_back({ currentRow, currentCol, weight_loc });
				}
				currentCol++;
			}
			currentRow++;
		}

		file.close();

		cout << "Spisok reber:" << endl;
		for (int i = 0; i < edges.size(); ++i) {
			const edge_data& e = edges[i];
			/*int from_info = edges[i].from;
			int to_info = edges[i].to;				надо расти!!!!!!
			int weight_info = edges[i].weight;*/
			cout << "Vershina " << e.from << " -> " << e.to
				<< " (ves: " << e.weight << ")" << endl;
		}
	}
	else {
		cout << "\nOSHIBKA: otkritiya!" << endl;
	}
	return 0;
}

int file_data_edges() {      // считывание файла без первой строки для списка ребер
	edges.clear();
	ifstream file(path_read);

	if (file.is_open()) {
		string line;
		getline(file, line);

		while (getline(file, line)) {
			if (line.empty()) continue;
			stringstream ss(line);
			int currentRow, currentCol, weight_loc;
			if (ss >> currentRow >> currentCol >> weight_loc) {
				edges.push_back({ currentRow, currentCol, weight_loc });
			}
			else {
				cout << "Oshibka formata dannih v stroke!" << endl;
			}
		}

		file.close();

		cout << "Spisok reber:" << endl;
		for (int i = 0; i < edges.size(); ++i) {
			const edge_data& e = edges[i];
			/*int from_info = edges[i].from;
			int to_info = edges[i].to;				надо расти!!!!!!
			int weight_info = edges[i].weight;*/
			cout << "Vershina " << e.from << " -> " << e.to
				<< " (ves: " << e.weight << ")" << endl;
		}
	}
	else {
		cout << "\nOSHIBKA: otkritiya!" << endl;
	}
	return 0;
}

int file_data_spisok() { // считывание файла без первой строки для списка смежности
	edges.clear();
	ifstream file(path_read);
	if (file.is_open()) {
		string line;
		getline(file, line);
		int currentSource = 1;
		while (getline(file, line)) {
			stringstream ss(line);
			string part;

			while (ss >> part) {
				size_t colonPos = part.find(':');
				if (colonPos != string::npos) {

					int target = stoi(part.substr(0, colonPos));
					int weight = stoi(part.substr(colonPos + 1));

					edges.push_back({ currentSource, target, weight });
				}
			}
			currentSource++;
		}

		file.close();

		cout << "Spisok reber:" << endl;
		for (int i = 0; i < edges.size(); ++i) {
			const edge_data& e = edges[i];
			/*int from_info = edges[i].from;
			int to_info = edges[i].to;				надо расти!!!!!!
			int weight_info = edges[i].weight;*/
			cout << "Vershina " << e.from << " -> " << e.to
				<< " (ves: " << e.weight << ")" << endl;
		}
	}
	else {
		cout << "\nOSHIBKA: otkritiya!" << endl;
	}
	return 0;
}

int marshrut() {  //определение сценария обработки
	switch (method) {
	case 1:
		if (vertex_test() == 1) {
			file_data_edges();
			vector_stepeney();
			svyznosti_components();
			floyd_warshall();
			deikstera();
			kruskal_mst();

		}
		break;
	case 2:
		if (vertex_test() == 1) {
			file_data_mat();
			vector_stepeney();
			svyznosti_components();
			floyd_warshall();
			deikstera();
			kruskal_mst();

		}
		break;
	case 3:
		if (vertex_test() == 1) {
			file_data_spisok();
			vector_stepeney();
			svyznosti_components();
			floyd_warshall();
			deikstera();
			kruskal_mst();

		}
		break;
	default:
		cout << "\nOSHIBKA: ne vibrano chem zadan graf/orgraf" << endl;
		break;
	}
	return 0;
}


int first_line() {   // считывание первой строки
	ifstream file(path_read);
	string firstLine;

	if (file.is_open()) {
		if (getline(file, firstLine)) {

			string digits = "";
			char type_loc = ' ';

			for (char c : firstLine) {
				if (isdigit(c)) {
					digits += c;
				}
				else {
					type_loc = c;
					break;
				}
			}
			if (type_loc == 'd') type = 1;
			else if (type_loc == 'u') type = 2;
			else {
				cout << "\nOSHIBKA: ne ponytniy tip!" << endl;
				return 0;
			}
			vertexCount = stoi(digits);
			cout << "\nKolichestvo vershin: " << vertexCount << endl;
		}
		file.close();
		return 1;
	}
	else {
		cout << "\nOSHIBKA: ne udalos otkrit file!" << endl;
	}

	return 0;
}

int vertex_test() {     // проверка не превышает ли первая или последняя вершина количество вершин
	if (first_line() == 0);
	else if (begin_vertex > vertexCount || begin_vertex < 1) cout << "\nOSHIBKA: nepravilniy nomer hachalnoy vershini" << endl;
	else if (end_vertex > vertexCount || end_vertex < 1) cout << "\nOSHIBKA: nepravilniy nomer konechnoy vershini" << endl;
	else if (vertexCount <= 0) cout << "\nOSHIBKA: otricatelnoe kolichestvo vershin" << endl;
	else {
		return 1;
	}
	return 0;
}

int number_finder(string path) {   //поиск какого-либо числа, если оно должно быть приложено к ключу
	string path_1 = path;
	string nomera = "0123456789";
	size_t start = path_1.find_first_of(nomera);
	if (start == string::npos) return 0;
	stringstream test(path.substr(start));
	int number_res;

	if (test >> number_res) {
		promejutoc_number = number_res;
		return 1;
	}
	else {
		return 0;
	}

}

int path_test(string path) {     //проверка путя на открытие
	ifstream inputFile;
	inputFile.open(path);
	if (!inputFile.is_open()) {
		return 0;
	}
	return 1;
}

string path_finder(string path) {   //поиск путя, если он должен быть приложен к ключу
	size_t start = path.find('"');
	size_t end = path.find('"', start + 1);
	if ((start != string::npos) && (end != string::npos)) return path.substr(start + 1, end - start - 1);
	return "0";

}

array<string, 2> obrabot(string vhod) {   //определение ключа
	array<string, 2> info;
	if (vhod.length() < 2) {
		info[0] = "9";
		info[1] = "";
		return info;
	}
	info[0] = vhod.substr(0, 2);
	info[1] = vhod.substr(2);

	info[0] = (info[0] == "-e") ? "1" :
		(info[0] == "-m") ? "2" :
		(info[0] == "-l") ? "3" :
		(info[0] == "-o") ? "4" :
		(info[0] == "-h") ? "5" :
		(info[0] == "-n") ? "6" :
		(info[0] == "-d") ? "7" :
		(info[0] == "-s") ? "8" : "9";

	return info;
}


int main()
{
	cout << "Привет" << endl;
	int choice, number_finder_res;
	string vhod, path, path_finder_res;
	while (true) {
		cout << "\n-----------------------------------------" << endl;
		cout << "(Vvedi -h dly pomoshi)" << endl;
		cout << "Jdu klych: ";
		getline(cin, vhod);

		array<string, 2> obrabot_res = obrabot(vhod);

		choice = stoi(obrabot_res[0]);
		path = obrabot_res[1];

		switch (choice) {
		case 1:
			path_finder_res = path_finder(path);
			if ((path_test(path_finder_res) == 1) && (path_finder_res != path_save)) {
				path_read = path_finder_res;
				method = choice;
				cout << "\nVash document " << path_read << endl;
			}
			else cout << "OSHIBKA: ne udalos nayti document/viberite drugoi document!" << endl;
			break;
		case 2:
			path_finder_res = path_finder(path);
			if ((path_test(path_finder_res) == 1) && (path_finder_res != path_save)) {
				path_read = path_finder_res;
				method = choice;
				cout << "\nVash document " << path_read << endl;
			}
			else cout << "OSHIBKA: ne udalos nayti document/viberite drugoi document!" << endl;
			break;
		case 3:
			path_finder_res = path_finder(path);
			if ((path_test(path_finder_res) == 1) && (path_finder_res != path_save)) {
				path_read = path_finder_res;
				method = choice;
				cout << "\nVash document " << path_read << endl;
			}
			else cout << "OSHIBKA: ne udalos nayti document/viberite drugoi document!" << endl;
			break;
		case 4:
			path_finder_res = path_finder(path);
			if ((path_test(path_finder_res) == 1) && (path_finder_res != path_read)) {
				path_save = path_finder_res;
				cout << "\nRezultati budut sohraneni v " << path_save << endl;
			}
			else cout << "OSHIBKA: ne udalos nayti document/viberite drugoi document!" << endl;
			break;
		case 5:
			cout << "\nVashi dannie:" << endl;
			cout << "Document - " << path_read << endl;
			cout << "method - " << method << endl;
			cout << "Rezultati sohranyutsy - " << path_save << endl;
			cout << "Hachalnya vershina - " << begin_vertex << endl;
			cout << "Konechnaya vershina - " << end_vertex << endl;
			cout << "\nSpisok kluchey:" << endl;
			cout << "\n-e \"edges_list_file_path\" (method 1)" << endl;
			cout << "Algoritm dly obrabotki grafa/orgrafa, zadannogo spiskom reber" << endl;
			cout << "\n-m \"adjacency_matrix_file_path\" (method 2)" << endl;
			cout << "Algoritm dly obrabotki grafa/orgrafa, zadannogo matricey smeznosti" << endl;
			cout << "\n-l \"adjacency_list_file_path\" (method 3)" << endl;
			cout << "Algoritm dly obrabotki grafa/orgrafa, zadannogo spiskom smeznosti" << endl;
			cout << "\nNE VIBRAN METHOD - (method 4)" << endl;
			cout << "\n-o \"output_file_path\"" << endl;
			cout << "Komanda dly zapisi rezultatov raboti v kakoy-nibud document" << endl;
			cout << "\n-n begin_vertex_number" << endl;
			cout << "Komanda dly vibora nachalnoy vershini poiska puti(po-umochaniyu 1-aya)" << endl;
			cout << "\n-d end_vertex_number" << endl;
			cout << "Komanda dly vibora konechnoy vershini poiska puti(po-umochaniyu 1-aya)" << endl;
			cout << "\n-s" << endl;
			cout << "Start programmi" << endl;
			break;
		case 6:
			number_finder_res = number_finder(path);
			if ((number_finder_res == 1) && (promejutoc_number != 0)) {
				begin_vertex = promejutoc_number;
				cout << "\nNachalnya vershina: " << begin_vertex << endl;
			}
			else cout << "\nOSHIBKA: nepravilniy nomer vershini" << endl;
			break;
		case 7:
			number_finder_res = number_finder(path);
			if ((number_finder_res == 1) && (promejutoc_number != 0)) {
				end_vertex = promejutoc_number;
				cout << "\nKonechnhaya vershina: " << end_vertex << endl;
			}
			else cout << "\nOSHIBKA: nepravilniy nomer vershini" << endl;
			break;
		case 8:
			marshrut();
			break;
		default:
			cout << "\nOSHIBKA: nepravilniy klych" << endl;
			break;
		}
	}
	return 0;
}


