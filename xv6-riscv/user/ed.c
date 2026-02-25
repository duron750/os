#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define	SIGHUP	1
#define	SIGINTR	2
#define	SIGQUIT	3
#define	FNSIZE	64
#define	LBSIZE	512
#define	ESIZE	128
#define	GBSIZE	256
#define	NBRA	5
#define	EOF	-1

#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12

#define	STAR	01

#define	READ	0
#define	WRITE	1

int	peekc;
int	lastc;
char	savedfile[FNSIZE];
char	file[FNSIZE];
char	linebuf[LBSIZE];
char	rhsbuf[LBSIZE/2];
char	expbuf[ESIZE+4];
int	circfl;
int	*zero;
int	*dot;
int	*dol;
int	*endcore;
int	*fendcore;
int	*addr1;
int	*addr2;
char	genbuf[LBSIZE];
int	total_bytes;
char	*nextip;
char	*linebp;
int	ninbuf;
int	io;
int	pflag;
int	onhup;
int	onquit;
int	vflag=1;
int	listf;
int	col;
char	*globp;
int	tfile=-1;
int	tline;
char	*tfname;
char	*loc1;
char	*loc2;
char	*locs;
char	ibuff[512];
int	iblock=-1;
char	obuff[512];
int	oblock=-1;
int	ichanged;
int	nleft;
char	TMPERR[]="TMP";
int	names[26];
char	*braslist[NBRA];
char	*braelist[NBRA];
int fchanged = 0; // 0 = clean, 1 = dirty


/* Prototypes to satisfy the compiler */
void commands(void);
void init(void);
void error(void);
void setdot(void);
void newline(void);
void nonzero(void);
void ed_putchar(int ac); // Use this to avoid name collisions
char *getline(int tl);
int  putline(void);
int *address(void);
int  execute(int gf, int *addr);
void compile(int aeof);
int    getchar(void);
int    gettty(void);
int    getcopy(void);
int    getsub(void);
int    append(int (*f)(void), int *a);
void   delete(void);
void   setnoaddr(void);
void   setall(void);
void   filename(void);
void   exfile(void);
void putfile(void);
int getfile(void);
char* getblock(int atl, int iof); /* Crucial: returns char* */
void    unix_line(void);      /* Renamed from unix */
void puts(char *as);
void blkio(int b, char *buf, int write_op);
int    compsub(void);
void   dosub(void);
char* place(char *sp, char *l1, char *l2); /* Returns a 64-bit pointer */
void  reverse(int *a1, int *a2);
int   advance(char *lp, char *ep);
int   cclass(char *set, int c, int af);
void  putd(void);
void    move(int cflag);
void    substitute(int inglob);
void    global(int k);
void    reverse(int *a1, int *a2); // You'll need this for move()

int
main(int argc, char *argv[])
{
    char *p1, *p2;

    // 1. xv6 doesn't use standard Unix signals like SIGHUP/SIGQUIT.
    // For a basic port, we omit these or replace with empty stubs.
    
    argv++;
    if (argc > 1 && **argv == '-') {
        vflag = 0;
        // In V6, '-q' allowed quit signals to hit the debugger.
        // We'll skip the signal(SIGQUIT, 0) call here.
        if ((*argv)[1] == 'q') {
            vflag++;
        }
        argv++;
        argc--;
    }

    // 2. Set the filename
    if (argc > 1) {
        p1 = *argv;
        p2 = savedfile;
        // Copy filename to savedfile buffer
        while ((*p2++ = *p1++));
        globp = "r"; // Set global command pointer to 'read' the file
    }

    // 3. Memory Management
    // sbrk(0) returns the current end of the heap.
	fendcore = (int *)sbrk(0);

    // 4. Initialize buffers and temporary files
    init();

    // 5. Execution Loop
    // xv6 doesn't have setexit/reset (longjmp/setjmp). 
    // You will likely need to implement a simple try/catch or 
    // modify commands() to return on error rather than jumping.
    
    // setexit(); // V6's version of setjmp
    commands();

    // 6. Cleanup
    unlink(tfname);
    exit(0); // xv6 requires explicit exit()
}

