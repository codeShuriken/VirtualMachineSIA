#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_REGISTERS 16

typedef enum{
	HALT, ADD, AND, DIVIDE, MULTIPLY, SUBTRACT, OR, LRSHIFT, INTERRUPT,
	ADDIMMEDIATE, BRANCHIFEQUAL, BRANCHIFLESS, JUMP, ITERATEOVER, LOAD, STORE
}Instructions;

int registers[MAX_REGISTERS];
int pc = 0;
unsigned char buffer[4];
unsigned char internal;
unsigned char memory[10000];
int op1, op2, rlt;
signed int OP1, OP2, result;


void fetch(){
	int i;
	int pc1 = pc;
	internal = memory[pc];
	switch((internal & 0xF0) >> 4){
		case ADD:
		case AND:
		case DIVIDE:
		case MULTIPLY:
		case OR:
		case SUBTRACT:
		case LRSHIFT:
		case LOAD:
		case STORE:
		case ADDIMMEDIATE:
		case INTERRUPT:
			for (i = 0; i < 2; i++){
				buffer[i] = memory[pc1];
				pc1 += 1;
			}
			break;
		case BRANCHIFEQUAL:
		case BRANCHIFLESS:
		case ITERATEOVER:
		case JUMP:
			for (i = 0; i < 4; i++){
				buffer[i] = memory[pc1];
				pc1 += 1;
			}
			break;
		default:
			break;
	}

}

void dispatch(){
	internal = buffer[0];
	//No jump, halt, interrupt
	switch ((internal & 0xF0) >> 4){
		case ADD:
		case AND:
		case DIVIDE:
		case MULTIPLY:
		case OR:
		case SUBTRACT:
			op1 = internal & 0x0F;
			op2 = (buffer[1] & 0xF0) >> 4;
			rlt = buffer[1] & 0x0F;
			OP1	= registers[op1];
			OP2	= registers[op2];
			break;
		case LRSHIFT:
		case ITERATEOVER:
			op1 = internal & 0x0F;
			OP1	= registers[op1];
			break;
		case BRANCHIFEQUAL:
		case BRANCHIFLESS:
		case LOAD:
		case STORE:
			op1 = internal & 0x0F;
			op2 = (buffer[1] & 0xF0) >> 4;
			OP1	= registers[op1];
			OP2	= registers[op2];
			break;
		case ADDIMMEDIATE:
			op1 = internal & 0x0F;
			OP1 = (signed char)buffer[1];
			break;
		default:
			break;
	}

}

void execute(){
	internal = buffer[0];
	switch ((internal & 0xF0) >> 4){
		case ADD:
			result = OP1 + OP2;
			pc += 2;
			break;
		case ADDIMMEDIATE:
			pc += 2;
			break;
		case AND:
			result = OP1 & OP2;
			pc += 2;
			break;
		case BRANCHIFEQUAL:
			if (OP1 == OP2){
				result = ((buffer[1] & 0x0F) << 16) | (buffer[2] << 8)  | (buffer[3]);
				if ((result >> 16) > 7){
					result = (signed)(result | 0xFFF00000);
				}
			}
			if (result == 0){
				pc += 4;
			}
			else{
				if (result % 2 == 0){
					pc += result;
				}
				else{
					fprintf(stderr, "Offset must be divisible by 2");
					exit(1);
				}
			}
			break;
		case BRANCHIFLESS:
			if (OP1 < OP2){
				result = ((buffer[1] & 0x0F) << 16) | (buffer[2] << 8)  | (buffer[3]);
				if ((result >> 16) > 7){
					result = (signed)(result | 0xFFF00000);
				}
			}
			if (result == 0){
				pc += 4;
			}
			else{
				if (result % 2 == 0){
					pc += result;
				}
				else{
					fprintf(stderr, "Offset must be divisible by 2");
					exit(1);
				}
			}
			break;
		case DIVIDE:
			result = OP1 / OP2;
			pc += 2;
			break;
		case INTERRUPT:
			result = ((buffer[0] & 0x0F) << 8) | buffer[1];
			int i;
			if (result == 0){
				printf("Register Values:\n");
				for (i = 0; i < MAX_REGISTERS; i++){
						printf("R%d: %d\n", i, registers[i]);
				}
			}
			else if (result == 1){
				printf("Memory Values of Registers:\n");
				for (i = 0; i < MAX_REGISTERS; i++){
					if (registers[i] != '\0'){
						printf("R%d: %d\n", i, memory[registers[i]]);
					}
				}
			}
			pc += 2;
			break;
		case ITERATEOVER:
			if (memory[OP1] != '\0'){
				registers[op1] += buffer[1];
				result = (buffer[2] << 8) | buffer[3];
				pc -= result;
			}
			else{
				pc += 4;
			}
			break;
		case JUMP:
			result = ((buffer[0] & 0x0F) << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
			pc = result;
			break;
		case LRSHIFT:
			if ((buffer[1] & 0x20) == 0x20){
				result = OP1 >> (buffer[1] & 0x1F);
			}
			else{
				result = OP1 << (buffer[1] & 0x1F);
			}
			pc += 2;
			break;
		case LOAD:
			if ((buffer[1] & 0x0F) > 7){
				OP2 += (signed char)((buffer[1] & 0x0F) | (0xF0));
			}
			else{
				OP2 += (buffer[1] & 0x0F);
			}
			result = memory[OP2];
			pc += 2;
			break;
		case MULTIPLY:
			result = OP1 * OP2;
			pc += 2;
			break;
		case OR:
			result = OP1 | OP2;
			pc += 2;
			break;
		case STORE:
			if ((buffer[1] & 0x0F) > 7){
				OP2 += (signed char)((buffer[1] & 0x0F) | (0xF0));
			}
			else{
				OP2 += (buffer[1] & 0x0F);
			}
			result = registers[op1];
			pc += 2;
			break;
		case SUBTRACT:
			result = OP1 - OP2;
			pc += 2;
			break;
		default:
			break;
	}

}

void store(){
	internal = buffer[0];
	//No jump, halt, interrupt, branches,
	switch ((internal & 0xF0) >> 4){
		case ADD:
		case AND:
		case DIVIDE:
		case MULTIPLY:
		case OR:
		case SUBTRACT:
			registers[rlt] = result;
			break;
		case ADDIMMEDIATE:
			if (registers[op1] ==  '\0'){
				registers[op1] = OP1;
			}
			else{
				registers[op1] += OP1;
			}
			break;
		case LRSHIFT:
			registers[op1] = result;
			break;
		case LOAD:
			registers[op1] = result;
			break;
		case STORE:
			memory[OP2] = result;
			break;
		default:
			break;
	}
}


long loadfile(FILE* src){
	if (src == NULL){
		fprintf(stderr, "Error opening the file!\n");
		exit(1);
	}
	fseek(src, 0L, SEEK_END);
	long sz = ftell(src);
	rewind(src);
	fread(memory, sizeof(unsigned char), sz, src);

	return sz;
}

int main(int argc, char **argv) {
	if (argv[1] == NULL){
		fprintf(stderr, "Enter a file name!\n");
		exit(1);
	}
	FILE* src = fopen(argv[1],"r");
	long size = loadfile(src);
	printf("SIZE of the file: %ld\n", size);
	fclose(src);

	while ((memory[pc] & 0xF0) != HALT){
		fetch();
		dispatch();
		execute();
		store();
	}

	return 0;
}