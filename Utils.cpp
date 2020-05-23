#include "Utils.h"

#include <cctype>
#include <iostream>
#include <limits>

void cinIgnore()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readBoundedInt(int min, int max)
{
    for (;;) {
        int input;
        std::cin >> input;
        if (std::cin.fail()) {
            std::cout << "Error: This is not an integer number.\n";
            std::cin.clear();
        } else if (std::cin.peek() != '\n') {
            std::cout << "Error: Unexpected characters after the number.\n";
        } else if (input < min || input > max) {
            std::cout << "Error: The number should be betweeen "
                      << min << " and " << max << ".\n";
        } else { // no errors
            std::cin.get(); // remove line break from buffer
            return input;
        }
        // ignore rest of the input if there is any
        cinIgnore();
    }
}

char readBoundedChar(int min, int max, bool ignoreCase)
{
    for (;;) {
        char input;
        std::cin >> input;
        if (std::cin.peek() != '\n') {
            std::cout << "Error: More than 1 character was given.\n";
        } else if ((input < min || input > max)
            && (!ignoreCase || tolower(input) < tolower(min)
                || tolower(input) > tolower(max))) {
            std::cout << "Error: The character should be betweeen "
                      << char(min) << " and " << char(max) << ".\n";
        } else { // no errors
            std::cin.get(); // remove line break from buffer
            return input;
        }
        // ignore rest of the input if there is any
        cinIgnore();
    }
}

bool containsChar(const char* arr, int len, char c)
{
    for (int i = 0; i < len; ++i) {
        if (arr[i] == c)
            return true;
    }
    return false;
}
