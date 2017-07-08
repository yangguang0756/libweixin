#include <stdio.h>
#include <stdlib.h>
#include "lib_weixin.h"

typedef enum argv_param 
{
	argv_procpath = 0,
	argv_openId = 1,
	argv_weChatAppId,
	argv_weChatAppSecret,
	ARGV_PARAM_END
}
ARGV_PARAM;

int main(int argc, char* argv[])
{
	int res = 0;
	if (argc < ARGV_PARAM_END)
	{
		printf("usage:\ngetUserAttentionState openId appId appSecret\n");
		exit(res);
	}
	
	res = getUserAttentionStateByGoahead(argv[argv_openId]
										, argv[argv_weChatAppId]
										, argv[argv_weChatAppSecret]
										);
	exit(res);
}
