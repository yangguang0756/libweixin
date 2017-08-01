#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <libgen.h>  
#include <libxml/xmlmemory.h>  
#include <libxml/parser.h>  
#include <libxml/xpath.h>
#include "cJSON.h"
#include <curl/curl.h>
#include "lib_weixin.h"

char * wx_query_str[WX_QUERY_END] = {0};
char * wx_query_string = NULL;

MACRO_STR_T g_query_str[] ={  
	MACRO_STR(_grant_type),
	MACRO_STR(_appid),
	MACRO_STR(_secret),
	MACRO_STR(_access_token),
	MACRO_STR(_openid),
	MACRO_STR(_lang),
	MACRO_STR(_openId),
	MACRO_STR(_weChatAppId),
	MACRO_STR(_weChatAppSecret),
	{-1, NULL}  
};

char * wx_json_result_str[WX_JSON_RESULT_END] = {0};

MACRO_STR_T g_json_result_str[] ={  
	MACRO_STR(r_access_token),
	MACRO_STR(r_expires_in),
	MACRO_STR(r_subscribe),
	MACRO_STR(r_nickname),
	MACRO_STR(r_errcode),
	MACRO_STR(r_errmsg),
	MACRO_STR(r_respCode),
	{-1, NULL}  
};

char * wx_xml_result_str[WX_XML_RESULT_END] = {0};//如果需要attr，将char改为结构体包含attr

MACRO_STR_T g_xml_result_str[] ={  
	MACRO_STR(_node1),
	MACRO_STR(_node3),
	MACRO_STR(_node4),
	{-1, NULL}  
};



char * lib_weixin_get_macro_name(MACRO_STR_T* table, int id)  
{  
   int i = 0;  
 
   while(table[i].id != -1 && table[i].name != NULL){  
	   if(table[i].id == id)  
		   return &table[i].name[1];  
 
	   i++;  
   }  
   return "";  
}  

int get_macro_id(MACRO_STR_T* table, char * name)  
{  
   int i = 0;  
 
   while(table[i].id != -1 && table[i].name != NULL){  
	   if(!strcmp(&table[i].name[1], name))
		   return table[i].id;  
 
	   i++;  
   }  
   return i;  
}  

int get_macro_id_r(MACRO_STR_T* table, char * name)  
{  
   int i = 0;  
 
   while(table[i].id != -1 && table[i].name != NULL){  
	   if(!strcmp(&table[i].name[2], name))
		   return table[i].id;  
 
	   i++;  
   }  
   return i;  
}  

inline char * get_query_name(WX_QUERY wx_query)  
{  
    return lib_weixin_get_macro_name(g_query_str, wx_query);  
}  

inline int get_json_result_id(char* wx_json_result)
{
	return get_macro_id_r(g_json_result_str, wx_json_result);  
}

inline int get_xml_result_id(char* wx_xml_result)
{
	return get_macro_id(g_xml_result_str, wx_xml_result);  
}

//return value need free
inline char * MakeQueryString(void)
{
	int i = 0;
	int query_len = 0;
	for (i = 0; i < WX_QUERY_END; i++)
	{
		if (wx_query_str[i] != NULL)
		{
			query_len += (strlen(get_query_name(i)) + strlen(wx_query_str[i]) + 2);
		}
	}
	if (wx_query_string != NULL) {free(wx_query_string);wx_query_string = NULL;}
	wx_query_string = calloc(1, query_len * sizeof(char));
	query_len = 0;
	for (i = 0; i < WX_QUERY_END; i++)
	{
		if (wx_query_str[i] != NULL)
		{
			char * queryname = get_query_name(i);
			int queryname_len = strlen(queryname);
			int queryvalue_len = strlen(wx_query_str[i]);
			memcpy(wx_query_string + query_len, queryname, queryname_len);query_len += queryname_len;
			memset(wx_query_string + query_len, '=', 1);query_len += 1;
			memcpy(wx_query_string + query_len, wx_query_str[i], queryvalue_len);query_len += queryvalue_len;
			memset(wx_query_string + query_len, '&', 1);query_len += 1;
		}
	}
	wx_query_string[query_len - 1] = '\0';
	return wx_query_string;
}

