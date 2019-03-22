#include <iostream>
#include "GACipher.h"

int main()
{
    GACipher test(.1,.1,60000,500);
    test.run("../Test_Files/Cipher/test_1.txt");
}
