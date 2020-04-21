#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#ifndef ALPHABETS_HPP
#define ALPHABETS_HPP

struct Alphabet{
	char letter;
	int state;
};

extern Alphabet alphabets[26];
extern string words[30];
extern string str;


void initAlphabets();

void initWords();

#endif
