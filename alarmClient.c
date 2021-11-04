/*
 * clientGet.c: A very, very primitive HTTP client for console.
 *
 * To run, prepare config-cg.txt and try:
 *      ./clientGet
 *
 * Sends one HTTP request to the specified HTTP server.
 * Prints out the HTTP response.
 *
 * For testing your server, you will want to modify this client.
 *
 * When we test your server, we will be using modifications to this client.
 *
 */
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include "stems.h"

/*
 * Send an HTTP request for the specified file
 */
void clientSend(int fd, char *filename, char *body)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];

  Gethostname(hostname, MAXLINE);

  /* Form and send the HTTP request */
  sprintf(buf, "POST %s HTTP/1.1\n", filename);
  sprintf(buf, "%sHost: %s\n", buf, hostname);
  sprintf(buf, "%sContent-Type: text/plain; charset=utf-8\n", buf);
  sprintf(buf, "%sContent-Length: %d\n\r\n", buf, strlen(body));
  sprintf(buf, "%s%s\n", buf, body);
  Rio_writen(fd, buf, strlen(buf));
}

/*
 * Read the HTTP response and print it out
 */
void clientPrint(int fd)
{
  rio_t rio;
  char buf[MAXBUF];
  int length = 0;
  int n;

  Rio_readinitb(&rio, fd);

  /* Read and display the HTTP Header */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (strcmp(buf, "\r\n") && (n > 0))
  {
    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1)
      // printf("Length = %d\n", length);
      // printf("Header: %s", buf);
      n = Rio_readlineb(&rio, buf, MAXBUF);
  }

  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0)
  {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

/* currently, there is no loop. I will add loop later */
void userTask(char *hostname, int port, char *filename, char *msg)
{
  int clientfd;
  char input[MAXLINE];
  int n;

  clientfd = Open_clientfd(hostname, port);
  clientSend(clientfd, filename, msg);
  clientPrint(clientfd);
  Close(clientfd);
}

//파이프에서 인자를 가져옴
void getArgs(char msg[])
{
  char size;
  int pipeSize = Open("size", O_RDWR, 0);
  Read(pipeArgs, size, sizeof(char) * 1);
  int sizePipe = size - '0';

  int pipeArgs = Open("args", O_RDWR, 0);
  Read(pipeArgs, msg, sizeof(char) * sizePipe);

  printf("alarmClient args: ");
  printf(msg);
  printf("\n");
}
///

void getargs_pc(char *hostname, int *port, char *filename, char *sensorName, float *threshold)
{
  FILE *fp;

  fp = fopen("config-pc.txt", "r");
  if (fp == NULL)
    unix_error("config-pc.txt file does not open.");

  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%s", sensorName);
  fscanf(fp, "%f", threshold);

  fclose(fp);
}

///값을 검사
int checkData(char msg[], char *sensorName, float threshold)
{
  temp = strtok(buf, "=");
  if (strcmp(temp, "name") == 0)
  {
    temp = strtok(NULL, "&");
    if (strcmp(temp, sensorName) == 0)
    {
      temp = strtok(NULL, "="); // time 가르기
      temp = strtok(NULL, "&");

      temp = strtok(NULL, "="); // value
      temp = strtok(NULL, "&");
      if (atof(temp) > threshold)
      {
        return 1; //문제가 있다는 것을 알림
      }
    }
  }
  return 0; // 문제가 없을 시 alarmclient 종료
}

int main(void)
{
  char sensorName[MAXLINE], hostname[MAXLINE], filename[MAXLINE], msg[MAXLINE];
  int port;
  float threshold, value;

  getArgs(msg);
  getargs_pc(hostname, &port, filename, &sensorName, &threshold);

  if (checkData(msg, sensorName, threshold) == 1)
  {
    userTask(hostname, port, filename, msg);
  }

  return (0);
}
