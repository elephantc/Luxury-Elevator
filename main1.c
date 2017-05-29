//main문 자체가 실행이 안되서 수정중


#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#define EV_MAX 6
#define FLOOR_MAX 20


//pthread_t thread;
//pthread_t thread2;

//void *thread_timeflow();//시간흐름에 따른 EV상태변화
//void *thread_show(); //모든 출력제어

void print_menu();//메뉴보여주는 함수(이동,프로그램종료,시뮬레이션정지 등)
void print_EV();//EV이미지 보여주는 함수
//void print_log();//EV log 보여주는 함수(5개정도?)
//void choice_EV(int from,int to);//사용자 입력에  따라 EV선택,기록하는 함수

//void gotoxy(int x,int y);
int map[20][6];

struct elevator {

	int current;
	int stop[FLOOR_MAX];
	char direction;
};

struct elevator EV[EV_MAX];

int main(void){
	printf("start");
	int menu; //몇번 메뉴를 선택했는지
	int from,to; //EV 요청층과 목적층
	
	int i,j;
	
	int simul_work=0;

	printf("a1");
		
	//초기화
	for(i=0;i<EV_MAX;i++){
		EV[i].current=0;
		EV[i].direction = 'e';
		for(j=0;j<FLOOR_MAX;j++){
			EV[i].stop[j]=0;
		}
	}	
	
	printf("a2");
	
	// (추가함) map초기화
	for(i=0;i<FLOOR_MAX;i++){
		for(j=0;j<EV_MAX;j++){
			if(i=0){
				map[FLOOR_MAX][EV_MAX]=1;//1층set
			}else{
				map[FLOOR_MAX][EV_MAX]=0;//empty
			}
		}
	}
	
	printf("a3");
		
	
	//pthread_create(&thread2,NULL,&thread_show,NULL);

	while(menu!=-1) //menu가 -1이면 프로그램 종료
	{
		printf("t1");
		print_EV();
		printf("t2");
		print_menu(from);
		printf("t3");
		scanf("%d",&menu);
	
		switch(menu){
			case 1:	//시뮬레이션시작
				if(simul_work==0){
					//pthread_create(&thread,NULL,&thread_timeflow,NULL);
					simul_work=1;}
				break;
			case 2: //시뮬레이션 정지
				//pthread_cancel(thread);
				simul_work=0;
				break;
			case 3: //EV이동요청
				scanf("%d %d",&from, &to);
				//choice_EV(from,to);
				break;
			
			default: //error message
				break;
		}
	}

	//pthread_cancel(thread);
	//pthread_cancel(thread2);
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
			
			print_menu(0);
			//print_menu();
			flag=0;
		}
	}
};
/*
void choice_EV(int from,int to){

	int choice;
	
	//...
	
	EV[choice].stop[from]=1;
	EV[choice].stop[to]=0;

	
}
*/

void print_menu(int from){
	
	
	
	//dist를 찾는다!!
	for(int i=0; i<EV_MAX;i++){
		printf("Elevator%d : dist = %d, ",i+1, from-EV[i].current);
		//abs(from-EV[i].current)
		
	switch(EV[i].direction){
		
		
		case 'e':
		printf("타고 있지 않습니다.\n");
		break;
		
		case 'u':
		printf("올라가고 있습니다.\n");
		break;
		
		case 'd':
		printf("내려가고 있습니다.\n");
		break;
		
		default:
		printf("타고 있지 않습니다.\n");
		break;
		}
		
	}
	
	printf("=================================\n");
	printf("1. 올라가기\n");
	printf("2. 내려가기\n");
	printf("3. 시뮬레이션 정지\n");
	printf("4. VIP모드\n");
	printf("5. 엘레베이터 고치기\n");
	printf("6. 종료\n");
	printf("==================================\n");

	printf("메뉴선택 : ");
}

void print_EV(){
	//char* state[EV_MAX];
	
	for(int i=FLOOR_MAX-1;i>=0;i--){ //층수표기의 혼동방지
		printf("┌---------------┐┌---------------┐┌---------------┐┌---------------┐┌---------------┐┌---------------┐\n");	
		for(int j=0;j<EV_MAX;j++){
			if(map[i][j]==1){
				printf("│    %2d %2d %c    │",EV[j].current,EV[j].stop[0],EV[j].direction);
			}else{
				printf("│               │");
			}
		}
		printf("└---------------┘└---------------┘└---------------┘└---------------┘└---------------┘└---------------┘\n");
	}
	
}
/*
void gotoxy(int x, int y){

	printf("\099[]%d;%df",y,x);
	fflush(stdout);

}
*/