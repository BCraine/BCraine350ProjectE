//Brandon Craine

void syscall(int,int,int,int,int);

void main(){
	
	char type[6];
	char execute[9];
	char buffer[13312];
	char* h;
	int i;
	int checkType;
	int checkExecute;
	char* dir[5];
	int checkDir;
	char* dirBuffer[512];
	char* dirArray[8];
	int entry;
	int symb;
	char del[5];
	int checkDel;	
	char hCopy[80];
	char copy[6];
	char* dupBuff;
	int checkCopy;
	char cre[5];
	char textfl[512];
	int checkCre;
	int offset;
	int addedOffset;

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


	dir[0] = 'd';
	dir[1] = 'i';
	dir[2] = 'r';
	dir[3] = ' ';
	dir[4] = '\0';

	del[0] = 'd';
	del[1] = 'e';
	del[2] = 'l';
	del[3] = ' ';
	del[4] = '\0';

	copy[0] = 'c';
	copy[1] = 'o';
	copy[2] = 'p';
	copy[3] = 'y';
	copy[4] = ' ';
	copy[5] = '\0';
	
	cre[0] = 'c';
	cre[1] = 'r';
	cre[2] = 'e';
	cre[3] = ' ';
	cre[4] = '\0';

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
	
	checkCopy=1;
	for(i=0;i<5;i++){
		if(h[i]!=copy[i]){
			checkCopy=0;
			break;
		}
	}

	checkCre=1;
	for(i=0;i<4;i++){
		if(h[i] != cre[i]){
			checkCre=0;
			break;
		}
	}

	checkDir =1;
	for(i=0;i<4;i++){
		if(h[i] != dir[i]){
			checkDir=0;
			break;
		}
	}

	checkDel=1;
	for(i=0;i<4;i++){
		if(h[i]!=del[i]){
			checkDel=0;
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

	if(checkCre==1){
		for(i=0;i<80;i++){
			if(h[i]==0){
				break;
			}
			hCopy[i]=h[i];
		}
		offset=0;
		while(1){
			syscall(0x21,1,textfl+offset,0,0);
			for(addedOffset=0;addedOffset<512;addedOffset++){
				if(textfl[addedOffset]==0){
					textfl[addedOffset]=' ';
					break;
				}
			}
			if(addedOffset == offset+2){
				break;
			}
			offset=addedOffset-1;
		}
		syscall(0x21,8,hCopy+4,textfl,1);
	}

	if(checkDir ==1){

		syscall(0x21,2,dirBuffer,2,0);
		for(entry = 0;entry<512;entry+=32){
			if(dirBuffer[entry] ==0){
				break;
			}
			for(symb = 0;symb<6;symb++){
				if(dirBuffer[entry+symb]!= 0xD || dirBuffer[entry+symb] != 0xA|| dirBuffer[entry+symb] != 0x0){
					dirArray[symb] = dirBuffer[entry+symb];
				}
				else{
					dirArray[symb]=' ';
				}
			}
			dirArray[6]=0xD;
			dirArray[7]=0x0;
			syscall(0x21,0,dirArray,0,0);
		}


	}

	if(checkCopy==1){
		for(i=0;i<80;i++){
			if(h[i]==0){
				break;
			}
			hCopy[i]=h[i];
		}
		syscall(0x21,3,h+5,dupBuff,0);
		syscall(0x21,8,hCopy+12,dupBuff,1);
	}


	if(checkDel==1){
		syscall(0x21,7,h+4,0,0);
	}



	buffer[0] = '\0';
	h[0] = '\0';
	hCopy[0]='\0';
	syscall(0x21,5,0,0,0);

	
}
