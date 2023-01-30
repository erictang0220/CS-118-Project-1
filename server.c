// Server side C/C++ program to demonstrate Socket programming
// Source: https://www.geeksforgeeks.org/socket-programming-cc/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 15635

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    // Note: https://stackoverflow.com/questions/58599070/socket-programming-setsockopt-protocol-not-available
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    while(1) {

      // Should I do this?
      // sleep(5);

      // create socket and fill the buffer
      if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }
      else {
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
      }
      
      // --------- response header ---------
    
      // parse the first line to get the file name
      int i=0, j=0;
      char fileName[1024] = {0};
      int addToFileName = 0;
      while(1) {
        if(!addToFileName && buffer[i] == '/') {
          addToFileName = 1;
          i++;
        }
        if(addToFileName && buffer[i] == ' ') {
          break;
        }
        if(addToFileName) {
          fileName[j] = buffer[i];
          j++;
        } 
        i++;
      }

      // handle space (%20)
      char newFileName[1024] = {0};
      int p = 0;
      for(i=0, p=0; i < strlen(fileName); p++, i++) {
        if(i+2 < strlen(fileName) && fileName[i] == '%' && fileName[i+1] == '2' && fileName[i+2] == '0') {
          newFileName[p] = ' ';
          i += 2;
        }
        else {
          newFileName[p] = fileName[i];
        }
      }

      printf("%s\n", fileName);
      printf("%s\n", newFileName);
      
    
      // Get the file extension (scan from the back)
      char extension[100] = {0};
      int m, k;
      for(k = j-1, m = 0; k >= 0; k--, m++) {
        if(newFileName[k] == '.') {
          break;
        }
        else {
          extension[m] = newFileName[k];
        }
      }

      // reverse extension
      int left = 0, right = m-1;
      while(left <= right) {
        char tmp = extension[left];
        extension[left] = extension[right];
        extension[right] = tmp;
        left++;
        right--;
      }
      printf("%s\n", extension);

      // send status
      char *responseStatus= "HTTP/1.0 200 OK\r\n";
      send(new_socket, responseStatus, strlen(responseStatus), 0);

      // send content type
      char *contentType;
      if(strcmp(extension, "txt") == 0) {
        contentType = "Content-Type: text/plain\r\n";
      }
      else if(strcmp(extension, "html") == 0) {
        contentType = "Content-Type: text/html\r\n";
      }
      else if(strcmp(extension, "jpg") == 0) {
        contentType = "Content-Type: image/jpeg\r\n";
      }
      else if(strcmp(extension, "png") == 0) {
        contentType = "Content-Type: image/png\r\n";
      }
      else if(strcmp(extension, "pdf") == 0) {
        contentType = "Content-Type: application/pdf\r\n"; 
      }
      else {
        contentType = "Content-Type: application/octet-stream\r\n"; 
      }
      send(new_socket, contentType, strlen(contentType), 0);

      // send separator (mark end of response header)
      char *separator = "\r\n";
      send(new_socket, separator, strlen(separator), 0);

      // EXTRA: add content length???
      // bottomline: server shouldn't crash 
      // --------- response body ---------
      
      // read the file (fseek, fwind, rewind)
      
      FILE *fp;
      if(strcmp(extension, "txt") == 0 || strcmp(extension, "html") == 0) {
        fp = fopen(newFileName, "r");
      }
      else {
        fp = fopen(newFileName, "rb");
        printf("here1\n");
      }
      
      if(fp == NULL) {
          perror("Error opening file");
          continue;
      }

      int fileContentSize = 100;
      char fileContent[100] = {0};
      
      // find len = file size
      fseek(fp, 0, SEEK_END);
      int len = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      while(len > fileContentSize) {
        
        // TODO: important
        // printf("here3\n");
        fread(fileContent, sizeof(char), fileContentSize, fp);
        send(new_socket, fileContent, fileContentSize, 0);
        len -= fileContentSize;
      }

      fread(fileContent, sizeof(char), len, fp);
      send(new_socket, fileContent, len, 0);

      fclose(fp);
      close(new_socket);

      printf("success!\n");
    }
    
    // printf("%s\n", buffer);
    // send(new_socket, hello, strlen(hello), 0);
    // printf("Hello message sent\n");
    return 0;
}
