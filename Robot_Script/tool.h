#ifndef RSI_TOOL_H
#define RSI_TOOL_H

typedef struct sta_tool_{
	void **sta;				// ջ��ָ��
	void **top;				// ջ��ָ��
	int size;				// ��ǰ��ŵĸ���
	int maxsize;			// ��ǰջ��С
}*sta_tool;

sta_tool T_Sta_tool();
void *T_staPop(sta_tool sta);
void T_staPush(sta_tool sta, void *p);
void T_D_staTool(sta_tool *sta);
void T_C_staTool(sta_tool sta);

#endif