/**
 ****************************************************************************
 * <P> McbXML.c - implementation file for basic XML parser written in ANSI C 
 * for portability.
 * It works by using recursion and a node tree for breaking down the elements
 * of an XML document.  </P>
 *
 * @version     V1.0
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	21st August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */

/*
 ****************************************************************************
 * Include all necessary include files
 ****************************************************************************
 */
#include "McbXML.h"
#include "stdlib.h"


//#include <malloc.h>
//#include <memory.h>
#include <assert.h>
/*
 ****************************************************************************
 * Enumeration used to decipher what type a token is
 ****************************************************************************
 */
typedef enum McbTokenType {
    eTokenText = 0,
    eTokenQuotedText,       //引用，如果是 ” ‘字符，就找超找对用的字符
    eTokenTagStart,         /* "<"		2	*/
    eTokenTagEnd,           /* "</"		3	*/
    eTokenCloseTag,         /* ">"		4	*/
    eTokenEquals,           /* "="		5	*/
    eTokenDeclaration,      /* "<?"		6	*/
    eTokenShortHandClose,   /* "/>"		7	*/
    eTokenExplain,          /* <--   --> */
    eTokenClear,
    eTokenError       /*如果没有找到 “ ‘和文件的结束符*/
} McbTokenType;

/*
 ****************************************************************************
 * Defines to dictate grow by value when adding nodes.
 ****************************************************************************
 */
#define McbGROWBY			5

#define McbINDENTCHAR	_T('\t')

typedef struct McbClearTag {
    LPTSTR lpszOpen;
    LPTSTR lpszClose;

} McbClearTag;

typedef struct McbNextToken {
    McbClearTag *pClr;
    LPCTSTR     pStr;
} McbNextToken;

/*
 ****************************************************************************
 * Main structure used for parsing XML
 ****************************************************************************
 */
typedef struct McbXML {
    LPCTSTR             lpXML;   //XML文件
    char                Uncode;//文件编码0--ASC;1---UTF-8
    int                 nIndex;   //索引
    enum McbXMLError        error; //错误
    LPCTSTR             lpEndTag;
    int                 cbEndTag;
    LPCTSTR             lpNewElement;
    int                 cbNewElement;
    int                 nFirst;
    McbClearTag         *pClrTags;

} McbXML;

/*
 ****************************************************************************
 * Enumeration used when parsing attributes
 ****************************************************************************
 */
typedef enum McbAttrib {
    eAttribName = 0,
    eAttribEquals,
    eAttribValue
} McbAttrib;

/*
 ****************************************************************************
 * Enumeration used when parsing elements to dictate whether we are currently
 * inside a tag
 ****************************************************************************
 */
typedef enum McbStatus {
    eInsideTag = 0,
    eOutsideTag
} McbStatus;

/*
 ****************************************************************************
 * Structure used to obtain List.
 ****************************************************************************
 */
typedef struct List_ST {
    struct List_ST *pNext;
    struct List_ST *pPrev;
}List_st;

typedef struct {
    List_st pList;
    int iNum;
}ListHead_st;

/*
 ****************************************************************************
 * Structure used to obtain List Node.
 ****************************************************************************
 */

#ifdef DYNAMIC_MEM
typedef struct {
    List_st List;
    LPTSTR lpszText;
}NodeText_st;
#else
typedef struct {
    List_st List;
    char lpszText[256];
}NodeText_st;
#endif

static ListHead_st g_StrList;

void InitList(ListHead_st *pListHead)
{
    pListHead->pList.pNext = &(pListHead->pList);
    pListHead->pList.pPrev = &(pListHead->pList);
    pListHead->iNum = 0;
}/* InitList */

void AddToList(ListHead_st *pListHead,List_st *pListNode)
{
    pListNode->pPrev = &(pListHead->pList);
    pListNode->pNext = pListHead->pList.pNext;

    pListHead->pList.pNext->pPrev = pListNode;
    pListHead->pList.pNext = pListNode;
    pListHead->iNum++;
}/* AddToList */

List_st *DeleteFromListTial(ListHead_st *pListHead)
{
    List_st *pTemp;
    //Empty List?
    if (pListHead->pList.pPrev == &(pListHead->pList))
        return 0;
    pTemp = pListHead->pList.pPrev;
    pTemp->pPrev->pNext = pTemp->pNext;
    pTemp->pNext->pPrev = pTemp->pPrev;
    pListHead->iNum--;
    return pTemp;
}/* DeleteFromListTial */

void DeleteFromList(List_st *pList)
{
    pList->pNext->pPrev = pList->pPrev;
    pList->pPrev->pPrev = pList->pNext;
}/*  DeleteFromList */

void FreeList(ListHead_st *pListHead)
{
    List_st *pList;
    NodeText_st *pNodeText;
    //empty List
    if (pListHead->pList.pNext == &(pListHead->pList))
        return;
    while (pListHead->iNum) {
        pList = DeleteFromListTial(pListHead);
        pNodeText = ListEntry(pList,NodeText_st,List);
#ifdef DYNAMIC_MEM
        free(pNodeText->lpszText);
#endif
        free(pNodeText);
    }

}/*FreeList*/

/**
 ****************************************************************************
 * <P> Initialise an element.  </P>
 *
 * @methodName  McbInitElement
 *
 * @param       *pEntry		
 * @param       lpszName		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	14th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
/***************************************************************
* 
*  说明：
***************************************************************/
void McbInitElement(McbXMLElement *pEntry, McbXMLElement *pParent, 
                    LPTSTR lpszName, int nIsDeclaration)
{
    //assert(pEntry);

    pEntry->nMax = 0;
    pEntry->nSize = 0;
    pEntry->pEntries = NULL;
    pEntry->pParent = pParent;//添加到上一次的父节点上去
    pEntry->nIsDeclaration = nIsDeclaration;
    pEntry->lpszName = lpszName;

}/* McbInitElement */

/**
 ****************************************************************************
 * <P> Create the root element.  </P>
 *
 * @methodName  * McbCreateRoot
 *
 * @param       none
 *
 * @return      McbXMLElement
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	1st February  	2002	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
/***************************************************************
*创建一个父节点
***************************************************************/
McbXMLElement * McbCreateRoot()
{
    McbXMLElement * pElement;

    pElement = (McbXMLElement*)malloc(sizeof(McbXMLElement));
    McbInitElement(pElement, NULL, 0, FALSE);
    //Init the NodeName LinkList
    //Toby add 2006/7/3/9:31
    //InitList(&g_StrList);
    return pElement;

}/* McbCreateRoot */

/**
 ****************************************************************************
 * <P> Delete the root element and set it to NULL.  </P>
 *
 * @methodName  McbDeleteRoot
 *
 * @param       ** ppElement		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	1st February  	2002	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbDeleteRoot(McbXMLElement * pElement)
{
    McbDeleteElement(pElement);
    free(pElement);
    //FreeList(&g_StrList);

}/* McbDeleteRoot */

/**
 ****************************************************************************
 * <P> Delete an attribute.  </P>
 *
 * @methodName  McbDeleteAttribute
 *
 * @param       *pEntry		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	14th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbDeleteAttribute(McbXMLAttribute *pEntry)
{
    //assert(pEntry);

    if (pEntry->lpszName) {
        free(pEntry->lpszName);
        pEntry->lpszName = NULL;
    }

    if (pEntry->lpszValue) {
        free(pEntry->lpszValue);
        pEntry->lpszValue=  NULL;
    }

}/* McbDeleteAttribute */

/**
 ****************************************************************************
 * <P> Attach attributes from one list to another.  </P>
 *
 * @methodName  McbAttributeAttach
 *
 * @param       *pDst		
 * @param       *pSrc		
 * @param       nNum		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	14th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbAttributeAttach(McbXMLAttribute *pDst, McbXMLAttribute *pSrc, int nNum)
{
    int n;

    for (n=0; n<nNum; n++) {
        pDst[n].lpszName = pSrc[n].lpszName;
        pDst[n].lpszValue = pSrc[n].lpszValue;

        pSrc[n].lpszName = NULL;
        pSrc[n].lpszValue = NULL;
    }

}/* McbAttributeAttach */

/**
 ****************************************************************************
 * <P> Obtain the next character from the string.  </P>
 *
 * @methodName  McbGetNextChar
 *
 * @param       *pXML		
 *
 * @return      TCHAR
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	17th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
//获取 pXML的下一个字符
TCHAR McbGetNextChar(McbXML *pXML)
{
    TCHAR ch;

    ch = pXML->lpXML[pXML->nIndex];

    if (ch != 0) pXML->nIndex++;

    return ch;

}/* McbGetNextChar */

/**
 ****************************************************************************
 * <P> Find next non-white space character.  </P>
 *
 * @methodName  McbFindNonWhiteSpace
 *
 * @param       *pXML		
 *
 * @return      TCHAR
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	18th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
//获得一个字符，除 回车，换行，空格之外
TCHAR McbFindNonWhiteSpace(McbXML *pXML)
{
    TCHAR ch;
    LPCTSTR lpXML = pXML->lpXML;
    int nExit = FALSE;

    //assert(pXML);

    /*
     *************************************************************************
     * Iterate through characters in the string until we find a NULL or a
     * non-white space character
     *************************************************************************
     */
    while ((nExit == FALSE) && (ch = McbGetNextChar(pXML))) {
        switch (ch) {
        /*
         *********************************************************************
         * Ignore white space
         *********************************************************************
         */
        case _T('\n'):
        case _T(' '):
        case _T('\t'):
        case _T('\r'):
            continue;           

        default:
            nExit = TRUE;

        }
    }

    return ch;

}/* McbFindNonWhiteSpace */