void
commands(void)
{
    // In xv6, we use specific types. a1 usually points to 
    // the address of a line in the buffer.
    int *a1; 
    int c;

    for (;;) {
        if (pflag) {
            pflag = 0;
            addr1 = addr2 = dot;
            goto print;
        }
        addr1 = 0;
        addr2 = 0;
        
        // Address parsing loop
        do {
            addr1 = addr2;
            if ((a1 = address()) == 0) {
                c = getchar();
                break;
            }
            addr2 = a1;
            if ((c = getchar()) == ';') {
                c = ',';
                dot = a1;
            }
        } while (c == ',');

        if (addr1 == 0)
            addr1 = addr2;

        switch(c) {

        case 'a':
            setdot();
            newline();
            append(gettty, addr2);
            fchanged = 1;
            continue;

        case 'd':
            delete();
            fchanged = 1;
            continue;

        case 'e':
            setnoaddr();
            if (getchar() != ' ') {
                printf("?\n");
                continue; 
            }
            savedfile[0] = 0;
            init();
            addr2 = zero;
            goto caseread;

		case 'f':
            setnoaddr();
            if ((c = getchar()) != '\n') {
                peekc = c;
                savedfile[0] = 0;
                filename();
            }
            puts(savedfile); // Now using your internal puts logic
            continue;
            
		case 'c': // Change: Delete range, then start appending
            delete();
            append(gettty, addr1-1);
            fchanged = 1;
            continue;

        case 'i': // Insert: Like append, but before the current line
            setdot();
            nonzero();
            newline();
            append(gettty, addr2-1);
            fchanged = 1;
            continue;

        case 'k': // Mark: Save a position to a register (a-z)
            if ((c = getchar()) < 'a' || c > 'z')
                printf("?\n");
            else {
                newline();
                setdot();
                nonzero();
                /* We use the low bit to mark a line as 'dirty' in some ports, 
                   but here we just store the address pointer */
                names[c-'a'] = *addr2; 
            }
            continue;

        case 'm': // Move lines
            move(0);
            fchanged = 1;
            continue;

        case 't': // Transfer (copy) lines
            move(1);
            fchanged = 1;
            continue;

        case 's': // Substitute!
            setdot();
            nonzero();
			substitute(globp != 0);
            fchanged = 1;
            continue;

        case 'g': // Global: Run command on all lines matching regex
            global(1);
            continue;

        case 'v': // Inverted Global: Run command on lines NOT matching
            global(0);
            continue;

        case '!': // Unix escape: Run a shell command
            unix_line();
            continue;

        case '\n': // Just hitting enter: Go to next line and print
            if (addr2 == 0)
                addr2 = dot + 1;
            addr1 = addr2;
            goto print;

		case 'p':
            newline();
        print:
            setdot();
            nonzero();
            a1 = addr1;
            do {
                puts(getline(*a1++));
            } while (a1 <= addr2);
            dot = addr2;
            pflag = 0;
            continue; // 'continue' skips the suffix logic below

        case 'n':
            newline();
            setdot();
            nonzero();
            for (a1 = addr1; a1 <= addr2; a1++) {
                total_bytes = a1 - zero; 
                putd();
                ed_putchar('\t');
                puts(getline(*a1));
            }
            dot = addr2;
            pflag = 0;
            continue; // 'continue' skips the suffix logic below

        case 'q':
            setnoaddr();
            newline();
			/* If changed and we haven't warned them yet (using pflag as a temporary toggle) */
            if (fchanged && pflag == 0) {
				fchanged = 0; //ok, if you press q again it will exit
                pflag = 1; // This isn't the 'print' pflag, just a way to catch the next 'q'
                printf("Unsaved changes, exit?\n");
                continue;
            }
            unlink(tfname);
            exit(0); // xv6 exit

        case 'r':
        caseread:
            filename();
            // xv6 open: 0 is O_RDONLY
            if ((io = open(file, 0)) < 0) {
                lastc = '\n';
                printf("?\n");
                continue;
            }
            setall();
            ninbuf = 0;
            append(getfile, addr2);
            exfile(); // handles closing the file
            continue;

        case 'w':
            setall();
            nonzero();
            filename();
            // xv6 creat equivalent
            if ((io = open(file, O_CREATE | O_WRONLY | O_TRUNC)) < 0) {
                printf("?\n");
                continue;
            }
            putfile();
            exfile();
            fchanged = 0;
            continue;

        case '=':
            setall();
            newline();
            // Calculate line number: pointer arithmetic 
            // divided by word size
            printf("%d\n", (int)(addr2 - zero));
            continue;

        case EOF:
            return;
        }
		if (pflag) {
			if (pflag == 2) { // Numbered suffix
				total_bytes = dot - zero;
				putd();
				ed_putchar('\t');
			}
			puts(getline(*dot));
		}
        // Default error: ed is famous for just printing '?'
        printf("?\n");
    }
}

int *address(void)
{
    int *a1;
    int minus, c;
    int n, relerr;

    minus = 0;
    a1 = 0;

    for (;;) {
        c = getchar();
        if ('0' <= c && c <= '9') {
            n = 0;
            do {
                n *= 10;           /* Fixed from n =* 10 */
                n += (c - '0');    /* Fixed from n =+ ... */
            } while ((c = getchar()) >= '0' && c <= '9');
            
            peekc = c;
            if (a1 == 0)
                a1 = zero;
            if (minus < 0)
                n = -n;
            a1 += n;               /* Fixed from a1 =+ n */
            minus = 0;
            continue;
        }

        relerr = 0;
        if (a1 || minus)
            relerr++;

        switch(c) {
        case ' ':
        case '\t':
            continue;
    
        case '+':
            minus++;
            if (a1 == 0)
                a1 = dot;
            continue;

        case '-':
        case '^':
            minus--;
            if (a1 == 0)
                a1 = dot;
            continue;
    
        case '?':
        case '/':
            compile(c);
            a1 = dot;
            for (;;) {
                if (c == '/') {
                    a1++;
                    if (a1 > dol)
                        a1 = zero;
                } else {
                    a1--;
                    if (a1 < zero)
                        a1 = dol;
                }
                if (execute(0, a1))
                    break;
                if (a1 == dot) {
                    /* In xv6, error usually triggers a longjmp 
                       back to the main loop */
                    error(); 
                }
            }
            break;
    
        case '$':
            a1 = dol;
            break;
    
        case '.':
            a1 = dot;
            break;

        case '\'': // Handle marks (e.g., 'a)
            if ((c = getchar()) < 'a' || c > 'z')
                error();
            for (a1 = zero; a1 <= dol; a1++)
                if (names[c - 'a'] == (*a1 | 01))
                    break;
            break;
    
        default:
            peekc = c;
            if (a1 == 0)
                return 0;
            a1 += minus;
            if (a1 < zero || a1 > dol)
                error();
            return a1;
        }
        
        if (relerr)
            error();
    }
}

