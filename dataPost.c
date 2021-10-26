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
  char *temp;
  char *tempT = NULL;
  char *tempV = NULL;
  char *name = NULL;
  float time;
  float value;
  char query[255];
  
  MYSQL *dbfd;
  MYSQL_RES *res;
  MYSQL_ROW row;

  struct connection_details mysqlD;
  mysqlD.server = "localhost";
  mysqlD.user = "root";//username
  mysqlD.password = "mysql1234";//pw
  mysqlD.database = "sensorDB";//db name

  printf("HTTP/1.0 200 OK\r\n");
  printf("Server: My Web Server\r\n");
  printf("Content-Length: %d\r\n", strlen(astr));
  printf("Content-Type: text/plain\r\n\r\n");
  printf(astr);
  
  read(STDIN_FILENO, astr, MAXBUF);
  //printf(astr);
  
  //인자 자르기
  
  temp = strtok(astr, "=");
  temp = strtok(NULL, "&");
  name = temp;
  
  temp = strtok(NULL, "=");
  temp = strtok(NULL, "&");
  time = atof(temp);

  temp = strtok(NULL, "=");
  temp = strtok(NULL, "&");
  value = atof(temp);
  //
  //printf("%s %f %f\n", name, time, value);
  //db연결
  dbfd = mysql_connection_setup(mysqlD);
  
  //테이블에 튜플 존재여부 확인
  sprintf(query, "select exists (select name from sensorList where name='%s' limit 1) as success", name);
  res = mysql_perform_query(dbfd, query);
  row = mysql_fetch_row(res);
  if(atoi(row[0]) == 1){
      //printf("true\n");
  }
  else{//존재 하지 않을시 튜플 추가      
      mysql_free_result(res);
      sprintf(query, "INSERT INTO sensorList(id, name, cnt, ave, max) VALUES(null, '%s', 0, 0, 0)", name);//sensorList에 추가
      res = mysql_perform_query(dbfd, query);
      mysql_free_result(res);      

      //새로운 테이블 생성
      
      sprintf(query, "CREATE TABLE IF NOT EXISTS `%s` (`idx` INT NOT NULL auto_increment, `time` FLOAT NULL, `value` FLOAT NULL, PRIMARY KEY (`idx`))ENGINE = InnoDB  DEFAULT CHARACTER SET utf8  DEFAULT COLLATE utf8_general_ci;", name);
      res = mysql_perform_query(dbfd, query);
  }
  mysql_free_result(res);
  //

  //센서db에 저장
  sprintf(query, "INSERT INTO %s(time, value, idx) VALUES(%f, %f, null)", name, time, value);
  res = mysql_perform_query(dbfd, query);
  mysql_free_result(res);
  //

  //sensor의 cnt avg max값 추출
  sprintf(query, "select count(*) from %s", name);
  res = mysql_perform_query(dbfd, query);
  row = mysql_fetch_row(res);
  int count = atoi(row[0]);
  mysql_free_result(res);

  sprintf(query, "select avg(value) from %s", name);
  res = mysql_perform_query(dbfd, query);
  row = mysql_fetch_row(res);
  float ave = atof(row[0]);
  mysql_free_result(res);

  sprintf(query, "select MAX(value) from %s", name);
  res = mysql_perform_query(dbfd, query);
  row = mysql_fetch_row(res);
  float max = atof(row[0]);
  mysql_free_result(res);
  //

  //sensorList의 값 업데이트
  sprintf(query, "update sensorList set cnt = %d, ave = %f, max = %f where name = '%s'", count, ave, max, name);
  res = mysql_perform_query(dbfd, query);
  mysql_free_result(res);
  //
  mysql_close(dbfd);
  //

  fflush(stdout);
  return(0);
}
