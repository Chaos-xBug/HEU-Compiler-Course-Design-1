#include "RemoveLeftRecursion.h"
#include <string.h>
#include <stdlib.h>

const char* VoidSymbol = "$"; // "��"
const char* Postfix = "'";

char rule_table_ci[20][256];
char ruleNameArr[20][64];

int main(int argc, char* argv[])
{
	//
	// ���� InitRules ������ʼ���ķ�
	//
	
#ifdef CODECODE_CI
	Rule* pHead = InitRules_CI();  	// ���д�����������ˮ������
#else
	Rule* pHead = InitRules();		// ���д����� CP Lab ������
#endif

	//
	// ���������ݹ�֮ǰ���ķ�
	//
	printf("Before Remove Left Recursion:\n");
	PrintRule(pHead);

	//
	// ���� RemoveLeftRecursion ���������ķ��е���ݹ�
	//
	RemoveLeftRecursion(pHead);
	
	//
	// ���������ݹ�֮����ķ�
	//
	printf("\nAfter Remove Left Recursion:\n");
	PrintRule(pHead);
	
	FreeRule(pHead);
	return 0;
}

/*
���ܣ�
	�жϵ�ǰ Rule �е�һ�� Symbol �Ƿ���Ҫ���滻��
	��� Symbol ��һ�����ս������ Symbol ��Ӧ��
	Rule �ڵ�ǰ Rule ֮ǰ������Ҫ���滻��

������
	pCurRule -- ��ǰ Rule ��ָ�롣
	pSymbol -- Symbol ָ�롣
	
����ֵ��
	��Ҫ�滻���� 1��
	����Ҫ�滻���� 0��
*/
int SymbolNeedReplace(const Rule* pHead, const Rule* pCurRule, const RuleSymbol* pSymbol)
{
	if(pSymbol->isToken==1) return 0;
	Rule* pTmpRule=pHead;
	while(pTmpRule!=pCurRule){
		if(pTmpRule==pSymbol->pRule) return 1;
		pTmpRule=pTmpRule->pNextRule;
	}
	return 0;
}

/*
���ܣ�
	����һ�� Symbol��

������
	pSymbolTemplate -- ��Ҫ�������� Symbol ָ�롣
	  
����ֵ��
	������õ��� Symbol ��ָ�롣
*/
RuleSymbol* CopySymbol(const RuleSymbol* pSymbolTemplate)
{
	RuleSymbol* pNewSymbol=CreateSymbol();
	pNewSymbol->pNextSymbol=pSymbolTemplate->pNextSymbol;
	pNewSymbol->pOther=pSymbolTemplate->pOther;
	pNewSymbol->isToken=pSymbolTemplate->isToken;
	strcpy(pNewSymbol->TokenName,pSymbolTemplate->TokenName);
	pNewSymbol->pRule=pSymbolTemplate->pRule;
	return pNewSymbol;
}

/*
���ܣ�
	����һ�� Select��

������
	pSelectTemplate -- ��Ҫ�������� Select ָ�롣
	  
����ֵ��
	������õ��� Select ��ָ�롣
*/
RuleSymbol* CopySelect(const RuleSymbol* pSelectTemplate)
{
	RuleSymbol* pNewSymbol=CopySymbol(pSelectTemplate);
	RuleSymbol* pCurSymbol=pNewSymbol;
	while(pCurSymbol->pNextSymbol){
		pCurSymbol->pNextSymbol=CopySymbol(pCurSymbol->pNextSymbol);
		pCurSymbol=pCurSymbol->pNextSymbol;
	}
	return pNewSymbol;
}

