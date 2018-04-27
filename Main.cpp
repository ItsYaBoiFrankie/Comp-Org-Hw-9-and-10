#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdlib>
#include <cstdio>

#include "Memory.h"

using namespace std;

//file name variable
string fileName;

//regex globals
match_results<std::string::const_iterator> result; 
string line;
int address = 0;
regex fullLine("(\\d+)\\s?(.*)");

//Memory String Array
Memory memory;

//halt mem loc for print funct
int endHalt = 0;

//Registers
int MAR = 0;
string MBR;
string IR = "nop";
int PC = 0;
string MQ = "0";
string AC = "0";

//method declarations
int decode(int, string);
void execution(int, Memory, string);

void fileRead(string fileName, Memory *memory)
{
	string instruction;
	int lineNumber = 0;

	ifstream fileReadFrom(fileName);
	if (fileReadFrom.is_open())
	{
		//() signal three criteria for match, \\ signal literal operator, * to tell computer to copy everything after
		//(.*) = read all characters before the period, (\\.) = include . char
		//including these here too for checks while I set memory and get rid of line numbers
		regex comment("(.*)(\\.)(.*)");
		regex loneLetter("\\s?([a-z]{1})");
		regex crapSyntax("(-)?(\\d+).*");
		int i = 0;
		bool halt = false;
		while (halt == false)
		{
			if (fileReadFrom.eof())
			{
				break;
			}
			getline(fileReadFrom, line);
			regex_match(line, result, fullLine);
			instruction = result[2];
			lineNumber = stoi(result[1]);
				if (instruction == "begin")
				{
					PC = stoi(result[1]);
					lineNumber = PC;
					instruction = (result[2]);
					
				}
				else if (instruction == "halt")
				{
					lineNumber = stoi(result[1]);
					instruction = (result[2]);
					endHalt = lineNumber;
					halt = true;
					
				}
				else if (regex_match(instruction, result, comment))
				{
					//Do nothing
				}
				else if (regex_match(instruction, result, loneLetter))
				{
					//Set lone letter variables = to 0
					instruction = (result[1]);
					instruction = "0 " + instruction;
					
				}
				else if (regex_match(instruction, result, crapSyntax))
				{
					//for stuff like -2b or 104askdj, keep the digits and sign, discard everything else
					string x = result[1];
					string y = result[2];
					instruction = x + y;
				}
				cout << i << endl;
			memory->set(lineNumber, instruction);
		}
	}
	else
	{
		cout << "Could Not Find File. Exiting..." << endl;
		system("exit");
	}
	fileReadFrom.close();
}

void fileWrite(string fileName, Memory *memory)
{
	ofstream fileWrite;
	fileWrite.open(fileName);
	if (fileWrite.is_open())
	{
		for (int i = 0; i <= 1000; i++)
		{
			fileWrite << i << " " << memory->get(i) << " " << endl;
		}
		fileWrite.close();
	}
	else
	{
		cout << "Unable to create file.";
		system("exit");
	}
}

int decode(int lineNumber, string instruction)
{
	//regex matches
	regex loadMQ("load\\s?MQ");								//1		loadMQ
	regex loadMQMX("load\\s?MQ,M\\((\\d+)\\)");				//2		loadMQ(MX)
	regex storMX("stor\\s?M\\((\\d+)\\)");					//3		storMX
	regex loadMX("load\\s?M\\((\\d+)\\)");					//4		loadMX
	regex loadNegMX("load\\s?-M\\((\\d+)\\)");				//5		load(-MX)
	regex loadAbsMX("load\\s?\\|M\\((\\d+)\\)\\|");			//6		load|MX|
	regex loadNegAbsMX("load\\s?-\\|M\\((\\d+)\\)\\|");		//7		load -|MX|
	regex jump("jump\\s?M\\((\\d+)\\)");					//8		jump
	regex jumpPlus("jump\\+\\s?M\\((\\d+)\\)");				//9		jump+
	regex addMX("add\\s?M\\((\\d+)\\)");					//10	addMX
	regex addAbsMX("add\\s?\\|M\\((\\d+)\\)\\|");			//11	add|MX|
	regex subMX("sub\\s?M\\((\\d+)\\)");					//12	subMX
	regex subAbsMX("sub\\s?\\|M\\((\\d+)\\)\\|");			//13	sub|MX|
	regex mulMX("mul\\s?M\\((\\d+)\\)");					//14	mulMX
	regex divMX("div\\s?M\\((\\d+)\\)");					//15	divMX
	regex comment("(.*)(\\.)(.*)");							//0 along with Begin/Halt
	regex loneLetter("\\s?([a-z]{1})");						//lone vars

	if (regex_match(instruction, result, loadMQMX))
	{
		execution(1, memory, result[1]);
	}
	else if (regex_match(instruction, result, storMX))
	{
		execution(2, memory, result[1]);
	}
	else if (regex_match(instruction, result, loadMX))
	{
		execution(3, memory, result[1]);
	}
	else if (regex_match(instruction, result, loadNegMX))
	{
		execution(4, memory, result[1]);
	}
	else if (regex_match(instruction, result, loadAbsMX))
	{
		execution(5, memory, result[1]);
	}
	else if (regex_match(instruction, result, loadNegAbsMX))
	{
		execution(6, memory, result[1]);
	}
	else if (regex_match(instruction, result, loadMQ))
	{
		execution(7, memory, result[1]);
	}
	else if (regex_match(instruction, result, jump))
	{
		//do nothing
	}
	else if (regex_match(instruction, result, jumpPlus))
	{
		//do nothing
	}
	else if (regex_match(instruction, result, addMX))
	{
		execution(10, memory, result[1]);
	}
	else if (regex_match(instruction, result, addAbsMX))
	{
		execution(11, memory, result[1]);
	}
	else if (regex_match(instruction, result, subMX))
	{
		execution(12, memory, result[1]);
	}
	else if (regex_match(instruction, result, subAbsMX))
	{
		execution(13, memory, result[1]);
	}
	else if (regex_match(instruction, result, mulMX))
	{
		execution(14, memory, result[1]);
	}
	else if (regex_match(instruction, result, divMX))
	{
		execution(15, memory, result[1]);
	}
	else if (instruction == "begin")
	{
		execution(0, memory, "begin");
	}
	else if (instruction == "halt")
	{
		return 0;
	}
	else if (instruction == "nop")
	{
		execution(0, memory, "nop");
	}
	else if (regex_match(instruction, result, comment))
	{
		execution(0, memory, "comment");
	}
	else if (regex_match(instruction, result, loneLetter))
	{
		execution(0, memory, "a");
	}
	else
	{
		cout << "Not the correct syntax, ending program." << endl;
		return 0;
	}
	return 1;
}