void
setdot(void)
{
    // If no address was provided, default to 'dot' (the current line)
    if (addr2 == 0)
        addr1 = addr2 = dot;
    
    // Logic check: You can't have a range like '5,2'
    if (addr1 > addr2)
        error();
}

void
setall(void)
{
    // If no address provided, many commands (like 'w') default to '1,$'
    if (addr2 == 0) {
        addr1 = zero + 1;
        addr2 = dol;
        // If the file is empty, set address 1 to the 'zero' line
        if (dol == zero)
            addr1 = zero;
    }
    setdot();
}

void
setnoaddr(void)
{
    // For commands that take NO addresses (like 'q' or 'e')
    if (addr2 != 0)
        error();
}

void
nonzero(void)
{
    // Ensures the range is within the actual content area (lines 1 to $)
    // You can't perform 'p' or 'd' on the 'zero' line.
    if (addr1 <= zero || addr2 > dol)
        error();
}

void error(void) {
    printf("?\n");
    // In a simple port, you might need to use a flag 
    // to break out of current command processing.
}

void
newline(void)
{
    int c;

    c = getchar();
    
    // Case 1: Standard termination
    if (c == '\n' || c == EOF)
        return;

    // Case 2: Command Suffixes
    // We check for 'p' (print), 'l' (list), and 'n' (number)
    if (c == 'p' || c == 'l' || c == 'n') {
        pflag++; // General signal to print the line after command
        
        if (c == 'l')
            listf++; // Visible characters ($ at end of line)
            
        if (c == 'n')
            pflag = 2; // Specific signal to include line numbers

        // After the suffix, we must ensure we reach the actual newline
        c = getchar();
        if (c == '\n' || c == EOF)
            return;
            
        // Handle cases where multiple suffixes might be combined (e.g., 'pn')
        // if your parser logic allows it, otherwise fall through to error.
    }

    // Case 3: Syntax Error 
    error();
}

void
filename(void)
{
    char *p1, *p2;
    int c;

    // In V6, total_bytes[1] was used for error/status reporting.
    // In xv6, we usually track this via local variables or skip it.
    
    c = getchar();
    
    /* Case 1: No filename provided (User just hit Enter) */
    if (c == '\n' || c == EOF) {
        p1 = savedfile;
        if (*p1 == 0) // Error if no filename was ever set
            error();
        
        p2 = file;
        // Copy the "remembered" filename into the active "file" buffer
        while ((*p2++ = *p1++));
        return;
    }
    
    /* Case 2: User is specifying a new filename */
    if (c != ' ')
        error();
        
    // Skip extra spaces
    while ((c = getchar()) == ' ');
    
    if (c == '\n')
        error();
        
    p1 = file;
    do {
        *p1++ = c;
    } while ((c = getchar()) != '\n' && c != EOF);
    *p1 = 0; // Null terminate the new filename
    
    /* If we didn't have a saved filename yet, remember this one */
    if (savedfile[0] == 0) {
        p1 = savedfile;
        p2 = file;
        while ((*p1++ = *p2++));
    }
}

void
exfile(void)
{
    // Close the file descriptor opened by the 'r' or 'w' command
    if (io >= 0) {
        close(io);
        io = -1;
    }

    // If vflag is on (verbose mode), print the number of characters
    // 'total_bytes' is a global long or int updated during the read/write process.
    if (vflag) {
        // In xv6, we use printf for decimal output instead of putd()
		printf("%ld\n", (long)total_bytes); // total_bytes should be long/int64  
	}
}

int
getchar(void)
{
    // 1. Check the look-ahead buffer (peekc)
    if (peekc) {
        lastc = peekc;
        peekc = 0;
        return lastc;
    }

    // 2. Check if we are executing a global command string
    if (globp) {
        if ((lastc = *globp++) != 0)
            return lastc;
        // If we hit the null terminator of the global string, stop
        globp = 0;
        return EOF;
    }

    // 3. Read from standard input (file descriptor 0)
    // We pass the address of lastc to read 1 byte
    if (read(0, &lastc, 1) <= 0) {
        lastc = EOF;
        return lastc;
    }

    // 4. Mask out the high bit (ASCII is 7-bit)
    // Original: lastc =& 0177 (Modern: lastc &= 0x7F)
    lastc &= 0x7F;

    return lastc;
}

