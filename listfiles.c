#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int fname_is_num(char * foo){
	while(isdigit(*(foo++)));
	return (*(--foo) != 0x00 ) ? 0 : 1 ;
}
int nodots(char *foo){
	while(*(foo)!='.' && *(foo++) != 0x00 );
	return (*(--foo) != 0x00) ? 0 : 1 ;
}

void fd_print(char * procitem, struct dirent * fdent){
	char fdpath[256];
	char linkpath[4096];
	memset(linkpath,0,4096);
	strcpy(fdpath,procitem);
	strcat(fdpath,"/");
	strcat(fdpath,fdent->d_name);
	printf("\t%s",fdent->d_name);
	if(readlink(fdpath,linkpath,4096)>0)
		printf(" -> %s",linkpath);
	printf("\n");
}

void dump_fds(char* fullpath,struct dirent * process){
	char procitem[256];
	DIR * fds;
	struct dirent * fdent;
	strcpy(procitem,fullpath);
	strcat(procitem,"/");
	strcat(procitem, process->d_name);
	if((fds = opendir(procitem)) != NULL){
		while((fdent = readdir(fds))!=NULL)
			if(fname_is_num(fdent->d_name))
				fd_print(procitem,fdent);
		closedir(fds);
	}
}

void get_cmdline(char * fullpath){
	char cmdline_path[256],cmdline_str[256];
	FILE * cmdline;
	memset(cmdline_str,0,256);
	strcpy(cmdline_path,fullpath);
	strcat(cmdline_path,"/cmdline");
	cmdline = fopen(cmdline_path, "r");
	if(fread(cmdline_str,1,256,cmdline) > 0)
		printf(" -> %s",cmdline_str);
	fclose(cmdline);
}





void dump_map_files(char* fullpath,struct dirent * process){
	char mapdir_path[256];
	DIR * mapdir;
	struct dirent *mapdirent;
	strcpy(mapdir_path,fullpath);
	strcat(mapdir_path,"/");
	strcat(mapdir_path,process->d_name);
	if((mapdir = opendir(mapdir_path))!=NULL){
		while((mapdirent = readdir(mapdir))!=NULL)
			if(mapdirent->d_name != NULL &&  nodots(mapdirent->d_name))
				fd_print(mapdir_path, mapdirent);
		closedir(mapdir);
	}
}

void dump_proc_dir(struct dirent * entry){
	DIR * proc;
	struct dirent * process;
	char fullpath[256] = "/proc/";
	strcat(fullpath,entry->d_name);
	printf("%s",fullpath);
	get_cmdline(fullpath);
	printf("\n");
	proc = opendir(fullpath);
	while((process = readdir(proc))!=NULL){
		if(strcmp(process->d_name,"fd") == 0)
			dump_fds(fullpath,process);
		if(strcmp(process->d_name,"map_files") == 0 )
			dump_map_files(fullpath,process);
	}
	closedir(proc);
}

int main(){
	DIR * proc = opendir("/proc");
	struct dirent * entry;
	while((entry = readdir(proc))!=NULL){
		if (fname_is_num(entry->d_name))
			dump_proc_dir(entry);
	}
	fflush(stdout);
	return 0;

}
