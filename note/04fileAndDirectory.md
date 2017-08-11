 # 04 文件与目录
 
* **stat：返回文件相关信息**
```
#include <sys/stat.h>
int stat(const char* restrict pathname, struct stat* restrict buf);
int fstat(int fd, struct stat* buf);
int lstat(const char* restrict pathname, struct stat* restrict buf);
fstatat(int fd, const char* restrict pathname, struct stat* restrict buf, int flag);
调用成功返回0，失败返回-1
```
stat：接收绝对路径名
fstat：接收一个打开文件的描述符fd
lstat：与stat的区别时，遇到符号链接时，lstat只列出该符号连接的信息，而不会跟随过去
fstatat：与fstat的区别是，可以使用相对于fd的相对路径作为参数

```
struct stat
{
	mode_t				st_mode;	// 文件类型与权限
	ino_t				st_ino;		// i节点号
	dev_t				st_dev;		// 设备号（文件系统）
	dev_t				st_rdev;	// 特殊文件设备号
	nlink_t				st_nlink;	// 链接数
	uid_t				st_uid;		// userID
	gid_t				st_gid;		// groupID
	off_t				st_size;	// 文件长度
	struct timespec		st_atime;	// 文件内容最后被访问时间
	struct timespec		st_mtime;	// 文件内容最后被修改时间
	struct timespec		st_ctime;	// 文件属性最后被修改时间，inode
	blksize_t			st_blksize;	// 最佳IO块长度
	blkcnt_t			st_blocks;	// 
};
```

* **文件类型**：存于st_mode中

1）S_ISREG()：普通文件：最一般的存储文本或二进制的文件
2）S_ISDIR()： 目录文件：里面包含一系列目录项，目录项中包含文件名以及它的inode号
3）S_ISCHR()：块特殊文件：提供对磁盘等设备带缓冲的访问，每次以固定长度进行
4）S_ISBLK()： 字符特殊文件：提供对设备不带缓冲的访问，每次长度可变
5）S_ISFIFO()：FIFO：有名管道，用于进程间通信
6）S_ISSOCK()：套接字：用于不同主机间的进程间通信，网络通信
7）S_ISLNK()： 符号链接：特殊的链接文件，用于指向其他文件，软链接

* **文件的权限**：存于st_mode中

每个文件有9个文件访问位
```
S_IRUSR: 用户可读
S_IWUSR: 用户可写
S_IXUSR: 用户可执行

S_IRGRP: 组内可读
S_IWGRP: 组内可写
S_IXGRP: 组内可执行

S_IROTH: 其他人可读
S_IWOTH: 其他人可写
S_IXOTH: 其他人可执行
```

* **access**：测试访问权限
```
#include <unistd.h>
int access(const char* pathname, int mode);
int faccessat(int fd, const char* pathname, int mode, int flag);
调用成功返回0，出错返回-1
```
使用实际用户ID与实际组ID进行访问权限测试


* **umask**：禁止某个权限位，又称为屏蔽位
```
#include <sys/stat.h>
int umask(mode_t cmask);
返回调用之前的权限值
```

* **chmod**：改变现有文件的访问权限
```
#include <sys/stat.h>
int chmod(const char* pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int fchmodat(int fd, const char* pathname, mode_t mode, int flag);
成功返回0，失败返回-1
```
接收路径名，与需要改变的mode，改变现有文件的权限

* **chown**：改变文件的所有者（用户ID与组ID）
```
#include <unistd.h>
int chown(const char* pathname, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int fchownat(int fd, const char* pathname, uid_t owner, gid_t group, int flag);
int lchown(const char* pathname, uid_t owner, gid_t group);
成功返回0，失败返回-1
```
chown改变文件拥有者，fchown可用于已打开的文件fd
lchown，fchownat对于符号链接文件，改变的是符号链接文件本身

* **truncate**：截断文件
```
int truncate(const char* path, off_t length);
int ftruncate(int fd, off_t length);
成功返回0，失败返回-1
```
若length小于先文件长度，则length之外的数据将无法访问
若length大于现文件长度，则文件长度增加，产生空洞

