#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <ncurses.h>

using namespace std;

void *Hello(void *arg){
  printw("ja to jo je wontek\n");
refresh();
	return NULL;
}
int main(void){
  initscr();
 pthread_t moj;
if(pthread_create(&moj,NULL,Hello,NULL)){
  printw("cos si zesralo\n"); 
refresh();
}
if(pthread_join(moj,NULL)){
	cout<< "nie umia dojsc\n"<<endl; 
	
}
 printw("i po wszystkim\n");
refresh();
getch();
 endwin();
    return 0; 
}
