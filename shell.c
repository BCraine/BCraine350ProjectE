void syscall(int,int,int,int,int);

void main(){
	
	char type[6];
	char execute[9];
	char buffer[13312];
	char* h;
	int i;
	int checkType;
	int checkExecute;

	type[0] = 't';
	type[1] = 'y';
	type[2] = 'p';
	type[3] = 'e';
	type[4] = ' ';
	type[5] = '\0';
	
	
	execute[0] = 'e';
	execute[1] = 'x';
	execute[2] = 'e';
	execute[3] = 'c';
	execute[4] = 'u';
	execute[5] = 't';
	execute[6] = 'e';
	execute[7] = ' ';
	execute[8] = '\0';

	syscall(0x21,0,"Shell> \0",0,0);
	checkType=1;
	syscall(0x21,1,h,0,0);

	for(i=0;i<5;i++){
		if(h[i] != type[i]){
			checkType =0;
			break;
		}
	}

	checkExecute = 1;
	
	for(i=0;i<8;i++){
		if(h[i] != execute[i]){
			checkExecute = 0;
			break;
		}
	}


	if(checkType ==1){
		syscall(0x21,3,h+5,buffer,0);
		syscall(0x21,0,buffer,0,0);
	}

	if(checkExecute ==1){
		syscall(0x21,4,h+8,0x2000,0);
	}
	buffer[0] = '\0';
	h[0] = '\0';
	syscall(0x21,5,0,0,0);

	
}
