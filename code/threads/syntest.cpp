#include "system.h"

int main(int argc, char const *argv[])
{
	Thread *procuder[5], *consumer[3];
	procuder[0] = new Thread("procuder 0", 1);
	procuder[1] = new Thread("procuder 1", 1);
	procuder[2] = new Thread("procuder 2", 1);
	procuder[3] = new Thread("procuder 3", 1);
	procuder[4] = new Thread("procuder 4", 1);
	consumer[0] = new Thread("consumer 0", 1);
	consumer[1] = new Thread("consumer 1", 1);
	consumer[2] = new Thread("consumer 2", 1);
	return 0;
}