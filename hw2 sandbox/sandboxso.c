#include "sandbox.h"
#include "sandbox.c"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <dirent.h>
#include <fcntl.h>

/* Detect if open needs mode as a third argument (or for openat as a fourth
   argument).  */
#ifdef __O_TMPFILE
# define __OPEN_NEEDS_MODE(oflag) \
  (((oflag) & O_CREAT) != 0 || ((oflag) & __O_TMPFILE) == __O_TMPFILE)
#else
# define __OPEN_NEEDS_MODE(oflag) (((oflag) & O_CREAT) != 0)
#endif

// #define	HOOK_VAR(type, name, argtype)	static type (*original_##name)(argtype) = NULL;
#define LOAD_ORIGIN(funcname)	\
	void *handle = dlopen("libc.so.6", RTLD_LAZY);	\
	if (handle != NULL)	\
	{	\
		original_##funcname = dlsym(handle, #funcname);	\
		dlclose(handle);	\
	}	\
	else	\
	{	\
		fprintf(stderr, "cannot open the libc.so.6\n");	\
	}

#define	CALL_ORIGIN(funcname, ...)	\
	if (original_##funcname != NULL)	\
	{	\
		return original_##funcname(__VA_ARGS__);	\
	}	\
	else	\
	{	\
		fprintf(stderr, "cannot link the loader(%s)\n", #funcname);	\
	}

static void SandboxsoInit() __attribute__((constructor));

void SandboxsoInit()
{
    // TODO init something
}

/* Compare this path with basedir */
bool comparePath(const char *pathname)
{
	if (pathname[0] != '.' && pathname[0] != '/' && strcmp(basedir, ".") == 0)
	{
		return true;
	}

	// deal with /home/*
	if (strstr(pathname, "/home") != NULL && strcmp(basedir, ".") == 0)
	{
		char cwd[FILENAME_MAX];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			if (strstr(pathname, cwd) != NULL)
			{
				return true;
			}
		}
		else
		{
			perror("getcwd() error");
			return false;
		}
	}

	for (int index = 0; index < strlen(basedir); index++)
	{
		if (index >= strlen(pathname))
		{
			break;
		}

		if (basedir[index] != pathname[index])
		{
			return false;
		}
	}

    if (strlen(basedir) < strlen(pathname) && pathname[strlen(basedir)] != '/')
    {
        return false;
    }

	return true;
}

#pragma region chown lchown create
static int (*original_chdir)(const char *) = NULL;
int chdir(const char *pathname)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
		return -1;
	}
	else
	{
		printf("Allow %s operation\n", __FUNCTION__);
		if (original_chdir == NULL)
		{
			LOAD_ORIGIN(chdir);
		}

		CALL_ORIGIN(chdir, pathname);
		return 0;
	}
}

static int (*original_chmod)(const char *, mode_t) = NULL;
int chmod(const char *pathname, mode_t mode)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
		return -1;
	}
	else
	{
		printf("Allow %s operation\n", __FUNCTION__);
		if (original_chmod == NULL)
		{
			LOAD_ORIGIN(chmod);
		}

		CALL_ORIGIN(chmod, pathname, mode);
		return 0;
	}
}

static int (*original_chown)(const char *, uid_t, gid_t) = NULL;
int chown(const char *pathname, uid_t owner, gid_t group)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
		return -1;
	}
	else
	{
		printf("Allow %s operation\n", __FUNCTION__);
		if (original_chown == NULL)
		{
			LOAD_ORIGIN(chown);
		}

		CALL_ORIGIN(chown, pathname, owner, group);
		return 0;
	}
}

static int (*original_lchown)(const char *, uid_t, gid_t) = NULL;
int lchown(const char *pathname, uid_t owner, gid_t group)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
		return -1;
	}
	else
	{
        printf("Allow %s operation\n", __FUNCTION__);
		if (original_lchown == NULL)
		{
			LOAD_ORIGIN(lchown);
		}

		CALL_ORIGIN(lchown, pathname, owner, group);
		return 0;
	}
}

static int (*original_creat)(const char *, mode_t) = NULL;
int creat(const char *pathname, mode_t mode)
{
    if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
		return -1;
	}
	else
	{
        printf("Allow %s operation\n", __FUNCTION__);
		if (original_creat == NULL)
		{
			LOAD_ORIGIN(creat);
		}

		CALL_ORIGIN(creat, pathname, mode);
		return 0;
	}
}
#pragma endregion chdir chmod chown creat


// TODO openat2
#pragma region fopen freopen open open64 openat(?) opendir
static FILE* (*original_fopen)(const char *, const char *) = NULL;
FILE *fopen(const char *pathname, const char *mode)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		printf("Allow %s operation\n", __FUNCTION__);
		if (original_fopen == NULL)
		{
			LOAD_ORIGIN(fopen);
		}

		CALL_ORIGIN(fopen, pathname, mode);
	}

	return NULL;
}

