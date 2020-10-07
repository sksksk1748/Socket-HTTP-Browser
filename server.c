//================================================================================
// Author : Chieh-Ting Chung
// Date : 2020/10/07
//================================================================================

#include <stdio.h>            /*  standard I/O  */
#include <stdlib.h>           /*  standard library */
#include <string.h>           /*  string library */
#include <sys/socket.h>       /*  socket definitions */
#include <sys/types.h>        /*  socket types       */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>			  /*  for close() modify bt tim*/
#include <ctype.h>
#include <string.h>         

#define  MYPORT    8808
#define  BACKLOG   5

int main(void)
{
	char inputBuffer[800] = {}; // browser HTTP request data is stored here
	char buffer[256] = {}; // regx data is stored here
	char p1[300] = {}; // p1.html data is stored here
	char p2[300] = {}; // p2.html data is stored here
	// 404 Not Found error mesenge with HTTP response  
	char notfound[] =                                                                             
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html; charset=UTF-8\r\n\r\n"
      "<!DOCTYPE html>\r\n"
      "<html><body>404 Not Found \r\n"
      "</body></html>\r\n";

	FILE *pFile;
	pFile = fopen("p1.html" ,"r");
	
	char word_p1;
	int i = 0;
  	if (pFile == NULL){
    	printf("file not found!");
  	}
  	else{
    	while ((word_p1 = fgetc(pFile)) != EOF){
			p1[i] = word_p1;
			i++;
    	}
    	fclose(pFile);
  	}
	
	int  sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct sockaddr_in   my_addr; // my address information
   	struct sockaddr_in   their_addr; // connector’s address information
   	int sin_size;

   	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
   		perror("socket");
   		exit(1);
   	}

   	my_addr.sin_family = AF_INET; // host byte order
   	my_addr.sin_port = htons(MYPORT); // short, network byte order
   	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
   	memset(&(my_addr.sin_zero), 0, 8); // zero the rest of the struct

  	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
  		perror("bind");
  	    exit(1);
  	}
  	if (listen(sockfd, BACKLOG) == -1) {
  	    perror("listen");
  	    exit(1);
  	}
  	while(1) {                     // main accept() loop
    	sin_size = sizeof(struct sockaddr_in);
      	if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
      		perror("accept");
      	    continue;
      	}

		if(recv(new_fd,inputBuffer,sizeof(inputBuffer), 0) == -1){
      		perror("recv");
		}
		char *ptr = strstr(inputBuffer, "GET");
		// regx inputBuffer of data
		sscanf(ptr, "%*[^ ] %[^ ]",buffer);
   		printf("%s\n", buffer);  
		// if buffer[] is p2.html
		if(buffer[1]=='p' && buffer[2]=='2' && buffer[3]=='.' && buffer[4]=='h' && buffer[5]=='t'
			&& buffer[6]=='m' && buffer[7]=='l'){

			pFile = fopen("p2.html" ,"r");
     		char word_p2;                                                                                                                               
     		int i = 0;

     		if (pFile == NULL){
        		printf("file not found!");
     		}
     		else{
        		while ((word_p2 = fgetc(pFile)) != EOF){
           			p2[i] = word_p2;
           			i++;
           			printf("%c", word_p2);
        		}
        		fclose(pFile);
				// sned p2.html data of HTTP response to browser
				if (send(new_fd, p2, sizeof(p2)-1, 0) == -1)
            		perror("send");
     		}
		}// not p2.html
		else if(buffer[0] == '/'){
			// default response p1.html                                                                                                            
			if(buffer[1] == '\0'){
           		if (send(new_fd, p1, sizeof(p1)-1, 0) == -1)
              		perror("send");
			}
			// if url not /p2.html or original url(e.g. xxx.xxx.xxx.xxx:port)
			else{
				if (send(new_fd, notfound, sizeof(notfound)-1, 0) == -1)
	            	perror("send");
			}
      	}
      	close(new_fd); 
	} // end of while
   return 0;
}