/**
 ****************************************************************************
 * <P> Duplicate a given string.  </P>
 *
 * @methodName  McbStrdup
 *
 * @param       lpszData		
 * @param       cbData		
 *
 * @return      LPTSTR
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	18th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
LPTSTR McbStrdup(LPCTSTR lpszData, int cbData)
{
    LPTSTR lpszNew;

    //assert(lpszData);

    if (cbData == 0) cbData = _tcslen(lpszData);

    lpszNew = malloc((cbData+1) * sizeof(TCHAR));

    if (lpszNew) {
        memcpy(lpszNew, lpszData, (cbData) * sizeof(TCHAR));
        lpszNew[cbData] = (TCHAR)NULL;
    }

    return lpszNew;

}/* McbStrdup */
//说明：刚函数只是在调用属性名称的时候才使用
LPTSTR McbStrdupstrflg(LPCTSTR lpszData, int cbData)
{
    LPTSTR lpszNew;

    //assert(lpszData);

    if (cbData == 0) cbData = _tcslen(lpszData);

    lpszNew = malloc((cbData+3) * sizeof(TCHAR));//两个“和一个NULL结束符

    if (lpszNew) {
        lpszNew[0] = '"';//gancj
        memcpy(lpszNew+1, lpszData, (cbData) * sizeof(TCHAR));
        lpszNew[cbData+1] = '"';//gancj
        lpszNew[cbData+2] = (TCHAR)NULL;
    }
    return lpszNew;

}/* McbStrdup */

/**
 ****************************************************************************
 * <P> Find the next token in a string.  </P>
 *
 * @methodName  McbGetNextToken
 *
 * @param       *pXML		
 * @param       *pcbToken		
 * @param       *pType		
 *
 * @return      LPCTSTR
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	17th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
/***********************************************************************
*说明：获得一个标记 ， 
*         包括"<", "</" ,">", "=", "<?", "/>", ""......"",
*         "'....'",文字，把并pXML查找的地址存放在发挥的指针，同时，pXML的索引+pcbToken+空格
* 参数：pcbToken：标记的长度
*       pType：标记类型
******************************************************************/
McbNextToken McbGetNextToken(McbXML *pXML, int *pcbToken, McbTokenType *pType)
{
    McbNextToken        result;
    LPCTSTR         lpXML;
    TCHAR           ch;
    TCHAR           chTemp;
    int             nSize;
    int             nFoundMatch;
    int             nExit;
    int             n;
    LPCTSTR         lpszOpen;
    int             cbOpen;
    int             nIsText = FALSE;
       TCHAR chTemp2;

    /*
     *************************************************************************
     * Find next non-whte space character
     *************************************************************************
     */
    ch = McbFindNonWhiteSpace(pXML);//超找一个有效的字符，除回车 ，空格，换行，制表符

    if (ch) {
        /*
         *********************************************************************
         * Cache the current string pointer
         *********************************************************************
         */
        lpXML = pXML->lpXML;
        result.pStr = &lpXML[pXML->nIndex-1];

        /*
         *********************************************************************
         * First check whether the token is in the clear tag list (meaning it 
         * does not need formatting).
         *********************************************************************
         */
        n = 0;
        //检测是否是
        while (TRUE) {
            /*
             *****************************************************************
             * Obtain the name of the open part of the clear tag
             *****************************************************************
             */
            lpszOpen = pXML->pClrTags[n].lpszOpen;

            if (lpszOpen) {
                /*
                 *************************************************************
                 * Compare the open tag with the current token
                 *************************************************************
                 */
                //比较是否是开始标示，如果是，就返回
                cbOpen = _tcslen(lpszOpen);

                if (_tcsnicmp(lpszOpen, result.pStr, cbOpen) == 0) {
                    result.pClr = &pXML->pClrTags[n];
                    pXML->nIndex += cbOpen-1;
                    *pType  = eTokenClear;
                    return result;
                }

                n++;
            } else {
                break;
            }
        }

        /*
         *********************************************************************
         * If we didn't find a clear tag then check for standard tokens
         *********************************************************************
         */
        //
        chTemp = 0;

        lpXML = pXML->lpXML;

        switch (ch) {
        /*
         *********************************************************************
         * Check for quotes
         *********************************************************************
         */
        //如果是 \" 和 \' 字符引用字符，就查找下一个字符
        
        case _T('\''):
        case _T('\"'):

            /*
             *****************************************************************
             * Type of token
             *****************************************************************
             */
            *pType = eTokenQuotedText;

            chTemp = ch;

            /*
             *****************************************************************
             * Set the size
             *****************************************************************
             */          
            nSize = 1;

            nFoundMatch = FALSE;

            /*
             *****************************************************************
             * Search through the string to find a matching quote
             *****************************************************************
             */
            while (ch = McbGetNextChar(pXML)) {
                nSize++;

                if (ch == chTemp) {
                    nFoundMatch = TRUE;
                    break;
                }
            }

            /*
             *****************************************************************
             * If we failed to find a matching quote
             *****************************************************************
             */
            if (nFoundMatch == FALSE) {
                /*
                 *************************************************************
                 * Indicate error
                 *************************************************************
                 */
                pXML->error = eXMLErrorNoMatchingQuote;             
                *pType = eTokenError;
            }

            /* MCB 4.02.2002 */
            if (McbFindNonWhiteSpace(pXML)) {
                pXML->nIndex--;
            }

            break;

            /*
             *********************************************************************
             * Equals (used with attribute values)
             *********************************************************************
             */
        case _T('='):
            nSize = 1;
            *pType = eTokenEquals;
            break;

            /*
             *********************************************************************
             * Close tag
             *********************************************************************
             */
        case _T('>'):
            nSize = 1;
            *pType = eTokenCloseTag;
            break;

            /*
             *********************************************************************
             * Check for tag start and tag end
             *********************************************************************
             */
        case _T('<'):

            /*
             *****************************************************************
             * Peek at the next character to see if we have an end tag '</',
             * or an xml declaration '<?'
             *****************************************************************
             */
            chTemp = pXML->lpXML[pXML->nIndex];
            chTemp2 = pXML->lpXML[pXML->nIndex+1];
            /*
             *****************************************************************
             * If we have a tag end...
             *****************************************************************
             */
            //是结束标签
            if (chTemp == _T('/')) {
                /*
                 *************************************************************
                 * Set the type and ensure we point at the next character 
                 *************************************************************
                 */
                McbGetNextChar(pXML);
                *pType = eTokenTagEnd;
                nSize = 2;
            }
            /*
             *****************************************************************
             * If we have an XML declaration tag
             *****************************************************************
             *///是申明标签？
            else if (chTemp == _T('?')) {
                /*
                 *************************************************************
                 * Set the type and ensure we point at the next character 
                 *************************************************************
                 */
                McbGetNextChar(pXML);
                *pType = eTokenDeclaration;
                nSize = 2;
            }
            /*
             *****************************************************************
             * Otherwise we must have a start tag
             *****************************************************************
             */
            else {
                *pType = eTokenTagStart;
                nSize = 1;
            }
            break;

            /*
             *********************************************************************
             * Check to see if we have a short hand type end tag ('/>').
             *********************************************************************
             */
        case _T('/'):
            /*
             *****************************************************************
             * Peek at the next character to see if we have an end tag '</'
             * or an xml declaration '<?'
             *****************************************************************
             */
            chTemp = pXML->lpXML[pXML->nIndex];

            /*
             *****************************************************************
             * If we have a short hand end tag...
             *****************************************************************
             */
            if (chTemp == _T('>')) {
                /*
                 *************************************************************
                 * Set the type and ensure we point at the next character 
                 *************************************************************
                 */
                McbGetNextChar(pXML);
                *pType = eTokenShortHandClose;
                nSize = 2;
                break;
            }

            /*
             *****************************************************************
             * If we haven't found a short hand closing tag then drop into the
             * text process
             *****************************************************************
             */

            /*
             *********************************************************************
             * Other characters
             *********************************************************************
             */
        default:
            nIsText = TRUE;
        }

        /*
         *********************************************************************
         * If this is a TEXT node
         *********************************************************************
         */
        //如果是文件直到超找到结束标签
        if (nIsText) {
            /*
             *****************************************************************
             * Indicate we are dealing with text
             *****************************************************************
             */
            *pType = eTokenText;

            nSize = 1;
            nExit = FALSE;

            while ((nExit == FALSE) && (ch = McbGetNextChar(pXML))) {
                switch (ch) {
                /*
                 *************************************************************
                 * Break when we find white space
                 *************************************************************
                 */
                case _T('\n'):
                case _T(' '):
                case _T('\t'):
                case _T('\r'):
                    nExit = TRUE;
                    break;

                    /*
                     *************************************************************
                     * If we find a slash then this maybe text or a short hand end
                     * tag.
                     *************************************************************
                     */
                case _T('/'):
                    /*
                     *********************************************************
                     * Peek at the next character to see it we have short hand
                     * end tag
                     *********************************************************
                     */
                    chTemp = pXML->lpXML[pXML->nIndex];

                    /*
                     *********************************************************
                     * If we found a short hand end tag then we need to exit 
                     * the loop
                     *********************************************************
                     */
                    if (chTemp == _T('>')) {
                        pXML->nIndex--; /* MCB 03.02.2002 */
                        nExit = TRUE;
                    } else {
                        nSize++;
                    }
                    break;

                    /*
                     *************************************************************
                     * Break when we find a terminator and decrement the index and
                     * column count so that we are pointing at the right character
                     * the next time we are called.
                     *************************************************************
                     */
                case _T('<'):
                case _T('>'):
                case _T('='):
                    pXML->nIndex--;                 
                    nExit = TRUE;
                    break;

                case 0:
                    nExit = TRUE;
                    break;

                default:
                    nSize++;
                }
            }
        }

        *pcbToken = nSize;
    }
    /*
     *************************************************************************
     * If we failed to obtain a valid character
     *************************************************************************
     */
    else {
        *pcbToken = 0;
        *pType = eTokenError;
    }

    return result;

}/* McbGetNextToken */

