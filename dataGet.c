#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include "/usr/include/mysql/mysql.h"

//
// This program is intended to help you test your web server.
// You can use it to test that you are correctly having multiple
// threads handling http requests.
//
// htmlReturn() is used if client program is a general web client
// program like Google Chrome. textReturn() is used for a client
// program in a embedded system.
//
// Standalone test:
// # export QUERY_STRING="name=temperature&time=3003.2&value=33.0"
// # ./dataGet.cgi

/////mariaDB 관련함수
struct connection_details
{

  char *server;
  char *user;
  char *password;
  char *database;
};

MYSQL *mysql_connection_setup(struct connection_details mysql_details)
{

  MYSQL *connection = mysql_init(NULL);

  if (!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0))
  {

    printf("Connection error : %s\n", mysql_error(connection));
    exit(1);
  }
  return connection;
}

MYSQL_RES *mysql_perform_query(MYSQL *connection, char *sql_query)
{

  if (mysql_query(connection, sql_query))
  {

    printf("MYSQL query error : %s\n", mysql_error(connection));
    exit(1);
  }
  return mysql_use_result(connection);
}
/////

void htmlReturn(void)
{
  char content[MAXLINE];
  char *buf;
  char *ptr;

  /* Make the response body */
  sprintf(content, "%s<html>\r\n<head>\r\n", content);
  sprintf(content, "%s<title>CGI test result</title>\r\n", content);
  sprintf(content, "%s</head>\r\n", content);
  sprintf(content, "%s<body>\r\n", content);
  sprintf(content, "%s<h2>Welcome to the CGI program</h2>\r\n", content);
  buf = getenv("QUERY_STRING");
  sprintf(content, "%s<p>Env : %s</p>\r\n", content, buf);
  ptr = strsep(&buf, "&");
  while (ptr != NULL)
  {
    sprintf(content, "%s%s\r\n", content, ptr);
    ptr = strsep(&buf, "&");
  }
  sprintf(content, "%s</body>\r\n</html>\r\n", content);

  /* Generate the HTTP response */

  printf("Content-Length: %d\r\n", strlen(content));
  printf("Content-Type: text/html\r\n\r\n");
  printf("%s", content);

  fflush(stdout);
}

void textReturn(void)
{
  char content[MAXLINE];
  char *command = "NONE";
  char *sname;
  int n = 1;
  char *strTime;
  char *buf;
  char *temp;
  char *ptr;
  char query[255];

  MYSQL *dbfd;
  MYSQL_RES *res;
  MYSQL_ROW row;

  struct connection_details mysqlD;
  mysqlD.server = "localhost";
  mysqlD.user = "root";          // username
  mysqlD.password = "mysql1234"; // pw
  mysqlD.database = "sensorDB";  // db name
  
  printf("Content-Length: %d\n", strlen(content));
  printf("Content-Type: text/plain\r\n\r\n");

  buf = getenv("QUERY_STRING");
  //인자분리
  
  temp = strtok(buf, "=");  
  if (strcmp(temp, "command") ==0)
  {
    temp = strtok(NULL, "&");
    command = temp;
  }
  else if (strcmp(temp, "NAME")==0)
  {
    temp = strtok(NULL, "&");
    sname = temp;
  }

  temp = strtok(NULL, "=");

  if (strcmp(temp, "N")==0)
  {
    temp = strtok(NULL, "&");
    n = atoi(temp);
  }
  else if (strcmp(temp, "value")==0)
  {
    temp = strtok(NULL, "&");
    sname = temp;
  }


  dbfd = mysql_connection_setup(mysqlD);
  
  // INFO
  if (strcmp(command, "INFO")==0)
  {
    sprintf(query, "select exists (select name from sensorList where name='%s' limit 1) as success", sname);
    res = mysql_perform_query(dbfd, query);
    row = mysql_fetch_row(res);
    if(atoi(row[0]) == 1){
      mysql_free_result(res);

      sprintf(query, "select cnt, ave, max from sensorList where name='%s'", sname);
      res = mysql_perform_query(dbfd, query);
      row = mysql_fetch_row(res);
      sprintf(content, "%s %s %s\n",row[0],row[1],row[2]);
      mysql_free_result(res);
    }
    else{
      sprintf(content,"There is no sensor named sname.\n");
    }
    
  }//GET
  else
  {
    sprintf(query, "select time, value from %s order by idx DESC limit %d", sname, n);
    res = mysql_perform_query(dbfd, query);

    while((row = mysql_fetch_row(res))){
      time_t timeGet = atoi(row[0]);
      sprintf(content, "%s%s%s\n", content , ctime(&timeGet), row[1]);
    }
  }
  mysql_close(dbfd);
  
  /* Generate the HTTP response */
  
  
  printf("%s", content);
  fflush(stdout);
}

int main(void)
{
  //htmlReturn();
  textReturn();
  return (0);
}