int
gettty(void)
{
    int c, gf;
    char *p;

    p = linebuf;
    gf = (globp != 0); // Keep track if we are in a global command
    
    while ((c = getchar()) != '\n') {
        if (c == EOF) {
            if (gf)
                peekc = c;
            return c;
        }

        // Clean the character (7-bit ASCII)
        c &= 0x7F;
        if (c == 0)
            continue;

        *p++ = c;

        // Buffer overflow check
        // LBSIZE is typically 512 or 1024
        if (p >= &linebuf[LBSIZE - 2]) {
            printf("line too long\n");
            return EOF; // Replacing 'error' with a failure return
        }
    }

    *p = 0; // Null terminate the string

    // The famous ed "stop" condition: a single dot on a line
    if (linebuf[0] == '.' && linebuf[1] == 0)
        return EOF;

    return 0;
}

int
getfile(void)
{
    int c;
    char *lp, *fp;

    lp = linebuf;
    fp = nextip;
    do {
        if (--ninbuf < 0) {
            // Read a block of data into genbuf
            if ((ninbuf = read(io, genbuf, LBSIZE) - 1) < 0)
                return (EOF);
            fp = genbuf;
        }
        if (lp >= &linebuf[LBSIZE]) {
            printf("line too long\n");
            return (EOF); // Simplified error handling
        }
        
        // Clean high bits and check for null chars
        if ((*lp++ = c = *fp++ & 0177) == 0) {
            lp--;
            continue;
        }
        
        // Increment global byte total_bytes
        total_bytes++; 
        
    } while (c != '\n');
    
    *--lp = 0;   // Replace newline with null terminator
    nextip = fp; // Save pointer for next call
    return (0);
}

void
putfile(void)
{
    int *a1;
    char *fp, *lp;
    int nib;

    nib = 512;
    fp = genbuf;
    a1 = addr1;
    do {
        // Get the actual text of the line from the temp file
        lp = getline(*a1++);
        for (;;) {
            if (--nib < 0) {
                // Buffer full, flush to disk
                write(io, genbuf, fp - genbuf);
                nib = 511;
                fp = genbuf;
            }
            
            total_bytes++; // Track bytes written

            if ((*fp++ = *lp++) == 0) {
                // Terminate line with newline on disk
                fp[-1] = '\n';
                break;
            }
        }
    } while (a1 <= addr2);
    
    // Final flush of the remaining buffer
    write(io, genbuf, fp - genbuf);
}

int
append(int (*f)(void), int *a)
{
    int *a1, *rdot;
    int nline, tl;

    nline = 0;
    dot = a;

    // While the input function (gettty or getfile) keeps providing lines
    while ((*f)() == 0) {
        
        // 1. Memory Management (Heap Growth)
        // Check if the address table (dol) has reached the end of allocated memory
        if ((char *)dol >= (char *)endcore) {
            if (sbrk(1024) == (void *)-1) {
                printf("out of memory\n");
                return nline; // Or handle error
            }
            endcore = (int *)((char *)endcore + 1024);
        }

        // 2. Commit the line to the temp file
        // putline() writes linebuf to .edtmp and returns the file offset
        tl = putline();
        nline++;

        // 3. The "Shift" Logic (Array Insertion)
        // To insert a line in the middle, we must move all subsequent 
        // line-pointers down by one slot.
        a1 = ++dol;   // Move the 'end of file' pointer up
        rdot = ++dot; // The position where the new line will live
        
        a1 = dol;     // Start from the new end
        while (a1 > rdot) {
            *a1 = *(a1-1); // Shift pointers one by one
            a1--;
        }

        // 4. Store the new line's offset in the table
        *rdot = tl;
    }
    return nline;
}

void
unix_line(void)
{
    int pid, rpid;
    int retcode;

    setnoaddr(); // Ensure the user didn't type "1,5!"
    
    if ((pid = fork()) == 0) {
        // Child process
        // In xv6, we just execute the shell directly
		char *args[] = {"sh", 0};
		exec("sh", args);
        
        // If execl fails
        printf("exec sh failed\n");
        exit(1);
    }
    
    // Parent process: Wait for the shell to finish
    if (pid > 0) {
        while ((rpid = wait(&retcode)) != pid && rpid != -1);
    }

    printf("!\n"); // The traditional "I'm back" signal from ed
}

void
delete(void)
{
    int *a1, *a2, *a3;

    setdot();    // Validate current line if no address given
    newline();  // Check for 'p' or 'l' suffixes and final \n
    nonzero();  // Ensure we aren't trying to delete line 0

    a1 = addr1;      // First line to delete
    a2 = addr2 + 1;  // First line to keep (after the range)
    a3 = dol;        // The current last line

    // Update the 'dol' ($) pointer
    // Original: dol =- a2 - a1; (Modern: dol -= (a2 - a1))
    dol -= (a2 - a1);

    // Shift the address table: Slide the remaining lines up
    do {
        *a1++ = *a2++;
    } while (a2 <= a3);

    // Update 'dot' (.)
    // If we deleted the end of the file, dot becomes the new last line
    a1 = addr1;
    if (a1 > dol)
        a1 = dol;
    dot = a1;
}

