#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termio.h>
#include <time.h>

#define EV_MAX 6
#define FLOOR_MAX 20
#define SECONDLINE 50
#define WAIT 2
#define HISTORY_MAX 5
#define QUE_MAX 10
#define abs(x) ((x)>0 ? (x) : -(x))

pthread_t thread;
pthread_t thread2;

void *thread_timeflow();//시간흐름에 따른 EV상태변화를 주고 결과를 출력하는  thread
void *thread_show(); //timeflow 멈추었을때 출력을 위한 thread

void print_menu();//메뉴보여주는 함수(이동,프로그램종료,시뮬레이션정지 등)
void print_EV();//EV이미지 보여주는 함수
void print_log();//EV log 보여주는 함수(5개정도?)
int choice_EV(int from,int to);//사용자 입력에  따라 EV선택,기록하는 함수
int movement_define(int EVnum);//EV움직임정의(상승:1, 정지:0, 하강:-1)

void history_message(int from, int to, int c,int waitnum);
void clear(int startline,int endline);
void strclear(int start,int end);
void gotoxy(int x,int y);
int getch(void);


void enQue(int from, int to);
void deQue(void);
int QisEmpty(void);

struct Queue {
	int from;
	int to;
	int num;
};

struct Queue Que[QUE_MAX];
struct Queue deq;

int front = 0;
int rear = 1;
int number = 0;


struct elevator {

	int current;
	int stop[FLOOR_MAX];
	int people;
	int dooropen; //열릴경우가 아니라면 0	
};

struct elevator EV[EV_MAX];

int sec=0;

char str[3][SECONDLINE]={0,};
char history[HISTORY_MAX][2][SECONDLINE]={0,};
int history_new=0;


int main(){
	
	int menu=99; //몇번 메뉴를 선택했는지
	int from,to; //EV 요청층과 목적층
	int c;
	
	int simul_work=0;


	//초기화
	for(int i=0;i<EV_MAX;i++){
		EV[i].current=0;
		EV[i].people=0;
		for(int j=0;j<FLOOR_MAX;j++){
			EV[i].stop[j]=0;
		}
	}
        clear(0,100);	
	
	pthread_create(&thread,NULL,&thread_timeflow,NULL);
	simul_work=1;
	strcpy(str[0],"ELEVATOR ON");
        
	print_EV();
        print_menu();
	
//	pthread_create(&thread2,NULL,&thread_show,NULL);

	while(menu!=0) //menu가 0이면 프로그램 종료
	{
		menu=getch()-'0';
		
		switch(menu){
			case 1:	//시뮬레이션시작
				strclear(1,2);
				if(simul_work==0){		
					strcpy(str[0],"ELEVATOR ON");
					pthread_cancel(thread2);
					pthread_create(&thread,NULL,&thread_timeflow,NULL);
					simul_work=1;}
				break;
			case 2: //시뮬레이션 정지
				strclear(1,2);
				if(simul_work==1) {
					pthread_cancel(thread);
					strcpy(str[0],"ELEVATOR OFF");
					pthread_create(&thread2,NULL,&thread_show,NULL);
				//print_log();
					simul_work=0;}
				break;
			case 3: //EV이동요청
				strncpy(str[1],"3.현재층과 이동층 입력:            ",SECONDLINE-1);
				strclear(2,2);
				while(1){
					from=getch()-'a';
					
					strncpy(str[2],"                              ",SECONDLINE-1);
					str[2][0]=from+'a';
					str[2][1]='>';
					to=getch()-'a';
					str[2][2]=to+'a';
					
					if(from<20&&from>=0&&to<20&&to>=0&&from!=to) break;
					else strncpy(&str[2][3],"     !층선택error! 재입력           ",SECONDLINE-1);
				}
				
				history_message(from,to,choice_EV(from,to),-1);
				break;
			
			default://error message
				strclear(1,2);
				strncpy(str[1],"!메뉴선택error!재입력                 ",SECONDLINE-1);
				break;
		}
	}

	clear(0,100);
	printf("엘리베이터 프로그램이 종료 되었습니다.\n");
	pthread_cancel(thread);
	//pthread_cancel(thread2);
	return 0;
}

