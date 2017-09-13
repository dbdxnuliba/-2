#ifndef DOF_STACK_H
#define DOF_STACK_H

typedef struct dof_stack_{
	void **sta;
	void **top;
	int size;
	int maxsize;
}*dof_stack;

dof_stack Dof_stack();

void *D_pops(dof_stack sta);

void D_pushs(dof_stack sta, void *p);

void D_Dof_stack(dof_stack *sta);

void C_Dof_stack(dof_stack sta);

#endif