void execution(int caseNum, Memory memory, string address)
{
	switch (caseNum)
	{
	case(0):
	{
		//default local for keywords like begin/halt/(.)
		break;
	}
	case(1):
	{
		int tmp = stoi(address);
		MQ = memory.get(tmp);
		break;
	}

	case(2):
	{
		int tmp = stoi(address);
		memory.set(tmp, AC);
		break;
	}

	case(3):
	{
		int tmp = stoi(address);
		AC = memory.get(tmp);
		break;
	}

	case(4):
	{
		int tmp = stoi(memory.get(stoi(address)));
		tmp = -tmp;
		AC = to_string(tmp);
		break;
	}

	case(5):
	{
		int value = stoi(memory.get(stoi(address)));
		value = abs(value);
		AC = to_string(value);
		break;
	}

	case(6):
	{
		int value = stoi(memory.get(stoi(address)));
		value = abs(value);
		value = -value;
		AC = to_string(value);
		break;
	}

	case(7):
	{
		AC = MQ;
		break;
	}

	case(8):
	{
		//void
	}

	case(9):
	{
		//void
	}

	case(10):
	{
		int tmp = stoi(AC);
		int val = stoi(memory.get(stoi(address)));
		tmp += val;
		AC = to_string(tmp);
		break;
	}

	case(11):
	{
		int tmp = stoi(AC);
		int value = stoi(memory.get(stoi(address)));
		value = abs(value);
		tmp += value;
		AC = to_string(tmp);
		break;
	}

	case(12):
	{
		int tmp = stoi(AC);
		int value = stoi(memory.get(stoi(address)));
		tmp -= value;
		AC = to_string(tmp);
		break;
	}

	case(13):
	{
		int tmp = stoi(AC);
		int val = stoi(memory.get(stoi(address)));
		val = abs(val);
		tmp -= val;
		AC = to_string(tmp);
		break;
	}

	case(14):
	{
		int tmp = stoi(MQ);
		int val = stoi(memory.get(stoi(address)));
		tmp *= val;
		AC = to_string(tmp);
		break;
	}

	case(15):
	{
		int tmp = stoi(AC);
		int temp = tmp;
		int val = stoi(memory.get(stoi(address)));
		tmp /= temp;
		MQ = to_string(tmp);
		temp %= val;
		AC = to_string(temp);
		break;
	}
	default:
	{
		break;
	}
	}
	PC++;
}

void printMemory(Memory *memory)
{
	for (int i = 0; i <= endHalt; i++)
	{
		cout << i << " " << memory->get(i) << endl;
	} 
}

void printPreExecution(int PC, string IR)
{
	cout << "PC: " << PC << " IR = " << IR << endl;
}

void printPostExecution(int PC, string AC, string MQ)
{
	cout << "PC: " << PC << " AC: " << AC << " MQ: " << MQ << endl;
	cout << "----------------------------------------------------" << endl;
}

int main()
{
	memory.initialize();
	cout << "What is the file name to read from?" << endl;
	cin >> fileName;

	fileRead(fileName, &memory);
	printMemory(&memory);

	cout << endl;

			for (int i = 0; i < endHalt; i++)
			{
					int stop;
					MAR = PC;
					MBR = memory.get(PC);
					IR = MBR;
					printPreExecution(PC, IR);
					stop = decode(i, IR);
					if (stop == 0)
					{
						printPostExecution(PC, AC, MQ);
						printMemory(&memory);
						system("Pause");
						return 0;
					}
					printPostExecution(PC, AC, MQ);
			}

	cout << "Would you like to save your output file? ('Y' if so, or press any other key to exit.)" << endl;
	cin >> fileName;
	if (fileName == "Y")
	{
		cout << "Enter a file name with the .txt extension" << endl;
		cin >> fileName;
		fileWrite(fileName, &memory);
	}
	return 0;
}


