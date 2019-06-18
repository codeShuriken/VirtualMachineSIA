#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *ltrim(char *s) {
	while (*s == ' ' || *s == '\t') s++;
	return s;
}

char getRegister(char *text) {
	if (*text == 'r' || *text=='R') text++;
	return atoi(text);
}

int getNumber(char *text){
	return atoi(text);
}

int assembleLine(char *text, unsigned char* bytes) {
	text = ltrim(text);
	char *keyWord = strtok(text," ");
	if (strcmp("add",keyWord) == 0) {
		bytes[0] = 0x10;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}
	else if (strcmp("addimmediate",keyWord) == 0){
		bytes[0] = 0x90;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getNumber(strtok(NULL, " "));
		return 2;
	}
	else if (strcmp("and",keyWord) == 0){
		bytes[0] = 0x20;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}
	else if (strcmp("branchifequal",keyWord) == 0){
		bytes[0] = 0xa0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," "));
		int tmp = getNumber(strtok(NULL, " "));
		bytes[1] = (bytes[1] << 4) | (tmp >> 16 & 0x0f);
		bytes[2] = tmp >> 8;
		bytes[3] = 0x000ff & tmp;
		return 4;
	}
	else if (strcmp("branchifless",keyWord) == 0){
		bytes[0] = 0xb0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," "));
		int tmp = getNumber(strtok(NULL, " "));
		bytes[1] = (bytes[1] << 4) | (tmp >> 16 & 0x0f);
		bytes[2] = tmp >> 8;
		bytes[3] = 0x000ff & tmp;
		return 4;
	}
	else if (strcmp("divide",keyWord) == 0){
		bytes[0] = 0x30;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}
	else if (strcmp("halt",keyWord) == 0){
		bytes[0] = 0x00;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}
	else if (strcmp("interrupt",keyWord) == 0){
		bytes[0] = 0x80;
		int tmp = getNumber(strtok(NULL, " "));
		bytes[0] |= (tmp >> 8 & 0x0f);
		bytes[1] = (0x0ff & tmp);
		return 2;
	}
	else if (strcmp("iterateover",keyWord) == 0){
		bytes[0] = 0xd0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getNumber(strtok(NULL, " "));
		int tmp = getNumber(strtok(NULL, " "));
		bytes[2] = tmp >> 8;
		bytes[3] = tmp & 0x00ff;

		return 4;
	}
	else if (strcmp("jump",keyWord) == 0){
		bytes[0] = 0xc0;
		int tmp = getNumber(strtok(NULL, " "));
		bytes[0] |= (tmp >> 24 & 0x0f);
		bytes[1] = tmp >> 16;
		bytes[2] = tmp >> 8;
		bytes[3] = tmp & 0x00000ff;
		return 4;
	}
	else if (strcmp("leftshift",keyWord) == 0){//Error check
		bytes[0] = 0x70;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = (getNumber(strtok(NULL," ")) & 0x1f);
		return 2;
	}
	else if (strcmp("load",keyWord) == 0){
		bytes[0] = 0xe0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | (getNumber(strtok(NULL," ")) & 0x0f);
		return 2;
	}
	else if (strcmp("multiply",keyWord) == 0){
		bytes[0] = 0x40;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}
	else if (strcmp("or",keyWord) == 0){
		bytes[0] = 0x60;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}
	else if (strcmp("rightshift",keyWord) == 0){
		bytes[0] = 0x70;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = 0x20 | (getNumber(strtok(NULL," ")) & 0x1f);
		return 2;
	}
	else if (strcmp("store",keyWord) == 0){
		bytes[0] = 0xf0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | (getNumber(strtok(NULL," ")) & 0x0f);
		return 2;
	}
	else if (strcmp("subtract",keyWord) == 0){
		bytes[0] = 0x50;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}
	else{
		fprintf(stderr ,"Invalid Syntax: %s\n", text);
		exit(1);
	}
}

int main(int argc, char **argv) {
	FILE *src = fopen(argv[1],"r");
	FILE *dst = fopen(argv[2],"w");
	while (!feof(src)) {
		unsigned char bytes[4];
		char line[1000];
		printf ("about to read\n");
		if (NULL != fgets(line, 1000, src)) {
			printf ("read: %s\n",line);
			int byteCount = assembleLine(line,bytes);
			fwrite(bytes,byteCount,1,dst);
		}
	}
	fclose(src);
	fclose(dst);
	return 0;
}
//HexDump