/**
 ****************************************************************************
 * <P> Parse XML errors into a user friendly string.  </P>
 *
 * @methodName  McbGetError
 *
 * @param       error		
 *
 * @return      LPCTSTR
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	19th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
LPCTSTR McbGetError(McbXMLError error)
{
    LPCTSTR lpszErr = _T("Unknown");
    int n;

    /*
     *************************************************************************
     * Structure for errors array
     *************************************************************************
     */
    typedef struct McbErrorList {
        enum McbXMLError err;
        LPCTSTR lpszErr;
    } McbErrorList;

    /*
     *************************************************************************
     * Static array containing helpful error text.
     *************************************************************************
     */
    static struct McbErrorList errs[] = 
    {
        { eXMLErrorNone,                _T("No error")},
        { eXMLErrorEmpty,               _T("No XML data")},
        { eXMLErrorFirstNotStartTag,    _T("First token not start tag")},
        { eXMLErrorMissingTagName,      _T("Missing start tag name")},
        { eXMLErrorMissingEndTagName,   _T("Missing end tag name")},
        { eXMLErrorNoMatchingQuote,     _T("Unmatched quote")},
        { eXMLErrorUnmatchedEndTag,     _T("Unmatched end tag")},
        { eXMLErrorUnexpectedToken,     _T("Unexpected token found")},
        { eXMLErrorInvalidTag,          _T("Invalid tag found")},
        { eXMLErrorNoElements,          _T("No elements found")},
        { 0,                            NULL}
    };

    /*
     *************************************************************************
     * Iterate through the list of errors to find a matching error
     *************************************************************************
     */
    for (n = 0; errs[n].lpszErr; n++) {
        if (errs[n].err == error) {
            lpszErr = errs[n].lpszErr;
            break;
        }
    }

    return lpszErr; 

}/* McbGetError */

/**
 ****************************************************************************
 * <P> Delete memory associated with a text node.   </P>
 *
 * @methodName  McbDeleteText
 *
 * @param       *pText		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbDeleteText(McbXMLText *pText)
{
    //assert(pText);

    if (pText->lpszValue) {
        free(pText->lpszValue);
        pText->lpszValue = NULL;
    }

}/* McbDeleteText */

/**
 ****************************************************************************
 * <P> Delete memory associated with a clear (unformatted) node.   </P>
 *
 * @methodName  McbDeleteClear
 *
 * @param       *pClear		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbDeleteClear(McbXMLClear *pClear)
{
    //assert(pClear);

    if (pClear->lpszValue) {
        free(pClear->lpszValue);
        pClear->lpszValue = NULL;
    }

}/* McbDeleteClear */

/**
 ****************************************************************************
 * <P> Delete a given node.  </P>
 *
 * @methodName  McbDeleteNode
 *
 * @param       *pEntry		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbDeleteNode(McbXMLNode *pEntry)
{
    if (pEntry) {
        if (pEntry->type == eNodeEmpty) {
            return;
        }

        /*
         *********************************************************************
         * Delete the appropriate node
         *********************************************************************
         */
        switch (pEntry->type) {
        case eNodeAttribute:
            McbDeleteAttribute(pEntry->node.pAttrib);
            break;

        case eNodeElement:
            McbDeleteElement(pEntry->node.pElement);
            break;

        case eNodeText:
            McbDeleteText(pEntry->node.pText);
            break;

        case eNodeClear:
            McbDeleteClear(pEntry->node.pClear);
            break;

        default:
            ;//;assert(TRUE);
        }   

        free(pEntry->node.pAttrib);
        pEntry->type = eNodeEmpty;
    }

}/* McbDeleteNode */

/**
 ****************************************************************************
 * <P> Delete an element and all it's contained nodes.  </P>
 *
 * @methodName  McbDeleteElement
 *
 * @param       *pEntry		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbDeleteElement(McbXMLElement *pEntry)
{
    int n;

    //assert(pEntry);

    /*
     *************************************************************************
     * Delete each node (this may recurse)
     *************************************************************************
     */
    for (n = 0; n<pEntry->nSize; n++) {
        McbDeleteNode(&pEntry->pEntries[n]);
    }

    /*
     *************************************************************************
     * Cleanup the list of nodes
     *************************************************************************
     */
    pEntry->nMax = 0;
    pEntry->nSize = 0;

    free(pEntry->pEntries);
    pEntry->pEntries = NULL;

    /*
     *************************************************************************
     * Delete the name of the element
     *************************************************************************
     */
    if (pEntry->lpszName) {
        free(pEntry->lpszName);
        pEntry->lpszName = NULL;
    }

    /* free(pEntry); */

}/* McbDeleteElement */

/**
 ****************************************************************************
 * <P> Attach nodes from one list to another.  </P>
 *
 * @methodName  McbAttachNodes
 *
 * @param       *pDst		
 * @param       *pSrc		
 * @param       nNum		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	14th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbAttachNodes(McbXMLNode *pDst, McbXMLNode *pSrc, int nNum)
{
    int n;

    for (n=0; n<nNum; n++) {
        pDst[n] = pSrc[n];
        pSrc[n].type = eNodeEmpty;
    }

}/* McbAttachNodes */

/**
 ****************************************************************************
 * <P> Reserve memory for additional nodes.  </P>
 *
 * @methodName  McbAllocNodes
 *
 * @param       *pEntry		
 * @param       nGrowBy		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	15th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
/***************************************************************
*说明： 之前的节点内存全部删除，重新申请最大的节点，并把旧的节点添加进来，在释放旧的节点内存
*nGrowBy：申请节点的个数
***************************************************************/
void McbAllocNodes(McbXMLElement *pEntry, int nGrowBy)
{
    int nMax;
    McbXMLNode * pNew;

    //assert(pEntry);
    //assert(nGrowBy > 0);


    /**************************************************************************
     * Allocate storage for new nodes
     **************************************************************************/
    nMax = pEntry->nMax += nGrowBy;
    pNew = malloc(sizeof(McbXMLNode) * nMax);//申请子节点的内存内存
    //把旧的节点附加到新的内存上面，并把旧的节点释放掉
    McbAttachNodes(pNew, pEntry->pEntries, pEntry->nSize);

    if (pEntry->pEntries) {
        free(pEntry->pEntries);
    }

    pEntry->pEntries = pNew;

}/* McbAllocNodes */


/***************************************************************
*添加一个节点到该节点上面 
* 参数：pEntry 要添加父的节点
*      lpszName：添加的子节点名字
*      nIsDeclaration：节点类型，如果为<?表示申明节点
*      如果子节点的内存不够，就申请5个
***************************************************************/
McbXMLElement * McbAddElement(McbXMLElement *pEntry, LPTSTR lpszName, 
                              int nIsDeclaration, int nGrowBy)
{
    McbXMLNode *pNode;
    McbXMLElement * pElement;

    //assert(pEntry);
    //assert(nGrowBy > 0);

    /*
     *************************************************************************
     * Check we have enough storage
     *************************************************************************
     */
    if (pEntry->nSize == pEntry->nMax) {
        McbAllocNodes(pEntry, nGrowBy);
    }

    /*
     *************************************************************************
     * Obtain the new node, set the type and initialise it.
     *************************************************************************
     */
    pNode = &pEntry->pEntries[pEntry->nSize];
    pNode->type = eNodeElement;//

    pElement = malloc(sizeof(McbXMLElement));
    pNode->node.pElement = pElement;

    McbInitElement(pElement, pEntry, lpszName, nIsDeclaration);

    /*
     *************************************************************************
     * Increment the number of contained nodes
     *************************************************************************
     */
    pEntry->nSize++;

    /*
     *************************************************************************
     * Return the new element.
     *************************************************************************
     */
    return pElement;

}/* McbAddElement */


/***************************************************************
*说明：给节点添加个属性 
*   lpszName：  节点属性的名称
*   lpszValue： 节点属性的内容
***************************************************************/
McbXMLAttribute * McbAddAttribute(McbXMLElement *pEntry, LPTSTR lpszName, 
                                  LPTSTR lpszValue, int nGrowBy)
{
    McbXMLNode *pNode;
    McbXMLAttribute *pAttr;

    //assert(pEntry);
    //assert(nGrowBy > 0);

    /*
     *************************************************************************
     * Check we have enough storage
     *************************************************************************
     */
    if (pEntry->nSize == pEntry->nMax) {
        McbAllocNodes(pEntry, nGrowBy);
    }

    /*
     *************************************************************************
     * Obtain the new node, set the type and initialise it.
     *************************************************************************
     */
    pNode = &pEntry->pEntries[pEntry->nSize];
    pNode->type = eNodeAttribute;

    pAttr = malloc(sizeof(McbXMLAttribute));
    pNode->node.pAttrib = pAttr;

    //pAttr->lpszName = lpszName;
    //pAttr->lpszValue = lpszValue;
    //toby add 2006/7/4 10:03
    pAttr->lpszName = lpszName;//McbStrdup(lpszName,0);

    
    pAttr->lpszValue = lpszValue;//McbStrdup(lpszValue,0);

    /*
     *************************************************************************
     * Increment the number of contained nodes
     *************************************************************************
     */
    pEntry->nSize++;

    /*
     *************************************************************************
     * Return the new attribute.
     *************************************************************************
     */
    return pAttr;

}/* McbAddAttribute */