void strclear(int start,int end){
	for(int i=start;i<=end;i++)
	strncpy(str[i],"                              ",SECONDLINE-1);
}
void history_message(int from, int to, int c,int waitnum){
	
	static int num=0;	//history num, 0<=num<HISTORY_MAX
	int count=0;


	for(int i=0;i<SECONDLINE;i++)
	{
		history[num][0][i]='\0';
		history[num][1][i]='\0';
	}
	if(waitnum>0) {
		strcpy(&history[num][0][0],"대기 . ");
		history[num][0][6]=waitnum+'0';
		count=9;	
	}

	history[num][0][count++]=from+'a';
	history[num][0][count++]='>';
	history[num][0][count++]=to+'a';
	strcpy(&history[num][0][count]," 이동요청:");
	

	history[num][1][0]=c+'0';

	if(c<2) strcpy(&history[num][1][1],"번ELEVATOR(저층용)응답");
	else if(c<4) strcpy(&history[num][1][1],"번ELEVATOR(전층용)응답");
	else if(c<6) strcpy(&history[num][1][1],"번ELEVATOR(고층용)응답");
	else {
		strcpy(history[num][1],"엘리베이터 응답 대기중    ");
	}

	history_new=num;
	num++;
	if(num>HISTORY_MAX) num=0;
	
}

void *thread_timeflow(){

	int flag=0;//flag==0인경우 EV 움직임이 없다
        


	while(1){

		for(int i=0;i<EV_MAX;i++){
                        for(int j=0;j<FLOOR_MAX;j++){
                                flag+=EV[i].stop[j];
                        }
                }
		sleep(1);
		sec++;
                print_log();
                
		if(flag!=0){
                        print_EV();
                }

		for(int j=0;j<EV_MAX;j++){
			int d;

			if(EV[j].stop[EV[j].current]>0){	//현재층이 멈춰야할층
				if(EV[j].stop[EV[j].current]==2) EV[j].people++; //출발층이라면
				if(EV[j].people>0){
				       	EV[j].stop[EV[j].current]=0; //사람이 이동이 있는 경우만  방문층제거
					EV[j].dooropen=WAIT;
				}
			}

			if(EV[j].dooropen==0){
			
				d = movement_define(j);		
				if(d>0) EV[j].current++; 
				else if(d<0) EV[j].current--; //하강
				else EV[j].people=0; //정지한다면 사람들 다 내림
			}
			else {EV[j].dooropen--;}
		}
		
		if(QisEmpty()==0){//대기하던 요청들꺼내기
			int temp;
			deQue();
			temp=choice_EV(deq.from,deq.to);
			if(temp<EV_MAX||temp>=0) history_message(deq.from,deq.to,temp,deq.num);
			//EV를 배정받은경우 출력
		}

	}
	
}
	
void *thread_show(){
	while(1){	
		sleep(1);	
		print_log();
		print_EV();
	}
}

