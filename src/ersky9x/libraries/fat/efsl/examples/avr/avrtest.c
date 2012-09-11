#include <efs.h>

void hang(void);

void main(void)
{
	EmbeddedFileSystem efs;
	EmbeddedFile file_r, file_w;
	unsigned short i,e;
	char buf[512];

	debug_init();

	DBG((TXT("Hello :-)")));
	
	if(efs_init(&efs,0)!=0){
		hang();
	}
	
	if(file_fopen(&file_r,&efs.myFs,"orig.txt",'r')!=0){
		hang();
	}

	if(file_fopen(&file_w,&efs.myFs,"copy.txt",'w')!=0){
		hang();
	}
	
	while(e=file_read(&file_r,512,buf)){
		file_write(&file_w,e,buf);
	}

	file_fclose(&file_r);
	file_fclose(&file_w);

	fs_umount(&efs.myFs);

	hang();
}

void hang(void)
{
	while((1))
		_NOP();
}
