#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define EV_MAX 6
#define FLOOR_MAX 20


pthread_t thread;
pthread_t thread2;

void *thread_timeflow();//시간흐름에 따른 EV상태변화
void *thread_show(); //모든 출력제어

void print_menu();//메뉴보여주는 함수(이동,프로그램종료,시뮬레이션정지 등)
void print_EV();//EV이미지 보여주는 함수
void print_log();//EV log 보여주는 함수(5개정도?)
void choice_EV(int from,int to);//사용자 입력에  따라 EV선택,기록하는 함수

void gotoxy(int x,int y);

struct elevator {

	int current;
	int stop[FLOOR_MAX];
};

struct elevator EV[EV_MAX];

int main(){
	
	int menu; //몇번 메뉴를 선택했는지
	int from,to; //EV 요청층과 목적층
	
	int i,j;
	
	int simul_work=0;

	//초기화
	for(i=0;i<EV_MAX;i++){
		EV[i].current=0;
		for(j=0;j<FLOOR_MAX;j++){
			EV[i].stop[j]=0;
		}
	}
	
	pthread_create(&thread2,NULL,&thread_show,NULL);

	while(menu!=0) //menu가 0이면 프로그램 종료
	{
		scanf("%d",&menu);
	
		switch(menu){
			case 1:	//시뮬레이션시작
				if(simul_work==0){
					pthread_create(&thread,NULL,&thread_timeflow,NULL);
					simul_work=1;}
				break;
			case 2: //시뮬레이션 정지
				pthread_cancel(thread);
				simul_work=0;
				break;
			case 3: //EV이동요청
				scanf("%d %d",&from, &to);
				choice_EV(from,to);
				break;
			
			default://error message
				break;
		}
	}

	pthread_cancel(thread);
	pthread_cancel(thread2);
	return 0;
}

void *thread_timeflow(){
};
void *thread_show(){


	int flag=0;//flag==0인경우 EV 움직임이 없다
        int i,j;
	while(1){
		for(i=0;i<EV_MAX;i++){
			for(j=0;j<FLOOR_MAX;j++){
				flag+=EV[i].stop[j];
			}
  		}

		if(flag!=0){
			print_EV();
			print_menu();
			flag=0;
		}
	}
};

void choice_EV(int from,int to){

	int choice;
	
	//...
	
	EV[choice].stop[from]=1;
	EV[choice].stop[to]=0;

	
}
void print_menu(){

}
void print_EV(){



}

void gotoxy(int x, int y){

	printf("\099[]%d;%df",y,x);
	fflush(stdout);

}


int find_Ev(int input1,int input2){

}


