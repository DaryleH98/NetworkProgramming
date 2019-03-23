#include<netinet/in.h>    
#include<stdio.h>    
#include<stdlib.h>    
#include<sys/socket.h>    
#include<sys/stat.h>    
#include<sys/types.h>    
#include<unistd.h> 
#include<string.h>
#include<sys/wait.h>

#define MAXLINE 128
int socket_fd, new_socket;    
socklen_t addrlen;    
int bufsize = 1024;    
char *buffer;
struct sockaddr_in address; 

//Method that splits the String command
char ** splitString(char *command,char delim,int *args_c){
    int ct = 0,i;
    char **args = (char **)malloc(sizeof(char *)*(MAXLINE/2+1));
    args[0] = (char *)malloc(MAXLINE);
    *args_c = 0;
    for(i=0;command[i]!='\0';i++){
        if(command[i] == delim){
            args[*args_c][ct] = '\0';
            ct = 0;
            *args_c += 1;
            args[*args_c] = (char *)malloc(MAXLINE);
        }
        else{
            args[*args_c][ct] = command[i];
            ct++;
        }
    }
    args[*args_c][ct] = '\0';
    *args_c += 1;
    return args;
}

//Simple method to start the server
void startServer(int port){
    buffer = malloc(bufsize);    
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) > 0){    
      printf("The socket was created\n");
    }

    address.sin_family = AF_INET;    
    address.sin_addr.s_addr = INADDR_ANY;    
    address.sin_port = htons(port);    

    if (bind(socket_fd, (struct sockaddr *) &address, sizeof(address)) == 0){    
        printf("Binding Socket\n");
    }
    else{ 
        perror("Socket binding failed");    
        exit(1);    
    }
}
//Reads the command from the browser
char *read_line(FILE *fp)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    if (fp == NULL){
        printf("Unable to open the file\n");
        exit(EXIT_FAILURE);
    }

    if ((read = getline(&line, &len, fp)) != -1) {
        if(strlen(line) > MAXLINE){
            fprintf(stderr, "%s\n", "Size of command exceeded the limit : 512 ");
        }
        return line;
    }
    return NULL;
}

int main(int argc, char const *argv[]){
    if(argc < 2){
        startServer(3838);
    }
    else{
        startServer(atoi(argv[1]));
    }
    while (1) {    
        if (listen(socket_fd, 10) < 0) {    
            perror("server: listen");    
            exit(1);    
        }    

        if ((new_socket = accept(socket_fd, (struct sockaddr *) &address, &addrlen)) < 0) {    
            perror("server: accept");    
            exit(1);    
        }    
        
        if (new_socket > 0){    
            printf("The Client is connected...\n");
        }

        recv(new_socket, buffer, bufsize, 0);    
        int numlines;
        char **lines = splitString(buffer,'\n',&numlines);
        int i;
        for(i=0;i<numlines;i++){
            int numWords;
            char **words = splitString(lines[i],' ',&numWords);
            if(strcmp(words[0],"GET") == 0){
                if(strcmp(words[1],"/favicon.ico") != 0){
                    int i;
                    for(i=0;i<strlen(words[1]);i++){
                        words[1][i] = words[1][i+1];
                    }
                    strcat(words[1]," > output 2>&1");
                    if(fork() == 0){
                        system(words[1]);
                        char *buf;
                        FILE *fp = fopen("output","r");
                        char content[1024] = "";
                        fseek(fp, 0L, SEEK_END);
                        long int sz = ftell(fp);
                        fseek(fp, 0L, SEEK_SET);
                        while(ftell(fp) < sz){
                            buf = read_line(fp);
                            strcat(content,buf);
                            strcat(content,"<br>");
                        }
                        printf("%s %lld\n",content ,strlen(content));
                        buf ="";
                        sscanf(buf,"Content-length: %d\n",strlen(content));
                        write(new_socket, "HTTP/1.1 200 OK\n", 16);
                        write(new_socket, buf, strlen(buf));
                        write(new_socket, "Content-Type: text/html\n\n", 25);
                        write(new_socket, "<html><body><H1>",16);
                        write(new_socket, content,1024);
                        write(new_socket, "</H1></body></html>",19);
                        close(new_socket);
                        fclose(fp);
                        system("rm output");
                        exit(0);
                    }
                }
            }
        }
    }    
    close(socket_fd);    
    return 0;    
}
