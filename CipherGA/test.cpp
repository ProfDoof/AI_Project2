#include <iostream>
#include "GACipher.h"

int main()
{
    GACipher test(.25,.005,180000,100);
    test.run("../Test_Files/Cipher/test_1.txt");
}
