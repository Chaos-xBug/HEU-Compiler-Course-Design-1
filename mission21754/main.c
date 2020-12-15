#include "NFAToDFA.h"
#include "RegexpToPost.h"
#include "PostToNFA.h"
#include "NFAStateStack.h"
#include "NFAFragmentStack.h"
#include "OutputResult.h"
#include <stdlib.h>
#include <string.h>

NFAFragmentStack FragmentStack; // ջ�����ڴ��� NFA Ƭ��
NFAStateStack StateStack;		// ջ�����ڴ��� NFA ״̬

const char VoidTrans = '$'; // ��ʾ��ת��


//char* regexp = "a(a|1)*";			 // �� 1
// char* regexp = "(aa|b)*a(a|bb)*"; // �� 2
 char* regexp = "(a|b)*a(a|b)?"; 	 // �� 3

char regexp_ci[256];

int main(int argc, char **argv)
{
	char *post;
	DFA* dfa = (DFA*)malloc(sizeof(DFA));
	dfa->length = 0;
	
	//
	// ��ʼ��ջ
	//
	InitNFAFragmentStack(&FragmentStack);
	
	// �� CP Lab��ִ�г���ʱ����ʹ������궨������Ĵ��룬�ύ��ҵ��������������ˮ��ʱ������궨���ⲿ�ִ��롣
	// ����궨��������������ˮ��ʱ��input.txt�ļ��ж�ȡ������ʽ��������������֤��
	// ���У�input1.txt �� input3.txt�ļ��а�����������ʽ����1 �� ��3��������ʽ�Ƕ�Ӧ�ġ�	
#ifdef CODECODE_CI
	scanf("%255s", regexp_ci);
	regexp = regexp_ci;  	 
#endif	
	
	//
	// ���� re2post ������������ʽ�ַ���ת���ɽ������ĺ����������
	//
	post = re2post(regexp);
	
	//
	// ���� post2dfa �������������ĺ����������ת��Ϊ DFA 
	//
	dfa = post2dfa(dfa, post);
	
	//
	// �� DFA ��ӡ���
	//
	OutputResult(dfa);

	FreeDFA(dfa);
	FreeNFA();
	return 0;
}

/*
���ܣ�
	����һ�� DFA ״̬��ת����
	
������
	TransformChar -- ת�����š�
	NFAStateArray -- NFA ״ָ̬�����顣
	Count -- ����Ԫ�ظ�����	
	  
����ֵ��
	 Transform �ṹ��ָ�롣
*/
Transform* CreateDFATransform(char TransformChar, NFAState** NFAStateArray, int Count)
{
	int i;
	Transform* pTransform = (Transform*)malloc(sizeof(Transform));
	
	for (i=0; i<Count; i++)
	{
		pTransform->NFAlist[i] = NFAStateArray[i];
	}
	
	pTransform->NFAStateCount = Count;	
	pTransform->TransformChar = TransformChar;
	pTransform->DFAStateIndex = -1;
	pTransform->NextTrans = NULL;
	
	return pTransform;
}

/*
���ܣ�
	����һ�� DFA ״̬��
	
������
	pTransform -- DFA ״̬ת��ָ�롣	
	  
����ֵ��
	 DFAState �ṹ��ָ�롣
*/
DFAState* CreateDFAState(Transform* pTransform)
{
	int i;
	DFAState* pDFAState = (DFAState*)malloc(sizeof(DFAState));
	
	for (i=0; i<pTransform->NFAStateCount; i++)
	{
		pDFAState->NFAlist[i] = pTransform->NFAlist[i];
	}

	pDFAState->NFAStateCount = pTransform->NFAStateCount;
	pDFAState->firstTran = NULL;

	return pDFAState;
}

