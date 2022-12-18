# mooner
捕获 Android 基于“pthread_create” 产生的子线程中异常业务逻辑产生信号，导致的native crash
## 详细介绍


## 使用说明
已发布到mavencenter仓库
gradle 导入
```
todo
```
### 使用
该项目可以用于本地配置使用，只需拷贝mooner-core这个module到自己的项目即可，请按照以下条件使用

使用方式非常简单：

参数1:需要进行兜底的so库名，全名称 

参数2:需要捕获的信号，比如11 SIGSEGV 

参数3:回调，当异常捕获后就会回调该参数


```
Mooner.initMooner("libmooner.so",11){
                Log.e("mooner","catch exception")
            }

```





## 项目层级介绍
* **app下是使用例子**
* **mooner-core 是mooner的核心实现**

## 环境准备
建议直接用最新的稳定版本Android Studio打开工程。目前项目已适配`Android Studio Arctic Fox | 2022.3.1`
### 
