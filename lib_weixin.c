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
        printf("---name=%s\n", strName);  
        printf("---sex=%s\n", trSex);  
        printf("---age=%d\n", iAge);  
    }
	cJSON_Delete(pRoot);  
    return 0;  
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
  
    szJson_Result = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot);
	printf("%s\n",szJson_Result);  
	return szJson_Result;
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
		xmlFreeDoc(XmlDoc);
	}
	else if (szXml_Result != NULL)
	{
		free(szXml_Result);
		szXml_Result = NULL;
		szXml_Result_len = 0;
		xmlFreeDoc(XmlDoc);
	}
}
//发送http报文函数
//接收http报文函数

int main(void)
{
	printf("hello world!\n");
	//CreatJsonData(NULL);
	//ParseJsonData(szJson_Result);
	//FreeJsonData(NULL);
	//xmlChar * szXml = NULL;
	//int szXml_len = 0;
	//xmlDocDumpFormatMemoryEnc(ParseXmlData(CreatXmlData(NULL)), &szXml, &szXml_len, "UTF-8", 1);
	//printf("len:%d\n%s\n", szXml_len, szXml);
	//FreeXmlData(NULL);
	return 0;
}