inline void SetQueryValue(int queryname, char * queryvalue)
{
	CURL *curl = curl_easy_init();
	if (wx_query_str[queryname] != NULL)
	{
		curl_free(wx_query_str[queryname]);
		wx_query_str[queryname] = NULL;
	}
	wx_query_str[queryname] = curl_easy_escape(curl, queryvalue, 0);
	curl_easy_cleanup(curl);
}

inline void FreeQueryValue(void)
{
	int i = 0;
	for(i = 0; i < WX_QUERY_END; i++)
	{
		if (wx_query_str[i] != NULL)
		{
			curl_free(wx_query_str[i]);
			wx_query_str[i] = NULL;
		}
	}
	if (wx_query_string != NULL) {free(wx_query_string);wx_query_string = NULL;}
}


//解析json字符串函数
int ParseJsonData(char * json_string)
{
	cJSON* pRoot = cJSON_Parse(json_string);  
    cJSON* pArray = cJSON_GetObjectItem(pRoot, "students_info");  
    if (NULL == pArray) {  
        return -1;  
    }  
  
    int iCount = cJSON_GetArraySize(pArray);  
    int i = 0;  
    for (; i < iCount; ++i) {  
        cJSON* pItem = cJSON_GetArrayItem(pArray, i);  
        if (NULL == pItem){  
            continue;  
        }  
        char *strName = cJSON_GetObjectItem(pItem, "name")->valuestring;  
        char *trSex = cJSON_GetObjectItem(pItem, "sex")->valuestring;  
        int iAge = cJSON_GetObjectItem(pItem, "age")->valueint;  
        fprintf(stderr, "---name=%s\n", strName);  
        fprintf(stderr, "---sex=%s\n", trSex);  
        fprintf(stderr, "---age=%d\n", iAge);  
    }
	cJSON_Delete(pRoot);  
    return 0;  
}

inline char* GetJsonType(int id)
{
	switch(id)
	{
		case cJSON_Number:return "cJSON_Number";
		case cJSON_String:return "cJSON_String";
		case cJSON_Array:return "cJSON_Array";
		case cJSON_Object:return "cJSON_Object";
		default:return "UnkownJsonType";
	}
}

inline void SetJsonResult(char* wx_json_result, char * value)  
{  
	char * json_result = strdup(value);
	int json_result_id = get_json_result_id(wx_json_result);
	if (wx_json_result_str[json_result_id] != NULL)
	{
		free(wx_json_result_str[json_result_id]);
		wx_json_result_str[json_result_id] = NULL;
	}
    wx_json_result_str[json_result_id] = json_result;
}  

inline char * GetJsonResult(int json_result_id)
{
	return wx_json_result_str[json_result_id];
}


inline char * GetJsonResultByName(char* wx_json_result)
{
	int json_result_id = get_json_result_id(wx_json_result);
	return wx_json_result_str[json_result_id];
}

inline void ParseChildJson(cJSON* pcJSON)
{
	if (pcJSON != NULL)
	{
		//fprintf(stderr, "value:%s\n", GetJsonType(pcJSON->type));
		switch(pcJSON->type)
		{
			case cJSON_Number:
				//fprintf(stderr, "name:%s\n",pcJSON->string);
				//fprintf(stderr, "value:%d\n",pcJSON->valueint);
				{
				char intstring[16] = {0};
				sprintf(intstring, "%d", pcJSON->valueint);
				SetJsonResult(pcJSON->string, intstring);
				}
				break;
			case cJSON_String:
				//fprintf(stderr, "name:%s\n",pcJSON->string);
				//fprintf(stderr, "value:%s\n",pcJSON->valuestring);
				SetJsonResult(pcJSON->string, pcJSON->valuestring);
				break;
			case cJSON_Array:
				//fprintf(stderr, "arrayname:%s\n",pcJSON->string);
				ParseChildJson(pcJSON->child);
				break;
			case cJSON_Object:
				ParseChildJson(pcJSON->child);
				break;
			default:
				break;
		}
		ParseChildJson(pcJSON->next);
	}
}

