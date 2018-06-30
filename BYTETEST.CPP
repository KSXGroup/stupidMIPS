#include <iostream>
using namespace std;
typedef unsigned char BYTE;
void mprint(const int32_t &a){cout << a << "\n";}
int main(){
	int32_t c = 10;
	int a = -1;
	BYTE b=  a;
	a = b;
	mprint(c);
}
