#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include "/usr/include/mysql/mysql.h"
#define _CRT_SECURE_NO_WARNINGS
//
// This program is intended to help you test your web server.
// 

//mysql을 실행하기 위한 함수
struct connection_details {
 
    char *server;
    char *user;
    char *password;
    char *database;
 
};
 
MYSQL* mysql_connection_setup(struct connection_details mysql_details) {
 
    MYSQL *connection = mysql_init(NULL);
 
    if(!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0)) {
 
        printf("Connection error : %s\n", mysql_error(connection));
        exit(1);
 
    }
    return connection;
}
 
MYSQL_RES* mysql_perform_query(MYSQL *connection, char *sql_query) {
 
    if(mysql_query(connection, sql_query)) {
 
        printf("MYSQL query error : %s\n", mysql_error(connection));
        exit(1);
 
    }
    return mysql_use_result(connection);
}


//출처: https://knoow.tistory.com/72 [ICT Story]

//
int main(int argc, char *argv[])
{
  char astr[MAXBUF];
  
  
  MYSQL *dbfd;
  MYSQL_RES *res;
  MYSQL_ROW row;

  struct connection_details mysqlD;
  mysqlD.server = "localhost";
  mysqlD.user = "root";//username
  mysqlD.password = "mysql1234";//pw
  mysqlD.database = "testDB";//db name

  dbfd = mysql_connection_setup(mysqlD);

  res = mysql_perform_query(dbfd, "show tables");

  printf("HTTP/1.0 200 OK\r\n");
  printf("Server: My Web Server\r\n");
  printf("Content-Length: %d\r\n", strlen(astr));
  printf("Content-Type: text/plain\r\n\r\n");


  //printf("MySQL Tables in mysql database:\n");
  while((row = mysql_fetch_row(res)) != NULL)
      printf("%s\n", row[0]);

  mysql_free_result(res);
  mysql_close(dbfd);
  
  
  read(STDIN_FILENO, astr, MAXBUF);
  /*
  printf("HTTP/1.0 200 OK\r\n");
  printf("Server: My Web Server\r\n");
  printf("Content-Length: %d\r\n", strlen(astr));
  printf("Content-Type: text/plain\r\n\r\n");
  printf(astr);*/
  fflush(stdout);
  return(0);
}