void ParseResponseJson(char * json_string)
{
	cJSON* pRoot = cJSON_Parse(json_string);  
	ParseChildJson(pRoot);
	cJSON_Delete(pRoot);
}

inline void FreeJsonResult(void)
{
	int i = 0;
	for(i = 0; i < WX_JSON_RESULT_END; i++)
	{
		if (wx_json_result_str[i] != NULL)
		{
			free(wx_json_result_str[i]);
			wx_json_result_str[i] = NULL;
		}
	}
}

//生成json字符串函数
char* szJson_Result = NULL;

char * CreatJsonData(char ** json_string)
{
	cJSON* pRoot = cJSON_CreateObject();  
    cJSON* pArray = cJSON_CreateArray();  
    cJSON_AddItemToObject(pRoot, "students_info", pArray);  
  
    cJSON* pItem = cJSON_CreateObject();  
    cJSON_AddStringToObject(pItem, "name", "chenzhongjing");  
    cJSON_AddStringToObject(pItem, "sex", "male");  
    cJSON_AddNumberToObject(pItem, "age", 28);  
    cJSON_AddItemToArray(pArray, pItem);  
  
    pItem = cJSON_CreateObject();  
    cJSON_AddStringToObject(pItem, "name", "fengxuan");  
    cJSON_AddStringToObject(pItem, "sex", "male");  
    cJSON_AddNumberToObject(pItem, "age", 24);  
    cJSON_AddItemToArray(pArray, pItem);  
  
    pItem = cJSON_CreateObject();  
    cJSON_AddStringToObject(pItem, "name", "tuhui");  
    cJSON_AddStringToObject(pItem, "sex", "male");  
    cJSON_AddNumberToObject(pItem, "age", 22);  
    cJSON_AddItemToArray(pArray, pItem);  

	if (json_string != NULL)
	{
		if (*json_string != NULL)
		{
			free(*json_string);
			*json_string = NULL;
		}
		*json_string = cJSON_Print(pRoot);
		fprintf(stderr, "json_string:%s\n",*json_string);
		cJSON_Delete(pRoot);
		return *json_string;
	}
	else
	{
		if (szJson_Result != NULL)
		{
			free(szJson_Result);
			szJson_Result = NULL;
		}
		szJson_Result = cJSON_Print(pRoot);
		fprintf(stderr, "szJson_Result:%s\n",szJson_Result);
		cJSON_Delete(pRoot);
		return szJson_Result;
	}
	return "";
}
//释放json字段的内存空间
void FreeJsonData(char * json_string)
{
	if (json_string != NULL)
	{
		free(json_string);
		json_string = NULL;
	}
	else if (szJson_Result != NULL)
	{
		free(szJson_Result);
		szJson_Result = NULL;
	}
}

xmlDocPtr XmlDoc = NULL;
xmlChar * szXml_Result = NULL;
int szXml_Result_len = 0;
//解析xml字符串函数
xmlDocPtr ParseXmlData (xmlChar * xml_string)
{
	XmlDoc = xmlParseDoc(xml_string);  
    if(XmlDoc == NULL)  
    {  
        fprintf(stderr, "Document not parsed successfully.\n");  
        return NULL;  
    }  
    return XmlDoc;  
}

