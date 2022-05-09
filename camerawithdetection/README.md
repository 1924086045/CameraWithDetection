# CameraWithDetection
#### 使用说明

在项目根目录下的build.gradle文件中添加
        
    allprojects {
        repositories {
        ...
        maven { url 'https://jitpack.io' }
        }
    }


在主模块的build.gradle文件中引入依赖：

    dependencies {
        implementation 'com.gitee.yyc2413:camera-with-detection:v1.0.0'
    }


单通道使用

    CameraDetection.builder(MainActivity.this)
    .setMultiChannel(false)//是否为多通道
    .setDetectionProject(project)//设置检测项目
    .build(new OnResultCallback() {

        @Override
        public void resultWithProject(String path, String checkValue, String checkResult) {
                 
        }
                        
        @Override
        public void resultWithProjectLists(String path, ArrayList<QueryRes> queryReBeans) {
                        
        }
    }); 

多通道使用

    CameraDetection.builder(MainActivity.this)
    .setMultiChannel(true)//是否为多通道
    .setDetectionType(DetectionType.JIAO_JIN_TI)//设置检测类型
    .setDetectionProjectList(projects)//设置检测项目列表
    .setMultiList(listData)//设置数据列表
    .build(new OnResultCallback() {
        @Override
        public void resultWithProject(String path, String checkValue, String checkResult) {
                        
        }
                        
        @Override
        public void resultWithProjectLists(String path, ArrayList<QueryRes> queryReBeans) {
        
        }
    });
