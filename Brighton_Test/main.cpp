#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <iterator>

using namespace std;


void read_file(string filename, map<string, double>& freq_1, map<string, double>& freq_2, map<string, double>& freq_3) {
    string key;
    int i;
    double value;
    map<double, string> temp_1, temp_2, temp_3;
    
    ifstream in_file;
    in_file.open(filename.c_str());
    if (in_file.is_open()) {
        while (in_file >> key >> value) {
            if (key.length() == 1)
                temp_1[value] = key;

            else if (key.length() == 2)
                temp_2[value] = key;

            else
                temp_3[value] = key;
        }
        in_file.close();

        for (auto it = temp_1.rbegin(); it != temp_1.rend(); it++)
            freq_1[it->second] = it->first;

        i = 0;
        for (auto it = temp_2.rbegin(); i < 50; i++, it++)
            freq_2[it->second] = it->first;

        i = 0;
        for (auto it = temp_3.rbegin(); i < 50; i++, it++)
            freq_3[it->second] = it->first;
    }
}

int main() {
    map<string, double> freq_1, freq_2, freq_3;


    // READ IN FREQ DATA //
    read_file("../Frequency/Frequency.txt", freq_1, freq_2, freq_3);
    // for_each(freq_1.begin(), freq_1.end(), [](auto it) { cout << it.first << " "; });cout << endl;
    // for_each(freq_2.begin(), freq_2.end(), [](auto it) { cout << it.first << " "; });cout << endl;
    // for_each(freq_3.begin(), freq_3.end(), [](auto it) { cout << it.first << " "; });cout << endl;

    // READ INPUT FILE //


    // BEGIN //
}