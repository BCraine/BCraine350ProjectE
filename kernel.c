// Brandon Craine

extern int interrupt(int,int,int,int,int);
void printChar(char*);
void readString(char*);
void readSector(char*,int);
void handleInterrupt(int,int,int,int);
void readFile(char*,char*);
void executeProgram(char*);
extern void launchProgram(int);
void terminate(int,int,int,int);

void main(){

	/*
	int startVidMem = 0xb800;
	int vidMemOffset = 0x0;
	int white = 0x7;
	char* letters = "Hello World\0"; 
	while(*letters != 0x0){
		putInMemory(startVidMem, vidMemOffset, *letters);
		vidMemOffset++;
		putInMemory(startVidMem, vidMemOffset, white);
		vidMemOffset++;
		//advance letters pointer
		letters++;
	}
	*/
	
	/*char line[80];
	printChar("Enter a line: ");
	readString(line);
	printChar(line);
	*/
	
	/*char buffer[512];
	readSector(buffer,30);
	printChar(buffer);
	*/

	/*makeInterrupt21();
	interrupt(0x21,0,0,0,0);	
	*/

	/*char line[80];
	makeInterrupt21();
	interrupt(0x21,1,line,0,0);
	interrupt(0x21,0,line,0,0);
	*/
	
	/*char buffer[13312];
	int sectorsRead;
	makeInterrupt21();
	interrupt(0x21,3,"messag",buffer,&sectorsRead);
	if(sectorsRead>0){
		interrupt(0x21,0,buffer,0,0);
	}
	else{
		interrupt(0x21,0,"messag not found\r\n",0,0);
	}
	*/

	//makeInterrupt21();
	//interrupt(0x21,4,"tstpr1",0,0);
	//interrupt(0x21,5,"tstpr2",0,0);
	//interrupt(0x21,5,0,0,0);
	
	int sectorsRead;
	char buffer[13312];
	readFile(buffer, "messag",&sectorsRead);
	printChar(buffer);
	
	while(1);
	
}

void printChar(char* chars){
	int i;
	for(i=0; chars[i]!=0x0;i++){
		interrupt(0x10,0xe*256+chars[i],0,0,0);
	}

}

void readString(char* line){
	int j=0;
	char k = 0;
	while(k != 0xd){
		k = interrupt(0x16,0,0,0,0);

		*(line + j) = k;
		interrupt(0x10,0xe*256+k,0,0,0);
	
		if(k==13){
			interrupt(0x10,0xe*0x100+10,0,0,0);
		}
		j++;
	}
	*(line +j) = 0;
	//return j;
}

void readSector(char* buffer, int sector){
	int head;
	int relSector;
	int trackNumber;
	relSector = sector+1;
	head = 0;
	trackNumber = 0;
	interrupt(0x13,2*256+1, buffer, trackNumber*256+relSector,head*256+0x80);
}

void readFile(char* name, char* buffer, int* sectorsRead){
	char dirBuffer[512];
	char cantFindFile[15];
	int entry;
	int fileFound;
	int char1;
	int sector;
	

	interrupt(0x21,2,dirBuffer,2,0);

	for(entry = 0; entry<512;entry+=32){
		fileFound = 1;

		for(char1=0;char1<6;char1+=1){
			if(dirBuffer[entry+char1] != name[char1]){
				fileFound =0;
				break;
			}
		}
		if(fileFound == 1){
			*sectorsRead = entry/32;
			break;
		}
	}
	if (fileFound ==0){
		cantFindFile[0] = 'C';
		cantFindFile[1] = 'A';
		cantFindFile[2] = 'N';
		cantFindFile[3] = 'T';
		cantFindFile[4] = ' ';
		cantFindFile[5] = 'F';
		cantFindFile[6] = 'I';
		cantFindFile[7] = 'N';
		cantFindFile[8] = 'D';
		cantFindFile[9] = ' ';
		cantFindFile[10] = 'F';
		cantFindFile[11] = 'I';
		cantFindFile[12] = 'L';
		cantFindFile[13] = 'E';
		cantFindFile[14] = 0xD;
		cantFindFile[15] = '\0';
	
		interrupt(0x21, 0, cantFindFile,0,0);
		*sectorsRead = 0;
		return;
		
	}
	for(sector = 6;sector<32; sector++){
		int sectorNumber;
		sectorNumber = (int) dirBuffer[entry+sector];

		if( sectorNumber != 0){
			interrupt(0x21,2,buffer+(512*(sector-6)),sectorNumber,0);
		
		}
		else{
			break;
		}

	}
	return;

}
void executeProgram(char* name){
	char buffer[13312];
	int loc;

	readFile(name, buffer);
	
	for(loc = 0; loc< 13312; loc+=1){
		putInMemory(0x2000,loc,buffer[loc]);
	}	
	launchProgram(0x2000);

}


void terminate(int ax, int bx, int cx, int dx){
	char shell[6];

	if (ax==4){
		executeProgram(bx,cx);
	}
	else if(ax==5){
		interrupt(0x21,4,"shell",0x2000,0);
		shell[0] = 's';
		shell[1] = 'h';
		shell[2] = 'e';
		shell[3] = 'l';
		shell[4] = 'l';
		shell[5] = '\0';
		executeProgram(shell);
	}
	else{
		printChar("error\0");
	}
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	if(ax==0){
		printChar(bx);
	}
	else if(ax==1){
		readString(bx);
	}
	else if(ax==2){
		readSector(bx,cx);
	}
	else{
		printChar("Error");
	}
}	