inline char* GetXmlType(int id)
{
	switch(id)
	{
		case XML_ELEMENT_NODE:return "XML_ELEMENT_NODE";
		case XML_ATTRIBUTE_NODE:return "XML_ATTRIBUTE_NODE";
		case XML_TEXT_NODE:return "XML_TEXT_NODE";
		case XML_CDATA_SECTION_NODE:return "XML_CDATA_SECTION_NODE";
		case XML_ENTITY_REF_NODE:return "XML_ENTITY_REF_NODE";
		case XML_ENTITY_NODE:return "XML_ENTITY_NODE";
		case XML_PI_NODE:return "XML_PI_NODE";
		case XML_COMMENT_NODE:return "XML_COMMENT_NODE";
		case XML_DOCUMENT_NODE:return "XML_DOCUMENT_NODE";
		case XML_DOCUMENT_TYPE_NODE:return "XML_DOCUMENT_TYPE_NODE";
		case XML_DOCUMENT_FRAG_NODE:return "XML_DOCUMENT_FRAG_NODE";
		case XML_NOTATION_NODE:return "XML_NOTATION_NODE";
		case XML_HTML_DOCUMENT_NODE:return "XML_HTML_DOCUMENT_NODE";
		case XML_DTD_NODE:return "XML_DTD_NODE";
		case XML_ELEMENT_DECL:return "XML_ELEMENT_DECL";
		case XML_ATTRIBUTE_DECL:return "XML_ATTRIBUTE_DECL";
		case XML_ENTITY_DECL:return "XML_ENTITY_DECL";
		case XML_NAMESPACE_DECL:return "XML_NAMESPACE_DECL";
		case XML_XINCLUDE_START:return "XML_XINCLUDE_START";
		case XML_XINCLUDE_END:return "XML_XINCLUDE_END";
		case XML_DOCB_DOCUMENT_NODE:return "XML_DOCB_DOCUMENT_NODE";
		default:return "UnkownXmlType";
	}
}

inline void SetXmlResult(char* wx_xml_result, char * value)  
{  
	char * xml_result = strdup(value);
	int xml_result_id = get_xml_result_id(wx_xml_result);
	if (wx_xml_result_str[xml_result_id] != NULL)
	{
		free(wx_xml_result_str[xml_result_id]);
		wx_xml_result_str[xml_result_id] = NULL;
	}
    wx_xml_result_str[xml_result_id] = xml_result;
}  

inline char * GetXmlResult(int xml_result_id)
{
	return wx_xml_result_str[xml_result_id];
}


inline char * GetXmlResultByName(char* wx_xml_result)
{
	int xml_result_id = get_xml_result_id(wx_xml_result);
	return wx_xml_result_str[xml_result_id];
}

inline void ParseChildXml(xmlNodePtr subXmlNode)
{
	if (subXmlNode != NULL)
	{
		//fprintf(stderr, "type:%s\n", GetXmlType(subXmlNode->type));
		switch(subXmlNode->type)
		{
			case XML_ELEMENT_NODE:
				//fprintf(stderr, "name:%s\n", subXmlNode->name);
				ParseChildXml((xmlNodePtr)subXmlNode->properties);
				ParseChildXml(subXmlNode->children);
				break;
			case XML_ATTRIBUTE_NODE:
				//fprintf(stderr, "name:%s\n", subXmlNode->name);
				ParseChildXml(subXmlNode->children);
				break;
			case XML_TEXT_NODE:
				switch(subXmlNode->parent->type)
				{
					case XML_ELEMENT_NODE:
						//fprintf(stderr, "name:%s\n", subXmlNode->parent->name);
						//fprintf(stderr, "value:%s\n", subXmlNode->content);
						SetXmlResult((char *)subXmlNode->parent->name, (char *)subXmlNode->content);
						break;
					case XML_ATTRIBUTE_NODE:
						//fprintf(stderr, "name:%s\n", subXmlNode->parent->parent->name);
						//fprintf(stderr, "attr:%s\n", subXmlNode->parent->name);
						//fprintf(stderr, "value:%s\n", subXmlNode->content);
						break;
					default:
						//fprintf(stderr, "name:%s\n", subXmlNode->name);
						//fprintf(stderr, "value:%s\n", subXmlNode->content);
						break;
				}
				break;
			case XML_CDATA_SECTION_NODE:break;
			case XML_ENTITY_REF_NODE:break;
			case XML_ENTITY_NODE:break;
			case XML_PI_NODE:break;
			case XML_COMMENT_NODE:break;
			case XML_DOCUMENT_NODE:
				ParseChildXml(subXmlNode->children);
				break;
			case XML_DOCUMENT_TYPE_NODE:break;
			case XML_DOCUMENT_FRAG_NODE:break;
			case XML_NOTATION_NODE:break;
			case XML_HTML_DOCUMENT_NODE:break;
			case XML_DTD_NODE:break;
			case XML_ELEMENT_DECL:break;
			case XML_ATTRIBUTE_DECL:break;
			case XML_ENTITY_DECL:break;
			case XML_NAMESPACE_DECL:break;
			case XML_XINCLUDE_START:break;
			case XML_XINCLUDE_END:break;
			case XML_DOCB_DOCUMENT_NODE:break;
			default:break;
		}
		ParseChildXml(subXmlNode->next);
	}
}