char*
getline(int tl)
{
    char *bp, *lp;
    int nl;

    lp = linebuf;
    // getblock(offset, mode) fetches the 512-byte block containing offset
    bp = getblock(tl, READ);
    nl = nleft; // nleft is the number of bytes remaining in the current block
    
    // Mask out the lower 8 bits to find the start of the current block
    tl &= ~0377; 
    
    while ((*lp++ = *bp++)) {
        if (--nl == 0) {
            // If we hit the end of the block, get the next one (offset + 512 bytes)
            tl += 0400;
            bp = getblock(tl, READ);
            nl = nleft;
        }
    }
    return linebuf;
}

int
putline(void)
{
    char *bp, *lp;
    int nl, tl, ret_tl;

    lp = linebuf;
    ret_tl = tline; // The offset where this line starts
    tl = tline;
    
    bp = getblock(tl, WRITE);
    nl = nleft;
    tl &= ~0377;

    while ((*bp = *lp++)) {
        // If we find a newline, stop (ed lines are stored without \n in memory)
        if (*bp++ == '\n') {
            *--bp = 0;
            break;
        }
        if (--nl == 0) {
            // If block is full, move to the next block
            tl += 0400;
            bp = getblock(tl, WRITE);
            nl = nleft;
        }
    }
    
    // Calculate the next available offset in the temp file.
    // Pointers are word-aligned (rounded up to nearest 2 bytes).
    // Original: tline += (((lp - linebuf) + 3) >> 1) & 077776;
    int len = lp - linebuf;
    tline += (len + 03) & ~01; 
    
    return ret_tl;
}

char*
getblock(int atl, int iof)
{
    int bno, off;
    
    // atl is the 'address' (word offset). 
    // We convert it to block number and byte offset.
    bno = (atl >> 8) & 0377;  // High bits determine the 512-byte block
    off = (atl << 1) & 0774;  // Low bits determine the byte within the block

    if (bno >= 255) {
        printf("temp file too big\n");
        return 0; // Or error handle
    }

    nleft = 512 - off; // Bytes remaining in this block

    // 1. Is it already in the input buffer?
    if (bno == iblock) {
        ichanged |= iof; // Mark as 'dirty' if we are writing to it
        return (ibuff + off);
    }

    // 2. Is it in the output buffer?
    if (bno == oblock)
        return (obuff + off);

    // 3. If we are reading and the current input buffer is dirty, save it first
    if (iof == READ) {
        if (ichanged)
            blkio(iblock, ibuff, 1); // 1 for write
        ichanged = 0;
        iblock = bno;
        blkio(bno, ibuff, 0);       // 0 for read
        return (ibuff + off);
    }

    // 4. If we are writing to a new block, flush the old output block
    if (oblock >= 0)
        blkio(oblock, obuff, 1);
    oblock = bno;
    return (obuff + off);
}

void
blkio(int b, char *buf, int write_op)
{
    // WARNING: xv6 doesn't have lseek. 
    // This port assumes sequential-ish temp file access 
    // OR you must implement a seek-like behavior.
    if(write_op) {
        if(write(tfile, buf, 512) != 512)
            error();
    } else {
        if(read(tfile, buf, 512) != 512)
            error();
    }
}

void
init(void)
{
    char *p;
    int pid;

    // 1. Close and Reset
    close(tfile);
    tline = 0;
    iblock = -1;
    oblock = -1;
    ichanged = 0;

    // 2. Generate a Unique Temp Filename
    // Original: /tmp/eXXXXX (using PID for uniqueness)
    // xv6: We'll use a local name like ".edXXXXX"
    static char tfname_buf[] = ".edXXXXX";
    pid = getpid();
    p = &tfname_buf[7]; // Point to the last 'X'
    
    // Convert PID to Octal string to fill the 'XXXXX'
    while (p > &tfname_buf[2]) {
        *--p = (pid & 07) + '0';
        pid >>= 3; // Modern shift operator
    }
    tfname = tfname_buf;

    // 3. Create and Open the Temp File
    unlink(tfname); // Delete if it already exists
    if ((tfile = open(tfname, O_CREATE | O_RDWR)) < 0) {
        printf("ed: cannot create temp file\n");
        exit(1);
    }

    // 4. Memory/Heap Management
    // Reset pointers to the start of the address table
	sbrk((char*)fendcore - (char*)sbrk(0));
    dot = zero = dol = (int *)fendcore;
    
    // In xv6, we usually keep endcore as the literal limit of our sbrk()
    endcore = (int *)fendcore; 
}

