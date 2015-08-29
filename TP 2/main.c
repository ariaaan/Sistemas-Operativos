#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFERSIZE 256

void getPath(char *command);

int main(int argc, char **argv) {

	int exit = 0;

	char command[BUFFERSIZE];

	while(!exit) {
		printf("prompt: ");
   		scanf("%s", command);

   		getPath(command);
	}	

	return 0;
}

void getPath(char *command) {
	int found = 0;

	char *path = getenv("PATH");

	char *token;
	char *divider = ":";
	
	token = strtok (path, divider);

	char filePath[BUFFERSIZE];

	while (token != NULL && !found) {
		filePath[0] = '\0';

		strcat(filePath, token);
		strcat(filePath, "/");
		strcat(filePath, command);

		token = strtok (NULL, divider);

		if(access(filePath, F_OK) != -1 ) {
			printf("File exists in: %s\n", filePath);
			found = 1;
			break;
		} else {
			//If it doesn't exist, do nothing
		}
	}

	if(!found) {
		printf("Couldn't find file\n");
	}

}