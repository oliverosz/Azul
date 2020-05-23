#pragma once

void cinIgnore();

int readBoundedInt(int min, int max);

char readBoundedChar(int min, int max, bool ignoreCase = true);

bool containsChar(const char* arr, int len, char c);