/**
 ****************************************************************************
 * <P> Add text to the element.  </P>
 *
 * @methodName  * McbAddText
 *
 * @param       *pEntry		
 * @param       lpszValue		
 * @param       nGrowBy		
 *
 * @return      McbXMLText
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
/***************************************************************
* 
*  说明：添加一个文本到节点上面，一般为标签的名称
***************************************************************/
McbXMLText * McbAddText(McbXMLElement *pEntry, LPTSTR lpszValue, int nGrowBy)
{
    McbXMLNode *pNode;
    McbXMLText *pText;
    /*
     *************************************************************************
     * Check we have enough storage
     *************************************************************************
     */
    if (pEntry->nSize == pEntry->nMax) {
        McbAllocNodes(pEntry, nGrowBy);
    }

    /*
     *************************************************************************
     * Obtain the new node, set the type and initialise it.
     *************************************************************************
     */
    pNode = &pEntry->pEntries[pEntry->nSize];
    pNode->type = eNodeText;

    pText = malloc(sizeof(McbXMLText));
    pNode->node.pText = pText;
    pText->lpszValue = lpszValue;

    /*
     *************************************************************************
     * Increment the number of contained nodes
     *************************************************************************
     */
    pEntry->nSize++;

    /*
     *************************************************************************
     * Return the new attribute.
     *************************************************************************
     */
    return pText;

}/* McbAddText */

/**
 ****************************************************************************
 * <P> Add clear (unformatted) to the element.  </P>
 *
 * @methodName  * McbAddClear
 *
 * @param       *pEntry		
 * @param       lpszValue		
 * @param       nGrowBy		
 *
 * @return      McbXMLClear
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLClear * McbAddClear(McbXMLElement *pEntry, LPTSTR lpszValue, 
                          McbClearTag *pClear, int nGrowBy)
{
    McbXMLNode *pNode;
    McbXMLClear *pNewClear;

    //assert(pEntry);
    //assert(nGrowBy > 0);

    /*
     *************************************************************************
     * Check we have enough storage
     *************************************************************************
     */
    if (pEntry->nSize == pEntry->nMax) {
        McbAllocNodes(pEntry, nGrowBy);
    }

    /*
     *************************************************************************
     * Obtain the new node, set the type and initialise it.
     *************************************************************************
     */
    pNode = &pEntry->pEntries[pEntry->nSize];
    pNode->type = eNodeClear;

    pNewClear = malloc(sizeof(McbXMLClear));
    pNode->node.pClear = pNewClear;
    pNewClear->lpszValue = lpszValue;
    pNewClear->lpszOpenTag = pClear->lpszOpen;
    pNewClear->lpszCloseTag = pClear->lpszClose;

    /*
     *************************************************************************
     * Increment the number of contained nodes
     *************************************************************************
     */
    pEntry->nSize++;

    /*
     *************************************************************************
     * Return the new attribute.
     *************************************************************************
     */
    return pNewClear;

}/* McbAddClear */

/**
 ****************************************************************************
 * <P> Enumerate nodes in the list.  </P>
 *
 * @methodName  McbEnumNodes
 *
 * @param       *pEntry		
 * @param       *pnIndex		
 *
 * @return      McbXMLNode
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLNode * McbEnumNodes(McbXMLElement *pEntry, int *pnIndex)
{   
    McbXMLNode * pResult = NULL;

    //assert(pnIndex);
    //assert(pEntry);

    if (*pnIndex < pEntry->nSize) {
        pResult = &pEntry->pEntries[*pnIndex];
        (*pnIndex)++;
    }

    return pResult;

}/* McbEnumNodes */

/**
 ****************************************************************************
 * <P> Enumerate elements on a base element.  </P>
 *
 * @methodName  McbEnumElements
 *
 * @param       *pEntry		
 * @param       *pnIndex		
 *
 * @return      McbXMLNode
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLElement * McbEnumElements(McbXMLElement *pEntry, int *pnIndex)
{   
    McbXMLElement * pResult = NULL;
    int nIndex;

    //assert(pnIndex);
    //assert(pEntry);

    nIndex = *pnIndex;

    for (;nIndex < pEntry->nSize && !pResult; nIndex++) {
        if (pEntry->pEntries[nIndex].type == eNodeElement) {
            pResult = pEntry->pEntries[nIndex].node.pElement;
        }
    }

    *pnIndex = nIndex;

    return pResult;

}/* McbEnumElements */

/**
 ****************************************************************************
 * <P> Enumerate attributes on a base element.  </P>
 *
 * @methodName  McbEnumAttributes
 *
 * @param       *pEntry		
 * @param       *pnIndex		
 *
 * @return      McbXMLNode
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLAttribute * McbEnumAttributes(McbXMLElement *pEntry, int *pnIndex)
{   
    McbXMLAttribute * pResult = NULL;
    int nIndex;

    //assert(pnIndex);
    //assert(pEntry);

    nIndex = *pnIndex;

    for (;nIndex < pEntry->nSize && !pResult; nIndex++) {
        if (pEntry->pEntries[nIndex].type == eNodeAttribute) {
            pResult = pEntry->pEntries[nIndex].node.pAttrib;
        }
    }

    *pnIndex = nIndex;

    return pResult;

}/* McbEnumAttributes */

/**
 ****************************************************************************
 * <P> Trim the end of the text to remove white space characters.  </P>
 *
 * @methodName  McbFindEndOfText
 *
 * @param       lpszToken		
 * @param       *pcbText		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
/***************************************************************
*找到一段文字，如果遇见了结束符，回车，换行，空格，表示文件结束，pcbText表示文件的下一个位置
***************************************************************/
void McbFindEndOfText(LPCTSTR lpszToken, int *pcbText)
{
    TCHAR   ch;
    int     cbText;

    //assert(lpszToken);
    //assert(pcbText);

    cbText = (*pcbText)-1;  

    while (TRUE) {
        //assert(cbText >= 0);

        ch = lpszToken[cbText];

        switch (ch) {
        case _T('\r'):
        case _T('\n'):
        case _T('\t'):
        case _T(' '):
            cbText--;
            break;

        default:
            *pcbText = cbText+1;
            return;
        }
    }

}/* McbFindEndOfText */

/**
 ****************************************************************************
 * <P> Parse a clear (unformatted) type node.  </P>
 *
 * @methodName  McbParseClearTag
 *
 * @param       *pXML		
 * @param       *pElement		
 * @param       lpszClose		
 * @param       lpszToken		
 *
 * @return      int
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	26th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
/*添加一个注释到节点中*/
int McbParseClearTag(McbXML *pXML, McbXMLElement *pElement, McbClearTag * pClear)
{
    int cbTemp = 0;

    LPTSTR lpszTemp;
    LPCTSTR lpszXML = &pXML->lpXML[pXML->nIndex];

    /*
     *************************************************************************
     * Find the closing tag
     *************************************************************************
     */
     lpszTemp = _tcsstr(lpszXML, pClear->lpszClose);

    /*
     *************************************************************************
     * Iterate through the tokens until we find the closing tag.
     *************************************************************************
     */
    if (lpszTemp) {
        /*
         *********************************************************************
         * Cache the size and increment the index
         *********************************************************************
         */
        cbTemp = lpszTemp - lpszXML;

        pXML->nIndex += cbTemp;
        pXML->nIndex += _tcslen(pClear->lpszClose);

        /*
         *********************************************************************
         * Add the clear node to the current element
         *********************************************************************
         */
        lpszTemp = McbStrdup(lpszXML, cbTemp);

        McbAddClear(pElement, lpszTemp, pClear, McbGROWBY);

#ifdef McbSTOREOFFSETS
        pElement->pEntries[pElement->nSize-1].nStringOffset = (lpszXML -
                                                               _tcslen(pClear->lpszOpen)) - pXML->lpXML;           
#endif /* McbSTOREOFFSETS */

        return TRUE;
    }
    /*
     *************************************************************************
     * If we failed to find the end tag
     *************************************************************************
     */
    else {
        pXML->error = eXMLErrorUnmatchedEndTag;
        return FALSE;
    }

}/* McbParseClearTag */