/*
���ܣ�
	�滻һ�� Select �ĵ�һ�� Symbol��

������
	pSelectTemplate -- ��Ҫ���滻�� Select ָ�롣
	  
����ֵ��
	�滻���õ��� Select ��ָ�롣
	ע�⣬�滻����ܻ���һ���µ� Select��
	Ҳ���ܻ��ж�� Select ������һ��
*/
RuleSymbol* ReplaceSelect(const RuleSymbol* pSelectTemplate)
{
	RuleSymbol* pNewSelect=CopySelect(pSelectTemplate->pRule->pFirstSymbol);
	RuleSymbol* pCurSelect=pNewSelect;
	while(pCurSelect->pOther){
		pCurSelect->pOther=CopySelect(pCurSelect->pOther);
		RuleSymbol* pRestSelect=CopySelect(pSelectTemplate->pNextSymbol);
		AddSymbolToSelect(pCurSelect,pRestSelect);
		pCurSelect=pCurSelect->pOther;
	}
	RuleSymbol* pRestSelect=CopySelect(pSelectTemplate->pNextSymbol);
	AddSymbolToSelect(pCurSelect,pRestSelect);
	pCurSelect->pOther=pSelectTemplate->pOther;
	return pNewSelect;
}

/*
���ܣ�
	�ͷ�һ�� Select ���ڴ档

������
	pSelect -- ��Ҫ�ͷŵ� Select ��ָ�롣
*/
void FreeSelect(RuleSymbol* pSelect)
{
	if(pSelect==NULL) return ;
    FreeSelect(pSelect->pNextSymbol);
	free(pSelect);
    return ;
}

/*
���ܣ�
	�ͷ����� Rule ���ڴ档

������
	pRule -- ���� Rule ��ͷָ�롣
*/
void FreeRule(Rule* pRule)
{
	if(pRule==NULL) return ;
    FreeRule(pRule->pNextRule);
	RuleSymbol *pCurSelect=pRule->pFirstSymbol,*pNxtSelect;
	while(pCurSelect){
		pNxtSelect=pCurSelect->pOther;
		FreeSelect(pCurSelect);
		pCurSelect=pNxtSelect;
	}
	free(pRule);
    return ;
}

/*
���ܣ�
	�ж�һ�� Rule �Ƿ������ݹ顣

������
	prRule -- Rule ָ�롣
	  
����ֵ��
	���ڷ��� 1��
	�����ڷ��� 0��
*/
int RuleHasLeftRecursion(Rule* pRule)
{

	RuleSymbol *pSelect=pRule->pFirstSymbol;
	while(pSelect){
		if(pSelect->isToken==0&&pSelect->pRule==pRule) return 1;
		pSelect=pSelect->pOther;
	}
	return 0;
}

/*
���ܣ�
	��һ�� Symbol ��ӵ� Select ��ĩβ��

������
	pSelect -- Select ָ�롣
	pNewSymbol -- Symbol ָ�롣
*/
void AddSymbolToSelect(RuleSymbol* pSelect, RuleSymbol* pNewSymbol)
{
	while(pSelect->pNextSymbol!=NULL) pSelect=pSelect->pNextSymbol;
	pSelect->pNextSymbol=pNewSymbol;
	return ;
}

/*
���ܣ�
	��һ�� Select ���뵽�ķ�ĩβ���� Select Ϊ NULL ʱ�ͽ�һ�����ս�����뵽�ķ�ĩβ��

������
	pRule -- �ķ�ָ�롣
	pNewSelect -- Select ָ�롣
*/
void AddSelectToRule(Rule* pRule, RuleSymbol* pNewSelect)
{

	RuleSymbol** pTmpSelect=&(pRule->pFirstSymbol);
    while((*pTmpSelect)!=NULL) pTmpSelect=&((*pTmpSelect)->pOther);
    (*pTmpSelect)=pNewSelect;
    return ;
}