void ParseResponseXml(xmlChar * xml_string)
{
	xmlKeepBlanksDefault(0);
	xmlDocPtr XmlRoot = xmlParseDoc(xml_string);
	if(XmlRoot == NULL)
    {
        fprintf(stderr, "ParseResponseXml not parsed successfully.\n");
        return;
    }
	//fprintf(stderr, "type:%s\n", GetXmlType(XmlRoot->children->type));
	ParseChildXml((xmlNodePtr)XmlRoot);
	xmlFreeDoc(XmlRoot);
}

inline void FreeXmlResult(void)
{
	int i = 0;
	for(i = 0; i < WX_XML_RESULT_END; i++)
	{
		if (wx_xml_result_str[i] != NULL)
		{
			free(wx_xml_result_str[i]);
			wx_xml_result_str[i] = NULL;
		}
	}
}

//生成xml字符串函数
xmlChar * CreatXmlData(char ** xml_string)
{
	xmlDocPtr doc = NULL;		/* document pointer */
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;/* node pointers */
	// Creates a new document, a node and set it as a root node
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	//creates a new node, which is "attached" as child node of root_node node.
	xmlNewChild(root_node, NULL, BAD_CAST "node1",BAD_CAST "content of node1");
	// xmlNewProp() creates attributes, which is "attached" to an node.
	node=xmlNewChild(root_node, NULL, BAD_CAST "node3", BAD_CAST"node has attributes");
	xmlNewProp(node, BAD_CAST "attribute", BAD_CAST "yes");
	//Here goes another way to create nodes.
	node = xmlNewNode(NULL, BAD_CAST "node4");
	node1 = xmlNewText(BAD_CAST"other way to create content");
	xmlAddChild(node, node1);
	xmlAddChild(root_node, node);
	//Dumping document to stdio or file
	//xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
	xmlDocDumpFormatMemoryEnc(doc, &szXml_Result, &szXml_Result_len, "UTF-8", 1);
	/*free the document */
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();//debug memory for regression tests
	return szXml_Result;
}
//释放xml字段的内存空间
void FreeXmlData(char * xml_string)
{
	if (xml_string != NULL)
	{
		free(xml_string);
		xml_string = NULL;
		szXml_Result_len = 0;
	}
	else if (szXml_Result != NULL)
	{
		free(szXml_Result);
		szXml_Result = NULL;
		szXml_Result_len = 0;
		
	}
	if(XmlDoc != NULL)
	{
		xmlFreeDoc(XmlDoc);
		XmlDoc = NULL;
	}
}

