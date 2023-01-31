// Server side C/C++ program to demonstrate Socket programming
// Source: https://www.geeksforgeeks.org/socket-programming-cc/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 15635

void get_filename(char* buffer, char* fileName) {
  /*
  Finds the filename of what the client is requesting. (i.e. "test.txt")

  Inputs:
  - buffer: A char array containing the buffer from reading the socket. 
  - fileName: An empty array that will be filled with filename of request.
  */
  int i = 0;
  int j = 0;
  int addToFileName = 0;
  while (1) {
    if (!addToFileName && buffer[i] == '/') {
      addToFileName = 1;
      i++;
    }
    if (addToFileName && buffer[i] == ' ') {
      break;
    }
    if (addToFileName) {
      fileName[j] = buffer[i];
      j++;
    } 
    i++;
  }
}

void handle_filename_space(char* fileName) {
  /*
  Replaces '%20' to ' ' in the filename.

  Inputs:
  - fileName: An array containing the filename of request.
  */
  char newFileName[1024] = {0};
  // char *newFileName = malloc(1024);
  int i = 0;
  int j = 0;
  for (; i < strlen(fileName); i++, j++) {
    if (i+2 < strlen(fileName) && 
        fileName[i] == '%' && fileName[i+1] == '2' && 
        fileName[i+2] == '0') {
      newFileName[j] = ' ';
      i += 2;
    }
    else {
      newFileName[j] = fileName[i];
    }
  }
  strcpy(fileName, newFileName);
}

char* get_file_extension(char* fileName) {
  /*
  Gets the file extension of the file

  Inputs:
  - fileName: An array containing the filename of request.

  Returns:
  - extension: The extension of the file of request.
  */

  char *extension = malloc(100);
  // char extension[100] = {0};
  int i = strlen(fileName) - 1;
  int j = 0;

  // get the chars before '.' in reverse
  for(; i >= 0; i--, j++) {
    if(fileName[i] == '.') {
      break;
    }
    else {
      extension[j] = fileName[i];
    }
  }

  // reverse the string
  int left = 0, right = j-1;
  while(left <= right) {
    char tmp = extension[left];
    extension[left] = extension[right];
    extension[right] = tmp;
    left++;
    right--;
  }

  return extension;
}

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

    // Forcefully attaching socket to the port 15635
    // Note: https://stackoverflow.com/questions/58599070/socket-programming-setsockopt-protocol-not-available
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 15635
    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    // Listen to request
    while(1) {
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
      char fileName[1024] = {0};
      get_filename(buffer, fileName);
      printf("after get filename: %s\n", fileName);

      // handle space (%20)
      handle_filename_space(fileName);
      printf("after handling space: %s\n", fileName);
      
      // Get the file extension (scan from the back)
      // char extension[100] = {0};
      char *extension = get_file_extension(fileName);
      // strcpy(extension, get_file_extension(&fileName));
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

      
      FILE *fp;
      if(strcmp(extension, "txt") == 0 || strcmp(extension, "html") == 0) {
        fp = fopen(fileName, "r");
      }
      else {
        fp = fopen(fileName, "rb");
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
        
        fread(fileContent, sizeof(char), fileContentSize, fp);
        send(new_socket, fileContent, fileContentSize, 0);
        len -= fileContentSize;
      }

      fread(fileContent, sizeof(char), len, fp);
      send(new_socket, fileContent, len, 0);

      fclose(fp);
      close(new_socket);
      free(extension);

      printf("success!\n");
    }
    
    return 0;
}
