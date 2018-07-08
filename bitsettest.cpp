#include <iostream>
#include <bitset>
using namespace std;
int main(){
	bitset<4> a("0010");
	bitset<4> b("0010");
	cout << (a == b) << "\n";
}