static FILE* (*original_freopen)(const char *, const char *, FILE *) = NULL;
FILE *freopen(const char *pathname, const char *mode, FILE *stream)
{
    if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		printf("Allow %s operation\n", __FUNCTION__);
		if (original_freopen == NULL)
		{
			LOAD_ORIGIN(freopen);
		}

		CALL_ORIGIN(freopen, pathname, mode, stream);
	}

	return NULL;
}

static int (*original_open)(const char *, int, ...) = NULL;
int open(const char *pathname, int oflag, ...)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
        printf("Allow %s operation\n", __FUNCTION__);
		if (original_open == NULL)
		{
			LOAD_ORIGIN(open);
		}

		// open with mode
		if (__OPEN_NEEDS_MODE(oflag))
		{
			va_list arg;
			va_start(arg, oflag);
			mode_t mode = va_arg(arg, mode_t);
			va_end (arg);

			CALL_ORIGIN(open, pathname, oflag, mode);
		}
		else
		{
			CALL_ORIGIN(open, pathname, oflag);
		}
	}

	return -1;
}

static int (*original_open64)(const char *, int, ...) = NULL;
int open64(const char *pathname, int oflag, ...)
{
    if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
        printf("Allow %s operation\n", __FUNCTION__);
		if (original_open64 == NULL)
		{
			LOAD_ORIGIN(open);
		}

        // open64 with mode
		mode_t mode = 0;  // default
		if (__OPEN_NEEDS_MODE(oflag))
		{
			va_list arg;
			va_start(arg, oflag);
			mode = va_arg(arg, mode_t);
			va_end (arg);

		}
		
		CALL_ORIGIN(open64, pathname, oflag, mode);
	}

	return -1;
}

static int (*original_openat)(const char *, int, ...) = NULL;
int openat(int dirfd, const char *pathname, int oflag, ...)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
        printf("Allow %s operation\n", __FUNCTION__);
		if (original_openat == NULL)
		{
			LOAD_ORIGIN(openat);
		}

		// openat with mode
		if (__OPEN_NEEDS_MODE(oflag))
		{
			va_list arg;
			va_start(arg, oflag);
			mode_t mode = va_arg(arg, mode_t);
			va_end (arg);

			CALL_ORIGIN(openat, pathname, oflag, mode);
		}
		else
		{
			CALL_ORIGIN(openat, pathname, oflag);
		}
	}

	return -1;
}

static DIR *(*original_opendir)(const char *) = NULL;
DIR *opendir(const char *pathname)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original_opendir == NULL)
		{
			LOAD_ORIGIN(opendir);
		}

		CALL_ORIGIN(opendir, pathname);
	}

	return NULL;
}
#pragma endregion fopen freopen open open64 openat opendir 


#pragma region mkdir remove rename rmdir
static int (*original_mkdir)(const char *, mode_t) = NULL;
int mkdir(const char *pathname, mode_t mode)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original_mkdir == NULL)
		{
			LOAD_ORIGIN(mkdir);
		}

		CALL_ORIGIN(mkdir, pathname, mode);
	}

	return -1;
}

static int (*original_rmdir)(const char *) = NULL;
int rmdir(const char *pathname)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original_rmdir == NULL)
		{
			LOAD_ORIGIN(rmdir);
		}

		CALL_ORIGIN(rmdir, pathname);
	}

	return -1;
}

static int (*original_rename)(const char *, const char *) = NULL;
int rename(const char *oldpath, const char *newpath)
{
	if (!comparePath(oldpath) || !comparePath(newpath))
	{
		fprintf(stderr, "[sandbox] %s: access to %s(oldpath) or %s(newpath) is not allowed\n", __FUNCTION__, oldpath, newpath);
	}
	else
	{
		if (original_rename == NULL)
		{
			LOAD_ORIGIN(rename);
		}

		CALL_ORIGIN(rename, oldpath, newpath);
	}

	return -1;
}

/*
static uid_t (*original_renameat)(int, const char *, int, const char *) = NULL;
int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath)
{
	if (!comparePath(oldpath) || !comparePath(newpath))
	{
		fprintf(stderr, "[sandbox] %s: access to %s(oldpath) or %s(newpath) is not allowed\n", __FUNCTION__, oldpath, newpath);
	}
	else
	{
		if (original_renameat == NULL)
		{
			LOAD_ORIGIN(renameat);
		}

		CALL_ORIGIN(renameat, olddirfd, oldpath, newdirfd, newpath);
	}

	return -1;
}

static uid_t (*original_renameat2)(int, const char *, int, const char *, unsigned int) = NULL;
int renameat2(int olddirfd, const char *oldpath,
		int newdirfd, const char *newpath, unsigned int flags)
{
	if (!comparePath(oldpath) || !comparePath(newpath))
	{
		fprintf(stderr, "[sandbox] %s: access to %s(oldpath) or %s(newpath) is not allowed\n", __FUNCTION__, oldpath, newpath);
	}
	else
	{
		if (original_renameat2 == NULL)
		{
			LOAD_ORIGIN(renameat2);
		}

		CALL_ORIGIN(renameat2, olddirfd, oldpath, newdirfd, newpath, flags);
	}

	return -1;
}
*/

