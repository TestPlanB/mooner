# mooner
mooner 是一系列疑难native crash解决手段与黑科技实践的集合体，现包括以下功能：

功能1. 捕获 Android 基于“pthread_create” 产生的子线程中异常业务逻辑产生信号，导致的native crash

功能2. 突破Java虚拟机堆大小限制，实现LargeObjectSpace的内存隐藏
## 功能1 捕获范围
1.由pthread_create 创建的线程中，执行的异常业务逻辑

2.捕获sigaction所支持的信号

3.监听信号时采用的是回溯处理，因此不像java 层try catch一样，而是将本次操作“清除”，如果业务强依赖这次操作，请做好开关降级处理

## 功能2 生效范围
隐藏LargeObjectSpace的内存使用（FreeListSpace），提高堆内存的上限，提升大小为当前（FreeListSpace的使用内存大小）

## 详细介绍
功能1：https://juejin.cn/post/7178341941480783931/

功能2: https://juejin.cn/post/7218379300505059365

## 使用说明
已发布到mavencenter仓库
gradle 导入
```
待更新 
implementation 'io.github.TestPlanB:mooner:1.0.0'
```
### 功能1使用

使用方式非常简单：

参数1:需要进行兜底的so库名，全名称 

参数2:需要捕获的信号，比如11 SIGSEGV 

参数3:回调，当异常捕获后就会回调该参数


```
Mooner.initMooner("libmooner.so",11){
                Log.e("mooner","catch exception")
            }

```

### 功能2使用


合适的时候调用以下即可实现堆内存限制突破

```
 Mooner.initMSponge()

```





## 项目层级介绍
* **app下是使用例子**
* **mooner-core 是mooner的核心实现**

## 环境准备
建议直接用最新的稳定版本Android Studio打开工程。目前项目已适配`Android Studio Arctic Fox | 2022.3.1`
### 