/*
���ܣ�
	������ݹ顣

������
	pHead -- �ķ������ͷָ�롣
*/
void RemoveLeftRecursion(Rule* pHead)
{
	Rule* pRule;				// Rule �α�
	RuleSymbol* pSelect; 		// Select �α�
	Rule* pNewRule;			  	// Rule ָ��
	int isChange;				// Rule �Ƿ��滻�ı��
	RuleSymbol **pSelectPrePtr; // Symbol ָ���ָ��
	
	for(pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
	{
		//
		// �滻
		//
		do
		{
			isChange = 0;

			// �� Rule ������ Select �в����Ƿ���Ҫ�滻
			for(pSelect = pRule->pFirstSymbol, pSelectPrePtr = &pRule->pFirstSymbol;
				pSelect != NULL;
				pSelectPrePtr = &pSelect->pOther, pSelect = pSelect->pOther)
			{
				if(SymbolNeedReplace(pHead, pRule, pSelect)) // �ж� Select �ĵ�һ�� Symbol �Ƿ���Ҫ�滻
				{
					isChange = 1;

					// ���� ReplaceSelect �������滻 Select �ĵ�һ�� Symbol ��õ��µ� Selects
					RuleSymbol* pNewSelects = ReplaceSelect(pSelect);

					// ʹ���µ� Selects �滻ԭ�е� Select�������� FreeSelect �����ͷ�ԭ�е� Select �ڴ�
					*pSelectPrePtr=pNewSelects;			
					FreeSelect(pSelect);
					break;
				}

				if(isChange)
					break;
			}
		}while(isChange);

		// ����û����ݹ�� Rule;
		if(!RuleHasLeftRecursion(pRule))
			continue;

		//
		// ������ݹ�
		//
		pNewRule = CreateRule(pRule->RuleName); // ������ Rule
		strcat(pNewRule->RuleName, Postfix);

		pSelect = pRule->pFirstSymbol; // ��ʼ�� Select �α�
		pSelectPrePtr = &pRule->pFirstSymbol;
		while(pSelect != NULL) // ѭ���������е� Select
		{
			if(0 == pSelect->isToken && pSelect->pRule == pRule) // Select ������ݹ�
			{
				// �Ƴ�������ݹ�� Select������ת��Ϊ�ҵݹ����ӵ��� Rule ��ĩβ�����ƶ��α�
				
				RuleSymbol* pNxtSymbol=pSelect->pNextSymbol;
				RuleSymbol* pNewSymbol=CreateSymbol();
				pNewSymbol->isToken=0;
				pNewSymbol->pRule=pNewRule;
				AddSymbolToSelect(pNxtSymbol,pNewSymbol);
				AddSelectToRule(pNewRule,pNxtSymbol);
				RuleSymbol* pFreeSymbol=pSelect;
				pSelect=pSelect->pOther;
				(*pSelectPrePtr)=pSelect;
				free(pFreeSymbol);
				
			}
			else // Select ��������ݹ�
			{
				// ��û����ݹ�� Select ĩβ���ָ���� Rule �ķ��ս�������ƶ��α�
				
				RuleSymbol* pNewSymbol=CreateSymbol();
				pNewSymbol->isToken=0;
				pNewSymbol->pRule=pNewRule;
				AddSymbolToSelect(pSelect,pNewSymbol);
				pSelectPrePtr=&pSelect->pOther;
				pSelect=pSelect->pOther;
			}
		}

		RuleSymbol *pNewSymbol=CreateSymbol();
		pNewSymbol->isToken=1;
		strcpy(pNewSymbol->TokenName,VoidSymbol);
		AddSelectToRule(pNewRule,pNewSymbol);
		pNewRule->pNextRule=pRule->pNextRule;
		pRule->pNextRule=pNewRule;
		pRule = pNewRule;
	}
}

/*
���ܣ�
	ʹ�ø��������ݳ�ʼ���ķ�����

����ֵ��
	Rule ָ��
*/
typedef struct _SYMBOL
{
	int isToken;
	char Name[MAX_STR_LENGTH];
}SYMBOL;

typedef struct _RULE_ENTRY
{
	char RuleName[MAX_STR_LENGTH];
	SYMBOL Selects[64][64];
}RULE_ENTRY;

static const RULE_ENTRY rule_table[] =
{
	/* A -> Ba | Aa | c */
	{ "A", 
			{
				{ { 0, "B" }, { 1, "a"} },
				{ { 0, "A" }, { 1, "a"} },
				{ { 1, "c" } }
			}	
	},

	/* B -> Bb | Ab | d */
	{ "B", 
			{
				{ { 0, "B" }, { 1, "b"} },
				{ { 0, "A" }, { 1, "b"} },
				{ { 1, "d" } }
			}	
	}
};

/*
���ܣ�
	��ʼ���ķ�����
	
����ֵ��
	�ķ���ͷָ�롣
*/
Rule* InitRules()
{
	Rule *pHead, *pRule;
	RuleSymbol **pSymbolPtr1, **pSymbolPtr2;
	int nRuleCount = sizeof(rule_table) / sizeof(rule_table[0]);
	int i, j, k;

	Rule** pRulePtr = &pHead;
	for (i=0; i<nRuleCount; i++)
	{
		*pRulePtr = CreateRule(rule_table[i].RuleName);
		pRulePtr = &(*pRulePtr)->pNextRule;
	}

	pRule = pHead;
	for (i=0; i<nRuleCount; i++)
	{
		pSymbolPtr1 = &pRule->pFirstSymbol;
		for (j=0; rule_table[i].Selects[j][0].Name[0] != '\0'; j++)
		{
			pSymbolPtr2 = pSymbolPtr1;
			for (k=0; rule_table[i].Selects[j][k].Name[0] != '\0'; k++)
			{
				const SYMBOL* pSymbol = &rule_table[i].Selects[j][k];

				*pSymbolPtr2 = CreateSymbol();
				(*pSymbolPtr2)->isToken = pSymbol->isToken;
				if (1 == pSymbol->isToken)
				{
					strcpy((*pSymbolPtr2)->TokenName, pSymbol->Name);
				}
				else
				{
					(*pSymbolPtr2)->pRule = FindRule(pHead, pSymbol->Name);
					if (NULL == (*pSymbolPtr2)->pRule)
					{
						printf("Init rules error, miss rule \"%s\"\n", pSymbol->Name);
						exit(1);
					}
				}
				
				pSymbolPtr2 = &(*pSymbolPtr2)->pNextSymbol;
			}
			
			pSymbolPtr1 = &(*pSymbolPtr1)->pOther;
		}

		pRule = pRule->pNextRule;
	}

	return pHead;
}

/*
���ܣ�
	��ʼ���ķ�����(��ִ����ˮ��ʱ����)��
	
����ֵ��
	�ķ���ͷָ�롣
*/
Rule* InitRules_CI()
{
	int nRuleCount = 0;
	for (int i = 0; i < 20; i++)
	{
		gets(rule_table_ci[i]);	
		int length = strlen(rule_table_ci[i]);
		if (length == 0)
		{
			break;
		}
		
		for (int j = 0; j < length; j++)
		{
			if (rule_table_ci[i][j] == ' ')
			{
				ruleNameArr[i][j] = '\0';
				break;
			}
			ruleNameArr[i][j]= rule_table_ci[i][j];
		}	  
		
		nRuleCount++;
	}
			
	Rule *pHead, *pRule;
	RuleSymbol **pSymbolPtr1, **pSymbolPtr2;
		
	int i, j, k;

	Rule** pRulePtr = &pHead;
	for (i=0; i<nRuleCount; i++)
	{
		*pRulePtr = CreateRule(ruleNameArr[i]);
		pRulePtr = &(*pRulePtr)->pNextRule;
	}

	pRule = pHead;
	for (i=0; i<nRuleCount; i++)
	{
		pSymbolPtr1 = &pRule->pFirstSymbol;
		
		int start = 0;
		for (int j=0; rule_table_ci[i][j] != '\0'; j++)
		{
			if (rule_table_ci[i][j] == ' '
			 && rule_table_ci[i][j + 1] == '-'
			&& rule_table_ci[i][j + 2] == '>' 
			&& rule_table_ci[i][j + 3] == ' ')
			{
				start = j + 4;
				break;
			}
		}
			
		for (k = start; rule_table_ci[i][k] != '\0'; k++)
		{
			if (rule_table_ci[i][k] == '|')
			{
				pSymbolPtr1 = &(*pSymbolPtr1)->pOther;
				pSymbolPtr2 = pSymbolPtr1;
				continue;
			}
			if (rule_table_ci[i][k] == ' ')
			{
				continue;
			}
			if (k == start)
			{
				pSymbolPtr2 = pSymbolPtr1;
			}

			*pSymbolPtr2 = CreateSymbol();
			
			char tokenName[MAX_STR_LENGTH] = {};
			tokenName[0] = rule_table_ci[i][k];
			tokenName[1] = '\0';
			(*pSymbolPtr2)->isToken = 1;
			for (int m = 0; m < nRuleCount; m++)
			{
				if (strcmp(tokenName, ruleNameArr[m]) == 0)
				{
					(*pSymbolPtr2)->isToken = 0;
					(*pSymbolPtr2)->pRule = FindRule(pHead, tokenName);
					if (NULL == (*pSymbolPtr2)->pRule)
					{
						printf("Init rules error, miss rule \"%s\"\n", tokenName);
						exit(1);
					}
				}
			}
			if ((*pSymbolPtr2)->isToken == 1)
			{
				strcpy((*pSymbolPtr2)->TokenName, tokenName);
			}
			
			pSymbolPtr2 = &(*pSymbolPtr2)->pNextSymbol;
			
		}
			
		pRule = pRule->pNextRule;
	}

	return pHead;
}

/*
���ܣ�
	����һ���µ� Rule��

������
	pRuleName -- �ķ������֡�
	
����ֵ��
	Rule ָ��
*/
Rule* CreateRule(const char* pRuleName)
{
	Rule* pRule = (Rule*)malloc(sizeof(Rule));

	strcpy(pRule->RuleName, pRuleName);
	pRule->pFirstSymbol = NULL;
	pRule->pNextRule = NULL;

	return pRule;
}

/*
���ܣ�
	����һ���µ� Symbol��
	
����ֵ��
	RuleSymbol ָ��	
*/
RuleSymbol* CreateSymbol()
{
	RuleSymbol* pSymbol = (RuleSymbol*)malloc(sizeof(RuleSymbol));

	pSymbol->pNextSymbol = NULL;
	pSymbol->pOther = NULL;
	pSymbol->isToken = -1;
	pSymbol->TokenName[0] = '\0';
	pSymbol->pRule = NULL;

	return pSymbol;
}

/*
���ܣ�
	���� RuleName ���ķ������в���������ͬ���ķ���

������
	pHead -- �ķ���ͷָ�롣
	RuleName -- �ķ������֡�
	
����ֵ��
	Rule ָ��	
*/
Rule* FindRule(Rule* pHead, const char* RuleName)
{
	Rule* pRule;
	for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
	{
		if (0 == strcmp(pRule->RuleName, RuleName))
		{
			break;
		}
	}
	
	return pRule;
}	

/*
���ܣ�
	����ķ���

������
	pHead -- �ķ���ͷָ�롣
*/
void PrintRule(Rule* pHead)
{
	
	Rule* pTmpRule=pHead;
    while(pTmpRule!=NULL){
        printf("%s->",pTmpRule->RuleName);
        RuleSymbol *pTmpSelect=pTmpRule->pFirstSymbol;
        while(pTmpSelect!=NULL){
            RuleSymbol *pTmpSymbol=pTmpSelect;
            while(pTmpSymbol!=NULL){
                printf("%s",pTmpSymbol->isToken?pTmpSymbol->TokenName:pTmpSymbol->pRule->RuleName);
                pTmpSymbol=pTmpSymbol->pNextSymbol;
            }
            pTmpSelect=pTmpSelect->pOther;
            if(pTmpSelect) printf("%s","|");
        }
        printf("\n");
        pTmpRule=pTmpRule->pNextRule;
    }
	return ;
}