#include <stdio.h>
#include <efs.h>

int main(void)
{
	EmbeddedFileSystem efs;
	EmbeddedFile file;	
	unsigned short i,e;
	char buf[512];
	
	if(efs_init(&efs,"/dev/sde")!=0){
		printf("Could not open filesystem.\n");
		return(-1);
	}
	
	if(file_fopen(&file,&efs.myFs,"group",'r')!=0){
		printf("Could not open file.\n");
		return(-2);
	}
	
	while(e=file_read(&file,512,buf)){
		for(i=0;i<e;i++){
			printf("%c",buf[i]);
		}
	}

	return(0);
}
