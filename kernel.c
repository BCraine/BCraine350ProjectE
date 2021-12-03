// Brandon Craine

extern int interrupt(int,int,int,int,int);
extern void enableInterrupts();
void printChar(char*);
void readString(char*);
void readSector(char*,int);
void writeSector(char*,int);
void handleInterrupt(int,int,int,int);
void readFile(char*,char*);
void executeProgram(char*);
extern void launchProgram(int);
void terminate(int,int,int,int);
void deleteFile(char*);
void writeFile(char*,char*,int);
void handleTimerInterrupt(int,int);
void killProcess(int);

int processActive[8];
int processStackPointer[8];
int processWaiting[8];
int processWaitingFor[8];
int currentProcess;

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
	/*
	int sectorsRead;
	char buffer[13312];
	readFile(buffer, "messag",&sectorsRead);
	printChar(buffer);
	*/

	//char buffer[512];
	//int sector;

	int j;
	char shell[6];
	for(j=0;j<8;j++){
		processStackPointer[j]=0xff00;
		processActive[j]=0;
		processWaiting[j]=0;
		processWaitingFor[j]=0;
	}

	currentProcess=-1;
	makeInterrupt21();
	shell[0]='s';
	shell[1]='h';
	shell[2]='e';
	shell[3]='l';
	shell[4]='l';
	shell[5]='\0';
	//interrupt(0x21,6,buffer,&sector,0);
	//interrupt(0x21,7,buffer,0,0);
	
	//interrupt(0x21,8,"this is a test message","testmg",3);
	//interrupt(0x21,5,0,0,0);
	executeProgram(&shell);
	makeTimerInterrupt();

	
	while(1);
	
}

void killProcess(int number){
	
	int h;
	int dataseg;
	dataseg=setKernelDataSegment();
	
	for(h=0;h<8;h++){
		if(processWaitingFor[h]==number){
			processWaiting[h]=0;
		}
	}
	processActive[number]=0;
	restoreDataSegment(dataseg);

}

void handleTimerInterrupt(int segment, int sp){
	int i;
	int addedSegment;
	int addedSP;
	int allProcInactive;
	char bufferPrint[16];

	/*
	bufferPrint[0] = 'T';
	bufferPrint[1] = 'i';
	bufferPrint[2] = 'c';
	bufferPrint[3] = '\0';
	printChar(bufferPrint);
	*/
	int dataseg;
	dataseg=setKernelDataSegment();
	if(currentProcess != -1){
		processStackPointer[currentProcess] = sp;
	}
	allProcInactive = 1;

	for(i=0;i<8;i++){
		if(processActive[i]==1){
			allProcInactive=0;
			break;
		}
	}

	if(allProcInactive){
		returnFromTimer(segment,sp);
	}
	currentProcess=(currentProcess+1)%8;
	while(processActive[currentProcess]==0){
		currentProcess=(currentProcess+1)%8;

		if(processWaiting[currentProcess]==1){
			currentProcess=(currentProcess+1)%8;
		}
	}
	addedSegment=0x1000*(currentProcess+2);
	addedSP=processStackPointer[currentProcess];

	restoreDataSegment(dataseg);
	returnFromTimer(addedSegment,addedSP);
	
}

void writeFile(char* buffer,char* filename,int numberOfSectors){
	char dirBuffer[512];
	char map[512];
	int sector[256];
	int entry;
	int i;
	int j;

	
	interrupt(0x21,2,dirBuffer,2,0);
	interrupt(0x21,2,map,1,0);


	for(entry=0;entry<512;entry+=32){
		if(dirBuffer[entry]==0){
			break;
		}
	}
	for(i=0;i<6;i++){
		dirBuffer[entry+i]=filename[i];
	}
	for(i=0;i<numberOfSectors;i++){
		for(j=3;j<512;j++){
			if(map[j]==0){
				map[j]=0xFF;
				sector[i]=j;
				dirBuffer[entry+6+i]=sector[i];
				writeSector(buffer+512*i,sector[i]);
				break;
			}	
		}
	}
	for(i=entry+numberOfSectors+6;i<32;i++){
		dirBuffer[i]=0;
	}
	interrupt(0x21,6,dirBuffer,2,0);
	interrupt(0x21,6,map,1,0);
	return;
}	

void deleteFile(char* fileName){
	char dirBuffer[512];
	char cantFindFile[15];
	char map[512];
	int entry;
	int fileFound;
	int symb;
	int sector;
	
	interrupt(0x21,2,dirBuffer,2,0);
	interrupt(0x21,2,map,1,0);

	for(entry = 0; entry<512;entry+=32){
		fileFound=1;

		for(symb=0;symb<6;symb++){
			if(dirBuffer[entry+symb] != fileName[symb]){
				fileFound=0;
				break;

			}
		}
		if(fileFound==1){
			break;
		}
	}
	if(fileFound==0){
		cantFindFile[0]='C';
		cantFindFile[1]='A';
		cantFindFile[2]='N';
		cantFindFile[3]='T';
		cantFindFile[4]=' ';
		cantFindFile[5]='F';
		cantFindFile[6]='I';
		cantFindFile[7]='N';
		cantFindFile[8]='D';
		cantFindFile[9]=' ';
		cantFindFile[10]='F';
		cantFindFile[11]='I';
		cantFindFile[12]='L';
		cantFindFile[13]='E';
		cantFindFile[14]=0xD;
		cantFindFile[15]='\0';

		interrupt(0x21,0,cantFindFile,0,0);
		return;
	}
	for(sector=6;sector<32;sector++){
		int sectorNumber;
		sectorNumber = dirBuffer[entry+sector];
		if(sectorNumber!=0){
			map[sectorNumber]=0;
		}
	}
	dirBuffer[entry]=0;
	interrupt(0x21,6,dirBuffer,2,0);
	interrupt(0x21,6,map,1,0);
	return;
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

void writeSector(char* buffer, int sector){
	int head;
	int relSector;
	int trackNumber;

	relSector = sector+1;
	head=0;
	trackNumber=0;
	interrupt(0x13,3*256+1,buffer,trackNumber*256+relSector,head*256+0x80);
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
void executeProgram(char* name, int waitFor){
	char buffer[13312];
	int i;
	int address;
	int seg;
	int dataseg;

	dataseg=setKernelDataSegment();

	for(i=0;i<8;i++){
		if(processActive[i]==0){
			break;
		}
	}
	if(waitFor==1){
		processWaitingFor[currentProcess]=i;
	}
	processStackPointer[i]=0xff00;
	processActive[i]=1;

	restoreDataSegment(dataseg);

	seg = 0x1000*(i+2);
	readFile(name,buffer);
	
	for(address = 0;address<13312;address++){
		putInMemory(seg,address,buffer[address]);
	}
	initializeProgram(seg);
}


void terminate(int ax, int bx, int cx, int dx){
	/*char shell[6];

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
	}*/
	int dataseg;
	dataseg=setKernelDataSegment();
	processActive[currentProcess]=0;

	restoreDataSegment(dataseg);
	
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

