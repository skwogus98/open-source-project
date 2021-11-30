#include "stems.h"
#include <semaphore.h>

sem_t sem;
// 
// To run:
// 1. Edit config-ws.txt with following contents
//    <port number>
// 2. Run by typing executable file
//    ./server
// Most of the work is done within routines written in request.c
//

//queue 관련 함수 (출처: https://code-lab1.tistory.com/6 [code_lab])
typedef struct Node 
{
  int data;
  struct Node *next;
}Node;
 
 
typedef struct Queue 
{
  Node *front;
  Node *rear; 
  int count; // 큐 안의 노드 개수  
  int maxSize; //최대 개수
}Queue;
 
void initQueue(Queue *queue, int maxSize)
{
  queue->front = queue->rear = NULL; 
  queue->count = 0;    // 큐 안의 노드 개수를 0으로 설정
  queue->maxSize = maxSize;
}

int isEmpty(Queue *queue)
{
  return queue->count == 0;    // 큐안의 노드 개수가 0이면 빈 상태
}

int isFull(Queue *queue)
{
  return queue->count == queue->maxSize;    // 큐안의 노드 개수가 0이면 빈 상태
}

void enqueue(Queue *queue, int data)
{
  Node *newNode = (Node *)malloc(sizeof(Node)); // newNode 생성
  newNode->data = data;
  newNode->next = NULL;

  if (isEmpty(queue))    // 큐가 비어있을 때
  {
    queue->front = newNode;       
  }
  else    // 비어있지 않을 때
  {
    queue->rear->next = newNode;    //맨 뒤의 다음을 newNode로 설정
  }
  queue->rear = newNode;    //맨 뒤를 newNode로 설정   
  queue->count++;    //큐안의 노드 개수를 1 증가
}

int dequeue(Queue *queue)
{
  int data;
  Node *ptr;
  if (isEmpty(queue))    //큐가 비었을 때
  {
    printf("Error : Queue is empty!\n");
    return 0;
  }
  ptr = queue->front;    //맨 앞의 노드 ptr 설정 
  data = ptr->data;    // return 할 데이터  
  queue->front = ptr->next;    //맨 앞은 ptr의 다음 노드로 설정
  free(ptr);    // ptr 해제 
  queue->count--;    //큐의 노드 개수를 1 감소
  
  return data;
}
//queue 전역변수
Queue *reqQueue;

//
void getargs_ws(int *port, int *poolSize, int *queueSize)
{
  FILE *fp;

  if ((fp = fopen("config-ws.txt", "r")) == NULL)
    unix_error("config-ws.txt file does not open.");

  fscanf(fp, "%d", port);
  fscanf(fp, "%d", poolSize);
  fscanf(fp, "%d", queueSize);
  fclose(fp);
}


void producer(void * listenfd){
  //while(1)
  {
    //printf("producer: %d\n", reqQueue->count);
    sem_wait(&sem);
    if(!isFull(reqQueue)){
      int clientlen;
      struct sockaddr_in clientaddr;
      clientlen = sizeof(clientaddr);
      int connfd = Accept( *((int *)listenfd), (SA *)&clientaddr, (socklen_t *) &clientlen);
      enqueue(reqQueue, connfd);
      /*requestHandle(connfd, getWatch());
      Close(connfd);*/
    }

    sem_post(&sem);
  }
  pthread_exit((void *)0);
}
//connfd
void consumer(void *i)
{
  sem_wait(&sem);
  
  if(!isEmpty(reqQueue)){
    int connfd = dequeue(reqQueue);
    requestHandle(connfd, getWatch());
    Close(connfd);
  }
  
  sem_post(&sem);
  pthread_exit((void *)0);
}

int main(void)
{
  int listenfd, port, poolSize, queueSize, thr_id, result;
  struct sockaddr_in clientaddr;
  pthread_t p_thread;
  pthread_t thProducer;
  pthread_t *thConsumers;

  int a= 1;

  reqQueue = (Queue *)malloc(sizeof(Queue));
  sem_init(&sem, 0, 1);
  initWatch();
  getargs_ws(&port, &poolSize, &queueSize);
  initQueue(reqQueue, queueSize);

  listenfd = Open_listenfd(port);
  while(1)
  {
    thr_id = pthread_create(&thProducer, NULL, producer, (void*)&listenfd);
    if (thr_id < 0)
    {
      perror("thread create error : ");
      exit(0);
    }
    pthread_join(thProducer, (void *)&result);
    
    thConsumers = (pthread_t*)malloc(sizeof(pthread_t)*poolSize);
    for(int i = 0; i<poolSize; i++){
      pthread_create(&thConsumers[i], NULL, consumer, (void*)&(i));
      pthread_detach(thConsumers[i]);
    }
  }
  return(0);
}
