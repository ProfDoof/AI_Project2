#include <iostream>
//#include "GACipher.h"
#include "go_4.h"

using namespace std;

int main()
{
    GACipher test(.1,.1,720000,100);
    test.run("../Test_Files/test_4.txt");

    cout << "DONE" << endl;
}