void
global(int k)
{
    char *gp;
    int c;
    int *a1;
    char globuf[GBSIZE];

    if (globp) {
        printf("? (recursive global)\n");
        return;
    }

    setall();   /* Default to 1,$ if no addresses given */
    nonzero();  /* Cannot run global on line 0 */

    if ((c = getchar()) == '\n') {
        printf("?\n");
        return;
    }

    compile(c); /* Compile the regex provided after the 'g' */
    
    gp = globuf;
    while ((c = getchar()) != '\n') {
        if (c == EOF) {
            printf("?\n");
            return;
        }
        if (c == '\\') {
            c = getchar();
            if (c != '\n')
                *gp++ = '\\';
        }
        *gp++ = c;
        if (gp >= &globuf[GBSIZE - 2]) {
            printf("? (global buffer overflow)\n");
            return;
        }
    }
    *gp++ = '\n';
    *gp++ = 0;

    // --- Pass 1: Marking ---
    // We use the LSB (least significant bit) of the address pointer 
    // to "mark" the line for processing.
    for (a1 = zero; a1 <= dol; a1++) {
        *a1 &= ~01; // Clear any old marks
        if (a1 >= addr1 && a1 <= addr2 && execute(0, a1) == k)
            *a1 |= 01; // Mark the line
    }

    // --- Pass 2: Execution ---
    for (a1 = zero; a1 <= dol; a1++) {
        if (*a1 & 01) {
            *a1 &= ~01;     // Clear the mark before executing
            dot = a1;       // Move '.' to the matched line
            globp = globuf; // Point getchar() to our command string
            commands();     // Run the editor commands on this line
            
            // After commands() runs, the file structure might have changed 
            // (lines deleted/added), so we restart the scan from line 0.
            a1 = zero; 
        }
    }
}

void
substitute(int inglob)
{
    int gsubf, *a1, nl;

    // 1. Compile the replacement string and check for the 'g' suffix
    gsubf = compsub(); 

    for (a1 = addr1; a1 <= addr2; a1++) {
        // 2. Search for the pattern on the current line
        if (execute(0, a1) == 0)
            continue;

        inglob |= 01; // Mark that at least one substitution occurred
        dosub();      // Perform the first substitution

        // 3. If the 'g' (global per line) suffix was used, keep subbing
        if (gsubf) {
            while (*loc2) {
                if (execute(1, 0) == 0) // execute(1) continues from loc2
                    break;
                dosub();
            }
        }

        // 4. Commit the new string to the temp file
        // putline() takes the contents of linebuf (modified by dosub)
        // and returns a new offset.
        *a1 = putline();

        /* If your dosub() creates multiple lines (rare in ed but possible 
           via certain hacks), append() handles the table expansion. */
        nl = append(getsub, a1);
        a1 += nl;
        addr2 += nl;
    }

    // 5. If no matches were found across the whole range
    if (inglob == 0) {
        printf("?\n");
    }
}

int
compsub(void)
{
    int seof, c;
    char *p;

    // 1. Get the delimiter (e.g., the '/' in s/old/new/)
    if ((seof = getchar()) == '\n') {
        printf("?\n");
        return 0;
    }

    // 2. Compile the search pattern (the LHS)
    compile(seof);

    // 3. Parse the replacement string (the RHS)
    p = rhsbuf;
    for (;;) {
        c = getchar();
        if (c == '\\') {
            // Escape the next character. 
            // We use 0x80 (0200) to flag it as "escaped" internally.
            c = getchar() | 0x80;
        }
        if (c == '\n') {
            printf("?\n");
            return 0;
        }
        if (c == seof) // Hit the closing delimiter
            break;
        
        *p++ = c;
        if (p >= &rhsbuf[LBSIZE / 2]) {
            printf("? (replacement string too long)\n");
            return 0;
        }
    }
    *p++ = 0; // Null terminate the replacement buffer

    // 4. Check for the 'g' (global) suffix
    if ((peekc = getchar()) == 'g') {
        peekc = 0;
        newline();
        return 1; // Signal that global substitution is requested
    }

    newline();
    return 0;
}

int
getsub(void)
{
    char *p1, *p2;

    p1 = linebuf;
    if ((p2 = linebp) == 0)
        return EOF;
    
    // Copy the contents of linebp (the substituted line) into linebuf
    while ((*p1++ = *p2++))
        ;
    
    linebp = 0; // Clear the pointer after use
    return 0;
}

void
dosub(void)
{
    char *lp, *sp, *rp;
    int c;

    lp = linebuf;
    sp = genbuf;
    rp = rhsbuf;

    // 1. Copy everything from the start of the line up to the match (loc1)
    while (lp < loc1)
        *sp++ = *lp++;

    // 2. Process the replacement string (rhsbuf)
    while ((c = *rp++)) {
        if (c == '&') {
            // '&' represents the entire string that was matched
            sp = place(sp, loc1, loc2);
            continue;
        } else if (c < 0) { 
            // In compsub, we flagged escaped chars with the 8th bit (c | 0200)
            // If it's an escaped digit (e.g., \1, \2), it's a sub-match reference
            c &= 0x7F; // Strip the flag bit
            if (c >= '1' && c < NBRA + '1') {
                sp = place(sp, braslist[c - '1'], braelist[c - '1']);
                continue;
            }
        }
        // Otherwise, it's just a literal character
        *sp++ = c & 0x7F;
        if (sp >= &genbuf[LBSIZE]) {
            printf("? (result too long)\n");
            return;
        }
    }

    // 3. Copy everything after the match (loc2) to the end of the line
    lp = loc2;
    // Update loc2 to point to the end of the NEW text in the original linebuf 
    // context (used by the 'g' suffix in substitute)
    loc2 = sp + (linebuf - genbuf); 

    while ((*sp++ = *lp++)) {
        if (sp >= &genbuf[LBSIZE]) {
            printf("? (result too long)\n");
            return;
        }
    }

    // 4. Copy the completed result from the scratchpad (genbuf) back to linebuf
    lp = linebuf;
    sp = genbuf;
    while ((*lp++ = *sp++))
        ;
}

