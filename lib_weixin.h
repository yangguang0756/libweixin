#ifndef LIB_WEIXIN_H
#define LIB_WEIXIN_H

#define NORMAL_RESULT_LEN 256
#define CREATCOMM_HEADER "Accept: WeiXinFrame"
#define WEIXIN_TOKEN_API "https://api.weixin.qq.com/cgi-bin/token"
#define WEIXIN_USERINFO_API "https://api.weixin.qq.com/cgi-bin/user/info"
#define WEIXIN_USER_ATTENTION_STATE_API "https://127.0.0.1/cgi-bin/authresult"
#define ACCESS_TOKEN_INVALIED 40001
#define ACCESS_TOKEN_EXPIRED 42001

typedef enum http_send_method 
{
	GET = 0,
	POST,
	HTTP_METHOD_END
}
HTTP_SEND_METHOD;

#define MACRO_STR(x) {x, #x}

#ifndef _MACRO_STR_T
#define _MACRO_STR_T
typedef struct _macro_str {	
   int id;	
   char *name;	
}MACRO_STR_T;  
#endif

char * lib_weixin_get_macro_name(MACRO_STR_T* table, int id);
typedef enum wx_query 
{
	_grant_type = 0,
	_appid,
	_secret,
	_access_token,
	_openid,
	_lang,
	_openId,
	_weChatAppId,
	_weChatAppSecret,
	_wx_query_error,
	WX_QUERY_END
}
WX_QUERY;

typedef enum wx_json_result 
{
	r_access_token = 0,
	r_expires_in,
	r_subscribe,
	r_nickname,
	r_errcode,
	r_errmsg,
	_wx_json_error,
	WX_JSON_RESULT_END
}
WX_JSON_RESULT;

typedef enum wx_xml_result 
{
	_node1 = 0,
	_node3,
	_node4,
	_wx_xml_error,
	WX_XML_RESULT_END
}
WX_XML_RESULT;

char * getAccessToken(char * appid, char * appsecret);
int getUserAttentionState(char * access_token, char * openid);
int getUserAttentionStateByGoahead(char * openId, char * weChatAppId, char * weChatAppSecret);

#endif