#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char *token;
	char name[MAXLINE], time[MAXLINE], value[MAXLINE], tempName[MAXLINE], tempTime[MAXLINE], tempValue[MAXLINE], bodyForTok[MAXLINE];
	char takeBody[MAXBUF];
	int count = 0;
	char buff[MAXLINE];

	read(STDIN_FILENO, takeBody, MAXBUF);
	
	
	takeBody[strlen(takeBody)-1] = '\0';

	//Body Seperate
	strcpy(bodyForTok, takeBody);
	token = strtok(bodyForTok, "&");
	while (token != NULL)
	{
		if (count == 0)
			strcpy(tempName, token);
		if (count == 1)
			strcpy(tempTime, token);
		if (count == 2)
			strcpy(tempValue, token);
		token = strtok(NULL, "&");
		count++;
	}

	//Part name
	token = strtok(tempName, "=");
	count = 0;
	while (token != NULL)
	{
		if (count == 1)
			strcpy(name, token);
		token = strtok(NULL, "=");
		count++;
	}
	// Part time
	token = strtok(tempTime, "=");
	count = 0;
	while (token != NULL)
	{
		if (count == 1)
			strcpy(time, token);
		token = strtok(NULL, "=");
		count++;
	}
	// Part value
	token = strtok(tempValue, "=");
	count = 0;
	while (token != NULL)
	{
		if (count == 1)
			strcpy(value, token);
		token = strtok(NULL, "=");
		count++;
	}
	value[strlen(value)-1] = '\0';

	char content[MAXBUF];
	sprintf(content, "경고 : %s sensor로부터 %s 시각에 %s 라는 값이 발생했습니다.\r\ncurtimename=%s&time=%s&value=%s\n", name, time, value, name, time, value);
	fprintf(stderr, "HTTP/1.0 200 OK\r\n");
	fprintf(stderr, "Server: My Web Server\r\n");
	fprintf(stderr, "Content-Length: %d\r\n", strlen(content));
	fprintf(stderr, "Content-Type: text/plain\r\n\r\n");
	fprintf(stderr, content);
	/*
	sprintf(content, "경고 : %s sensor로부터 %s 시각에 %s 라는 값이 발생했습니다.\r\nname=%s&time=%s&value=%s", name, time, value, name, time, value);
	printf("HTTP/1.0 200 OK\r\n");
	printf("Server: My Web Server\r\n");
	printf("Content-Length: %d\r\n", strlen(content));
	printf("Content-Type: text/plain\r\n\r\n");
	printf(content);
*/
	fflush(stderr);
	
	return 0;
}