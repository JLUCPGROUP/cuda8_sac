#include <iostream>
#include <vector>

using namespace std;
int main() {
	vector<vector<int>> a(5, vector<int>(1));
	a.clear();
	cout << sizeof(a) << endl;
	return 0;
}