/**
 ****************************************************************************
 * <P> Recursively parse an XML element.  </P>
 *
 * @methodName  McbParseXMLElement
 *
 * @param       *pXML		
 * @param       * pElement		
 *
 * @return      int
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	19th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
int McbParseXMLElement(McbXML *pXML, McbXMLElement * pElement)
{
    LPCTSTR lpszToken;
    int cbToken;

    enum McbTokenType type;
    McbNextToken token;

    LPCTSTR lpszTemp;
    int cbTemp;
    int offset,start,end;

    int nDeclaration;

    LPCTSTR lpszText = NULL;

    McbXMLElement *pNew;

    enum McbStatus status;
    enum McbAttrib attrib = eAttribName;

    if (pXML->nFirst) {
        pXML->nFirst = FALSE;
        status = eOutsideTag;
    }
    /*
     *************************************************************************
     * If this is not the first call then we should only be called when inside
     * a tag.
     *************************************************************************
     */
    else {
        status = eInsideTag;
    }
    while (TRUE) {
        token = McbGetNextToken(pXML, &cbToken, &type);
        //printf("McbGetNextToken1:cbToken = %d,type = %d,pXML->nIndex = %d\n\r",cbToken,type,pXML->nIndex);
        if (type != eTokenError) {
            switch (status) {
            case eOutsideTag:
                //printf("eOutsideTag\n\r");
                switch (type) {
                //内容里面可以包括文字 "",= ,''
                case eTokenText:
                case eTokenQuotedText:
                case eTokenEquals:
               

                   //printf("eTokenEquals \n\r");
                    if (lpszText==NULL) {
                        lpszText = token.pStr;
                        start = pXML->nIndex - cbToken;//gancj
                        //printf("start1 = %d\n\r",start);
                    }

                    break;              

                    //如果是开始标签，就函数冲入
                case eTokenTagStart:
                case eTokenDeclaration:
                    //如果type == eTokenDeclaration，那么 nDeclaration =1,否则就为0
                    nDeclaration = type == eTokenDeclaration;
                    //If we have node text then add this to the element
                    if (lpszText) {
                        cbTemp = token.pStr - lpszText;
                        //printf("cbTemp1 = %d\n\r",cbTemp);

                        McbFindEndOfText(lpszText, &cbTemp);

                        McbAddText(pElement, McbStrdup(lpszText, cbTemp), 
                                   McbGROWBY);
#ifdef McbSTOREOFFSETS
                        pElement->pEntries[pElement->nSize-1].nStringOffset
                        = lpszText - pXML->lpXML;
#endif /* McbSTOREOFFSETS */

                        lpszText = NULL;
                    }
                    //查找标签的名字，如果不是文件，就返回错误
                    token = McbGetNextToken(pXML, &cbToken, &type);

                    //printf("McbGetNextToken2:cbToken = %d,type = %d,pXML->nIndex =%d\n\r",cbToken,type,pXML->nIndex);
                    if (type != eTokenText) {
                        pXML->error = eXMLErrorMissingTagName;
                        return FALSE;
                    }


                    /**********************************************************
                    * If we found a new element which has the same as this 
                    * element then we need to pass this back to the caller..
                    **********************************************************/

                    //  //printf("ss:%s\n\r",McbStrdup(token.pStr,cbToken));
                    if (pElement->lpszName && 
                        _tcsnicmp(pElement->lpszName, token.pStr, 
                                  cbToken<_tcslen(pElement->lpszName)?_tcslen(pElement->lpszName): cbToken) == 0) {

                        //printf("caller back pXML->nIndex = %d;pElement->lpszName = %s\n\r",pXML->nIndex,pElement->lpszName);

                        /*
                         *****************************************************
                         * Indicate to the caller that it needs to create a 
                         * new element.
                         *****************************************************
                         */
                        pXML->lpNewElement = token.pStr;
                        pXML->cbNewElement = cbToken;
                        return TRUE;
                    }
                    /*
                     *********************************************************
                     * If the name of the new element differs from the name of
                     * the current element we need to add the new element to 
                     * the current one and recurse
                     *********************************************************
                     */
                    //如果是新的节点？
                    else {
                        /*
                         *****************************************************
                         * Now we need to add the new element and recurse
                         *****************************************************
                         */
                        LPTSTR  NAME = McbStrdup(token.pStr, cbToken);
                        pNew = McbAddElement(pElement, NAME
                                             , nDeclaration, 
                                             McbGROWBY);
                        //printf("NEW McbAddElement1 pXML->nIndex = %d;pElement->nSize=%d,NAME = %s,\n\r",pXML->nIndex,pElement->nSize,NAME);

#ifdef McbSTOREOFFSETS
                        pElement->pEntries[pElement->nSize-1].nStringOffset = 
                        token.pStr - pXML->lpXML;
#endif /* McbSTOREOFFSETS */

                        while (pNew) {
                            if (!McbParseXMLElement(pXML, pNew)) {
                                return FALSE;
                            } else {
                                /*
                                 *********************************************
                                 * If the call to recurse this function 
                                 * evented in a end tag specified in XML then 
                                 * we need to unwind the calls to this 
                                 * function until we find the appropriate node 
                                 * (the element name and end tag name must 
                                 * match)
                                 *********************************************
                                 */
                                if (pXML->cbEndTag) {
                                    /*
                                     *****************************************
                                     * If we are back at the root node then we 
                                     * have an unmatched end tag
                                     *****************************************
                                     */
                                    if (!pElement->lpszName) {
                                        pXML->error = 
                                        eXMLErrorUnmatchedEndTag;
                                        return FALSE;
                                    }

                                    /*
                                     *****************************************
                                     * If the end tag matches the name of this 
                                     * element then we only need to unwind 
                                     * once more...
                                     *****************************************
                                     */
                                    if (_tcsnicmp(pXML->lpEndTag, 
                                                  pElement->lpszName, 
                                                  _tcslen(pElement->lpszName)) == 0) {
                                        pXML->cbEndTag = 0;
                                    }

                                    return TRUE;
                                }
                                /*
                                 *********************************************
                                 * If the call indicated a new element is to 
                                 * be created on THIS element.
                                 *********************************************
                                 */
                                else if (pXML->cbNewElement) {
                                    /*
                                     *****************************************
                                     * If the name of this element matches the
                                     * name of the element we need to create 
                                     * then we need to return to the caller 
                                     * and let it process the element.
                                     *****************************************
                                     */
                                    if (_tcsnicmp(pXML->lpNewElement, 
                                                  pElement->lpszName, 
                                                  _tcslen(pElement->lpszName)) == 0) {
                                        return TRUE;
                                    }

                                    /*
                                     *****************************************
                                     * Add the new element and recurse
                                     *****************************************
                                     */
                                    pNew = McbAddElement(pElement, 
                                                         McbStrdup(pXML->lpNewElement, 
                                                                   pXML->cbNewElement), FALSE, 
                                                         McbGROWBY);

#ifdef McbSTOREOFFSETS
                                    pElement->pEntries[pElement->nSize-1].
                                    nStringOffset = 
                                    pXML->lpNewElement - pXML->lpXML;
#endif /* McbSTOREOFFSETS */


                                    pXML->cbNewElement = 0;
                                }
                                /*
                                 *********************************************
                                 * If we didn't have a new element to create
                                 *********************************************
                                 */
                                else {
                                    pNew = NULL;
                                }
                            }
                        }
                    }
                    break;
                    /***************************************************************
                    *发现了结束符符号，就返回回
                    ***************************************************************/
                case eTokenTagEnd:
                    //printf("find endtag \n\r",pXML->nIndex);
                    if (lpszText) {
                        //printf("cbTemp2 = %d\n\r",cbTemp);
                        cbTemp = token.pStr - lpszText;
                        McbFindEndOfText(lpszText, &cbTemp);

                        McbAddText(pElement, McbStrdup(lpszText, cbTemp), 
                                   McbGROWBY);

#ifdef McbSTOREOFFSETS
                        pElement->pEntries[pElement->nSize-1].nStringOffset
                        = lpszText - pXML->lpXML;
#endif /* McbSTOREOFFSETS */

                        lpszText = NULL;                        
                    }
                    // Find the name of the end tag
                    token = McbGetNextToken(pXML, &cbTemp, &type);                    
                    ////printf("McbGetNextToken2:cbToken = %d,type = %d,pXML->nIndex = %d\n\r",cbToken,type,pXML->nIndex);
                    //The end tag should be text
                    if (type != eTokenText) {
                        pXML->error = eXMLErrorMissingEndTagName;
                        return FALSE;
                    }
                    lpszTemp = token.pStr;

                    // After the end tag we should find a closing tag
                    token = McbGetNextToken(pXML, &cbToken, &type);

                    //printf("McbGetNextToken4:cbToken = %d,type = %d,pXML->nIndex = %d \n\r",cbToken,type,pXML->nIndex);
                    //如果没有发现关闭标签，就返回错误
                    if (type != eTokenCloseTag) {
                        pXML->error = eXMLErrorMissingEndTagName;
                        return FALSE;
                    }

                    /*
                     *********************************************************
                     * We need to return to the previous caller.  If the name 
                     * of the tag cannot be found we need to keep returning to
                     * caller until we find a match
                     *********************************************************
                     */

                    /*如果结束的标签名字和当前的标签名字不同，*/
                    if (_tcsnicmp(lpszTemp, pElement->lpszName, 
                                  _tcslen(pElement->lpszName)) != 0) {
                        pXML->lpEndTag = lpszTemp;//返回结束标签
                        pXML->cbEndTag = cbTemp;//结束标签的大小
                    }
                    return TRUE;

                case eTokenClear://If we found a clear (unformatted) token
                    /*
                     *********************************************************
                     * If we have node text then add this to the element
                     **********************************************************/ 
                      
                     //printf("find eTokenClear \n\r");
                    if (lpszText) {
                        //printf("eTokenClear cbTemp3 = %d\n\r",cbTemp);
                        cbTemp = token.pStr - lpszText;

                        McbFindEndOfText(lpszText, &cbTemp);

                        McbAddText(pElement, McbStrdup(lpszText, cbTemp), 
                                   McbGROWBY);

#ifdef McbSTOREOFFSETS
                        pElement->pEntries[pElement->nSize-1].nStringOffset
                        = lpszText - pXML->lpXML;
#endif /* McbSTOREOFFSETS */

                        lpszText = NULL;
                    }

                    if (!McbParseClearTag(pXML, pElement, token.pClr)) {
                        return FALSE;
                    }
                    break;

                    /*
                     *************************************************************
                     * Errors...
                     *************************************************************
                     */              
                case eTokenCloseTag:        /* '>'		*/
                case eTokenShortHandClose:  /* '/>'		*/
                    pXML->error = eXMLErrorUnexpectedToken;
                    return FALSE;
                }
                break;
///////////////////////////////////////////////////////////////////////////////////////////////////
            case eInsideTag:
                //printf("eInsideTag attrib = %d\n\r",attrib); 
                switch (attrib) {
                case eAttribName:
                    switch (type) {
                    case eTokenText:
                        lpszTemp = token.pStr;//属性名字
                        cbTemp = cbToken;    //属性字节数
                        attrib = eAttribEquals;//下一个预期的字符
                        break;
                    case eTokenCloseTag:
                        status = eOutsideTag;
                        break;

                    case eTokenShortHandClose:
                        return TRUE;
                    case eTokenQuotedText:  /* '"SomeText"'	*/
                    case eTokenTagStart:    /* '<'			*/
                    case eTokenTagEnd:      /* '</'			*/
                    case eTokenEquals:      /* '='			*/
                    case eTokenDeclaration: /* '<?'			*/
                    case eTokenClear:
                        pXML->error = eXMLErrorUnexpectedToken;
                        return FALSE;
                    }
                    break;
                case eAttribEquals:
                    switch (type) {
                    case eTokenText://预期得到的是字符，就添加节点
                        McbAddAttribute(pElement, McbStrdup(lpszTemp, 
                                                            cbTemp), NULL, McbGROWBY);
#ifdef McbSTOREOFFSETS
                        pElement->pEntries[pElement->nSize-1].nStringOffset =
                        lpszTemp - pXML->lpXML;
#endif /* McbSTOREOFFSETS */

                        lpszTemp = token.pStr;
                        cbTemp = cbToken;
                        break;
                    case eTokenShortHandClose:
                    case eTokenCloseTag:
                        /*
                         *****************************************************
                         * If we are a declaration element '<?' then we need
                         * to remove extra closing '?' if it exists
                         *****************************************************
                         */
                        if (pElement->nIsDeclaration && 
                            (lpszTemp[cbTemp-1]) == _T('?')) {
                            cbTemp--;
                        }
                        if (cbTemp) {
                            /**************************************************
                            * Add the unvalued attribute to the list
                            **************************************************/                            
                            McbAddAttribute(pElement, McbStrdup(lpszTemp, 
                                                                cbTemp), NULL, McbGROWBY);
#ifdef McbSTOREOFFSETS
                            pElement->pEntries[pElement->nSize-1].
                            nStringOffset = lpszTemp - pXML->lpXML;
#endif /* McbSTOREOFFSETS */
                        }
                        //If this is the end of the tag then return to the caller
                        if (type == eTokenShortHandClose) {
                            return TRUE;
                        }
                        status = eOutsideTag;
                        break;
                    case eTokenEquals:
                        attrib = eAttribValue;
                        break;
                    case eTokenQuotedText:  /* 'Attribute "InvalidAttr"'*/
                    case eTokenTagStart:    /* 'Attribute <'			*/
                    case eTokenTagEnd:      /* 'Attribute </'			*/
                    case eTokenDeclaration: /* 'Attribute <?'			*/
                    case eTokenClear:
                        pXML->error = eXMLErrorUnexpectedToken;
                        return FALSE;

                    }
                    break;

                    //If we are looking for an attribute value
                case eAttribValue:
                    switch (type) {
                    case eTokenText:
                    case eTokenQuotedText:
                        // If we are a declaration element '<?' then we need
                        // to remove extra closing '?' if it exists
                        if (pElement->nIsDeclaration && 
                            (token.pStr[cbToken-1]) == _T('?')) {
                            cbToken--;
                        }

                        if (cbTemp) {
                            lpszToken = McbStrdup(token.pStr, cbToken);
                        } else {
                            lpszToken = NULL;
                        }

                        //Add the valued attribute to the list
                        McbAddAttribute(pElement, McbStrdup(lpszTemp, 
                                                            cbTemp), (LPTSTR)lpszToken, McbGROWBY);
#ifdef McbSTOREOFFSETS
                        pElement->pEntries[pElement->nSize-1].nStringOffset
                        = lpszTemp - pXML->lpXML;
#endif /* McbSTOREOFFSETS */
                        attrib = eAttribName;
                        break;
                        //Errors
                    case eTokenTagStart:        /* 'Attr = <'			*/
                    case eTokenTagEnd:          /* 'Attr = </'			*/
                    case eTokenCloseTag:        /* 'Attr = >'			*/
                    case eTokenShortHandClose:  /* "Attr = />"			*/
                    case eTokenEquals:          /* 'Attr = ='			*/
                    case eTokenDeclaration:     /* 'Attr = <?'			*/
                    case eTokenClear:
                        pXML->error = eXMLErrorUnexpectedToken;
                        return FALSE;
                        break;
                    }
                }
            }
        }
        /*
         *********************************************************************
         * If we failed to obtain the next token
         *********************************************************************
         */
        else {
            return TRUE;
            //return FALSE;   
        }
    }

}/* McbParseXMLElement */

