#include "StdAfx.h"
#include "DemoTester.h"
#include "CodeTester.h"

TreeNodeData g_DemoUnitTestItem[] = 
{
	{1, L"Demoģ��"}
		,{2, L"Demo_Test1",	(TesterFun)NULL}
		,{2, L"Demo_Test2",	(TesterFun)NULL}
		,{2, L"Demo_Test3",	(TesterFun)NULL}
};

CDemoTester g_DemoTester;

CDemoTester::CDemoTester(void)
{
	//���ӵ�Ԫ������///////////////////////////////////////////
	g_TestCaseTreeData.AddUnitTreeNodeData(g_DemoUnitTestItem, sizeof(g_DemoUnitTestItem)/sizeof(TreeNodeData));
	g_TestCaseTreeData.AddIntegTreeNodeData(g_DemoUnitTestItem, sizeof(g_DemoUnitTestItem)/sizeof(TreeNodeData));

}


CDemoTester::~CDemoTester(void)
{
}