/*
���ܣ�
	�ж�һ��ת���е� NFA ״̬�����Ƿ�Ϊĳһ�� DFA ״̬�� NFA ״̬���ϵ��Ӽ���
	
������
	pDFA -- DFA ָ�롣
	pTransform -- DFA ״̬ת��ָ�롣	
	  
����ֵ��
	 ������ڷ��� DFA ״̬�±꣬�����ڷ��� -1��
*/
int NFAStateIsSubset(DFA* pDFA, Transform* pTransform)
{
	for(int i=0;i<pDFA->length;i++){
		int pTmpCount=0;
		DFAState *pTmpDFA=pDFA->DFAlist[i];
		for(int k=0;k<pTransform->NFAStateCount;k++)
			for(int j=0;j<pTmpDFA->NFAStateCount;j++){
				if(pTmpDFA->NFAlist[j]==pTransform->NFAlist[k]){
					++pTmpCount;
					break;
				}
		}
		if(pTmpCount==pTransform->NFAStateCount) return i;
	}
	return -1;
}

/*
���ܣ�
	�ж�ĳ�� DFA ״̬��ת���������Ƿ��Ѿ�����һ���ַ���ת����
	
������
	pDFAState -- DFAState ָ�롣
	TransformChar -- ת����ʶ����
	  
����ֵ��
	 Transform �ṹ��ָ�롣
*/
Transform* IsTransformExist(DFAState* pDFAState, char TransformChar)
{
	Transform *pTmpTransform=pDFAState->firstTran;
	while(pTmpTransform){
		if(pTmpTransform->TransformChar==TransformChar) return pTmpTransform;  
		pTmpTransform=pTmpTransform->NextTrans;
	}
	return NULL;
}

/*
���ܣ�
	��һ�� NFA ���Ϻϲ���һ�� DFA ת���е� NFA �����С�
	ע�⣬�ϲ���� NFA �����в�Ӧ���ظ��� NFA ״̬��
	
������
	NFAStateArray -- NFA ״ָ̬�����飬��������� NFA ���ϡ�
	Count -- ������� NFA ������Ԫ�ظ�����
	pTransform -- ת��ָ�롣
*/
void AddNFAStateArrayToTransform(NFAState** NFAStateArray, int Count, Transform* pTransform)
{
	for(int i=0;i<Count;i++){
		int Exist=0;
		for(int j=0;j<pTransform->NFAStateCount;j++)
			if(pTransform->NFAlist[j]==NFAStateArray[i]){
				Exist=1;
				break;
			}
		if(!Exist) pTransform->NFAlist[pTransform->NFAStateCount++]=NFAStateArray[i];
	}
	return ;
}

/*
���ܣ�
	ʹ�ö���������������㷨��һ�� NFA ״̬�Ħ�-�հ���
	
������
	State -- NFA ״ָ̬�롣�Ӵ� NFA ״̬��ʼ���-�հ���
	StateArray -- NFA ״ָ̬�����顣���ڷ��ئ�-�հ���
	Count -- Ԫ�ظ�����	���ڷ��ئ�-�հ��� NFA ״̬�ĸ�����
*/
void DFS(NFAState* State, NFAState** StateArray, int* Count){
	StateArray[(*Count)++]=State;
	if(State->Transform!=VoidTrans) return ;
	if(State->Next1) DFS(State->Next1,StateArray,Count);
	if(State->Next2) DFS(State->Next2,StateArray,Count);
}
void Closure(NFAState* State, NFAState** StateArray, int* Count)
{
	DFS(State,StateArray,Count);
}