* **硬链接与软链接**

背景知识：
1）目录中存在着一系列目录项，目录项包含文件名与该文件的i节点号，该文件可能是普通文件也可能是子目录文件
2）每个文件的i节点都有一个链接计数器（st_nlink），记录链接到此i节点的目录项数
**硬链接**：就是增加一个指向该文件的目录项，该文件的i节点会记录所有链接到此i节点的目录项。每删除一个硬链接，链接计数-1，只有当计数减到0时才会删除该文件
优点：只有链接技术为0了才真正删除文件，保证了每个硬链接指向的文件一定存在
缺点：不能链接另一个文件系统的文件；不能链接目录（避免循环链接）
**软链接**：又称为符号链接，是一种特殊的链接文件类型，系统可以识别。对一个文件增加一个软连接，就是新建一个符号链接文件，里面存的是被链接的文件的路径名，可以通过该链接文件直接访问被链接文件，但是被链接文件的i节点不会增加链接计数
优点：可以链接另一个文件系统的文件；可以链接目录
缺点：无法保证被链接的文件一定存在
			
* **link，unlink**：硬链接

任何文件都可以有多个目录项指向其i节点
创建一个指向现有文件的链接用的是link，linkat
创建一个硬链接会使i节点链接计数+1
```
#include <unistd.h>
int link(const char* exitingpathname, const char* newpath);
int linkat(int efd, const char* exitingpathname, int nfd, const char* newpath, int flag);
成功返回0， 失败返回-1
```
删除一个硬链接（目录项）使用的是unlink
删除一个硬链接会使得相应i节点链接计数-1
```
#include <unistd.h>
int unlink(const char* pathname);
int unlinkat(int fd, const char* pathname, int flag);
成功返回0，失败返回-1
```

* **文件删除的两个条件**

1）没有进程打开了该文件
2）该文件i节点链接计数为0

* **符号链接**：即软链接

符号链接是一种特殊的链接文件，系统可识别
该链接文件里面存的是被链接文件的路径名
软连接不增加被链接文件i节点的链接计数
**创建符号链接**
```
#include <unistd.h>
int symlink(const char* actualpath, const char* sympath);
int symlinkat(const char* actualpath, int fd, const char* sympath);
成功返回0，失败返回-1
```
**读取符号链接**
由于链接文件是系统可识别的特殊文件
所以使用open打开链接文件的话，会直接跟随到被链接的文件，而打开被链接的文件
如果要打开符号链接文件本身，而不跟随过去，则需要使用readlink
```
#include<unistd.h>
ssize_t readlink(const char* restrict pathname, char* restrict buf, size_t bufsize);
ssize_t readlinkat(int fd, const char* restrict pathname, char* restrict buf, size_t bufsize)
```

* **修改文件的时间**：atime，mtime，ctime

```
#include <sys/stat.h>
int futimens(int fd, const struct timespec times[2]);
int utimensat(int fd, const char* pathname, const struct timespec times[2], int flag);
调用成功返回0，失败返回-1
```
futimens只能对已打开文件调用，utimensat可以使用文件名来改变
另一种方法是utimes
```
#include <sys/time.h>
int utime(const char* pathname, const struct timeval time[2]);
其中：
struct timeval       // timespec与这个类似，都是用秒和毫秒共同来表示一个时间戳
{
	time_t tv_sec;  // seconds
	long tv_usec;  // microseconds
}
```

* **mkdir，rmdir**：创建与删除目录
```
#include <sys/stat.h>
int mkdir(const char* pathname, mode_t mode);
int mkdirat(int fd, const char* pathname, mode_t mode);

int rmdir(const char* pathname);
成功返回0，失败返回-1
```

* **opendir**：打开读取目录
```
#include<dirent.h>
DIR* opendir(const char* pathname);
DIR* fdopendir(int fd);
```

* **chdir**：改变当前目录，cd命令
```
#include<unistd.h>
int chdir(const char* pathname);
int fchdir(int fd);
```

