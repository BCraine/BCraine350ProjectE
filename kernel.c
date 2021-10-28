// Brandon Craine

void printChar(char*);
void readString(char*);
void readSector(char*,int);
void handleInterrupt(int,int,int,int);

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

	char line[80];
	makeInterrupt21();
	interrupt(0x21,1,line,0,0);
	interrupt(0x21,0,line,0,0);


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