/**
 ****************************************************************************
 * <P> Count the number of lines and columns in an XML string.  </P>
 *
 * @methodName  McbCountLinesAndColumns
 *
 * @param       lpXML		
 * @param       nUpto		
 * @param       *pResults		
 *
 * @return      void
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	21st August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
void McbCountLinesAndColumns(LPCTSTR lpXML, int nUpto, McbXMLResults *pResults)
{
    TCHAR ch;
    int n;

    //assert(lpXML);	
    ////assert(pResults);

    pResults->nLine = 1;
    pResults->nColumn = 1;

    for (n=0; n<nUpto; n++) {
        ch = lpXML[n];

        //assert(ch);

        if (ch == _T('\n')) {
            pResults->nLine++;
            pResults->nColumn = 1;
        } else {
            pResults->nColumn++;
        }
    }
}/* McbCountLinesAndColumns */

/**
 ****************************************************************************
 * <P> Obtain tags used for unformatted text within elements.  </P>
 *
 * @methodName  * McbGetClearTags
 *
 * @param       none
 *
 * @return      McbClearTag
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	3rd February  	2002	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbClearTag * McbGetClearTags()
{
    static struct McbClearTag tags[] =
    {
        {   _T("<![CDATA["),    _T("]]>")},
        {   _T("<PRE>"),        _T("</PRE>")},
        {   _T("<Script>"),     _T("</Script>")},
        {   _T("<!--"),         _T("-->")},
        {   _T("<!DOCTYPE"),    _T(">")},
        {   NULL,               NULL}
    };

    return tags;

}/* McbGetClearTags */

#define McbGETCDATA() &McbGetClearTags()[0]

/**
 ****************************************************************************
 * <P> Parse XML and return the root element.  </P>
 *
 * @methodName  * McbParseXML
 *
 * @param       lpszXML		
 * @param       *pResults		
 *
 * @return      McbXMLElement
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	20th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLElement * McbParseXML(LPCTSTR lpszXML, McbXMLResults *pResults)
{
    enum McbXMLError error;

    struct McbXMLElement * pElement = NULL;

    struct McbXML xml = 
    { NULL,0, 0, 0, NULL, 0, NULL, 0, TRUE};

    //如果是UTF-8编码，就跳过前3位
    if (memcmp(lpszXML,"/xef/xbb/xbf",0x03)==0) {
        xml.nIndex = 3;
        xml.Uncode = 1;
    }

    xml.lpXML = lpszXML;
    xml.pClrTags = McbGetClearTags();

    /*
     *************************************************************************
     * Create header element
     *************************************************************************
     */
    pElement = McbCreateRoot();//创建一个根列表
    McbParseXMLElement(&xml, pElement);
    error = xml.error;


    if (error != eXMLErrorNone) {
        McbDeleteRoot(pElement);
        pElement = NULL;
    }

    /*
     *************************************************************************
     * If we hae been given somewhere to place results
     *************************************************************************
     */
    if (pResults) {
        pResults->error = error;

        /*
         *********************************************************************
         * If we have an error
         *********************************************************************
         */
        if (error != eXMLErrorNone) {
            /*
             *****************************************************************
             * Find which line and column it starts on.
             *****************************************************************
             */
            McbCountLinesAndColumns(xml.lpXML, xml.nIndex, pResults);
        }

    }
    //free((char *)lpszXML);
    lpszXML = NULL;
    xml.lpXML = NULL;
    return pElement;

}/* McbParseXML */

/**
 ****************************************************************************
 * <P> Search for an element in the list.  </P>
 *
 * @methodName  * McbFindElement
 *
 * @param       lpszName
 *
 * @return      McbXMLElement
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	25th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLElement * McbFindElement(McbXMLElement *pEntry, LPCTSTR lpszPath)
{
    McbXMLElement * pChild;
    LPCTSTR lpszName;
    LPCTSTR lpszNext;
    int cbName;
    int nIndex;

    //assert(lpszPath);
    //assert(pEntry);

    /*
     *************************************************************************
     * Find the next name in the path in case we need to recurse
     *************************************************************************
     */
    lpszNext = _tcschr(lpszPath, _T('/'));

    /*
     *************************************************************************
     * Calculate the length of the current name we are searching for
     *************************************************************************
     */
      cbName = _tcslen(lpszPath);     
      #if 1
    if (!lpszNext) {
        cbName = _tcslen(lpszPath);     
    } else {
        cbName = lpszNext - lpszPath;

        if (lpszNext[1]) {
            lpszNext++;
        } else {
            lpszNext = NULL;
        }
    }
	#endif
    if (cbName) {
        /*
         *********************************************************************
         * Enumerate child elements
         *********************************************************************
         */
        nIndex = 0;
        while (pChild = McbEnumElements(pEntry, &nIndex)) {
            /*
             *****************************************************************
             * Obtain the name of the child element
             *****************************************************************
             */
            lpszName = pChild->lpszName;
			
            if (lpszName) {
                /*
                 *************************************************************
                 * If the name of the element is what we are looking for
                 *************************************************************
                 */
                if (_tcsnicmp(lpszPath, lpszName, cbName) == 0) {
                    /*
                     *********************************************************
                     * Check if this is the last element to search for
                     *********************************************************
                     */
                    if (!lpszNext) {
                        return pChild;
                    }
                    /*
                     *********************************************************
                     * If we have further nodes to find then recurse.
                     *********************************************************
                     */
                    else {
                        return McbFindElement(pChild, lpszNext);
                    }
                }
            }
        }
    }

    return NULL;

}/* McbFindElement */

