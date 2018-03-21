#include <ncurses.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MICRO 1000000
#define PITSTOP_TIME 6
#define MAX_X 24
#define MAX_Y 19
#define LAP_LENGTH 90
#define DEFAULT_PITSTOP 3 

struct thread_data_t{
  int id;
  int speed;
  int fuel_laps;
} ;

pthread_mutex_t refresh_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t paint_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pitstop_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t paint_cond = PTHREAD_COND_INITIALIZER;

pthread_rwlock_t cars_mutex;

int lapLength, threadsCount, rc, restCount;
bool pitstop = false;
bool repaint = true;
bool finish = false;
pthread_t * threads;
pthread_t paint_thread;
thread_data_t * threadsParameters;

char lap[MAX_Y + 2][MAX_X + 5];
int ** cars;


//===============================================
//Functions headers
void *car_thread_function(void *arg);
void *paint_thread_function(void *arg);
void init_threads();
void clear_car_threads();
void clear_paint_thread();
void clear_rest();
void print_lap();

//===============================================
int main(int argc, char **argv){
    
    if(argc < 3) {
        printf("[lap length] [thread count] [t1 speed] [t1 fuel] ...");
        return 0;
    }

    lapLength = atoi(argv[1]);
    threadsCount = atoi(argv[2]);

    if(argc < 3 + threadsCount * 2) {
        printf("[lap length] [thread count] [t1 speed] [t3ś fuel] ...");
        return 0;
    } else if (threadsCount < 2 || threadsCount > 7){
        printf("threads count <2,7>");
        return 0;
    }

    restCount = threadsCount;
    threads = new pthread_t[threadsCount];
    threadsParameters = new thread_data_t[threadsCount];

    cars = new int*[threadsCount];
    for(int i=0; i<threadsCount; i++){
        cars[i] = new int[2];
        cars[i][0] = MAX_X;
        cars[i][1] = 0;
    }

    for(int i=3, j=0; j<threadsCount; i += 2, j++){
        threadsParameters[j].id = j+1;
        threadsParameters[j].speed = atoi(argv[i]);
        threadsParameters[j].fuel_laps = atoi(argv[i+1]);
    }

    srand(time(NULL));
    initscr();

    init_threads();
    clear_car_threads();
    clear_paint_thread();
    clear_rest();
    return 0;
}

//===============================================
void init_threads() {

    pthread_rwlock_init(&cars_mutex, NULL);

    rc = pthread_create(&paint_thread, NULL, paint_thread_function, NULL);

    for(int i=0; i<threadsCount; i++) {
        if((rc = pthread_create(&threads[i], NULL, car_thread_function, &threadsParameters[i]))){
            printw("Thread %d init error \n", i) ;
        }
    }   
}

void clear_car_threads(){
    for (int i=0; i<threadsCount; i++) {
        pthread_join(threads[i], NULL);
    }   
    finish = true;
}

void clear_paint_thread(){
    pthread_join(paint_thread, NULL);  
}

//===============================================
void clear_rest() {

    pthread_rwlock_destroy(&cars_mutex);

    pthread_mutex_destroy(&paint_mutex);
    pthread_mutex_destroy(&pitstop_mutex);

    pthread_cond_destroy(&paint_cond);

    delete [] threads;
    delete [] threadsParameters;

    for(int i=0; i<threadsCount; i++){
        delete [] cars[i];
    }
    delete [] cars;

    printw("press any key\n");
    getch();
    endwin();
}

