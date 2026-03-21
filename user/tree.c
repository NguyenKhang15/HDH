#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void tree(char *path, int depth)
{
    for (int i = 0; i < depth; i++)
        printf("  ");

    char *name = path;
    for (char *p = path; *p; p++)
        if (*p == '/')
            name = p + 1;
    struct stat st;
    if (stat(path, &st) < 0) {
        fprintf(2, "tree: cannot stat %s\n", path);
        return;
    }
    if (st.type == T_DIR) {
        printf("%s/\n", name);
        int fd = open(path, 0);
        if (fd < 0) {
            fprintf(2, "tree: cannot open %s\n", path);
            exit(1); 
        }
        struct dirent de;
        char   child[512];
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            int plen = strlen(path);
            if (plen + 1 + DIRSIZ + 1 > (int)sizeof(child)) {
                fprintf(2, "tree: path too long\n");
                continue;
            }
            memmove(child, path, plen);
            child[plen] = '/';
            memmove(child + plen + 1, de.name, DIRSIZ);
            child[plen + 1 + DIRSIZ] = '\0';
            tree(child, depth + 1);
        }
        close(fd);

    } else {
        printf("%s\n", name);
    }
}

int main(int argc, char *argv[])
{
    if (argc > 2) {
        fprintf(2, "usage: tree [directory]\n");
        exit(1);
    }
    char *root = (argc == 2) ? argv[1] : ".";
    tree(root, 0);
    exit(0);
}