//处理数据体的回掉函数
size_t write_data(void* buffer,size_t size,size_t nmemb,void *userdata)
{
	if (buffer != NULL && (strlen(buffer) != 0))
	{
		char ** stream = (char **)userdata;
		*stream = calloc(nmemb, size);
		memcpy(*(char **)stream, buffer, size * nmemb);
	}
	return size * nmemb;
}

char NormalResult[NORMAL_RESULT_LEN] = {0};
void SendHttp(char * Url, char * Query, int Method, struct curl_slist *extern_headers)
{
	CURL *curl;
	struct curl_slist *headers = NULL;
	if (extern_headers == NULL)
	{
    	headers = curl_slist_append(headers, CREATCOMM_HEADER);
	}
	else
	{
		headers = extern_headers;
	}
	curl = curl_easy_init();
	if(curl) 
	{
		CURLcode res;    
		char * Response_Data = NULL;
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//改协议头
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//对返回的数据进行操作的函数地址
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Response_Data);//这是write_data的第四个参数值默认为stdout
		curl_easy_setopt(curl, CURLOPT_POST, Method);//0:GET 1:POST
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);//信任任何https证书
		
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L ); //在屏幕打印请求连接过程和返回http数据
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10 );//接收数据时超时设置，如果10秒内数据未接收完，直接退出
		//curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1); // 以下3个为重定向设置
		//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //返回的头部中有Location(一般直接请求的url没找到)，则继续请求Location对应的数据 
		//curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 1);//查找次数，防止查找太深
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3 );//连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
		switch(Method)
		{
			case GET:
			{
				int Url_Len = strlen(Url);
				int Query_Len = strlen(Query);
				char * FullUrl = alloca(Url_Len + Query_Len + 2);
				memcpy(FullUrl, Url, Url_Len);
				memset(FullUrl + Url_Len, '?', 1);
				memcpy(FullUrl + Url_Len + 1, Query, Query_Len);
				memset(FullUrl + Url_Len + Query_Len + 1, '\0', 1);
				//fprintf(stderr, "FullUrl: %s \n", FullUrl);
				curl_easy_setopt(curl, CURLOPT_URL, FullUrl);
			}
				break;
			case POST:
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Query);// 指定post内容
				curl_easy_setopt(curl, CURLOPT_URL, Url);
				break;
			default:
				fprintf(stderr, "Unknow HTTP Method :%d\n",Method);
				break;
		}
		res = curl_easy_perform(curl);

		if(CURLE_OK == res) 
		{
			char *ct;
			/* ask for the content-type */
			/* http://curl.haxx.se/libcurl/c/curl_easy_getinfo.html */
			res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);

			if((CURLE_OK == res) && ct)
			{
				//fprintf(stderr, "We received Content-Type: %s \n", ct);
				//fprintf(stderr, "We received Response_Data: %s \n", Response_Data);
				if (strstr(ct,"application/json") != NULL)
				{
					ParseResponseJson(Response_Data);
				}
				else if (strstr(ct,"application/xml") != NULL)
				{
					ParseResponseXml((xmlChar *)Response_Data);
				}
				else if (strstr(ct,"text/plain") != NULL)
				{
					memset(NormalResult, 0, NORMAL_RESULT_LEN);
					strncpy(NormalResult, Response_Data, NORMAL_RESULT_LEN);
				}
			}
			//fprintf(stderr, "Response_Data:%s\n",Response_Data);
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
		if (Response_Data != NULL) {free(Response_Data);Response_Data = NULL;}
	}
	return;
}