/*
���ܣ�
	���������ĺ����������ת��Ϊ DFA��

������
	pDFA -- DFA ָ�롣
	postfix -- ������ʽ�Ľ���������������С�
	  
����ֵ��
	DFA ָ�롣
*/
NFAState* Start = NULL;
DFA* post2dfa(DFA* pDFA, char *postfix)
{
	int i, j;								// �α�
	Transform* pTFCursor;  					// ת��ָ��
	NFAState* NFAStateArray[MAX_STATE_NUM]; // NFA ״ָ̬�����顣���ڱ����-�հ�
	int Count = 0;							// ��-�հ���Ԫ�ظ���
	
	//
	// ���� post2nfa �������������ĺ����������ת��Ϊ NFA �����ؿ�ʼ״̬
	//
	Start = post2nfa(postfix);
	
	//
	// ���� Closure �������쿪ʼ״̬�Ħ�-�հ�
	//
	Closure(Start, NFAStateArray, &Count);

	// ���� CreateDFATransform ��������һ��ת��(����ת���ַ�)��
	// Ȼ�󣬵��� CreateDFAState ���������øոմ�����ת���½�һ�� DFA ״̬
	Transform* pTransform = CreateDFATransform('\0', NFAStateArray, Count);
	DFAState* pDFAState = CreateDFAState(pTransform);

	// �� DFA ״̬���뵽 DFA ״̬���Ա���
	pDFA->DFAlist[pDFA->length++] = pDFAState;

	// �������Ա������� DFA ״̬
	for(i=0; i<pDFA->length; i++)
	{
		// ������ i �� DFA ״̬�е����� NFA ״̬
		for(j=0; j<pDFA->DFAlist[i]->NFAStateCount; j++)
		{
			NFAState* NFAStateTemp = pDFA->DFAlist[i]->NFAlist[j];

			// ��� NFA ״̬�ǽ���״̬����ת���ǿ�ת������������ NFA ״̬
			if(NFAStateTemp->Transform == VoidTrans || NFAStateTemp->AcceptFlag == 1)
				continue;

			// ���� Closure �������� NFA ״̬�Ħ�-�հ�
			Count=0;
			Closure(NFAStateTemp->Next1, NFAStateArray, &Count);

			// ���� IsTransfromExist �����жϵ�ǰ DFA ״̬��ת���������Ƿ��Ѿ����ڸ� NFA ״̬��ת��
			pTransform = IsTransformExist(pDFA->DFAlist[i], NFAStateTemp->Transform);
			if(pTransform == NULL)
			{
				// �����ڣ����� CreateDFATransform ��������һ��ת�����������ת�����뵽ת������Ŀ�ʼλ��
				pTransform=CreateDFATransform(NFAStateTemp->Transform,NFAStateArray,Count);
				pTransform->NextTrans=pDFA->DFAlist[i]->firstTran;
				pDFA->DFAlist[i]->firstTran=pTransform;
			}
			else
			{
				//���ڣ����� AddNFAStateArrayToTransform �����Ѧ�-�հ��ϲ����Ѵ��ڵ�ת����
				AddNFAStateArrayToTransform(NFAStateArray,Count,pTransform);
			}
		}

		// ���� DFA ״̬��ת����������ÿ��ת��������Ӧ�� DFA ״̬
		for(pTFCursor = pDFA->DFAlist[i]->firstTran; pTFCursor != NULL; pTFCursor = pTFCursor->NextTrans)
		{
			// ���� NFAStateIsSubset �����ж�ת���е� NFA ״̬�����Ƿ�Ϊĳһ�� DFA ״̬�� NFA ״̬���ϵ��Ӽ�
			int Index = NFAStateIsSubset(pDFA, pTFCursor);
			if(Index == -1)
			{
				// �����Ӽ������� CreateDFAState ��������һ���µ� DFA ״̬������ DFA ���Ա���
				// ��ת���� DFAStateIndex ��ֵΪ�¼���� DFA ״̬���±�
				pDFAState=CreateDFAState(pTFCursor);
				pTFCursor->DFAStateIndex=pDFA->length;
				pDFA->DFAlist[pDFA->length++] = pDFAState;
				
			}
			else
			{
				// ���Ӽ�����ת���� DFAStateIndex ��ֵΪ Index
				pTFCursor->DFAStateIndex=Index;
			}
		}
	}

	return pDFA;
}

void FreeDFA(DFA* pDFA){
	for(int i=0;i<pDFA->length;i++){
		Transform* nxt;
		for(Transform* j=pDFA->DFAlist[i]->firstTran;j;j=nxt){
			nxt=j->NextTrans;
			free(j);
		}
		free(pDFA->DFAlist[i]);
	}
	free(pDFA);
}