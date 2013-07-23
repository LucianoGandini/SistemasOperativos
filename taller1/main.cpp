    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/ptrace.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/reg.h>
    #include <string.h>
    #include <errno.h>
    //#include <bits/syscall.h> 
    #include <sys/syscall.h>

    using namespace std;
     
    int main(int argc, char* argv[])
    {
        int status;
        long orig_eax;
        long orig_rdi;
	long orig_rsi;
        int kill_ret;
        int child = fork();
        if (child == -1)
        {
            perror("ERROR fork");
            return 1;
        }
        if (child == 0)
        {
            /* Sólo se ejecuta en el Hijo */
            if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
                perror("ERROR child ptrace(PTRACE_TRACEME, ...)");
                exit(1);
            }
            execvp(argv[1], argv+1);
            /* Si vuelve de exec() hubo un error */
            perror("ERROR child esec(...)");
            exit(1);
        }
        else
        {
            /* Sólo se ejecuta en el Padre */
            //int count = 0;
            while(1)
            {
                if (wait(&status) < 0) { perror("wait"); break; }
                if (WIFEXITED(status)) break; /* Proceso terminado */
                orig_eax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
                printf("Syscall made is %ld\n", orig_eax);
                if (orig_eax == SYS_kill)
                {
			orig_rdi = ptrace(PTRACE_PEEKUSER, child, 8 * RDI, NULL);
			printf("RDI: %ld\n", orig_rdi);
			orig_rsi = ptrace(PTRACE_PEEKUSER, child, 8 * RSI, NULL);
                        printf("RSI: %ld\n", orig_rsi);
			ptrace(PTRACE_POKEUSER, child, 8 * RDI, &child);
                        kill_ret = kill(child, SIGKILL);
                        if (kill_ret == -1)
                        {
                            fprintf(stderr, "Failed to kill ---> %s\n", strerror(errno));
                        }
                        printf("Se ha hecho justicia!\n");
                }
                ptrace(PTRACE_SYSCALL, child, NULL, NULL); /* contina */
            }
            ptrace(PTRACE_DETACH, child, NULL, NULL);/*Liberamos al hijo*/
        }
        return 0;
    }