//===============================================
void *car_thread_function(void *arg) {

    thread_data_t *data = (thread_data_t *)arg;
    int fuel = data->fuel_laps;
    int loops = 0;
    int sleep = (int)((lapLength * MICRO / data->speed) / LAP_LENGTH); //max speed of car
    int x = MAX_X; // indexes in tab
    int y = 0; // indexes in tab
    int cid = data->id; // id of car

    bool changed_speed = false; // if speed was changed when someone in pitstop
    int new_speed = 0; // new slower speed
    int len = LAP_LENGTH; // current lap length raced

    while(fuel > 0){
        pthread_rwlock_rdlock(&cars_mutex);
        if(restCount > 1) {
            pthread_rwlock_unlock(&cars_mutex);
            pthread_mutex_lock(&pitstop_mutex);
            if(pitstop) {
                pthread_mutex_unlock(&pitstop_mutex);
                if(!changed_speed) {
                    changed_speed = true;
                    new_speed = (int)(DEFAULT_PITSTOP * MICRO / len);
                    new_speed = new_speed > sleep ? new_speed : sleep;
                }

                usleep(new_speed);

            } else {
                pthread_mutex_unlock(&pitstop_mutex);
                if(changed_speed) changed_speed = false;
                usleep(sleep);
            }

            len--;

            if(x > 0 && y == 0) { x--; }
            else if(x == 0 && y < MAX_Y) { y++; }
            else if(x < MAX_X && y == MAX_Y) { x++; }
            else if(x == MAX_X && y > 0 ) { y--; }

            if(x == MAX_X && y ==0) {
                loops++;
                fuel--;
                len = LAP_LENGTH;
                new_speed = sleep;
                
                pthread_mutex_lock(&pitstop_mutex);
                if(!pitstop) {
                    pitstop = true;
                    pthread_mutex_unlock(&pitstop_mutex);



                    pthread_mutex_lock(&paint_mutex);
                    cars[cid-1][0] = MAX_X + 2;
                    cars[cid-1][1] = 0;
                    repaint = true;
                    pthread_cond_signal(&paint_cond);
                    pthread_mutex_unlock(&paint_mutex);

                    usleep(((rand() % PITSTOP_TIME) +1) * MICRO);
                    fuel = data->fuel_laps;

                    pthread_mutex_lock(&pitstop_mutex);
                    pitstop = false;    
                    pthread_mutex_unlock(&pitstop_mutex);

                } else {
                    pthread_mutex_unlock(&pitstop_mutex);
                    pthread_mutex_lock(&paint_mutex);
                    cars[cid-1][0] = x;
                    cars[cid-1][1] = y;
                    repaint = true;
                    pthread_cond_signal(&paint_cond);
                    pthread_mutex_unlock(&paint_mutex); 
                }


            } else {
                pthread_mutex_lock(&paint_mutex);
                cars[cid-1][0] = x;
                cars[cid-1][1] = y;
                repaint = true;
                pthread_cond_signal(&paint_cond);
                pthread_mutex_unlock(&paint_mutex);            
        }
    } else {
        pthread_rwlock_unlock(&cars_mutex);
        break;
    }

        

    }
    
    pthread_rwlock_wrlock(&cars_mutex);
    if(restCount > 1) {
        restCount--;
        pthread_rwlock_unlock(&cars_mutex);

        pthread_mutex_lock(&paint_mutex);
        cars[cid-1][0] = -1;
        cars[cid-1][1] = -1;
        repaint = true;
        pthread_cond_signal(&paint_cond);
        pthread_mutex_unlock(&paint_mutex);
    } else {
        pthread_rwlock_unlock(&cars_mutex);

        pthread_mutex_lock(&paint_mutex);
        repaint = true;
        pthread_cond_signal(&paint_cond);
        pthread_mutex_unlock(&paint_mutex);        
    }

    pthread_exit(NULL);
}

//===============================================
void *paint_thread_function(void *arg){
    while(!finish){
        pthread_rwlock_rdlock(&cars_mutex);
        if(restCount > 1) {
            pthread_rwlock_unlock(&cars_mutex);
            pthread_mutex_lock(&paint_mutex);
            while(!repaint) pthread_cond_wait(&paint_cond, &paint_mutex);
            print_lap();
            repaint = false;
            pthread_mutex_unlock(&paint_mutex);
        } else {
            pthread_rwlock_unlock(&cars_mutex);
        }
    }

    for(int i=0; i<threadsCount; i++){
        if(cars[i][0] != -1 && cars[i][1] != -1) {
            printw("Car %d win!!!! \n", i+1);
            refresh();
            break;
        }
    }


    pthread_exit(NULL);
}

//===============================================
void print_lap(){
    clear();
    strcpy(lap[0], "-------------------------[ ]");
    for(int i=1; i<20;i++){
        strcpy(lap[i], "|                       |");
    }
    strcpy(lap[MAX_Y], "-------------------------");

    for(int i=0; i<threadsCount; i++){
        if(cars[i][0] != -1 && cars[i][1] != -1) {
            lap[cars[i][1]][cars[i][0]] = (char)(49 + i);
        }
    }

    for(int i=0; i<MAX_Y + 1; i++){
        printw("%s\n",lap[i]);
    }
    refresh();

}