int choice_EV(int from,int to){

	int choice=1;
	
	//...
	EV[choice].stop[from]=2;
	EV[choice].stop[to]=1;
	return choice;
	
}
void print_menu(){
	int x=0,y=2;
	gotoxy(x,y++); 
	printf("==================================\n");
	gotoxy(x,y++); 
	printf("1. 시뮬레이션 시작\n");
	gotoxy(x,y++); 
	printf("2. 시뮬레이션 정지\n");
	gotoxy(x,y++); 
	printf("3. EV 이동요청\n");
	//gotoxy(x,y++); 
	//printf("4. 엘레베이터 고치기\n");
	gotoxy(x,y++); 
	printf("0. 종료\n");
	gotoxy(x,y++); 
	printf("==================================\n");
	gotoxy(x,y++); 

}
void print_EV(){

	int x=SECONDLINE,y=1;
	gotoxy(x,y++);
	printf("__________________________________________\n");	
	gotoxy(x,y++);
	printf("|-----||-----||-----||-----||-----||-----|\n");


	for(int i=FLOOR_MAX-1;i>=0;i--){ 
		gotoxy(x,y++);
		for(int j=0;j<EV_MAX;j++){
			
			

			printf("│ ");
			//방문할 층 확인하기
			if(EV[j].stop[i]>0){	
				printf("*");
			}
			else printf(" ");
	
			if(i==EV[j].current){
				if(EV[j].dooropen > 0 && EV[j].dooropen < WAIT)
					printf("□  │"); //문열림
				else{
					
					int d = movement_define(j);		
					if(d>0) printf("▲  │"); //가야할 곳이 현재층(i)보다 크다=상승
					else if(d<0) printf("▼  │"); //작다=하강
					else {
						printf("■  │"); //같다면 정지중
					}
				}

			}else{

				printf("   │");

			}

		}
		gotoxy(x,y++);
		printf("|-----||-----||-----||-----||-----||-----| %2dfloor(%c)\n",i+1,i+'a');

	}

}

int movement_define(int EVnum){

	int depart=EV[EVnum].current;
	int count=0,low=0,high=0;

	for(int i=0;i<FLOOR_MAX;i++){ 
		if(EV[EVnum].stop[i]>0) {
			count++;
			if(EV[EVnum].stop[i]==2) depart=i;
			
			if(i<EV[EVnum].current) low++;
			else if(i>EV[EVnum].current) high++;

		}
	}

	if(count==0) return 0;
	else{
		if(EV[EVnum].current!=depart){//출발층이 존재
			if(EV[EVnum].current<depart) return 1;//상승
			else return -1;//하강
	
		}else{
			if(low>high) return -1;		//하강
			else if(low<high) return 1;	//상승
			else return 0;
		}
	}

	

}



void print_log(){
	int x=0,y= 8;
	int startstr;
	gotoxy(x,1);printf("%s(%d) ",str[0],sec);
	gotoxy(x,y++); printf("원하는 메뉴의 번호를 누르세요");
	gotoxy(x,y++); printf("%s",str[1]);
	gotoxy(x,y++); printf("%s",str[2]);
	y++;
	gotoxy(x,y++); printf("****history****");
	
	startstr=history_new;
	for(int i=0;i<HISTORY_MAX;i++){
		if(startstr<0) startstr=HISTORY_MAX;
		gotoxy(x,y++); printf("%s%s",history[startstr][0],history[startstr][1]);
		startstr--;
	}



	//strncpy(str[1],"                                 ",SECONDLINE-1);
}

void enQue(int from, int to)
{
	number++;
	Que[rear].from = from;
	Que[rear].to = to;
	Que[rear].num = number;
	rear++;
	rear = rear % 10;

}


void deQue(void)
{
	front++;
	front = front % QUE_MAX;
	deq.from = Que[front].from;
	deq.to = Que[front].to;
	deq.num = Que[front].num;
}

int QisEmpty(void)
{
	if ((rear - front) % 10 == 1)
		return 1;
	else
		return 0;

}

void clear(int startline,int endline)
{
	int i;
	gotoxy(0,0);
	for(i=startline;i<=endline;i++)
	printf("                                                                                \n");
	gotoxy(0,1);
}
void gotoxy(int x, int y){

	printf("\033[%d;%df",y,x);
	fflush(stdout);

}
int getch(void){
	int ch;
	struct termios buf, save;
	tcgetattr(0,&save);
	buf=save;
	buf.c_lflag&=~(ICANON|ECHO);
	buf.c_cc[VMIN]=1;
	buf.c_cc[VTIME]=0;
	tcsetattr(0,TCSAFLUSH,&buf);
	ch=getchar();
	tcsetattr(0,TCSAFLUSH,&save);
	return ch;
}