static int (*original_remove)(const char *) = NULL;
int remove(const char *pathname)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original_remove == NULL)
		{
			LOAD_ORIGIN(remove);
		}

		CALL_ORIGIN(remove, pathname);
	}

	return -1;
}
#pragma endregion mkdir remove rename rmdir


#pragma region stat(__xstat) lstat(__lxstat)
static int (*original_stat)(const char *, struct stat *) = NULL;
int stat(const char *pathname, struct stat *stat_buf)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original_stat == NULL)
		{
			LOAD_ORIGIN(stat);
		}

		CALL_ORIGIN(stat, pathname, stat_buf);
	}

	return -1;
}

static int (*original___xstat)(int, const char *, struct stat *) = NULL;
int __xstat(int ver, const char *pathname, struct stat *stat_buf)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original___xstat == NULL)
		{
			LOAD_ORIGIN(__xstat);
		}

		CALL_ORIGIN(__xstat, ver, pathname, stat_buf);
	}

	return -1;
}

static int (*original_lstat)(const char *, struct stat *) = NULL;
int lstat(const char *pathname, struct stat *stat_buf)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original_lstat == NULL)
		{
			LOAD_ORIGIN(lstat);
		}

		CALL_ORIGIN(lstat, pathname, stat_buf);
	}

	return -1;
}

static int (*original___lxstat)(int, const char *, struct stat *) = NULL;
int __lxstat(int ver, const char *pathname, struct stat *stat_buf)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original___lxstat == NULL)
		{
			LOAD_ORIGIN(__lxstat);
		}

		CALL_ORIGIN(__lxstat, ver, pathname, stat_buf);
	}

	return -1;
}
#pragma endregion stat


#pragma region link readlink symlink unlink
static int (*original_link)(const char *, const char *) = NULL;
int link(const char *path1, const char *path2)
{
	if (!comparePath(path1) || !comparePath(path2))
	{
		fprintf(stderr, "[sandbox] %s: access to %s(path1) or %s(path2) is not allowed\n", __FUNCTION__, path1, path2);
	}
	else
	{
		if (original_link == NULL)
		{
			LOAD_ORIGIN(link);
		}

		CALL_ORIGIN(link, path1, path2);
	}

	return -1;
}

static ssize_t (*original_readlink)(const char *, char *, size_t) = NULL;
ssize_t readlink(const char *pathname, char *buf, size_t bufsize)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original_readlink == NULL)
		{
			LOAD_ORIGIN(readlink);
		}

		CALL_ORIGIN(readlink, pathname, buf, bufsize);
	}

	return -1;
}

static int (*original_symlink)(const char *, const char *) = NULL;
int symlink(const char *path1, const char *path2)
{
	if (!comparePath(path1) || !comparePath(path2))
	{
		fprintf(stderr, "[sandbox] %s: access to %s(path1) or %s(path2) is not allowed\n", __FUNCTION__, path1, path2);
	}
	else
	{
		if (original_symlink == NULL)
		{
			LOAD_ORIGIN(symlink);
		}

		CALL_ORIGIN(symlink, path1, path2);
	}

	return -1;
}

static int (*original_unlink)(const char *) = NULL;
int unlink(const char *pathname)
{
	if (!comparePath(pathname))
	{
		fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", __FUNCTION__, pathname);
	}
	else
	{
		if (original_unlink == NULL)
		{
			LOAD_ORIGIN(unlink);
		}

		CALL_ORIGIN(unlink, pathname);
	}

	return -1;
}
#pragma endregion link readlink symlink unlink


#pragma region execl execle execlp execv system

int execl(const char *pathname, const char *arg, ...)
{
	fprintf(stderr, "[sandbox] %s(%s): not allowed\n", __FUNCTION__, pathname);
	return -1;
}

int execle(const char *pathname, const char *arg, ...)
{
	fprintf(stderr, "[sandbox] %s(%s): not allowed\n", __FUNCTION__, pathname);
	return -1;
}

int execlp(const char *file, const char *arg, ...)
{
	fprintf(stderr, "[sandbox] %s(%s): not allowed\n", __FUNCTION__, file);
	return -1;
}

int execve(const char *pathname, char *const argv[], char *const envp[])
{
	fprintf(stderr, "[sandbox] %s(%s): not allowed\n", __FUNCTION__, pathname);
	return -1;
}

int execv(const char *pathname, char *const argv[])
{
	fprintf(stderr, "[sandbox] %s(%s): not allowed\n", __FUNCTION__, pathname);
	return -1;
}

int execvp(const char *file, char *const argv[])
{
	fprintf(stderr, "[sandbox] %s(%s): not allowed\n", __FUNCTION__, file);
	return -1;
}

int system(const char *cmdstring)
{
	fprintf(stderr, "[sandbox] %s(%s): not allowed\n", __FUNCTION__, cmdstring);
	return -1;
}
#pragma endregion exec* system