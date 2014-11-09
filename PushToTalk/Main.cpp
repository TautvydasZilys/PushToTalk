#include "PrecompiledHeader.h"
#include "CoInitializeWrapper.h"
#include "PushToTalkController.h"

int main()
{
	CoInitializeWrapper coInit;
	PushToTalkController pushToTalk;
	pushToTalk.Run();

	return 0;
}