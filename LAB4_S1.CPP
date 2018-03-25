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

void setup_serial(void);
void send_character(int);
int get_character(void);
void send_word(char *);
char* get_word(void);
void send_frame(int, char *);

int main(void)
{
	clrscr();
	FILE *fp;
	char *fDir, *outword, status;
	int mode = 1, i = 0, n, isEOF = 0;
	setup_serial();

	printf("Send file :");
	fDir = "C:\test.txt";
	//gets(fDir);
	printf("%s",fDir);

	fp = fopen(fDir, "r");
	if (!fp)
	{
		return(0);
	}
	printf("\n");
	do
	{
		// if (mode == 1)
			// fgets(outword, 255, fp);
		// outword[strlen(outword)-1] = '\0';
		if (mode == 1)
		{
			for (n = 0; n < 7; n++)
			{
				//fscanf(fp, "%c", &outword[n]);
				outword[n] = fgetc(fp);
				if (outword[n] == EOF)
				{
					for (int j = n; j < 7; j++)
						outword[j] = '\0';
					isEOF = 1;
					break;
				}
			}
			outword[n] = '\0';
		}

		send_frame(i,outword);
		printf("\n\nSend frame : %d\n", i);
		printf("Data       : [%s]\n", outword);
		printf("Timeout    : ");
		status = get_character();
		if (status == 'a')
		{
			printf("\nReceive ACK%d", get_character());
			mode = 1;
			i = (i == 0 ? 1 : 0);
		}
		else if (status == 't')
		{
			printf("t\nRetransmit frame %d", i);
			mode = 0;
		}
		else //DEBUG
		{
			printf("Error! [%d - %c]", status, status);
		}
		//getch(); //DEBUG
	}
	while (!isEOF);

	outword[0] = '\0';
	outword[1] = '\0';
	send_frame((i == 0 ? 1 : 0), outword);

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
		send_character(word[i]);
		i++;
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