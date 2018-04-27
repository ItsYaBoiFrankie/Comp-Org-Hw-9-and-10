#ifndef MEMORY_H
#define MEMORY_H

#include <string>

using namespace std;

class Memory
{

private:
	string data[1000];
public:

	void Memory::initialize()
	{
		//incr by 10 to allow parrallel processing/more optimized use of machine resources
		for (int i = 0; i < (sizeof(data) / sizeof(*data)); i = i + 3)
		{
			data[i] = "nop";
			data[i + 1] = "nop";
			data[i + 2] = "nop";
			data[i + 3] = "nop";
		}
	}
	//takes in memory location and changes value in the memory at that location to new string
	void Memory::set(int i, string lineData)
	{
		if (i > 1000)
		{
			cout << "Exceeded limit." << endl;
		}
		else
		{
			data[i] = lineData;
		}
	}

	//Takes memory location and fishes out string at that location in memory array
	string Memory::get(int i)
	{
		return data[i];
	}
}; 
#endif MEMORY_H