char * getAccessToken(char * appid, char * appsecret)
{
	FreeQueryValue();
	FreeJsonResult();
	SetQueryValue(_grant_type, "client_credential");
	SetQueryValue(_appid, appid);
	SetQueryValue(_secret, appsecret);
	//fprintf(stderr, "%s:%d MakeQueryString:%s\n", __func__, __LINE__, MakeQueryString());
	SendHttp(WEIXIN_TOKEN_API, MakeQueryString(), GET, NULL);
	char * p_access_token = GetJsonResult(r_access_token);
	if (p_access_token == NULL)
	{
		fprintf(stderr, "%s:%d errcode:%s errmsg:%s\n", 
			__func__, __LINE__, GetJsonResult(r_errcode), GetJsonResult(r_errmsg));
	}
	return p_access_token;
}

int getUserAttentionState(char * access_token, char * openid)
{
	FreeQueryValue();
	FreeJsonResult();
	SetQueryValue(_access_token, access_token);
	SetQueryValue(_openid, openid);
	//fprintf(stderr, "%s:%d MakeQueryString:%s\n", __func__, __LINE__, MakeQueryString());
	SendHttp(WEIXIN_USERINFO_API, MakeQueryString(), GET, NULL);
	char * p_subscribe = GetJsonResult(r_subscribe);
	int subscribe = 0;
	if (p_subscribe != NULL && strlen(p_subscribe) != 0)
	{
		subscribe = atoi(p_subscribe);
	}
	else
	{
		fprintf(stderr, "%s:%d errcode:%s errmsg:%s\n", 
			__func__, __LINE__,  GetJsonResult(r_errcode), GetJsonResult(r_errmsg));
		if (GetJsonResult(r_errcode) != NULL)
		{
			subscribe = atoi(GetJsonResult(r_errcode));
		}
	}
	return subscribe;
}

int getUserAttentionStateByGoahead(char * openId, char * weChatAppId, char * weChatAppSecret)
{
	FreeQueryValue();
	SetQueryValue(_openId, openId);
	SetQueryValue(_weChatAppId, weChatAppId);
	SetQueryValue(_weChatAppSecret, weChatAppSecret);
	//fprintf(stderr, "%s:%d MakeQueryString:%s\n", __func__, __LINE__, MakeQueryString());
	SendHttp(WEIXIN_USER_ATTENTION_STATE_API, MakeQueryString(), GET, NULL);
	int attention_state = 0;
	if (NormalResult != NULL && strlen(NormalResult) != 0)
	{
		attention_state = atoi(NormalResult);
	}
	return attention_state;
}

inline struct curl_slist * makeUcpaasAPIHeader(struct curl_slist **headers, char * encode_Authorization)
{	
	*headers = curl_slist_append(*headers, "Host:api.ucpaas.com");
	*headers = curl_slist_append(*headers, "Accept:application/json");
	*headers = curl_slist_append(*headers, "Content-type:application/json;charset=utf-8");
	*headers = curl_slist_append(*headers, encode_Authorization);
	return *headers;
}

inline char * MakeUcpaasJsonQueryString
	(char * smsAppId, char * smsTemplateId, char * param, char * phone_num, char ** ucpaas_Json_String)
{
	cJSON* pRoot = cJSON_CreateObject();  
    cJSON* pItem = cJSON_CreateObject();  
	if (smsAppId == NULL || smsTemplateId == NULL || param == NULL || phone_num == NULL)
	{
		return "";
	}
    cJSON_AddStringToObject(pItem, "appId", smsAppId);  
    cJSON_AddStringToObject(pItem, "param", param);  
    cJSON_AddStringToObject(pItem, "templateId", smsTemplateId);  
    cJSON_AddStringToObject(pItem, "to", phone_num);  
    cJSON_AddItemToObject(pRoot, "templateSMS", pItem);  
  
    *ucpaas_Json_String = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot);
	fprintf(stderr, "%s\n",*ucpaas_Json_String);  
	
	return *ucpaas_Json_String;
}

