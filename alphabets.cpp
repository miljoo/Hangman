#include "alphabets.hpp"


using namespace std;

Alphabet alphabets[26];

string words[30];
string str;

void initAlphabets(){
	int chara = 97; //pieni a.

	for(int i = 0 ; i < 26 ; i++){
		alphabets[i].letter = (char)chara;
		alphabets[i].state = 0;

		chara++;
	}
}

void initWords(){

	string line;
	int i = 0;
  	ifstream myfile ("senten.ces");
  	if (myfile.is_open())
  	{
    	while ( getline (myfile,line) )
    	{
	    	if(i < 30){
				words[i] = line;
	      		i++;
			}
	    }
    myfile.close();
  	}
  else cout << "Unable to open file";

}
