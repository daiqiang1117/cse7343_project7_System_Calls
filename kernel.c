/*	kernel.c
	A simple kernel that displays "Hello World"
	
	Qiang Dai
*/

/* macro */
#define LENGTH_LINE 80
#define LENGTH_SECTOR 512

/* Function Prototypes*/
void printString (char* word);
void readString (char* line);
void readSector (char* buffer, int sector);
int mod (int dividend, int divisor);
int div (int dividend, int divisor);
void handleInterrupt21(int ax, int bx, int cx, int dx);

int j = 0;

/* The main entry point of the program*/
int main ()
{
	/* todo
	 * read a line from keyboard, maximum is 80 charaters
	 * print this line to the screen
	 */

	/* step 1: print to screen*/
	// printString ("Hello World\0");

	/* step 2: read from keyboard*/
	// char line[LENGTH_LINE];
	// printString ("Enter a line: \0");
	// readString (line);
	// printString (line);

	/* step 3: read a sector from disk*/
	// char buffer[LENGTH_SECTOR];
	// readSector (buffer, 30);
	// printString (buffer);

	/* step 4: creating my own interrupt
	 * Link interrupt 21 to my service routine
	 * In other words, set the interrupt table
	 */
	makeInterrupt21();
	interrupt(0x21, 0, 0, 0, 0);

	while (1)
	{
		// Infinite Loop
	}
	return 0;
}

void printString (char* word)
{
	/* Printing to Screen, Interrupt 0x10
	 * C style string end with 0
	 * if the character of word isn't 0 do loop
	 * else return
	 */

	/* start at a new line */
	char al = '\n';
	char ah = 0x0e;
	int ax = ah * 256 + al;
	interrupt(0x10, ax, 0, 0, 0);

	while (*word != '\0')
	{
		/* INT 10/ AH = 0Eh - video - teletype output */
		al = *word;
		ax = ah * 256 + al;
		interrupt (0x10, ax, 0, 0, 0);
		word++;
	}
	return;
}

void readString (char* line)
{
	/* read a line from keyboard input, inpterrupt 0x 16
	 * end with the ENTER key whose ASCII is 0xd
	 * get this line and store in the line buffer
	 * deal with backspace and end characters
	 */

	int i = 0;
	char temp = 0;
	while (1)
	{
        /* the AH must equal 0 and AL doesn't matter */
		temp = interrupt (0x16, 0, 0, 0, 0);
		/* only if the input is "ENTER", the loop ends*/
		if (temp == 0xd)
			break;

		/* if input is backspace, only diplay not store and index minus one */
		if (temp == 0x8)
		{
			if (i > 0)
			{
				interrupt (0x10, 0x0e * 256 + temp, 0, 0, 0);
				i--;
			}
			else
				continue;
		}

		/* store and display normal characters*/
		else
		{
			line[i++] = temp;
			interrupt (0x10, 0x0e * 256 + temp, 0, 0, 0);	
		}
	}

	/* remeber to set the last two characters as 0xa and 0x0 */
	if (i > LENGTH_LINE - 2)
		i = LENGTH_LINE - 2;
	line[i++] = 0xa;
	line[i] = '\0';
	return;
}

void readSector (char* buffer, int sector){
	/* read a sector from disk, interrupt 0x13
	 * AH = 2 this number tells the BIOS to read
	 * AL = number of sectors to read
	 * BX = address where the data should be stored to
	 * CH = track number
	 * CL = relative sector number
	 * DH = head number
	 * DL = device number (for the floppy disk, use 0)
	 */
	int ax = 2 * 256 + 1;
	int bx = buffer;
	int ch = div (sector, 36);
	int cl = mod (sector, 18) + 1;
	int cx = ch * 256 + cl;
	int dx = mod (div (sector, 18), 2) * 256 + 0;

	interrupt (0x13, ax, bx, cx, dx);

	buffer[LENGTH_SECTOR - 1] = '\0';
}

int mod (int dividend, int divisor)
{
	while (dividend >= divisor)
		dividend = dividend - divisor;
	return dividend;
}

int div (int dividend, int divisor)
{
	int q = 0;
	while (q * divisor <= dividend)
		q = q + 1;
	return q-1;
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	/* User Mode: Application program, Wrapper Function
	 * Kernel Mode: Trap handler (interrupt table), System call service routine
	 * To conlude, the interrupt fucntion I defined is running in kernel mode
	 */
	printString("Hello World\0");
}