int SendSms(char * smsAccountId, char * smsAppId, char * smsTemplateId, char * smsSign, char * sms_Encode_Authorization, char * verification_Code, char * phone_Num)
{
	struct curl_slist *headers = NULL;
	char * ucpaas_Json_String = NULL;
	char ucpaas_Send_Sms_Api[UCPAAS_SEND_SMS_API_LEN] = {0};
	sprintf(ucpaas_Send_Sms_Api, UCPAAS_SEND_SMS_API, smsAccountId, smsSign);
	SendHttp(ucpaas_Send_Sms_Api, MakeUcpaasJsonQueryString
		(smsAppId, smsTemplateId, verification_Code, phone_Num, &ucpaas_Json_String), POST
		, makeUcpaasAPIHeader(&headers, sms_Encode_Authorization));
	if (ucpaas_Json_String != NULL)
	{
		free(ucpaas_Json_String);
		ucpaas_Json_String = NULL;
	}
	char * p_respCode = GetJsonResult(r_respCode);
	int respCode = -1;
	if (p_respCode != NULL && strlen(p_respCode) != 0)
	{
		respCode = atoi(p_respCode);
	}
	else
	{
		fprintf(stderr, "%s:%d errcode:%s errmsg:%s\n", 
			__func__, __LINE__,  GetJsonResult(r_errcode), GetJsonResult(r_errmsg));
		if (GetJsonResult(r_errcode) != NULL)
		{
			respCode = atoi(GetJsonResult(r_errcode));
		}
	}
	return respCode;
}

int main(void)
{
	fprintf(stderr, "hello world!\n");
	//SendSms("ee53e0e7ec3449d77af1ae14f58e6843", "e8e608f36b4c4808adf034a14729fc6c", "96076", "2CCC0B07077FD646BBF3D7C3585937A9", "ZWU1M2UwZTdlYzM0NDlkNzdhZjFhZTE0ZjU4ZTY4NDM6MjAxNzA3MjExMTUwNTM=", "123456", "13951814052");
	//getUserAttentionStateByGoahead("1","2","4");
	//CreatJsonData(NULL);
	//ParseJsonData(szJson_Result);
	//ParseJsonData(CreatJsonData(NULL));
	//FreeJsonData(NULL);
	//char * json_string = NULL;
	//CreatJsonData(&json_string);
	//ParseJsonData(json_string);
	//ParseJsonData(CreatJsonData(&json_string));
	//FreeJsonData(json_string);
	#if 0
	xmlChar * szXml = NULL;
	int szXml_len = 0;
	xmlDocDumpFormatMemoryEnc(ParseXmlData(CreatXmlData(NULL)), &szXml, &szXml_len, "UTF-8", 1);
	fprintf(stderr, "len:%d\n%s\n", szXml_len, szXml);
	FreeXmlData(NULL);
	ParseResponseXml(CreatXmlData(NULL));
	fprintf(stderr, "node1:%s\n", GetXmlResultByName("node1"));
	fprintf(stderr, "node3:%s\n", GetXmlResultByName("node3"));
	fprintf(stderr, "node4:%s\n", GetXmlResult(_node4));
	FreeXmlResult();
	SetQueryValue(_grant_type, "client_credential");
	SetQueryValue(_appid, "wxfb84576193871a35");
	SetQueryValue(_secret, "c522890e544711b201a0d61e1b8bed4c");
	SendHttp(WEIXIN_TOKEN_API, MakeQueryString(), POST, NULL);
	FreeQueryValue();
	fprintf(stderr, "access_token:%s\n", GetJsonResultByName("access_token"));
	fprintf(stderr, "expires_in:%s\n", GetJsonResultByName("expires_in"));
	FreeJsonResult();
	#endif
	//char * access_token = getAccessToken("wxfb84576193871a35", "c522890e544711b201a0d61e1b8bed4c");
	//fprintf(stderr, "access_token:%s\n", access_token);
	//int attention_state = getUserAttentionState("aaa", "or8D3v1ahWB3jxvU7pYilLy7_b1E");
	//fprintf(stderr, "attention_state:%d\n", attention_state);
	return 0;
}
