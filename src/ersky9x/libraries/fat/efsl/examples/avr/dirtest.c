#include <efs.h>

void hang(void);
unsigned char c2str(unsigned char value,char *buf);

void main(void)
{
	EmbeddedFileSystem efs;
	EmbeddedFile file_w;
	unsigned short i;
	unsigned char e;
	unsigned char t;
	signed char err;
	unsigned char offset1;
	unsigned char offset2;
	char path[100];

	debug_init();
	
	DBG((TXT("Welcome (2).\n-------------\n\n")));
	if(efs_init(&efs,0)!=0){
		DBG((TXT("Could not init filesystem.\n")));
		hang();
	}

	for(i=1;i<=4;i++){
		offset1=c2str(1,path);
		path[offset1++]='/';
		for(e=1;e<=12;e++){
			offset2=offset1;
			offset2+=c2str(e,path+offset1);
			DBG((TXT("Dir name: %s.\n"),path));
		}
	}
	/*for(i=1;i<10;i++){	
		path[0]='D';
		path[1]='I';
		path[2]='R';
		path[3]=0x30+((i%10000)/1000);
		path[4]=0x30+((i%1000)/100);
		path[5]=0x30+((i%100)/10);
		path[6]=0x30+((i%10)/1);
		path[7]='\0';
		err=mkdir(&efs.myFs,path);
		if(err!=0){
			DBG((TXT("ERROR: Could not create directory %s (%d) - returned ERR %d.\n"),path,i,err));
			hang();
		}
		DBG((TXT("DIR %s created.\n"),path));
		for(e=1;e<6;e++){
			path[7]='/';
			path[8]=0x40+e;
			path[9]='\0';
			err=mkdir(&efs.myFs,path);
			if(err!=0){
				DBG((TXT("ERROR: Could not create directory %s (%d) - returned ERR %d.\n"),path,i,err));
				hang();
			}
			DBG((TXT("DIR %s created.\n"),path));
			for(t=0;t<26;t++){
				path[9]='/';
				path[10]='t';
				path[11]='e';
				path[12]='s';
				path[13]='t';
				path[14]='-';
				path[15]=0x41+t;
				path[16]='.';
				path[17]='t';
				path[18]='x';
				path[19]='t';
				path[20]='\0';
				if(file_fopen(&file_w,&efs.myFs,path,'w')!=0){
					DBG((TXT("Could not open file %s for writing.\n"),path));		
					hang();
				}
				file_write(&file_w,15,"Dit is een test");
				file_fclose(&file_w);
			}
		}
	}*/

	file_fclose(&file_w);
	fs_umount(&efs.myFs);

	DBG((TXT("Done :-)\n\n")));
	
	hang();
}

unsigned char c2str(unsigned char value,char *buf)
{
	unsigned char i=0;
	
	if(value>=100)
		buf[i++]=0x30+(value/100);
	if(value>=10)
		buf[i++]=0x30+((value%100)/10);
	buf[i++]=0x30+(value%10);
	buf[i]='\0';

	return(i);
}

void hang(void)
{
	while((1))
		_NOP();
}