char*
place(char *sp, char *l1, char *l2)
{
    while (l1 < l2) {
        *sp++ = *l1++;
        if (sp >= &genbuf[LBSIZE]) {
            // Use your error handler instead of printf if you've ported it
            error(); 
        }
    }
    return sp;
}

void
move(int cflag)
{
    int *adt, *ad1, *ad2;

    setdot();
    nonzero();
    if ((adt = address()) == 0) {
        printf("?\n");
        return;
    }
    newline();

    ad1 = addr1;
    ad2 = addr2;

    if (cflag) {
        // 't' command (transfer/copy)
        // We append a copy of the range to the end of the file first,
        // then move that new copy to the destination.
        ad1 = dol;
        append(getcopy, ad1++);
        ad2 = dol;
    }

    ad2++; // Point to the line immediately after the range
    
    if (adt < ad1) {
        // Moving lines "up" the file
        dot = adt + (ad2 - ad1);
        if ((++adt) == ad1)
            return;
        reverse(adt, ad1);
        reverse(ad1, ad2);
        reverse(adt, ad2);
    } else if (adt >= ad2) {
        // Moving lines "down" the file
        dot = adt++;
        reverse(ad1, ad2);
        reverse(ad2, adt);
        reverse(ad1, adt);
    } else {
        // Cannot move a block into itself!
        printf("?\n");
    }
}

void
reverse(int *a1, int *a2)
{
    int t;
    for (;;) {
        t = *--a2;
        if (a2 <= a1)
            return;
        *a2 = *a1;
        *a1++ = t;
    }
}

int
getcopy(void)
{
    if (addr1 > addr2)
        return EOF;
    getline(*addr1++);
    return 0;
}

void
compile(int aeof)
{
    int eof, c;
    char *ep;
    char *lastep = 0;
    char bracket[NBRA], *bracketp;
    int nbra;
    int cclcnt;

    ep = expbuf;
    eof = aeof;
    bracketp = bracket;
    nbra = 0;

    if ((c = getchar()) == eof) {
        if (*ep == 0) { printf("?\n"); return; }
        return;
    }

    circfl = 0;
    if (c == '^') {
        c = getchar();
        circfl++; // Pattern is "anchored" to the start of the line
    }

    if (c == '*')
        goto cerror;

    peekc = c;
    for (;;) {
        if (ep >= &expbuf[ESIZE])
            goto cerror;

        c = getchar();
        if (c == eof) {
            *ep++ = CEOF; // End of pattern bytecode
            return;
        }

        if (c != '*')
            lastep = ep; // Keep track of the last atom for the '*' operator

        switch (c) {
            case '\\':
                if ((c = getchar()) == '(') {
                    if (nbra >= NBRA) goto cerror;
                    *bracketp++ = nbra;
                    *ep++ = CBRA; // Start sub-match capture
                    *ep++ = nbra++;
                    continue;
                }
                if (c == ')') {
                    if (bracketp <= bracket) goto cerror;
                    *ep++ = CKET; // End sub-match capture
                    *ep++ = *--bracketp;
                    continue;
                }
                *ep++ = CCHR; // Literal character (escaped)
                if (c == '\n') goto cerror;
                *ep++ = c;
                continue;

            case '.':
                *ep++ = CDOT; // Match any character
                continue;

            case '\n':
                goto cerror;

            case '*':
                if (lastep == 0 || *lastep == CBRA || *lastep == CKET)
                    goto cerror;
                *lastep |= STAR; // Bit-flag the previous atom as "0 or more"
                continue;

            case '$':
                if ((peekc = getchar()) != eof)
                    goto defchar;
                *ep++ = CDOL; // Match end of line
                continue;

            case '[':
                *ep++ = CCL; // Character class [abc]
                *ep++ = 0;   // Placeholder for total_bytes
                cclcnt = 1;
                if ((c = getchar()) == '^') {
                    c = getchar();
                    ep[-2] = NCCL; // Negative character class [^abc]
                }
                do {
                    if (c == '\n') goto cerror;
                    *ep++ = c;
                    cclcnt++;
                    if (ep >= &expbuf[ESIZE]) goto cerror;
                } while ((c = getchar()) != ']');
                lastep[1] = cclcnt; // Store how many chars are in the brackets
                continue;

            defchar:
            default:
                *ep++ = CCHR; // Literal character
                *ep++ = c;
        }
    }

cerror:
    expbuf[0] = 0;
    nbra = 0;
    printf("?\n");
}

