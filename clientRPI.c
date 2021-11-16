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
#include <time.h>
#include "stems.h"
#include <wiringPi.h>


#define MAXTIMINGS 83
#define DHTPIN 7

int dht11_dat[5] = {0, 0, 0, 0, 0};

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
void userTask(char *hostname, int port, char *filename, int period)
{
  char input[MAXLINE];
  int n;
  int random;
  time_t ltime;
  time(&ltime);
  float curtime = (float)ltime;

  read_dht11_dat(hostname, port, curtime, filename, hostname, port, period);
}

void getargs_cp(char *hostname, int *port, char *filename, int *period)
{
  FILE *fp;

  fp = fopen("config-pi.txt", "r");
  if (fp == NULL)
    unix_error("config-pi.txt file does not open.");

  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%d", period);

  fclose(fp);
}

// sensor관련 코드
void read_dht11_dat(char *hostname, int port, float curtime, char *filename, char *hostname, int port, int period)
{
  char msg[MAXLINE];
  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  uint8_t flag = HIGH;
  uint8_t state = 0;
  float f;

  int clientfd;

  memset(dht11_dat, 0, sizeof(int) * 5);
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, LOW);
  delay(18);
  digitalWrite(DHTPIN, HIGH);
  delayMicroseconds(30);
  pinMode(DHTPIN, INPUT);
  for (i = 0; i < MAXTIMINGS; i++)
  {
    counter = 0;
    while (digitalRead(DHTPIN) == laststate)
    {
      counter++;
      delayMicroseconds(1);
      if (counter == 200)
        break;
    } // while
    laststate = digitalRead(DHTPIN);
    if (counter == 200)
      break;
    if ((i >= 4) && (i % 2 == 0))
    {
      dht11_dat[j / 8] <<= 1;
      if (counter > 40)
        dht11_dat[j / 8] |= 1;
      j++;
    } // if
  }   // for
  if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xff)))
  {
    printf("humidity = %d.%d %% Temperature = %d.%d *C \n", dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3]);

    clientfd = Open_clientfd(hostname, port);
    sprintf(msg, "name=humidity&time=%f&value=%d.%d", curtime, dht11_dat[0], dht11_dat[1]);
    clientSend(clientfd, filename, msg);
    Close(clientfd);
    sleep(period/2);

    clientfd = Open_clientfd(hostname, port);
    sprintf(msg, "name=temperature&time=%f&value=%d.%d", curtime, dht11_dat[2], dht11_dat[3]);
    clientSend(clientfd, filename, msg);
    Close(clientfd);
    sleep(period/2);
  }
  else
    printf("Data get failed\n");
}

int main(void)
{
  char hostname[MAXLINE], filename[MAXLINE];
  int port;
  float time;
  int period;

  getargs_cp(hostname, &port, filename, &period);

  if (wiringPiSetup() == -1)
    exit(1);
  
  while (1)
  {
    userTask(hostname, port, filename, period);
  }
  return (0);
}
