# LAB 6

<center>Wu Jiayao 5173701910257</center>

## 2. Zathura

Code in **zathura-txt**.

## 3. Linklist plugin ver

### 1. Modified

see in **src**

### 2. Add csv part

see in **src**. The test input file is **src/test.csv**

### 3. How to compile

~~~shell
cd src
./fast.sh
~~~

Actually there is one tricky thing that I don't not solve, when cmake makes the **.so**, its filename is a bit different from what I expect. Like, when making **"txt"**, I actually want **"txt.so"**, cmake gives me **"libtxt.so"**. I solve this by changing its name after compiling by shell script **src/fast.sh**