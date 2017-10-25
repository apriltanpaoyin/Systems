#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <grp.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>

int readLine() {
        char *line = NULL;
        size_t len = 0;
        char *split = NULL;
        char **cmds = NULL;
        int cnt = 0;
        int check = 0;

        check = getline(&line, &len, stdin);
        if (check == 1) {
                return 0;
        }

        //Seperate by spaces and next line character
        split = strtok(line, " \n");
        while(split != NULL) {
                cmds = (char**)realloc(cmds, (cnt+1)*sizeof(*cmds));
                cmds[cnt] = (char*)malloc(sizeof(split));
                strcpy(cmds[cnt], split);
                cnt++;
                split= strtok(NULL, " \n");
        }

        //Get first word of command, ignore tail
        if (strcmp(cmds[0], "pw") == 0) {
                char *wd = NULL;
                wd = (char*)malloc(100 * sizeof(char));
                getcwd(wd, 100);
                printf("%s\n", wd);
        } else if (strcmp(cmds[0], "ifc") == 0) {
                FILE *f;
                char *loc = "/sbin/ifconfig ";
                char *location[30];
                char info[64];

                if (cnt == 1) {
                        strcat(location, loc);
                        strcat(location, "eth0");
                } else {
                        if(strstr(cmds[1], "-") != NULL) {
                                printf("Please remove options. Enter 'help' for more information.\n");
                                 return 0;
                        } else {
                                strcat(location, loc);
                                strcat(location, cmds[1]);
                        }
                }

                f = popen(location, "r");
                while(fgets(info, 64, f) != NULL) {
                        printf("%s", info);
                }
                memset(location, 0, strlen(location));
                pclose(f);
        } else if (strcmp(cmds[0], "dt") == 0) {
                time_t timer;
                char buff[26];
                struct tm* tm_info;

                time(&timer);
                tm_info = localtime(&timer);
                strftime(buff, 26, "%Y%m%d%H%M%S", tm_info);
                puts(buff);
        } else if (strcmp(cmds[0], "ud") == 0) {
                register uid_t uid = NULL;
                register gid_t gid = NULL;
                char *uname = NULL;
                struct group *grp;
                struct passwd *pw;
                char *home = NULL;
                struct stat status;

                uid = getuid();
                gid = getgid();
                uname = (char*)malloc(10 * sizeof(char));
                uname = getlogin();
                grp = getgrgid(gid);

                //Get home dir
                pw = getpwuid(uid);
                home = pw->pw_dir;
                //Get inode
                stat(home, &status);
                printf("%d, %d, %s, %s, %ld\n", uid, gid, uname, grp->gr_name, status.st_ino);
        } else if (strcmp(cmds[0], "help") == 0) {
                printf("Welcome to your custom shell. The supported commands are:\n\n");
                printf("pw     - Displays current directory.\n\n");
                printf("ifc    - Displays specific network interface. Default is eth0. Add desired interface name to the end of command.\nE.g. ifc eth1\n\n");
                printf("dt     - Displays current date and time.\n\n");
                printf("ud     - Displays user details.\nFormat: User ID, Group ID, Username, Group Name, Home Dir's inode\n\n");
                printf("clear  - Clears the screen.\n")
                printf("exit   - Log out.\n");
        } else if (strcmp(cmds[0], "clear") == 0) {
                system("clear");
        } else if (strcmp(cmds[0], "exit") == 0) {
                return 1;
        } else {
                printf("Unknown command. Please enter 'help' for more informatio
n.\n");
        }
        free(line);
        return 0;
}

main(void) {
        int exit = 0;

        do {
                printf("> ");
                exit = readLine();
        } while(exit == 0);
}
