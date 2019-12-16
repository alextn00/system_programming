#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <curses.h>

/* 타이머 */
#define CCHAR 0
#ifdef CTIME
#undef CTIME
#endif
#define CTIME 1

/* 게임 시작, 종료 설정*/
#define GAME_START 0
#define GAME_END 1

static struct result
{
	char name[30];
	long nScore;
	int year;
	int month;
	int day;
	int hour;
	int min;
}temp_result;

int nScore = 0;
int best_nScore = 0;
char strScore[20] = "  ";
int nCombo = 0;
int game = GAME_END;
char result[50] = "  ";

void Map();
void ReadyMap();
void ScoreMap();
void node();
int getkey(void);
int print_result();
void execute(char *[]);

void main()
{
	int menu = 1, ch;
	char *arglist[3] = { "paplay", "Good_Times.wav", 0 };
	time_t ptime;
	struct tm *t;
	FILE *fp = NULL;
	while (menu) {
		char buf[100];
		initscr();
		clear();
		Map();
		ReadyMap();
		refresh();
		menu = getkey();
		if (menu == 49) {
			initscr();
			clear();
			Map();
			ScoreMap();
			refresh();
			execute(arglist);
			for (int i = 0; i < 38; i++) {
				node();
				refresh();

				if (i == 37) {
					// 기록 파일로 저장
					move(31, 0);
					sprintf(buf, "your score is : %d\n", nScore);
					addstr(buf);
					refresh();
					move(32, 0);
					addstr("what's your name? ");
					refresh();
					scanf("%s", temp_result.name);
					temp_result.nScore = nScore;

					if (temp_result.nScore >= best_nScore)
						best_nScore = temp_result.nScore;

					/*시간 저장 부분*/
					ptime = time(NULL); // 현재 시각을 초 단위로 얻기
					t = localtime(&ptime); // 초 단위의 시간을 분리하여 구조체에 넣기

					temp_result.year = t->tm_year + 1900;
					temp_result.month = t->tm_mon + 1;
					temp_result.day = t->tm_mday;
					temp_result.hour = t->tm_hour;
					temp_result.min = t->tm_min;

					fp = fopen("result", "ab");//추가할때는 ab
					fseek(fp, 1, SEEK_END); //파일속에서 위치를 찾는 함수(파일명,이동할바이트,SEEK_SET(시작점),CUR(읽고있는),END(끝))
					fwrite(&temp_result, sizeof(struct result), 1, fp);
					fclose(fp);
					nScore = 0;
					nCombo = 0;
				}
			}

		}
		else if (menu == 50) {
			print_result();
		}
		else if (menu == 51) {
			exit(0);
		}

	}
	endwin();

}

void execute(char *arglist[])
{
	int pid, exitstatus = -1;

	pid = fork();
	switch (pid) {
	case -1: exit(1);
	case 0: execvp(arglist[0], arglist);
	default: return;
	}
	return;
}
// 스테이지 기본 틀
void Map()
{
	move(0, 0);
	addstr("*****************************************");
	for (int i = 1; i < 29; i++) {
		move(i, 0);
		addstr("*\t\t\t\t\t*");
	}
	move(29, 0);
	addstr("*****************************************");
}

// 시작화면
void ReadyMap() {
	move(10, 14);
	addstr("Rhythm Game");
	move(13, 16);
	addstr("1. Start");
	move(14, 16);
	addstr("2. Result");
	move(15, 16);
	addstr("3. Exit");
	move(26, 2);
	addstr("  ******   ******   ******   ******  ");
	move(27, 2);
	addstr("    A        S        D        F    ");
	move(30, 1);
}


// 우측 점수 출력틀
void ScoreMap() {
	int ch;
	// 점수 목록
	move(10, 44);
	addstr(strScore);
	move(22, 44);
	addstr("Return to the Menu : Q(q)");
	// 점수
	char nScoree[20];
	sprintf(nScoree, "score : %d point", nScore);
	move(4, 44);
	addstr(nScoree);
	move(25, 44);
	addstr("<<< Hit Section(Great)");
	move(26, 44);
	addstr("<<< Hit Section(Perfect)");
	move(27, 44);
	addstr("<<< Hit Section(Great)");

	//콤보
	char strCombo[20];
	sprintf(strCombo, "%d combo", nCombo);
	move(13, 44);
	addstr(strCombo);


}

