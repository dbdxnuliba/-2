#ifndef RSI_TOOL_H
#define RSI_TOOL_H

typedef struct sta_tool_{
	void **sta;				// 栈底指针
	void **top;				// 栈顶指针
	int size;				// 当前存放的个数
	int maxsize;			// 当前栈大小
}*sta_tool;

sta_tool T_Sta_tool();
void *T_staPop(sta_tool sta);
void T_staPush(sta_tool sta, void *p);
void T_D_staTool(sta_tool *sta);
void T_C_staTool(sta_tool sta);

#endif