int
execute(int gf, int *addr)
{
    char *p1, *p2;
    int c;

    if (gf) {
        // gf (global flag) means we are continuing a search 
        // from where the last match ended (loc2).
        if (circfl)
            return 0; // ^ anchor can't match in the middle of a line
        
        // Copy genbuf to linebuf (standard ed setup for globals)
        p1 = linebuf;
        p2 = genbuf;
        while ((*p1++ = *p2++))
            ;
        locs = p1 = loc2;
    } else {
        if (addr == zero)
            return 0;
        p1 = getline(*addr); // Load the line text into linebuf
        locs = 0;
    }

    p2 = expbuf; // Start of the compiled bytecode
    
    // Case 1: Anchored search (Pattern starts with ^)
    if (circfl) {
        loc1 = p1;
        return advance(p1, p2);
    }

    // Case 2: Optimization - Fast check for first character match
    // If the first opcode is a literal character, find it before 
    // entering the expensive 'advance' recursion.
    if (*p2 == CCHR) {
        c = p2[1];
        do {
            if (*p1 != c)
                continue;
            if (advance(p1, p2)) {
                loc1 = p1;
                return 1;
            }
        } while (*p1++);
        return 0;
    }

    // Case 3: Standard search
    // Try matching at every character position in the line.
    do {
        if (advance(p1, p2)) {
            loc1 = p1;
            return 1;
        }
    } while (*p1++);
    
    return 0;
}

int
advance(char *lp, char *ep)
{
    char *curlp;

    for (;;) {
        switch (*ep++) {

        case CCHR: // Literal character match
            if (*ep++ == *lp++)
                continue;
            return 0;

        case CDOT: // Dot (.) matches any single character except null
            if (*lp++)
                continue;
            return 0;

        case CDOL: // Dollar ($) matches end of line
            if (*lp == 0)
                continue;
            return 0;

        case CEOF: // End of pattern bytecode - Success!
            loc2 = lp;
            return 1;

        case CCL: // Character class [abc]
            if (cclass(ep, *lp++, 1)) {
                ep += *ep; // Skip past the class data
                continue;
            }
            return 0;

        case NCCL: // Negated character class [^abc]
            if (cclass(ep, *lp++, 0)) {
                ep += *ep;
                continue;
            }
            return 0;

        case CBRA: // Start sub-match capture \(
            braslist[(int)*ep++] = lp;
            continue;

        case CKET: // End sub-match capture \)
            braelist[(int)*ep++] = lp;
            continue;

        // --- The Star (*) Operators ---
        
        case CDOT | STAR: // .* (Match any number of any character)
            curlp = lp;
            while (*lp++)
                ; // Move lp to the very end of the string
            goto star;

        case CCHR | STAR: // a* (Match any number of a specific character)
            curlp = lp;
            while (*lp++ == *ep)
                ;
            ep++; // Move ep past the character being matched
            goto star;

        case CCL | STAR:
        case NCCL | STAR: // [abc]* or [^abc]*
            curlp = lp;
            while (cclass(ep, *lp++, ep[-1] == (CCL | STAR)))
                ;
            ep += *ep;
            goto star;

        star:
            // Backtracking logic:
            // We matched too far. Back up and try matching the 
            // REST of the pattern (advance(lp, ep)) from here.
            do {
                lp--;
                if (lp == locs)
                    break;
                if (advance(lp, ep))
                    return 1;
            } while (lp > curlp);
            return 0;

        default:
            printf("? (advance error)\n");
            return 0;
        }
    }
}

int
cclass(char *set, int c, int af)
{
    int n;

    if (c == 0)
        return 0;

    n = *set++; // The first byte is the number of characters in the set
    while (--n) {
        if (*set++ == c)
            return af; // Found a match (af=1 for [abc], af=0 for [^abc])
    }
    return !af; // No match found
}

void
putd(void)
{
    long r = total_bytes % 10;
    total_bytes /= 10;
    if (total_bytes)
        putd();
    ed_putchar(r + '0');
}

void
puts(char *as)
{
    char *sp = as;
    col = 0;
    while (*sp)
        ed_putchar(*sp++); // Use ed_putchar here too
    ed_putchar('\n');
}

char line[70];
char *linp = line;

void
ed_putchar(int ac) // Renamed to avoid conflict with libc
{
    char *lp;
    int c;

    lp = linp;
    c = ac;

    if (listf) { // If the 'l' command is active
        col++;
        if (col >= 72) {
            col = 0;
            *lp++ = '\\';
            *lp++ = '\n';
        }
        if (c == '\t') {
            *lp++ = '-';
            *lp++ = '\b';
            *lp++ = '>';
            goto out;
        }
        if (c == '\b') {
            *lp++ = '-';
            *lp++ = '\b';
            *lp++ = '<';
            goto out;
        }
        if (c < ' ' && c != '\n') {
            *lp++ = '\\';
            *lp++ = (c >> 3) + '0';
            *lp++ = (c & 07) + '0';
            col += 2;
            goto out;
        }
    }
    *lp++ = c;

out:
    // Flush the buffer if we hit a newline or the limit
    if (c == '\n' || lp >= &line[64]) {
        linp = line;
        write(1, line, lp - line);
        return;
    }
    linp = lp;
}
