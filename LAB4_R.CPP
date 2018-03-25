#define COM3BASE1 0x3E8
#define COM3BASE2 0x3E8
#define TXDATA COM3BASE1
#define LCR (COM3BASE1+3)
#define LSR (COM3BASE2+5)
#define CLOSECMD 17
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void setup_serial(void);
void send_character(int);
int get_character(void);
void send_word(char *);
char* get_word(void);
void send_frame(char, int);

int main(void)
{
	clrscr();
	FILE *fp;
	char *fDir, *inword, *tmp, status;
	int mode, fid, i = 0;
	setup_serial();
	printf("Sender Send : ");
	gets(tmp);
	printf("Save as : ");
	gets(fDir);
       //	fDir = "C:\\TC\\BIN\\test.txt"; //save file to C:\\TC\\BIN
       //	puts(fDir);

	fp = fopen(fDir, "w+");
	if (!fp)
	{
		return(0);
	}
	do
	{

		fid = get_character();
		inword = get_word();
		if(inword[0]=='\0')
			break;
		printf("\n\nReceive frame : %d\n", i);
		printf("Data          : [%s]\n", inword);
		printf("Action frame  : ");
		do
		{
			status = getchar();
		}
		while (status == '\n');
		if (status == 'a')
		{
			if (fid == i)
			{
				printf("Received & Send ACK%d", i+1);
				fputs(inword, fp);
				mode = 1;
				send_character('a');
				send_character(i+1);
			}
			else
			{
				printf("Reject & Send ACK%d", i);
				mode = 0;
				send_character('a');
				send_character(i);
			}

		}
		else if (status == 'n')
		{
			if (fid == i)
			{
				printf("Received & Sleep");
				fputs(inword, fp);
				mode = 1;
			}
			else
			{
				printf("Reject & Sleep");
				mode = 0;
			}
		       //	delay(5000);
		       //	send_character('t');
		}
		else if (status == 'r')
		{
			printf("Reject & Sleep");
			//send_character('t');
			mode = 0;
		}
		else
		{
			printf("Error! [%d - %c]", status, status);
		}

		if (mode == 1)
			i = (i == 0 ? 1 : 0);

	}
	while (inword[0] != '\0');
	//while(1);

	fclose(fp);
	return(0);
}

void setup_serial(void)
{
	outportb(LCR, 0x80);
	outportb(TXDATA, 0x01);
	outportb(TXDATA+1, 0x00);
	outportb(LCR, 0x0B);
}

void send_character(int ch)
{
	char status;
	do
	{
		status = inportb(LSR) & 0x40;
	}
	while (status != 0x40);
	outportb(TXDATA, (char)ch);
}

int get_character(void)
{
	int status;
	do
	{
		status = inportb(LSR) & 0x01;
	}
	while (status != 0x01);
	return((int)inportb(TXDATA));
}

void send_word(char *word)
{
	int i = 0;
	while (word[i] != '\0')
	{
		send_character(word[i++]);
	}
	send_character('\0');
}

char* get_word(void)
{
	int i = 0;
	char *word;
	do
	{
		word[i++] = get_character();
	}
	while (word[i-1] != '\0');
	return(word);
}

void send_frame(int fid, char *word)
{
	send_character(fid);
	send_word(word);
}
