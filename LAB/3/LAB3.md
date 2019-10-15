# LAB3

<center>Wu Jiayao, 517370910257
</center>

## 1. Git

### Search what is git

Git is a free and open source distributed version control system designed to handle everything from small to very large projects with speed and efficiency.

### Install a git client.

~~~shell
apt-get install git
~~~

### Search the use of the following git commands:

- **help** Give help information about git

- **init** Create an empty git repository or reinitalize an existing one

- **checkout** Switch branches or restore working tree files

- **branch** List, create, or delete branches

- **push** Update remote refs along with associated objects

- **pull** Fetch from and integrate with another repository or a local branch

- **merge** Join two or more development histories together

- **add** Add file contents to the index

- **diff** Show changes between commits, commit and working tree, etc

- **tag** Create, list, delete or verify a tag object signed with GPG

- **log** Show commit logs

- **fetch** Download objects and refs from another repository

- **commit** Record changes to the repository

- **clone** Clone a repository into a new directory

- **reset** Reset current HEAD to the specified state

## 2. Git game

![](D:\OneDrive - sjtu.edu.cn\JI\2019FA\VE482\LAB\3\1.jpg)

![2](D:\OneDrive - sjtu.edu.cn\JI\2019FA\VE482\LAB\3\2.jpg)

![3](D:\OneDrive - sjtu.edu.cn\JI\2019FA\VE482\LAB\3\3.jpg)

## 3. Working with source code

### 3.1

~~~shell
# 1
pkgin install rsync
# 2
apt-get rsync
# 3
man rsync
# 4
cp -r /usr/src /usr/src_orig
# 5
rsync -az minix3:/usr/src_orig ~/minix3
~~~

### 3.2

~~~shell
# 1
man diff
man patch
# 2
diff -rc /usr/src_orig /usr/src
# 3
rsync -az minix3:/root/data ~/minix3_patch
# 4
cd ~/minix3
patch -p3 ../minix3_patch
# 5
cd ~/minix3
patch -RE -p3../minix3_patch
~~~



## Reference

Git, www.git-scm.com/