/*
 * clientPost.c: A very, very primitive HTTP client for sensor
 * 
 * To run, prepare config-cp.txt and try: 
 *      ./clientPost
 *
 * Sends one HTTP request to the specified HTTP server.
 * Get the HTTP response.
 */

#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/time.h>
#include "stems.h"

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
      printf("Length = %d\n", length);
    printf("Header: %s", buf);
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
void userTask(char *myname, char *hostname, int port, char *filename, float time, float value)
{
  int clientfd;
  char msg[MAXLINE];
  char input[MAXLINE] int n;
  int random;
  srand(time(NULL));

  printf("if you want to see command, type 'help'");

  while (1)
  {
    printf(">>");
    scanf("%s", &input);
    if (!strcmp(input, "help"))
    {
      printf("help: list available commands.\n");
      printf("name: print current sensor name.\n");
      printf("name <sensor>: change sensor name to sensor\n");
      printf("value: print current value of sensor.\n");
      printf("value <n>: set sensor value to <n>\n");
      printf("send: send (currnet sensor name, time, value) to server\n");
      printf("random <n>: send (name, time , random value) to server <n> times\n");
      printf("quit: quit the program\n");
    }
    else if (!strncmp(input, "name", 4)))
      {
        if (!strcmp(input, "name"))
        {
          printf("Current sensor is %s \n", myname);
        }
        else
        {
          printf("%s\n", input);
          sscanf(input, "name %s", myname);
          printf("Sensor name is changed to '%s'\n", myname);
        }
      }
    else if (!strncmp(input, "value", 5)))
      {
        if (!strcmp(input, "value"))
        {
          printf("Current value of sensor is %f \n", value);
        }
        else
        {
          printf("%s\n", input);
          sscanf(input, "value %s", n);
          value = atof(n);
          printf("Sensor name is changed to '%s'\n", value);
        }
      }
    else if (!strcmp(input, "send"))
    {
      sprintf(msg, "name=%s&time=%f&value=%f", myname, time, value);
      clientfd = Open_clientfd(hostname, port);
      clientSend(clientfd, filename, msg);
      clientPrint(clientfd);
    }
    else if (!strcmp(input, "random", 6))
    {
      sscanf(input, "random %d", &m);

      for (int i = 0; i < m; i++)
      {
        random = rand() % 21 - 10;
        value += random;
        sprintf(msg, "name=%s&time=%f&value=%f", myname, time, value);
        clientfd = Open_clientfd(hostname, port);
        clientSend(clientfd, filename, msg);
        clientPrint(clientfd);
        sleep(1);
      }
    }
    else if (!strcmp(input, "quit"))
    {
      break;
    }
  }
  Close(clientfd);
}

void getargs_cp(char *myname, char *hostname, int *port, char *filename, float *time, float *value)
{
  FILE *fp;

  fp = fopen("config-cp.txt", "r");
  if (fp == NULL)
    unix_error("config-cp.txt file does not open.");

  fscanf(fp, "%s", myname);
  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%f", time);
  fscanf(fp, "%f", value);
  fclose(fp);
}

int main(void)
{
  char myname[MAXLINE], hostname[MAXLINE], filename[MAXLINE];
  int port;
  float time, value;

  getargs_cp(myname, hostname, &port, filename, &time, &value);

  userTask(myname, hostname, port, filename, time, value);

  return (0);
}
