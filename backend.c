#include <stdio.h>

#ifndef _INTER_
#define _INTER_
#include "inter.h"
#endif

void Quads_to_Assembly(qnode **head) {
        qnode *temp;
        temp = *head;
        if (temp == NULL)
                fprintf(stdout, "\nThe list is empty!\n");
        else {
                while (temp != NULL) {
			switch(temp->op){
			case OP_UNIT:
				fprintf(stdout, ("%s\tproc near\npush bp\nmov bp,sp\nsub sp,size", temp->x->value.se->id);
				break;
			case OP_ENDU:
				fprintf(stdout, ("%s\tmov sp,bp\npop bp\nret\n%s\tendp", temp->x->value.se->id);
				break;
			case OP_JUMP:
				break;
			case OP_JUMPL:
				break;
			case OP_LABEL:
				break;
			case OP_IFB:
				break;
			case OP_CALL:
				break;
			case OP_PAR:
				break;
			case OP_RET:
				break;
			case OP_RETV:
				break;
			case OP_eq:
				break;
			case OP_neq:
				break;
			case OP_less:
				break;
			case OP_greater:
				break;
			case OP_leq:
				break;
			case OP_geq:
				break;
			case OP_PLUS:
				if (temp->x->type == QUAD_REAL)
					loadReal(temp->x);
				else load(ax,temp->x);
				if (temp->y->type == QUAD_REAL)
					loadReal(temp->y);	
				else load(dx,temp->y);
				if (temp->x->type == QUAD_REAL || temp->y->type == QUAD_REAL)
					fprintf(stdout, ("faddp ax,dx\n");
				else print("add ax,dx\n");
				if (temp->z->type == QUAD_REAL)
					storeReal(temp->z);
				else store(ax,temp->z);
				break;
			case OP_MINUS:
				if (temp->x->type == QUAD_REAL)
					loadReal(temp->x);
				else load(ax,temp->x);
				if (temp->y->type == QUAD_REAL)
					loadReal(temp->y);	
				else load(dx,temp->y);
				if (temp->x->type == QUAD_REAL || temp->y->type == QUAD_REAL)
					fprintf(stdout, ("fsubp ax,dx\n");
				else print("sub ax,dx\n");
				if (temp->z->type == QUAD_REAL)
					storeReal(temp->z);
				else store(ax,temp->z);
				break;

				break;
			case OP_bmul:
				break;
			case OP_bdiv:
				break;
			case OP_bmod:
				break;
			case OP_mod:
				break;
			case OP_assign:
				if(temp->x->type == QUAD_REAL)
					loadReal(x);
				else load(R,temp->x)
				if (temp->z->type == QUAD_REAL)
					storeReal(z);	
				else store(R,temp->z);
				break;
			case OP_ARRAY:
				load(ax ,y);
				fprintf(stdout, ("mov cx,size\nimul cx\n");
				loadAddr(cx,x);
				fprintf(stdout, ("add ax,cx\n");
				store(ax,z);
				break;
			default:
				break;
			}
                        temp = temp->next;
                }
        }
}

getAR(quad * q){
	fprintf(stdout, ("mov si,word ptr [bp+4]\n");
	int rep = n_cur->nestingLevel - q->e->nestingLevel-1;
	int i;
	for(i=0;i<rep;++i) fprintf(stdout, "mov si,word ptr [si+4]\n");
}

updateAL(){
	if (n_p < n_x)
		fprintf(stdout, ("push bp\n");
	else (n_p == n_x)
		fprintf(stdout, ("push word ptr [bp+4]\n");
	else {
		fprintf(stdout, ("mov si,word ptr [bp+4]\n");
		int rep = n_p->nestingLevel - n_x->nestingLevel-1;
		int i;
		for(i=0;i<rep;++i) fprintf(stdout, "mov si,word ptr [si+4]\n");
		fprintf(stdout, ("push word ptr [si+4]\n");
	}
}

load(R, quad * a){
	switch (a->type){
	case QUAD_SE:
		if(n_cur->nestingLevel == a->value.se->nestingLevel)
			if(a->value.se->entryType == ENTRY_PARAMETER && a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				fprintf(stdout, ("mov si, word ptr [si + offset]\n");
				fprintf(stdout, ("mov R,size ptr [si]\n");
			}
			else fprintf(stdout, "mov R, size ptr [bp + offset]\n");
		else 
			if(a->value.se->entryType == ENTRY_PARAMETER && a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				getAR(a);
				fprintf(stdout, ("mov si, word ptr [si + offset]\n");
				fprintf(stdout, ("mov R,size ptr [si]\n");
			}
			else getAR(a),fprintf(stdout, "mov R,size ptr [si+offset]\n");
		break;
	case QUAD_INTEGER:
		fprintf(stdout, ("mov R,%d", a->value.intval);
		break;
	case QUAD_CHAR:
		fprintf(stdout, ("mov R,",a->value.intval);
		break;
	case QUAD_BOOL:
		fprintf(stdout, ("mov R,%d", a->value.intval);
		break;
	/*case QUAD_REAL:
		fprintf(stdout, (",%Lf", a->value.floatval);
		break;
	case QUAD_STR:
		fprintf(stdout, (",%s", a->value.strval);
		break;
	*/
	case QUAD_POINTER:
		load(di,x);
		fprintf(stdout, ("mov R,size ptr[di]\n");
		break;
	default:
		break;
	}

}

loadAddr(R,a){
	switch (a->type){
	case QUAD_STR:
		fprintf(stdout, ("lea R,byte ptr a->value.strval ");
		break;
	}
	case QUAD_SE:
		if(n_cur->nestingLevel == a->value.se->nestingLevel)
			if(a->value.se->entryType == ENTRY_PARAMETER && a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				fprintf(stdout, ("mov R, word ptr [bp + offset]\n");
			}
			else fprintf(stdout, "lea R, size ptr [bp + offset]\n");
		else 
			if(a->value.se->entryType == ENTRY_PARAMETER && a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				getAR(a);
				fprintf(stdout, ("mov R,word ptr [si + offset]\n");
			}
			else getAR(a),fprintf(stdout, "lea R,size ptr [si+offset]\n");
		break;
	case QUAD_POINTER:
		load(R,x);
		break;

}

