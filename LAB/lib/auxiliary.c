#include "auxiliary.h"

int isusr_registered(char * user_name){
	DIR * directory;
	struct dirent * d_entry;
	char * dir_path = USERS_DIRECTORY;

	if((directory= opendir(dir_path))==NULL){
		perror("Error when opening users directory");
		closedir(directory);
		return -1;
	}
	while ((d_entry = readdir(directory)))
	{
		if(strcmp(d_entry->d_name,user_name)==0){
			/* USER IS ALREADY REGISTERED */
			closedir(directory);
			return 1;
		}
	}
	/* not registered */
	closedir(directory);
	return 0;
}

int remove_directory(const char *path){
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d)
   {
      struct dirent *p;
      r = 0;
      while (!r && (p=readdir(d)))
      {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
          {
             continue;
          }

          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);

          if (buf)
          {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);

             if (!stat(buf, &statbuf))
             {
                if (S_ISDIR(statbuf.st_mode))
                {
                   r2 = remove_directory(buf);
                }
                else
                {
                   r2 = unlink(buf);
                }
             }
             free(buf);
          }

          r = r2;
      }
      closedir(d);
   }
   if (!r)
   {
      r = rmdir(path);
   }
   return r;
}

int file_exists (char * file_name, char * directory_path){
	DIR * directory;
	struct dirent * d_entry;

	if((directory= opendir(directory_path))==NULL){
		perror("Error when opening users directory");
		closedir(directory);
		return -1;
	}
	while ((d_entry = readdir(directory)))
	{
		if(strcmp(d_entry->d_name,file_name)==0){
			/* file exists */
			closedir(directory);
			return 1;
		}
	}
	/* not exits */
	closedir(directory);
	return 0;
}

char * get_socket_address(int socket){
		/* Socket structure to store the socket address */
		struct sockaddr_in client_addr;
		memset((char *)&client_addr,0,sizeof(client_addr));
		socklen_t size = sizeof(client_addr);
		if((getsockname(socket,(struct sockaddr *)&client_addr,&size ))==-1){
			perror("Cannot get socket address");
			return NULL;
		}
		char * client_address = inet_ntoa(client_addr.sin_addr);
		return client_address;
}