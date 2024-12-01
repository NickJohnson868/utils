
#include <iostream>
#include <string>

#include "test.h"
#include "rename.h"
#include "util/_file.h"
#include "util/_string.h"
#include "util/util.h"
#include "pixel_on_cmd.h"
#include "util/_math.h"
#include "util/_time.h"

#include <algorithm>

using namespace std;

int main(int argc, char* argv[])
{
	PixelOnCmd::run(argc, argv);
	//Rename::run();

	//Test::test_excel();

	return 0;
}