/*문자를 바로 입력 받을 수 있는 함수*/
int getkey(void)
{
	char   ch;
	int   error;
	static struct termios Otty, Ntty;

	fflush(stdout);
	tcgetattr(0, &Otty);
	Ntty = Otty;
	Ntty.c_iflag = 0;
	Ntty.c_oflag = 0;
	Ntty.c_lflag &= ~ICANON;
#if 1
	Ntty.c_lflag &= ~ECHO;
#else
	Ntty.c_lflag |= ECHO;
#endif
	Ntty.c_cc[VMIN] = CCHAR;
	Ntty.c_cc[VTIME] = CTIME;

#if 1
#define FLAG TCSAFLUSH
#else
#define FLAG TCSANOW
#endif

	if (0 == (error = tcsetattr(0, FLAG, &Ntty)))
	{
		error = read(0, &ch, 1);
		error += tcsetattr(0, FLAG, &Otty);
	}

	return (error == 1 ? (int)ch : -1);
}

void node() {
	int row;
	int i, j;
	int ch;

	row = (rand() % 4) * 6 + 4;

	if (row == 10) row = 13;
	else if (row == 16) row = 22;
	else if (row == 22) row = 31;


	for (i = 1; i<28;i++) {
		move(i, row);
		addstr("******");
		refresh();
		usleep(1000 * 10);
		move(i, row);
		addstr("      ");
		move(26, 0);
		addstr("*****************************************");
		ch = getkey();

		if (ch == 'a' || ch == 'A') {
			if (row == 4 && i == 26) { nScore += 500; nCombo += 1; sprintf(strScore, "%s", "Perfect!"); ScoreMap(); }
			else if (row == 4 && i >= 25 && i <= 27) { nScore += 300; nCombo += 1; sprintf(strScore, "%s", "Great!  "); ScoreMap(); }
		}
		else if (ch == 's' || ch == 'S') {
			if (row == 13 && i == 26) { nScore += 500; nCombo += 1; sprintf(strScore, "%s", "Perfect!"); ScoreMap(); }
			else if (row == 13 && i >= 25 && i <= 27) { nScore += 300; nCombo += 1; sprintf(strScore, "%s", "Great!  "); ScoreMap(); }
		}
		else if (ch == 'd' || ch == 'D') {
			if (row == 22 && i == 26) { nScore += 500; nCombo += 1; sprintf(strScore, "%s", "Perfect!"); ScoreMap(); }
			else if (row == 22 && i >= 25 && i <= 27) { nScore += 300; nCombo += 1; sprintf(strScore, "%s", "Great!  "); ScoreMap(); }
		}
		else if (ch == 'f' || ch == 'F') {
			if (row == 31 && i == 26) { nScore += 500; nCombo += 1; sprintf(strScore, "%s", "Perfect!"); ScoreMap(); }
			else if (row == 31 && i >= 25 && i <= 27) { nScore += 300; nCombo += 1; sprintf(strScore, "%s", "Great!  "); ScoreMap(); }
		}
		else if (ch == 'q' || ch == 'Q') {
			main();
		}

	}

}

/* 메뉴에서 기록출력시 호출되어 기록을 출력하는 함수*/
int print_result(void)
{
	FILE *fp = NULL;
	char ch = 1, i = 10;

	fp = fopen("result", "rb");

	if (fp == NULL)
		return 0;

	system("clear");
	move(1, 19);
	addstr("Rhythm Game");
	move(2, 22);
	addstr("Result");
	move(4, 13);
	addstr("Return to the Menu : Q(q)");
	move(7, 8);
	addstr("Name\tScore\t    Date\tTime");
	move(8, 4);
	addstr("  ==========================================");

	while (1)
	{
		fread(&temp_result, sizeof(struct result), 1, fp);
		if (!feof(fp))
		{
			move(i, 8);
			char result[100];
			sprintf(result, "%s\t%ld\t%d. %d. %d.\t%d : %d\n", temp_result.name, temp_result.nScore, temp_result.year, temp_result.month, temp_result.day, temp_result.hour, temp_result.min);

			addstr(result);
			refresh();
			for (int j = 0; j <100; j++) result[i] = '\0';
			i++;

		}
		else break;

	}

	fclose(fp);
	while (1)
	{
		move(5, 8);
		ch = getkey();
		if (ch == 81 || ch == 113) break;
	}
	return 0;

}