/**
 ****************************************************************************
 * <P> Find an attribute on an element.  </P>
 *
 * @methodName  * McbFindAttribute
 *
 * @param       *pEntry		
 * @param       lpszAttrib		
 *
 * @return      McbXMLAttribute
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	25th August    	2001	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLAttribute * McbFindAttribute(McbXMLElement *pEntry, LPCTSTR lpszAttrib)
{
    McbXMLAttribute * pAttr;
    int cbAttrib;
    int nIndex;

    //assert(pEntry);
    //assert(lpszAttrib);

    cbAttrib = _tcslen(lpszAttrib);
    nIndex = 0;

    while (pAttr = McbEnumAttributes(pEntry, &nIndex)) {
        if (_tcsnicmp(pAttr->lpszName, lpszAttrib, cbAttrib) == 0) {
            return pAttr;
        }
    }

    return NULL;

}/* McbFindAttribute */

/**
 ****************************************************************************
 * <P> Add CData to the element.  </P>
 *
 * @methodName  * McbAddCData
 *
 * @param       *pEntry		
 * @param       lpszValue		
 * @param       nGrowBy		
 *
 * @return      McbXMLClear
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	3rd February  	2002	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLClear * McbAddCData(McbXMLElement *pEntry, LPTSTR lpszValue, int nGrowBy)
{
    return McbAddClear(pEntry, lpszValue, McbGETCDATA(), nGrowBy);

}/* McbAddCData */

/**
 ****************************************************************************
 * <P> Add elements to the list if they do not exist, return the final node.
 * </P>
 *
 * @methodName  * McbCreateElements
 *
 * @param       *pEntry		
 * @param       lpszPath		
 *
 * @return      McbXMLElement
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	29th January   	2002	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
McbXMLElement * McbCreateElements(McbXMLElement *pEntry, LPCTSTR lpszPath)
{
    McbXMLElement * pChild;
    McbXMLElement * pNew;
    LPCTSTR lpszName;
    LPCTSTR lpszNext;
    int cbName;
    int nIndex;

    //assert(lpszPath);
    //assert(pEntry);

    /*
     *************************************************************************
     * Find the next name in the path in case we need to recurse
     *************************************************************************
     */
    lpszNext = _tcschr(lpszPath, _T('/'));

    /*
     *************************************************************************
     * Calculate the length of the current name we are searching for
     *************************************************************************
     */
    if (!lpszNext) {
        cbName = _tcslen(lpszPath);     
    } else {
        cbName = lpszNext - lpszPath;

        if (lpszNext[1]) {
            lpszNext++;
        } else {
            lpszNext = NULL;
        }
    }

    if (cbName) {
        /*
         *********************************************************************
         * Enumerate child elements
         *********************************************************************
         */
        nIndex = 0;
        while (pChild = McbEnumElements(pEntry, &nIndex)) {
            /*
             *****************************************************************
             * Obtain the name of the child element
             *****************************************************************
             */
            lpszName = pChild->lpszName;

            if (lpszName) {
                /*
                 *************************************************************
                 * If the name of the element is what we are looking for
                 *************************************************************
                 */
                if (_tcsnicmp(lpszPath, lpszName, cbName) == 0) {
                    /*
                     *********************************************************
                     * Check if this is the last element to search for
                     *********************************************************
                     */
                    if (!lpszNext) {
                        return pChild;
                    }
                    /*
                     *********************************************************
                     * If we have further nodes to find then recurse.
                     *********************************************************
                     */
                    else {
                        return McbCreateElements(pChild, lpszNext);
                    }
                }
            }
        }

        /*
         *********************************************************************
         * If we got this far then we couldn't find the required element so we
         * need to add a new element to the current element
         *********************************************************************
         */
        pNew = McbAddElement(pEntry, McbStrdup(lpszPath, cbName), FALSE, 
                             McbGROWBY);

        /*
         *********************************************************************
         * If there are no more entries then return the node we just created.
         *********************************************************************
         */
        if (!lpszNext) {
            return pNew;
        }
        /*
         *********************************************************************
         * If there are more elements to search
         *********************************************************************
         */
        else {
            /*
             *****************************************************************
             * Recurse to add the remaining nodes
             *****************************************************************
             */
            return McbCreateElements(pNew, lpszNext);
        }
    }

    return NULL;

}/* McbCreateElements */

