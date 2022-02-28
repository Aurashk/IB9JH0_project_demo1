#include "Market.h"
#include "Trader.h"
#include "AssetHistory.h"
#include <iostream>

#define TEST_CLASS Trader

int main()
{
#ifdef TEST_CLASS
	TEST_CLASS::test();
#endif

}