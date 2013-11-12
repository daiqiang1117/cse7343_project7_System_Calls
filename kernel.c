/*	kernel.c
	A simple kernel that displays "Hello World"
	
	Qiang Dai
*/

/* Global Constant Variables*/
#define LENGTH_LINE 80

/* Function Prototypes*/
void printString(char* word);
void readString(char* line);



/* Global Variables*/


/* The main entry point of the program*/
int main()
{
	/* todo
	 * read a line from keyboard, maximum is 80 charaters
	 * print this line to the screen
	 */
	char line[LENGTH_LINE];
	printString("Enter a line: \0");
	readString(line);
	printString(line);

	while(1)
	{
		
	}
	return 0;
}

void printString(char* word)
{
	/* C style string end with 0
	 * if the character of word isn't 0 do loop
	 * else return
	 */
	/* start at a new line*/
	char al = '\n';
	char ah = 0x0e;
	int ax = ah * 256 + al;
	interrupt(0x10, ax, 0, 0, 0);

	while(*word != '\0')
	{
		/* INT 10/ AH = 0Eh - video - teletype output */
		al = *word;
		ax = ah * 256 + al;
		interrupt(0x10, ax, 0, 0, 0);
		word++;
	}
	return;
}

void readString(char* line)
{
	/* read a line from keyboard input
	 * end with the ENTER key whose ASCII is 0xd
	 * get this line and store in the line buffer
	 * deal with backspace and end characters
	 */

	int i = 0;
	char temp = 0;
	while(1)
	{
                /* the AH must equal 0 and AL doesn't matter */
		temp = interrupt(0x16, 0, 0, 0, 0);
		/* only if the input is "ENTER", the loop ends*/
		if( temp == 0xd)
			break;

		/* if input is backspace, only diplay not store and index minus one */
		if(temp == 0x8)
		{
			if(i > 0)
			{
				interrupt(0x10, 0x0e * 256 + temp, 0, 0, 0);
				i--;
			}
			else
				continue;
		}

		/* store and display normal characters*/
		else
		{
			line[i++] = temp;
			interrupt(0x10, 0x0e * 256 + temp, 0, 0, 0);	
		}
	}

	/* remeber to set the last two characters as 0xa and 0x0 */
	if(i > LENGTH_LINE - 2)
		i = LENGTH_LINE - 2;
	line[i++] = 0xa;
	line[i] = '\0';
	return;
}