/**
 ****************************************************************************
 * <P> Creates an user friendly XML string from a given element with 
 * appropriate white space and carriage returns.
 *
 * This recurses through all subnodes then adds contents of the nodes to the
 * string.
 * </P>
 *
 * @methodName  McbCreateXMLStringR
 *
 * @param       McbXMLElement * pEntry - XML Element
 * @param       LPTSTR lpszMarker	  - String to create results into, this 
 *										can be zero if you want to calculate
 *										the size of the returned string.
 * @param       int nFormat			  -	Specify -1 for no formatting or the
 *										indent level (0 initially).
 * @return      int					  - Size of the returned string, not 
 *										including the NULL terminator.
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	31st January   	2002	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
int McbCreateXMLStringR(McbXMLElement * pEntry, LPTSTR lpszMarker, int nFormat)
{
    int nResult = 0;
    int cb;
    int cbElement;
    int nIndex;
    int nChildFormat;
    int bHasChildren = FALSE;
    McbXMLNode * pChild;
    McbXMLAttribute * pAttr;

    //assert(pEntry);

#define McbLENSTR(lpsz) (lpsz ? _tcslen(lpsz) : 0)

    /*
     *************************************************************************
     * If the element has no name then assume this is the head node.
     *************************************************************************
     */
    cbElement = McbLENSTR(pEntry->lpszName);

    if (cbElement) {
        /*
         *********************************************************************
         * "<elementname "
         *********************************************************************
         */
        cb = nFormat == -1 ? 0 : nFormat;

        if (lpszMarker) {
            if (cb) {
                memset(lpszMarker, McbINDENTCHAR, sizeof(TCHAR)*cb);
            }

            nResult = cb;

            lpszMarker[nResult++] = _T('<');        
            _tcscpy(&lpszMarker[nResult], pEntry->lpszName);
            nResult += cbElement;
            lpszMarker[nResult++] = _T(' ');        
        } else {
            nResult += cbElement + 2 + cb;
        }

        /*
         *********************************************************************
         * Enumerate attributes and add them to the string
         *********************************************************************
         */
        nIndex = 0;
        while (pChild = McbEnumNodes(pEntry, &nIndex)) {
            switch (pChild->type) {
            /*
             *****************************************************************
             * Add attributes
             *****************************************************************
             */
            case eNodeAttribute:
                {
                    pAttr = pChild->node.pAttrib;

                    /*
                     *********************************************************
                     * "Attrib
                     *********************************************************
                     */
                    cb = McbLENSTR(pAttr->lpszName);

                    if (cb) {
                        if (lpszMarker) {
                            _tcscpy(&lpszMarker[nResult], pAttr->lpszName);             
                        }

                        nResult += cb;

                        /*
                         *****************************************************
                         * "Attrib=Value "
                         *****************************************************
                         */
                        cb = McbLENSTR(pAttr->lpszValue);

                        if (cb) {
                            if (lpszMarker) {
                                lpszMarker[nResult] = _T('=');
                                _tcscpy(&lpszMarker[nResult+1], 
                                        pAttr->lpszValue);                  
                            }
                            nResult += cb + 1;
                        }

                        if (lpszMarker) {
                            lpszMarker[nResult] = _T(' ');                              
                        }

                        nResult++;              
                    }
                }
                break;

            case eNodeEmpty:
                continue;

            default:
                /*
                 *************************************************************
                 * If this node isn't an attribute then flag that this element 
                 * has children.
                 *************************************************************
                 */
                bHasChildren = TRUE;
            }
        }

        /*
         *********************************************************************
         * If there are child nodes we need to terminate the start tag
         *********************************************************************
         */
        if (bHasChildren) {
            if (lpszMarker) {
                lpszMarker[nResult-1] = _T('>');
            }

            if (nFormat != -1) {
                if (lpszMarker) {
                    lpszMarker[nResult] = _T('\n');
                }

                nResult++;
            }
        } else {
            nResult--;
        }
    }

    /*
     *************************************************************************
     * Calculate the child format for when we recurse.  This is used to 
     * determine the number of spaces used for prefixes.
     *************************************************************************
     */
    if (nFormat == -1) {
        nChildFormat = -1;
    } else {
        if (cbElement) {
            nChildFormat = nFormat + 1;
        } else {
            nChildFormat = nFormat;
        }   
    }

    /*
     *************************************************************************
     * Enumerate through remaining children
     *************************************************************************
     */
    nIndex = 0; 
    while (pChild = McbEnumNodes(pEntry, &nIndex)) {
        switch (pChild->type) {
        /*
         *********************************************************************
         * Text nodes
         *********************************************************************
         */
        case eNodeText:
            {
                /*
                 *************************************************************
                 * "Text"
                 *************************************************************
                 */
                cb = McbLENSTR(pChild->node.pText->lpszValue);

                if (cb) {
                    if (nFormat != -1) {
                        if (lpszMarker) {
                            memset(&lpszMarker[nResult], McbINDENTCHAR, 
                                   sizeof(TCHAR)*(nFormat + 1));                       

                            _tcscpy(&lpszMarker[nResult + nFormat + 1], 
                                    pChild->node.pText->lpszValue);

                            lpszMarker[nResult + nFormat + 1 + cb] = 
                            _T('\n');
                        }

                        nResult += cb + nFormat + 2;
                    } else {
                        if (lpszMarker) {
                            _tcscpy(&lpszMarker[nResult], 
                                    pChild->node.pText->lpszValue);
                        }

                        nResult += cb;
                    }               
                }
            }
            break;

            /*
             *********************************************************************
             * Clear type nodes
             *********************************************************************
             */
        case eNodeClear:
            {
                /*
                 *************************************************************
                 * "OpenTag"
                 *************************************************************
                 */
                cb = McbLENSTR(pChild->node.pClear->lpszOpenTag);

                if (cb) {
                    if (nFormat != -1) {
                        if (lpszMarker) {
                            memset(&lpszMarker[nResult], McbINDENTCHAR, 
                                   sizeof(TCHAR)*(nFormat + 1));                       

                            _tcscpy(&lpszMarker[nResult + nFormat + 1], 
                                    pChild->node.pClear->lpszOpenTag);

                            /* lpszMarker[nResult + nFormat + 1 + cb] = 
                                _T('\n'); */
                        }

                        /* nResult += cb + nFormat + 2; */
                        nResult += cb + nFormat + 1;

                    } else {
                        if (lpszMarker) {
                            _tcscpy(&lpszMarker[nResult], 
                                    pChild->node.pClear->lpszOpenTag);
                        }

                        nResult += cb;
                    }               
                }

                /*
                 *************************************************************
                 * "OpenTag Value"
                 *************************************************************
                 */
                cb = McbLENSTR(pChild->node.pClear->lpszValue);

                if (cb) {
                    if (lpszMarker) {
                        _tcscpy(&lpszMarker[nResult], 
                                pChild->node.pClear->lpszValue);
                    }

                    nResult += cb;
                }

                /*
                 *************************************************************
                 * "OpenTag Value CloseTag"
                 *************************************************************
                 */
                cb = McbLENSTR(pChild->node.pClear->lpszCloseTag);

                if (cb) {
                    if (lpszMarker) {
                        _tcscpy(&lpszMarker[nResult], 
                                pChild->node.pClear->lpszCloseTag);
                    }

                    nResult += cb;
                }

                if (nFormat != -1) {
                    if (lpszMarker) {
                        lpszMarker[nResult] = _T('\n');
                    }

                    nResult++;
                }
            }
            break;
            /*
             *********************************************************************
             * Element nodes
             *********************************************************************
             */
        case eNodeElement:
            {
                /*
                 *************************************************************
                 * Recursively add child nodes
                 *************************************************************
                 */
                nResult += McbCreateXMLStringR(pChild->node.pElement, 
                                               lpszMarker ? lpszMarker + nResult : 0, nChildFormat);
            }
        }
    }

    if (cbElement) {
        /*
         *********************************************************************
         * If we have child entries we need to use long XML notation for 
         * closing the element - "<elementname>blah blah blah</elementname>"
         *********************************************************************
         */
        if (bHasChildren) {
            /*
             *****************************************************************
             * "</elementname>\0"
             *****************************************************************
             */
            if (lpszMarker) {
                if (nFormat != -1) {
                    if (nFormat) {
                        memset(&lpszMarker[nResult], McbINDENTCHAR, 
                               sizeof(TCHAR)*nFormat);

                        nResult+=nFormat;
                    }
                }

                _tcscpy(&lpszMarker[nResult], _T("</"));        
                nResult += 2;
                _tcscpy(&lpszMarker[nResult], pEntry->lpszName);
                nResult += cbElement;

                if (nFormat == -1) {
                    _tcscpy(&lpszMarker[nResult], _T(">"));
                    nResult++;
                } else {
                    _tcscpy(&lpszMarker[nResult], _T(">\n"));
                    nResult += 2;
                }
            } else {
                if (nFormat != -1) {
                    nResult += cbElement + 4 + nFormat;
                } else {
                    nResult += cbElement + 3;
                }
            }       
        }
        /*
         *********************************************************************
         * If there are no children we can use shorthand XML notation - 
         * "<elementname/>"
         *********************************************************************
         */
        else {
            /*
             *****************************************************************
             * "/>\0"
             *****************************************************************
             */
            if (lpszMarker) {
                if (nFormat == -1) {
                    _tcscpy(&lpszMarker[nResult], _T("/>")); 
                    nResult += 2;
                } else {
                    _tcscpy(&lpszMarker[nResult], _T("/>\n")); 
                    nResult += 3;
                }
            } else {
                nResult += nFormat == -1 ? 2 : 3;
            }
        }
    }

    return nResult;

}/* McbCreateXMLStringR */

/**
 ****************************************************************************
 * <P> Create an XML string from the head element.  </P>
 *
 * @methodName  McbCreateXMLString
 *
 * @param       McbXMLElement * pHead -	head element
 * @param       int nFormat			 -  0 if no formatting is required 
 *										otherwise nonzero for formatted text
 *										with carriage returns and indentation.
 * @param       int *pnSize			 -	[out] pointer to the size of the 
 *										returned string not including the 
 *										NULL terminator.
 *
 * @return      LPTSTR				 -	Allocated XML string, you must free
 *										this with free().
 *
 * @exception   none
 *
 * @author      Martyn C Brown
 *
 * @changeHistory  
 *	31st January   	2002	 - 	(V1.0) Creation (MCB)
 ****************************************************************************
 */
LPTSTR McbCreateXMLString(McbXMLElement * pHead, int nFormat, int *pnSize)
{
    LPTSTR lpszResult = NULL;
    int cbStr;

    if (pHead) {
        /*
         *********************************************************************
         * Recursively Calculate the size of the XML string
         *********************************************************************
         */
        nFormat = nFormat ? 0 : -1;
        cbStr = McbCreateXMLStringR(pHead, 0, nFormat);
        //assert(cbStr);

        /*
         *********************************************************************
         * Alllocate memory for the XML string + the NULL terminator and
         * create the recursively XML string.
         *********************************************************************
         */
        lpszResult = (LPTSTR)malloc(cbStr+1);

        if (lpszResult) {
            McbCreateXMLStringR(pHead, lpszResult, nFormat);

            if (pnSize) *pnSize = cbStr;
        }
    }

    return lpszResult;

}/* McbCreateXMLString */

/*
 *********************************************************************
 * Add New Node To the List;
 * Toby add 2006/7/3/9:40
 *********************************************************************
*/
McbXMLElement* AddElementToXML(McbXMLElement *pParent,LPTSTR lpszText)
{
    NodeText_st *pTemp = NULL;
    McbXMLElement *pElement;
    //pTemp = (NodeText_st *)malloc(sizeof(NodeText_st));
    //got the mem?
    //if(!pTemp)
    //	return NULL;

#ifdef DYNAMIC_MEM
//	pTemp->lpszText = (LPTSTR)malloc((char)*strlen(lpszText));
#endif

    //strcpy(pTemp->lpszText,lpszText);

    //AddToList(&g_StrList,&(pTemp->List));

    //pElement = McbCreateElements(pParent,pTemp->lpszText);
    pElement = McbCreateElements(pParent,lpszText);
    return pElement;
}

/*
 *********************************************************************
 * Add New Attr Node To the List;
 * Toby add 2006/7/3/9:40
 *********************************************************************
*/
McbXMLAttribute* AddAttrToXML(McbXMLElement *pEntry,LPTSTR lpszName,LPTSTR lpszValue)
{
    McbXMLAttribute *pAttr = NULL;
    if (!pEntry)
        return NULL;
    pAttr = McbAddAttribute(pEntry,
                            McbStrdup(lpszName,0),
                            McbStrdupstrflg(lpszValue,0),//添加 02 gancj
                            1);
    return pAttr;
}
/*添加属性到标签*/
McbXMLText* AddTextToXML(McbXMLElement *pEntry,LPTSTR lpszName)
{
    McbXMLText *pText = NULL;
    if (!pEntry)
        return NULL;
    pText = McbAddText(pEntry,
                            McbStrdup(lpszName,0),
                            1);
    return pText;
}



/*
 *********************************************************************
 * Delete Node From the List;
 * Toby add 2006/7/3/9:53
 *********************************************************************
*/
void DeleteElementFromXML(McbXMLElement *pEntry)
{
    NodeText_st *pTemp = NULL;
    if (!pEntry)
        return;

    pTemp = ListEntry(pEntry->lpszName,NodeText_st,lpszText);
    DeleteFromList(&(pTemp->List));
    free(pTemp->lpszText);
    free(pTemp);

    //delete from the list...add Later
    McbDeleteElement(pEntry);
}

/*
 *********************************************************************
 * Delete Node From the List;
 * Toby add 2006/7/3/10:01
 *********************************************************************
*/

void DeleteAttrFromXML(McbXMLAttribute *pEntry)
{
    NodeText_st *pTemp = NULL;

    pTemp = ListEntry(pEntry->lpszName,NodeText_st,lpszText);
    DeleteFromList(&(pTemp->List));
    free(pTemp->lpszText);
    free(pTemp);

    pTemp = ListEntry(pEntry->lpszValue,NodeText_st,lpszText);
    DeleteFromList(&(pTemp->List));
    free(pTemp->lpszText);
    free(pTemp);

    if (!pEntry)
        return;
    //delete from the list... add later
    McbDeleteAttribute(pEntry);
}
