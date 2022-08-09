#include <jni.h>
#include <string>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/objdetect.hpp"
#include <android/log.h>
#include <unordered_map>

using namespace cv;
using namespace std;

#define TAG "detection" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

Mat deskewAndCrop(Mat input, const RotatedRect &box) {
//    LOGD("this is channels :%d",input.channels());
    double angle = box.angle;
    Size2f size = box.size;
    if (angle < -45.0) {
        angle += 90.0;
        swap(size.width, size.height);
    }

    Mat transform = getRotationMatrix2D(box.center, angle, 1.0);
    Mat rotated;
    warpAffine(input, rotated, transform, input.size(), INTER_CUBIC);
    Mat cropped;
    Point p(box.center.x, box.center.y - size.height);
    getRectSubPix(rotated, Size(size.width, size.height * 3), p, cropped);
    copyMakeBorder(cropped, cropped, 10, 10, 10, 10, BORDER_CONSTANT, Scalar(0));
    if (cropped.rows > cropped.cols) {
        transpose(cropped, cropped);
        flip(cropped, cropped, 0);
    }
    resize(cropped, cropped, Size(350, 200));
    return cropped;
}

Mat deskewAndCrop2(Mat input, const RotatedRect &box, Mat &qrmat, Mat &kamat) {
//    LOGD("this is channels :%d",input.channels());
    double angle = box.angle;
    Size2f size = box.size;
    if (angle < -45.0) {
        angle += 90.0;
        swap(size.width, size.height);
    }

    Mat transform = getRotationMatrix2D(box.center, angle, 1.0);
    Mat rotated;
    warpAffine(input, rotated, transform, input.size(), INTER_CUBIC);
    Mat cropped;
    Point p(box.center.x + size.width, box.center.y);
    Point p2(box.center.x + size.width * 1.5, box.center.y);
    getRectSubPix(rotated, Size(size.width * 3, size.height), p, cropped);
    getRectSubPix(rotated, Size(size.width + 30, size.height + 30), box.center, qrmat);
    getRectSubPix(rotated, Size(size.width * 4.5, size.height * 1.2), p2, kamat);
    copyMakeBorder(cropped, cropped, 10, 10, 10, 10, BORDER_CONSTANT, Scalar(0));
    if (cropped.rows > cropped.cols) {
        transpose(cropped, cropped);
        flip(cropped, cropped, 0);
    }
    resize(cropped, cropped, Size(600, 200));
    return cropped;
}

Mat deskewAndCrop3(Mat input, const RotatedRect &box, Mat &qrmat, Mat &kamat) {
//    LOGD("this is channels :%d",input.channels());
    double angle = box.angle;
    Size2f size = box.size;
    if (angle < -45.0) {
        angle += 90.0;
        swap(size.width, size.height);
    }

    Mat transform = getRotationMatrix2D(box.center, angle, 1.0);
    Mat rotated;
    warpAffine(input, rotated, transform, input.size(), INTER_CUBIC);
//    imshow("rotated",rotated);
    Mat cropped;
    Point p(box.center.x + size.width * 1.2, box.center.y);
    Point p2(box.center.x + size.width * 1.5, box.center.y);
    getRectSubPix(rotated, Size(size.width, size.height), p, cropped);
    getRectSubPix(rotated, Size(size.width + 20, size.height + 20), box.center, qrmat);
    getRectSubPix(rotated, Size(size.width * 4.5, size.height * 1.2), p2, kamat);
//    imshow("kamat",kamat);
//    copyMakeBorder(cropped, cropped, 10, 10, 10, 10, BORDER_CONSTANT, Scalar(0));
    if (cropped.rows > cropped.cols) {
        transpose(cropped, cropped);
        flip(cropped, cropped, 0);
    }
//    resize(cropped, cropped, Size(600, 200));
    return cropped;
}

int R = 21;
int rate = 10;

bool getBin(Mat binMat) {
    int cnt = 0;
    for (int i = 0; i < rate; i++) {
        for (int j = 0; j < rate; j++) {
            int data = binMat.at<uchar>(i, j);
            if (data >= 200) {
                cnt++;
            }
        }
    }
    return cnt > (rate * rate * 0.4f);
}

Mat repair(Mat &im) {
    Mat whiteMat(rate, rate, CV_8UC1, Scalar(255));
    Mat blackMat(rate, rate, CV_8UC1, Scalar(0));
    Mat nmat(im.rows, im.cols, CV_8UC1, Scalar(255));

    vector<vector<bool>> b(rate, vector<bool>());
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < R; j++) {
            Rect rect(i * rate, j * rate, rate, rate);
            Mat binMat(im, rect);
            Mat roi = nmat(Rect(i * rate, j * rate, rate, rate));

            if (getBin(binMat)) {
//                cout<<"*"<<" ";
                whiteMat.copyTo(roi);
            } else {
//                cout<<"#"<<" ";
                blackMat.copyTo(roi);
            }

        }
        cout << endl;
    }
    return nmat;
}

Mat repairQRMat(Mat &im) {
    int m = im.rows;
    int n = im.cols;
    Rect rect(15, 15, n - 30, m - 30);
    Mat im2(im, rect);
    int W = R * rate;
    resize(im2, im2, Size(W, W));
    Mat repairMat = repair(im2);
    copyMakeBorder(repairMat, repairMat, 20, 20, 20, 20, BORDER_CONSTANT, Scalar(255));
    return repairMat;
}

Mat speed_rgb2gray(Mat src) {
    Mat dst(src.rows, src.cols, CV_8UC1);
#pragma omp parallel for num_threads(12)
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            dst.at<uchar>(i, j) = ((src.at<Vec3b>(i, j)[0] << 18) + (src.at<Vec3b>(i, j)[0] << 15) +
                                   (src.at<Vec3b>(i, j)[0] << 14) +
                                   (src.at<Vec3b>(i, j)[0] << 11) + (src.at<Vec3b>(i, j)[0] << 7) +
                                   (src.at<Vec3b>(i, j)[0] << 7) + (src.at<Vec3b>(i, j)[0] << 5) +
                                   (src.at<Vec3b>(i, j)[0] << 4) + (src.at<Vec3b>(i, j)[0] << 2) +
                                   (src.at<Vec3b>(i, j)[1] << 19) + (src.at<Vec3b>(i, j)[1] << 16) +
                                   (src.at<Vec3b>(i, j)[1] << 14) + (src.at<Vec3b>(i, j)[1] << 13) +
                                   (src.at<Vec3b>(i, j)[1] << 10) + (src.at<Vec3b>(i, j)[1] << 8) +
                                   (src.at<Vec3b>(i, j)[1] << 4) + (src.at<Vec3b>(i, j)[1] << 3) +
                                   (src.at<Vec3b>(i, j)[1] << 1) +
                                   (src.at<Vec3b>(i, j)[2] << 16) + (src.at<Vec3b>(i, j)[2] << 15) +
                                   (src.at<Vec3b>(i, j)[2] << 14) + (src.at<Vec3b>(i, j)[2] << 12) +
                                   (src.at<Vec3b>(i, j)[2] << 9) + (src.at<Vec3b>(i, j)[2] << 7) +
                                   (src.at<Vec3b>(i, j)[2] << 6) + (src.at<Vec3b>(i, j)[2] << 5) +
                                   (src.at<Vec3b>(i, j)[2] << 4) + (src.at<Vec3b>(i, j)[2] << 1)
                    >> 20);
        }
    }
    return dst;
}


Mat unevenLightCompensate(Mat src, int block_Size) {
    int row = src.rows;
    int col = src.cols;
    Mat gray(row, col, CV_8UC4);
    if (src.channels() == 3) {
        gray = speed_rgb2gray(src);
    } else {
        gray = src;
    }
    float average = mean(gray)[0];
    int new_row = ceil(1.0 * row / block_Size);
    int new_col = ceil(1.0 * col / block_Size);
    Mat new_img(new_row, new_col, CV_8UC4);
    for (int i = 0; i < new_row; i++) {
        for (int j = 0; j < new_col; j++) {
            int rowx = i * block_Size;
            int rowy = (i + 1) * block_Size;
            int colx = j * block_Size;
            int coly = (j + 1) * block_Size;
            if (rowy > row) rowy = row;
            if (coly > col) coly = col;
            Mat ROI = src(Range(rowx, rowy), Range(colx, coly));
            float block_average = mean(ROI)[0];
            new_img.at<float>(i, j) = block_average;
        }
    }
    new_img = new_img - average;
    Mat new_img2;
    resize(new_img, new_img2, Size(col, row), (0, 0), (0, 0), INTER_CUBIC);
    Mat new_src;
    gray.convertTo(new_src, CV_8UC4);
    Mat dst = new_src - new_img2;
    dst.convertTo(dst, CV_8UC4);
    return dst;
}

void marrEdge(const Mat src, Mat &result, int kerValue, double delta) {
    //计算LoG算子
    Mat kernel;
    //半径
    int kerLen = kerValue / 2;
    kernel = Mat_<double>(kerValue, kerValue);
    //滑窗
    for (int i = -kerLen; i <= kerLen; i++) {
        for (int j = -kerLen; j <= kerLen; j++) {
            //生成核因子
            kernel.at<double>(i + kerLen, j + kerLen) =
                    exp(-((pow(j, 2) + pow(i, 2)) / (pow(delta, 2) * 2)))
                    * ((pow(j, 2) + pow(i, 2) - 2 * pow(delta, 2)) / (2 * pow(delta, 4)));
        }
    }
    //设置输入参数
    int kerOffset = kerValue / 2;
    Mat laplacian = (Mat_<double>(src.rows - kerOffset * 2, src.cols - kerOffset * 2));
    result = Mat::zeros(src.rows - kerOffset * 2, src.cols - kerOffset * 2, src.type());
    double sumLaplacian;
    //遍历计算卷积图像的拉普拉斯算子
    for (int i = kerOffset; i < src.rows - kerOffset; ++i) {
        for (int j = kerOffset; j < src.cols - kerOffset; ++j) {
            sumLaplacian = 0;
            for (int k = -kerOffset; k <= kerOffset; ++k) {
                for (int m = -kerOffset; m <= kerOffset; ++m) {
                    //计算图像卷积
                    sumLaplacian += src.at<uchar>(i + k, j + m) *
                                    kernel.at<double>(kerOffset + k, kerOffset + m);
                }
            }
            //生成拉普拉斯结果
            laplacian.at<double>(i - kerOffset, j - kerOffset) = sumLaplacian;
        }
    }
    for (int y = 1; y < result.rows - 1; ++y) {
        for (int x = 1; x < result.cols - 1; ++x) {
            result.at<uchar>(y, x) = 0;
            //领域判定
            if (laplacian.at<double>(y - 1, x) * laplacian.at<double>(y + 1, x) < 0) {
                result.at<uchar>(y, x) = 255;
            }
            if (laplacian.at<double>(y, x - 1) * laplacian.at<double>(y, x + 1) < 0) {
                result.at<uchar>(y, x) = 255;
            }
            if (laplacian.at<double>(y + 1, x - 1) * laplacian.at<double>(y - 1, x + 1) < 0) {
                result.at<uchar>(y, x) = 255;
            }
            if (laplacian.at<double>(y - 1, x - 1) * laplacian.at<double>(y + 1, x + 1) < 0) {
                result.at<uchar>(y, x) = 255;
            }
        }
    }
}

int majorityElement(vector<int> &nums) {
    unordered_map<int, int> m;
    int i;
    for (i = 0; i < nums.size(); i++) {
        if (++m[nums[i]] > (nums.size() / 2))
            return nums[i];
    }
    return nums[0];
}

float majorityElement(vector<float> &nums) {
    unordered_map<float, float> m;
    int i;
    for (i = 0; i < nums.size(); i++) {
        if (++m[nums[i]] > (nums.size() / 2))
            return nums[i];
    }
    return nums[0];
}

bool ContoursSortFun(vector<Point> contour1, vector<Point> contour2) {
    return (contourArea(contour1) > contourArea(contour2));
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_zodolabs_zzf_zj350_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_zodolabs_zzf_zj350_activity_DetectionActivity_detection(JNIEnv *env, jobject instance,
                                                                 jintArray data_, jint width,
                                                                 jint height, jstring imagePath_) {
    jint *data = env->GetIntArrayElements(data_, NULL);
    string imagePath = env->GetStringUTFChars(imagePath_, 0);
    if (data == NULL) {
        return nullptr;
    }
    Mat org(height, width, CV_8UC4, (unsigned char *) data);
//    resize(org, org, Size(org.cols / 4, org.rows / 4));
    cvtColor(org, org, COLOR_BGRA2BGR);
    Mat grey, momat, outmat;
    cvtColor(org, grey, COLOR_BGR2GRAY);
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(grey, momat, MORPH_OPEN, element);
    threshold(momat, outmat, 0, 255, THRESH_OTSU);
    vector<vector<Point> > contours;
    findContours(outmat, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<RotatedRect> areas;
    LOGD("ok %d", 2);
    for (auto contour:contours) {
        auto box = minAreaRect(contour);
        Rect rect = box.boundingRect();
//        LOGD("width: %d, height: %d",rect.width,rect.height);
        if (rect.width > 300 && rect.width < 450 && rect.height > 50 && rect.height < 300) {
            cout << box.boundingRect().width << " , " << box.boundingRect().height << endl;
            areas.push_back(box);
        }
    }
    LOGD("ok %d", 3);
    if (areas.size() == 0) {
        return nullptr;
    }
    LOGD("ok %d", 4);
    jclass list_cls = env->FindClass("java/util/ArrayList");
    jmethodID list_costruct = env->GetMethodID(list_cls, "<init>", "()V");
    jobject list_obj = env->NewObject(list_cls, list_costruct);
    jmethodID list_add = env->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");
    LOGD("qr img cout: %zu", areas.size());
    for (int i = 0; i < areas.size(); i++) {
        Mat cres = deskewAndCrop(org, areas[i]);
        Mat res;
        cvtColor(cres, res, COLOR_BGR2GRAY);
        Mat scalar;
        Scharr(res, scalar, res.depth(), 1, 0);
        GaussianBlur(scalar, scalar, Size(3, 3), 0);
        threshold(scalar, scalar, 0, 255, THRESH_OTSU);
        vector<vector<Point> > linecontours;
        findContours(scalar, linecontours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<RotatedRect> lineareas;
        Point center;
        for (int j = 0; j < linecontours.size(); j++) {
            auto lincontour = linecontours[j];
            auto box = minAreaRect(lincontour);
            if (box.center.x > 180 && box.center.x < 220) {
                if (box.center.y < 120 && box.center.y > 50) {
                    if (box.boundingRect().height > 20) {
                        center = box.center;
                        Point2f P[4];
                        box.points(P);
                        for (int j = 0; j <= 3; j++) {
//                            line(res, P[j], P[(j + 1) % 4], Scalar(0), 1);
                        }
                    }
                }
            }
        }
        if (center.x == 0 || center.y == 0) {
            continue;
        }
        Point a(center.x - 40, center.y - 10);
        Point b(center.x + 100, center.y + 10);
        Rect roi(b, a);
        Mat dst(cres, roi);

        //测试平均灰度
        Mat greydst;
        Scalar meanScalar, devScalar;
        cvtColor(dst, greydst, COLOR_BGR2GRAY);
        meanStdDev(greydst, meanScalar, devScalar);
//        LOGD("平均灰度: %f", meanScalar[0]);
        double meanroatio = 200.0 / meanScalar[0];
        dst.convertTo(dst, -1, meanroatio, 0);

        stringstream ss;
        ss << imagePath;
        ss << "/dst_";
        ss << i;
        ss << "_pic.jpg";
        string imagename = ss.str();
//        Vec3b smean = getMinScalar(dst);//获取最白的点

        Mat bg;
        blur(dst, bg, Size(dst.cols / 10, dst.rows / 10));
        Vec3b smean = bg.at<Vec3b>(bg.rows / 2, bg.cols / 2);
        LOGD("smean: %d, %d, %d", smean[0], smean[1], smean[2]);

        GaussianBlur(dst, dst, Size(9, 9), 0);
        Mat lineMat;
        resize(dst, lineMat, Size(dst.cols, 1));

        vector<Mat> channels;
        split(dst, channels);
        vector<float> arr;
        int chazhi = 5;
        for (int i = 0; i < channels[1].cols; i++) {
            Vec3b ss = lineMat.at<Vec3b>(0, i);
//        LOGD("ss0:%d, smean0:%d, ss1:%d, smean1:%d, ss2:%d, smean2:%d",ss[0],smean[0],ss[1],smean[1],ss[2],smean[2]);
            float dd = pow(ss[0] - smean[0] - chazhi, 2) + pow(ss[1] - smean[1] - chazhi, 2) +
                       pow(ss[2] - smean[2] - chazhi, 2);
//            cout<<"ss0: "<<ss[0]<<" ,ss1:"<<ss[1]<<" ,ss2:"<<ss[2]<<endl;
            arr.push_back((float) sqrt(dd));
        }
        vector<float> beginarr, endarr;
        for (int i = 20; i < 60; i++) {
            beginarr.push_back(arr[i]);
//        cout << "end:::" << arr[i] << endl;
//            LOGD("begin: %f", arr[i]);
        }

        for (int i = 80; i < 120; i++) {
            endarr.push_back(arr[i]);
//        cout << "begin:::" << arr[i] << endl;
//            LOGD("end: %f", arr[i]);
        }
        sort(beginarr.begin(), beginarr.end());
        sort(endarr.begin(), endarr.end());
        float vc = (beginarr[beginarr.size() - 3] + beginarr[beginarr.size() - 2] +
                    beginarr[beginarr.size() - 1]) / 3;
        float vt =
                (endarr[endarr.size() - 3] + endarr[endarr.size() - 2] +
                 endarr[endarr.size() - 1]) / 3;
        cout << "vc:" << vc << " ,vt:" << vt << endl;
        stringstream ss1;
        ss1 << "vc: " << vc << " vt: " << vt;
        LOGD("%s", ss1.str().c_str());
        jclass stucls = env->FindClass("com/zodo/camerawithdetection/bean/JniBeans");
        jmethodID constrocMID = env->GetMethodID(stucls, "<init>",
                                                 "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        stringstream ssvc, ssvt;
        ssvc << vc;
        ssvt << vt;
        jstring jvc = env->NewStringUTF(ssvc.str().c_str());
        jstring jvt = env->NewStringUTF(ssvt.str().c_str());
        jstring jpath = env->NewStringUTF(imagename.c_str());
        jobject jniobj = env->NewObject(stucls, constrocMID, jvc, jvt, jpath);
        env->CallBooleanMethod(list_obj, list_add, jniobj);
    }
    env->ReleaseIntArrayElements(data_, data, 0);
    return list_obj;
}

//求Mat的中位数
int GetMatMidVal(Mat &img) {
    //判断如果不是单通道直接返回128
    if (img.channels() > 1) return 128;
    int rows = img.rows;
    int cols = img.cols;
    //定义数组
    float mathists[256] = {0};
    //遍历计算0-255的个数
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int val = img.at<uchar>(row, col);
            mathists[val]++;
        }
    }
    int calcval = rows * cols / 2;
    int tmpsum = 0;
    for (int i = 0; i < 255; ++i) {
        tmpsum += mathists[i];
        if (tmpsum > calcval) {
            return i;
        }
    }
    return 0;
}

//求自适应阈值的最小和最大值
void GetMatMinMaxThreshold(Mat &img, int &minval, int &maxval, float sigma) {
    int midval = GetMatMidVal(img);
    cout << "midval:" << midval << endl;
    // 计算低阈值
    minval = saturate_cast<uchar>((1.0 - sigma) * midval);
    //计算高阈值
    maxval = saturate_cast<uchar>((1.0 + sigma) * midval);
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_zodo_camerawithdetection_activity_CameraActivity_detection(JNIEnv *env, jobject instance,
                                                                    jintArray data_, jint width,
                                                                    jint height,
                                                                    jstring imagePath_,
                                                                    jboolean isJGD,
                                                                    jint minThresheold,
                                                                    jint maxThresheold) {
    jint *data = env->GetIntArrayElements(data_, NULL);
    string imagePath = env->GetStringUTFChars(imagePath_, 0);
    if (data == NULL) {
        return nullptr;
    }
    Mat org(height, width, CV_8UC4, (unsigned char *) data);
//    unevenLightCompensate(org,32);

    if (!isJGD) {
        resize(org, org, Size(1200, 1400));
    }

    /**
     * 光照补偿
     */
    vector<Mat> g_vChannels;
    Mat dstImage;
    //分离通道
    split(org, g_vChannels);
    Mat imageBlueChannel = g_vChannels.at(0);
    Mat imageGreenChannel = g_vChannels.at(1);
    Mat imageRedChannel = g_vChannels.at(2);
    double imageBlueChannelAvg = 0;
    double imageGreenChannelAvg = 0;
    double imageRedChannelAvg = 0;

    //求各通道的平均值
    imageBlueChannelAvg = mean(imageBlueChannel)[0];
    imageGreenChannelAvg = mean(imageGreenChannel)[0];
    imageRedChannelAvg = mean(imageRedChannel)[0];

    //求出个通道所占增益
    double K = (imageRedChannelAvg + imageGreenChannelAvg + imageRedChannelAvg) / 3;
    double Kb = K / imageBlueChannelAvg;
    double Kg = K / imageGreenChannelAvg;
    double Kr = K / imageRedChannelAvg;

    //更新白平衡后的各通道BGR值
    addWeighted(imageBlueChannel, Kb, 0, 0, 0, imageBlueChannel);
    addWeighted(imageGreenChannel, Kg, 0, 0, 0, imageGreenChannel);
    addWeighted(imageRedChannel, Kr, 0, 0, 0, imageRedChannel);
    merge(g_vChannels, dstImage);//图像各通道合并
    org = dstImage;

    if (org.empty()) {
        return nullptr;
    }
    cvtColor(org, org, COLOR_BGRA2BGR);
    Mat grey, momat, outmat;
    cvtColor(org, grey, COLOR_BGR2GRAY);

    Mat m1, m2;
    GaussianBlur(grey, m1, Size(3, 3), 0);
    blur(grey, m2, Size(grey.cols / 2, grey.rows / 2));
    Mat r = grey;
//    r=moveLightDiff(r,40);
    normalize(r, r, 255, 0, NORM_MINMAX);
//    marrEdge(r,r,11,1);
    Mat gblur, h2_result;
//    GaussianBlur(r, gblur, Size(5, 5), 0);
//    blur(gblur, r, Size(5, 5));
//    Laplacian(r, r, CV_8U, 3, 1, 0, BORDER_DEFAULT);
//    convertScaleAbs(r, r);
//    blur(r, r, Size(9, 9));
//    GaussianBlur(r, r, Size(9, 9), 0);
    vector<vector<Point> > contours;
//    vector<vector<Point> > linecontours;
//    int minthreshold, maxthreshold;
//    GetMatMinMaxThreshold(r, minthreshold, maxthreshold,0.9);//25   55
    //边缘算法呈现轮廓图像并增强效果
    Canny(r, r, minThresheold, maxThresheold, 3, true);// 35    85
    Laplacian(r, r, CV_8U, 7, 1, 0, BORDER_DEFAULT);
//    blur(r, r, Size(3, 3));
//    if (isJGD){
//        Laplacian(r, r, CV_8U, 7, 1, 0, BORDER_DEFAULT);
//    }
//    Laplacian(r, r, CV_8U, 5, 1, 0, BORDER_DEFAULT);
//    Laplacian(r, r, CV_8U, 5, 1, 0, BORDER_DEFAULT);
//    Laplacian(r, r, CV_8U, 3, 1, 0, BORDER_DEFAULT);
//    Laplacian(r, r, CV_8U, 3, 1, 0, BORDER_DEFAULT);
//    Laplacian(r, r, CV_8U, 3, 1, 0, BORDER_DEFAULT);
//    blur(r, r, Size(3, 3));
//    GaussianBlur(r, gblur, Size(3, 3), 0);
    //提取轮廓数据
    findContours(r, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<RotatedRect> areas;
//    vector<Rect> ctLists;
//    vector<Rect> firstList;
//    vector<Rect> rectLists;
    vector<Rect> lineLists;

//    ctLists.clear();

    Mat org_line = org;

    //提取CT区域轮廓
    double area_range = 230;
    for (auto contour:contours) {
        auto box = minAreaRect(contour);
        Rect rect = box.boundingRect();
//        rectLists.push_back(rect);
        //识别CT区域
//        if (rect.width > 100 && rect.width < 500 && rect.height > 15 && rect.height < 250) {
//            firstList.push_back(rect);
//            linecontours.push_back(contour);
//        }
        double rectXMin, rectXMax, rectYMin, rectYMax, rectHeightMin, rectHeightMax, rectWidthMin, rectWidthMax;
        rectXMin = 300;
        rectXMax = 700;
        rectYMin = 180;
        rectYMax = 1200;
        rectHeightMin = 30;
        rectHeightMax = 70;
        rectWidthMin = 180;
        rectWidthMax = 280;
        if (isJGD) {
            rectXMin = 120;
            rectXMax = 550;
            rectYMin = 180;
            rectYMax = 750;
            rectHeightMin = 30;
            rectHeightMax = 120;
            rectWidthMin = 180;
            rectWidthMax = 310;
        }
        if (rect.y > rectYMin && rect.y < rectYMax && rect.x > rectXMin && rect.x < rectXMax) {
            if (rect.width > 180 && rect.width < 280) {
                if (rect.height > rectHeightMin && rect.height < rectHeightMax) {
                    if (areas.size() > 0) {
                        //判断矩阵是否重复
                        bool isHave = false;
                        for (int i = 0; i < areas.size(); i++) {
                            Rect rect_i = areas[i].boundingRect();
                            if ((rect_i.y - rect.y) > -70 && (rect_i.y - rect.y) < 70) {
                                isHave = true;
                                break;
                            }
                        }
                        //判断是否是三联卡
                        if (areas.size() == 1) {
                            double distance = areas[0].center.y - box.center.y;
                            if (distance > 130 && distance < 160) {
                                area_range = 130;
                            }
                        }
                        if (!isHave) {
                            //排除中间无关轮廓
                            double area_dis = areas[areas.size() - 1].center.y - box.center.y;
                            if ((area_dis > 130 && area_dis < 160) ||
                                (area_dis > 230)) {
//                                    ctLists.push_back(rect);
                                areas.push_back(box);
                            }
                        }
                    } else {
//                        ctLists.push_back(rect);
                        areas.push_back(box);
                    }
                }
            }
        }
    }

//    Mat img=org;
//    drawContours(img,linecontours,-1,(0,255,0),2);
//
//    stringstream imstr;
//    imstr << imagePath;
//    imstr << "/line_";
//    imstr << "image.jpg";
//    imwrite(imstr.str().c_str(), img);

    //提取二维码区域轮廓
    Mat mat_qr = m2 - m1;
    normalize(mat_qr, mat_qr, 255, 0, NORM_MINMAX);
    threshold(mat_qr, mat_qr, 70, 255, THRESH_BINARY_INV);

    //开闭运算
    Mat element = getStructuringElement(MORPH_RECT, Size(9, 9));
    erode(mat_qr, mat_qr, element);
    morphologyEx(mat_qr, mat_qr, MORPH_OPEN, element);
    dilate(mat_qr, mat_qr, element);
    morphologyEx(mat_qr, mat_qr, MORPH_CLOSE, element);

    vector<vector<Point> > qr_contours;
    findContours(mat_qr, qr_contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<RotatedRect> qrAreas;
//    if (!isLuoTiao) {
    //识别二维码区域
    for (auto contour:qr_contours) {
        auto box = minAreaRect(contour);
        Rect rect = box.boundingRect();
        if (isJGD) {
            if (box.center.x > 50 && box.center.x < 200 && rect.width > 110 &&
                rect.width < 200 &&
                rect.height > 110 &&
                rect.height < 200) {
                qrAreas.push_back(box);
            }
        } else {
            //识别二维码区域
            if (box.center.x > 200 && box.center.x < 480 && rect.width > 150 &&
                rect.width < 300 &&
                rect.height > 150 &&
                rect.height < 300) {
//                if ((rect.width - rect.height) > -30 && (rect.width - rect.height) < 30) {
                qrAreas.push_back(box);
//                }
            }
        }
    }
//    }

    bool isHaveQr = false;
    if (qrAreas.size() > 0) {
        isHaveQr = true;
    }

    //识别裸条区域
    bool isLuoTiao = false;
    vector<RotatedRect> areaLines;
    vector<Rect> lt_rects;
    if (!isJGD && !isHaveQr) {
        if (areas.size() <= 1) {
            for (auto contour:contours) {
                auto box = minAreaRect(contour);
                Rect rect = box.boundingRect();
                //识别裸条区域
                if (rect.width > 50 && rect.width < 1200 && rect.height > 20 && rect.height < 90) {
                    lt_rects.push_back(rect);
                    if (rect.y > 200 && rect.y < 1200) {
                        if (areaLines.size() == 0) {
                            lineLists.push_back(rect);
                            areaLines.push_back(box);
                        } else {
                            //判断矩阵是否重复
                            bool isHave = false;
                            for (int i = 0; i < areaLines.size(); i++) {
                                Rect rect_i = areaLines[i].boundingRect();
                                if ((rect_i.y - rect.y) > -220 && (rect_i.y - rect.y) < 220) {
                                    isHave = true;
                                    break;
                                }
                            }
                            if (!isHave) {
                                lineLists.push_back(rect);
                                areaLines.push_back(box);
                            }
                        }
                    }
                }
            }
            if (lineLists.size() > 0) {
                if (areas.size() == 0) {
                    isLuoTiao = true;
                    areas = areaLines;
                } else {
                    bool isJunYunJianGe = true;
                    for (int i = 0; i < lineLists.size(); i++) {
                        if (i < lineLists.size() - 1) {
                            double line_range = lineLists[i].y - lineLists[i + 1].y;
                            if (line_range < 220 || line_range > 280) {
                                isJunYunJianGe = false;
                                break;
                            }
                        }
                    }
                    if (isJunYunJianGe) {
                        isLuoTiao = true;
                        areas = areaLines;
                    }
                }
            }

            for (int i = 0; i < lineLists.size(); i++) {
                lineLists[i].x = 750;
                lineLists[i].width = 250;
            }
        }
    }


    stringstream imagestr;
    imagestr << imagePath;
    imagestr << "/dst_";
    imagestr << "image.jpg";
    if (!r.empty()) {
        imwrite(imagestr.str().c_str(), r);
    }


    if (areas.size() == 0) {
        return nullptr;
    }
    jclass list_cls = env->FindClass("java/util/ArrayList");
    jmethodID list_costruct = env->GetMethodID(list_cls, "<init>", "()V");
    jobject list_obj = env->NewObject(list_cls, list_costruct);
    jmethodID list_add = env->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");
    jclass stucls = env->FindClass("com/zodo/camerawithdetection/bean/JniBeans");
    jmethodID constrocMID = env->GetMethodID(stucls, "<init>",
                                             "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    LOGD("qr img cout: %zu", areas.size());

    //如果带有二维码的卡没识别到检测区域，则用二维码进行二次定位
    if (qrAreas.size() > 0 && qrAreas.size() >= areas.size() && !isLuoTiao) {
        RotatedRect rotatedRect = areas[0];
        areas.clear();
        for (int i = 0; i < qrAreas.size(); i++) {
            if (isJGD) {
                rotatedRect.center.x = qrAreas[i].center.x + 235;
                rotatedRect.center.y = qrAreas[i].center.y;
            } else {
                rotatedRect.center.x = qrAreas[i].center.x + 263;
                rotatedRect.center.y = qrAreas[i].center.y - 10;
            }

//            rotatedRect.center.y=qrAreas[i].center.y;
            areas.push_back(rotatedRect);
        }
    }
    for (int i = 0; i < areas.size(); i++) {
        bool isHaveQr = false;
        //将二维码区域和检测卡匹配
        RotatedRect rotatedRect;
        if (qrAreas.size() > 0) {
            for (int j = 0; j < qrAreas.size(); j++) {
                if ((qrAreas[j].center.y - areas[i].center.y) < 50 &&
                    (qrAreas[j].center.y - areas[i].center.y) > -50
                    && (qrAreas[j].center.x - areas[i].center.x) < 350 &&
                    (qrAreas[j].center.x - areas[i].center.x) > -350) {
                    isHaveQr = true;
                    rotatedRect = qrAreas[j];
                    break;
                }
            }
        }

        //如果匹配到二维码，截取二维码图片
        stringstream ssqr;
        if (isHaveQr) {
            Mat qrmat, kamat;
            Mat cres = deskewAndCrop2(org, rotatedRect, qrmat, kamat);
            Mat res;
            cvtColor(cres, res, COLOR_BGR2GRAY);
            if (!qrmat.empty()) {
                cvtColor(qrmat, qrmat, COLOR_BGR2GRAY);
                Scalar meanScalar0, devScalar0;
                meanStdDev(qrmat, meanScalar0, devScalar0);
                double meanroatio0 = 150.0 / meanScalar0[0];
                qrmat.convertTo(qrmat, -1, meanroatio0, 0);
                normalize(qrmat, qrmat, 255, 0, NORM_MINMAX);

                //二维码优化
                Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
                morphologyEx(qrmat, qrmat, MORPH_OPEN, element2);
                morphologyEx(qrmat, qrmat, MORPH_CLOSE, element2);

                resize(qrmat, qrmat, Size(800, 800));
                normalize(qrmat, qrmat, 255, 0, NORM_MINMAX);
                Mat qrmatblur1, qrmatblur2;
                GaussianBlur(qrmat, qrmatblur1, Size(5, 5), 0);
                GaussianBlur(qrmat, qrmatblur2, Size(155, 155), 0);
                Mat mask = qrmatblur1 - qrmatblur2;
                qrmat = qrmat + mask;
                threshold(qrmat, qrmat, 0, 255, THRESH_OTSU);
                GaussianBlur(qrmat, qrmat, Size(5, 5), 0);

                ssqr << imagePath;
                ssqr << "/dst_";
                ssqr << i;
                ssqr << "_qr.jpg";
                imwrite(ssqr.str().c_str(), qrmat);
            }
        }

        Mat cres;
        if (isLuoTiao) {
            cres = org(lineLists[i]);
        } else {
//            if (isHaveQr&&qrAreas.size()>=areas.size()){
//                Rect cres_rect=Rect();
//                cres_rect.x=areas[i].center.x;
//                cres = org(areas[i].boundingRect());
//            } else{
            cres = org(areas[i].boundingRect());
//            }
        }
//        Mat h1_result;
//        Mat h1_kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
//        filter2D(cres, h1_result, CV_8U, h1_kernel);
//        convertScaleAbs(h1_result, cres);

        resize(cres, cres, Size(150, 50));
        stringstream sska;
        sska << imagePath;
        sska << "/ka_";
        sska << i;
        sska << ".jpg";
        if (!cres.empty()) {
            imwrite(sska.str().c_str(), cres);
        }
        Mat res;
        cvtColor(cres, res, COLOR_BGR2GRAY);

        //识别CT线区域
        Mat scalar;
        GaussianBlur(res, res, Size(11, 11), 0);
        Scharr(res, scalar, res.depth(), 1, 0);
        Mat scalar1;
        GaussianBlur(scalar, scalar1, Size(11, 11), 0);
        Mat scalar2;
        blur(scalar, scalar2, Size(scalar.cols / 2, scalar.rows / 2));
        Mat scalarrr = scalar1 - scalar2;
        Mat scalar_line = scalarrr;
        normalize(scalar_line, scalar_line, 255, 0, NORM_MINMAX);

        Mat gau;
//        GaussianBlur(scalarrr, gau, Size(11, 11), 0);
//        blur(scalarrr, scalarrr, Size(5, 5));
//        Laplacian(scalar_line, scalar_line, CV_8U, 5, 1, 0, BORDER_DEFAULT);
        convertScaleAbs(scalar_line, scalar_line);
//        Laplacian(scalarrr, scalarrr, CV_8U, 3, 1, 0, BORDER_DEFAULT);
//        cvtColor(cres, res, COLOR_BGRA2BGR);
//        cvtColor(res, res, COLOR_BGR2GRAY);
//        Mat scalarrr;
//        GaussianBlur(scalarrr, scalarrr, Size(5, 5), 0);
//        blur(scalarrr, scalarrr, Size(3, 3));
//        Canny(scalarrr, scalarrr, 100, 200, 5, true);

        vector<vector<Point> > linecontours; //CT线轮廓数据列表
//        vector<vector<Point> > org_contours;
        //提取CT线轮廓数据
        findContours(scalar_line, linecontours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<Point> lineareas;
        Point center;
        Point line_T;//T线
        vector<Rect> list;
        list.clear();
        Rect rect_c, rect_t;
        bool isHaveLineT = false;
        bool isHaveLineC = false;
        sort(linecontours.begin(), linecontours.end(), ContoursSortFun);
        for (int j = 0; j < linecontours.size(); j++) {
            auto lincontour = linecontours[j];
            auto box = minAreaRect(lincontour);
            if (box.center.x < 0) {
                box.center.x = 0;
            }
            if (box.center.y < 0) {
                box.center.y = 0;
            }
            Rect boxRect = box.boundingRect();
            if (boxRect.x < 0) {
                boxRect.x = 0;
            }
            if (boxRect.y < 0) {
                boxRect.y = 0;
            }

            //判断CT线区域位置
            if (box.boundingRect().height > 22) {
                if (box.center.y < 39 && box.center.y > 11) {
//                    org_contours.push_back(lincontour);
                    if (!isHaveLineC) {
                        if (box.center.x > 15 && box.center.x < 75 &&
                            box.boundingRect().width > 8 && box.boundingRect().width < 40) {
                            list.push_back(boxRect);
                            rectangle(scalar_line, boxRect, Scalar(255, 255, 0), 1);
                            center = box.center;
                            Point2f P[4];
                            box.points(P);
                            isHaveLineC = true;
                            rect_c = boxRect;
                        }
                    }
                    //T线已识别则跳过
                    if (!isHaveLineT) {
                        int lineTWidthMin = 8;
                        int lineTWidthMax = 40;
                        int lineTXMin = 85;
                        int lineTXMax = 130;
                        if (isJGD) {
                            lineTWidthMin = 8;
                            lineTWidthMax = 30;
                            lineTXMin = 75;
                            lineTXMax = 130;
                        }
                        if (box.center.x > lineTXMin && box.center.x < lineTXMax &&
                            box.boundingRect().width > lineTWidthMin &&
                            box.boundingRect().width < lineTWidthMax) {
                            list.push_back(boxRect);
                            rectangle(scalar_line, boxRect, Scalar(255, 255, 0), 2);
                            line_T = box.center;
                            isHaveLineT = true;
                            rect_t = boxRect;
                        }
                    }
                }
            }
        }

        if (!isHaveLineC && !isHaveLineT) {
            continue;
        }

        Mat cres_new = cres;
//        drawContours(cres_new,org_contours, -1, (0,0,255),2);
//
//    stringstream orgstr;
//    orgstr << imagePath;
//    orgstr << "/cres_new_";
//        orgstr << i;
//    orgstr << ".jpg";
//    if (!org_line.empty()) {
//        imwrite(orgstr.str().c_str(), cres_new);
//    }

        stringstream scalarimg;
        scalarimg << imagePath;
        scalarimg << "/scalar_";
        scalarimg << i;
        scalarimg << ".jpg";

        if (!scalar_line.empty()) {
            imwrite(scalarimg.str().c_str(), scalar_line);
        }
        if (center.x == 0 || center.y == 0) {
            //返回无效卡
            jstring jvc = env->NewStringUTF("0");
            jstring jvt = env->NewStringUTF("0");
            jstring jpro = env->NewStringUTF("");
            jstring jpath = env->NewStringUTF(sska.str().c_str());
            jstring jma = env->NewStringUTF("");
            jobject jniobj = env->NewObject(stucls, constrocMID, jvc, jvt, jpro, jpath, jma);
            env->CallBooleanMethod(list_obj, list_add, jniobj);
            continue;
        }
        if (center.x < 0) {
            center.x = 0;
        }
        if (center.y < 0) {
            center.y = 0;
        }
//        LOGD("test %d", 2);
//        Point a(center.x - 10, center.y - 8);
//        Point b(line_T.x+10, line_T.y + 8);
        Point a, b;
        //判断T线是否存在
        if (center.y < 11) {
            center.y = 11;
        }
        if (center.y > 39) {
            center.y = 39;
        }
        if (isHaveLineT) {
            //处理宽高超出范围的情况
            double point_c = rect_c.x;
            if (point_c < 0) {
                point_c = 0;
            }
            double point_t = rect_t.x + rect_t.width;
            if (point_t > cres.cols) {
                point_t = cres.cols;
            }
            double pointyc = center.y - 8;
            if (pointyc < 0) {
                pointyc = 0;
            }
            double pointyt = center.y + 8;
            if (pointyt > cres.rows) {
                pointyt = cres.rows;
            }
            a = Point(point_c, pointyc);
            b = Point(point_t, pointyt);
        } else {
            //处理宽高超出范围的情况
            double point_c = rect_c.x;
            if (point_c < 0) {
                point_c = 0;
            }
            double point_t = rect_c.x + rect_c.width + 30;
            if (isJGD) {
                point_t = rect_c.x + rect_c.width + 15;
            }
            if (point_t > cres.cols) {
                point_t = cres.cols;
            }
            double pointyc = center.y - 8;
            if (pointyc < 0) {
                pointyc = 0;
            }
            double pointyt = center.y + 8;
            if (pointyt > cres.rows) {
                pointyt = cres.rows;
            }
            a = Point(point_c, pointyc);
            b = Point(point_t, pointyt);
        }
        Rect roi(b, a);
        Mat dst(cres, roi);

        stringstream ss;
        ss << imagePath;
        ss << "/dst_";
        ss << i;
        ss << "_pic.jpg";
        string imagename = ss.str();
        if (!dst.empty()) {
            imwrite(ss.str().c_str(), dst);
        }

//        if (isHaveLineT) {
//            stringstream ss_end;
//            ss_end << imagePath;
//            ss_end << "/dst_";
//            ss_end << i;
//            ss_end << "_end";
//            ss_end << "_pic.jpg";
//            string end = ss_end.str();
//            Mat img_end = dst(Rect(Point(dst.cols - rect_t.width, 0), Point(dst.cols, dst.rows)));
//            imwrite(ss_end.str().c_str(), img_end);
//        }

        //测试平均灰度
        Mat greydst;
        Scalar meanScalar, devScalar;
        cvtColor(dst, greydst, COLOR_BGR2GRAY);
        meanStdDev(greydst, meanScalar, devScalar);
        double meanroatio = 200.0 - meanScalar[0];
        double devroatio = 10 / devScalar[0];
        dst.convertTo(dst, -1, 1, meanroatio);

//        Vec3b smean = getMinScalar(dst);

        Mat hsvmat, linemat, blurmat;
        Mat matArr[3];
        vector<float> arr;
        vector<float> linearr;
        cvtColor(dst, hsvmat, COLOR_BGR2HSV);

//        stringstream dstss;
//        dstss << imagePath;
//        dstss << "/hsv_";
//        dstss << i;
//        dstss << "_pic.jpg";
//        string dstname = dstss.str();
//        imwrite(dstss.str().c_str(), hsvmat);

        split(hsvmat, matArr);
        GaussianBlur(matArr[1], blurmat, Size(3, 3), 0);

        Mat mask, dst_mask;
        inRange(dst, Scalar(0, 13, 46), Scalar(190, 255, 255), mask);

        vector<vector<Point> > dst_contours;
        findContours(mask, dst_contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<Rect> dst_rects;
        double s_c, s;
        if (isLuoTiao) {
            bool isExistT = false;
            for (auto contour:dst_contours) {
                Rect rect = minAreaRect(contour).boundingRect();
                dst_rects.push_back(rect);
                if (rect.height > 5 && rect.x > mask.cols / 2) {
                    isExistT = true;
                    break;
                }
//                if (rect.height >= 15) {
//                    if (rect.y < 0) {
//                        rect.y = 0;
//                    }
//                    if (rect.x < 0) {
//                        rect.x = 0;
//                    }
//
//                    //重新定位T线
//                    if (rect.x > mask.cols / 2) {
//                        rect_t = rect;
//                    }
//                }
            }

//            Mat new_dst, cres_dst;
//            if (isExistT) {
//                double clur_x = rect_t.x + rect_t.width;
//                if (clur_x > blurmat.cols) {
//                    clur_x = blurmat.cols;
//                }
//                b = Point(clur_x, blurmat.rows);
//                Rect dst_roi(b, Point(0, 0));
//                new_dst = Mat(blurmat, dst_roi);
//                resize(new_dst, linemat, Size(new_dst.cols, 1));
////            dst_mask=Mat(dst,dst_roi);
//                cres_dst = dst;
//            } else {
//                resize(blurmat, linemat, Size(blurmat.cols, 1));
////            dst_mask=dst;
//                cres_dst = cres;
//            }
//        GaussianBlur(matArr[0], linemat, Size(3, 3), 0);


            isHaveLineT = isExistT;
        }
        resize(blurmat, linemat, Size(blurmat.cols, 1));
        // 图像均值 和 标准方差
        Mat meanMat_c, stdMat_c;
        double std_xc = rect_c.width;
        if (std_xc > dst.cols) {
            std_xc = dst.cols;
        }
        dst_mask = Mat(dst, Rect(Point(0, 0), Point(std_xc, dst.rows)));
        meanStdDev(Mat(dst, Rect(Point(0, 0), Point(std_xc, dst.rows))), meanMat_c, stdMat_c);
        double m_c = meanMat_c.at<double>(0, 0);
        s_c = stdMat_c.at<double>(0, 0);

//            stringstream ss_line;
//        ss_line << imagePath;
//
//        ss_line << "/dst_hsv_";
//        ss_line << i;
//        ss_line << "_pic.jpg";
//        imwrite(ss_line.str().c_str(), dst_mask);

        //T线
        if (isHaveLineT) {
            Mat meanMat, stdMat;
            double std_xc = dst.cols - rect_t.width;
            if (std_xc < 0) {
                std_xc = 0;
            }
            meanStdDev(Mat(dst, Rect(Point(std_xc, 0), Point(dst.cols, dst.rows))),
                       meanMat, stdMat);

            double m = meanMat.at<double>(0, 0);
            s = stdMat.at<double>(0, 0);
        } else {
            s = 0;
        }
//        } else{
//            resize(blurmat, linemat, Size(dst.cols, 1));
//        }


        for (int i = 0; i < linemat.cols; i++) {
            float data = (float) linemat.at<uchar>(0, i);
            arr.push_back(data);
        }
        float majority = majorityElement(arr);
        vector<float> beginarr, endarr;
        for (int i = 0; i < rect_c.width; i++) {
            beginarr.push_back(arr[i]);
        }
        sort(beginarr.begin(), beginarr.end());
        if (isHaveLineT) {

            for (int i = linemat.cols - rect_t.width; i < linemat.cols; i++) {
                endarr.push_back(arr[i]);
            }

            sort(endarr.begin(), endarr.end());
        }

        double vc, vt, scale;
        if (isJGD) {
            scale = 4.2;
        } else {
            scale = 3.5;
        }
        if (beginarr.size() == 0) {
            vc = 0;
        } else {
//            double total=0;
//            int min=0;
//            if (beginarr.size()>10){
//                min=beginarr.size()-10;
//            }
//            for (int j = beginarr.size()-1; j >=min; j--) {
//                total+=beginarr[j];
//            }
//            vc=total/(beginarr.size()-min);
//            if (majority>29) {
//                vc = (beginarr[beginarr.size() - 3] - majority + beginarr[beginarr.size() - 2] -
//                      majority +
//                      beginarr[beginarr.size() - 1] - majority) / 3;
//                if (vc<0){
//                    vc=0-vc;
//                }
//            }else{
//                vc = (beginarr[beginarr.size() - 3]  + beginarr[beginarr.size() - 2] +
//                      beginarr[beginarr.size() - 1] ) / 3;
//            if (!isLuoTiao){
//                vc = (beginarr[beginarr.size() - 3]  + beginarr[beginarr.size() - 2] +
//                      beginarr[beginarr.size() - 1] ) / 3;
//            } else {
            vc = s_c * scale * 35 / 12;
//            }
//            }
        }
        if (endarr.size() == 0 || !isHaveLineT) {
            vt = 0;
        } else {
//            double total=0;
//            int min=0;
//            if (endarr.size()>10){
//                min=endarr.size()-10;
//            }
//            for (int j = endarr.size()-1; j >=min; j--) {
//                total+=endarr[j];
//            }
//            vt=total/(endarr.size()-min);

//            if (majority>29) {
//                vt = (endarr[endarr.size() - 3] - majority + endarr[endarr.size() - 2] - majority +
//                      endarr[endarr.size() - 1] - majority) / 3;
//                if (vt < 0) {
//                    vt = 0 - vt;
//                }
//            } else{
//                vt = (endarr[endarr.size() - 3]  + endarr[endarr.size() - 2]  +
//                      endarr[endarr.size() - 1] ) / 3;
//            if (!isLuoTiao){
//                vt = (endarr[endarr.size() - 3]  + endarr[endarr.size() - 2]  +
//                      endarr[endarr.size() - 1] ) / 3;
//            } else {
            vt = s * scale * 35 / 12;
//            }
//            }

//            double sumvt = std::accumulate(std::begin(endarr), std::end(endarr), 0.0);
//            vt = sumvt / endarr.size(); //均值
//            vt = majorityElement(endarr);
        }


        cout << "vc:" << vc << " ,vt:" << vt << endl;
        stringstream ss1;
        ss1 << "vc: " << vc << " vt: " << vt;
        LOGD("ss1 :: %s", ss1.str().c_str());

        stringstream ssvc, ssvt, ssmajority;
        ssvc << vc;
        ssvt << vt;
        ssmajority << majority;
        jstring jvc = env->NewStringUTF(ssvc.str().c_str());
        jstring jvt = env->NewStringUTF(ssvt.str().c_str());
        jstring jmajority = env->NewStringUTF(ssmajority.str().c_str());
        jstring jpro = env->NewStringUTF("");
        if (isHaveQr) {
            jpro = env->NewStringUTF(ssqr.str().c_str());
        }
//        jstring jpath = env->NewStringUTF(imagename.c_str());
        jstring jpath = env->NewStringUTF(sska.str().c_str());
        jobject jniobj = env->NewObject(stucls, constrocMID, jvc, jvt, jpro, jpath, jmajority);
        env->CallBooleanMethod(list_obj, list_add, jniobj);
    }
    env->ReleaseIntArrayElements(data_, data, 0);
    return list_obj;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_zodolabs_zzf_zj350_activity_DetectionCardActivity_detectionWithQR(JNIEnv *env,
                                                                           jobject instance,
                                                                           jintArray data_,
                                                                           jint width, jint height,
                                                                           jstring imagePath_) {
    jint *data = env->GetIntArrayElements(data_, NULL);
    string imagePath = env->GetStringUTFChars(imagePath_, 0);
    if (data == NULL) {
        return nullptr;
    }
    Mat org(height, width, CV_8UC4, (unsigned char *) data);
    resize(org, org, Size(1200, 1400));
    cvtColor(org, org, COLOR_BGRA2BGR);
    Mat grey, momat, outmat;
    cvtColor(org, grey, COLOR_BGR2GRAY);

    Mat m1, m2;
    GaussianBlur(grey, m1, Size(3, 3), 0);
    blur(grey, m2, Size(grey.cols / 2, grey.rows / 2));
    Mat r = grey;
//    r=moveLightDiff(r,40);
    normalize(r, r, 255, 0, NORM_MINMAX);
//    threshold(r, r, 5, 255, THRESH_BINARY_INV);

    //开闭运算
//    Mat element = getStructuringElement(MORPH_RECT, Size(9, 9));
//    erode(r, r, element);
//    morphologyEx(r, r, MORPH_OPEN, element);
//    dilate(r, r, element);
//    morphologyEx(r, r, MORPH_CLOSE, element);
    GaussianBlur(r, r, Size(3, 3), 0);
//    blur(r, r, Size(3, 3));
    vector<vector<Point> > contours;
    Canny(r, r, 25, 50, 3, false);
    findContours(r, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<RotatedRect> areas;
    vector<Rect> rectLists;
    vector<Point> centers;
    vector<Rect> ctLists;

    rectLists.clear();
    centers.clear();
    ctLists.clear();

    for (auto contour:contours) {
        auto box = minAreaRect(contour);
        Rect rect = box.boundingRect();
        if (rect.width > 800 && rect.width < 1100 && rect.height > 100 && rect.height < 300) {
            rectangle(r, rect, Scalar(0, 0, 255), 2);
            rectLists.push_back(rect);
            centers.push_back(box.center);
        }
        if (rect.width > 170 && rect.width < 280 && rect.height > 30 && rect.height < 70) {
            ctLists.push_back(rect);
        }

//        LOGD("width: %d, height: %d",rect.width,rect.height);
        if (box.center.x < 480 && rect.width > 150 && rect.width < 300 && rect.height > 150 &&
            rect.height < 300) {
            cout << box.boundingRect().width << " , " << box.boundingRect().height << endl;
            areas.push_back(box);
        }
    }
    stringstream imagestr;
    imagestr << imagePath;
    imagestr << "/dst_";
    imagestr << "image.jpg";

    imwrite(imagestr.str().c_str(), r);
    if (areas.size() == 0) {
        return nullptr;
    }
    jclass list_cls = env->FindClass("java/util/ArrayList");
    jmethodID list_costruct = env->GetMethodID(list_cls, "<init>", "()V");
    jobject list_obj = env->NewObject(list_cls, list_costruct);
    jmethodID list_add = env->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");
    jclass stucls = env->FindClass("com/zodo/camerawithdetection/bean/JniBeans");
    jmethodID constrocMID = env->GetMethodID(stucls, "<init>",
                                             "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    LOGD("qr img cout: %zu", areas.size());
    for (int i = 0; i < areas.size(); i++) {
        Mat qrmat, kamat;
        Mat cres = deskewAndCrop2(org, areas[i], qrmat, kamat);
        stringstream sska;
        sska << imagePath;
        sska << "/ka_";
        sska << i;
        sska << ".jpg";
        Mat res;
        cvtColor(cres, res, COLOR_BGR2GRAY);
        stringstream ssqr;
        //如果没有识别到二维码区域则跳过
        if (!qrmat.empty()) {
            cvtColor(qrmat, qrmat, COLOR_BGR2GRAY);
            Scalar meanScalar0, devScalar0;
            meanStdDev(qrmat, meanScalar0, devScalar0);
//        LOGD("平均灰度: %f", meanScalar0[0]);
            double meanroatio0 = 150.0 / meanScalar0[0];
            qrmat.convertTo(qrmat, -1, meanroatio0, 0);
//        GaussianBlur(qrmat, qrmat, Size(3, 3), 0);
//        equalizeHist(qrmat, qrmat);

//        Mat qrm1, qrm2;
//        GaussianBlur(qrmat, qrm1, Size(3, 3), 0);
//        GaussianBlur(qrmat, qrm2, Size(555 , 555 ),0);
//        qrmat = qrm1 - qrm2;
            normalize(qrmat, qrmat, 255, 0, NORM_MINMAX);

            //二维码优化
//        adaptiveThreshold(qrmat, qrmat, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 5);
            Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
//        erode(qrmat, qrmat, element2);
            morphologyEx(qrmat, qrmat, MORPH_OPEN, element2);
//        dilate(qrmat, qrmat, element2);
            morphologyEx(qrmat, qrmat, MORPH_CLOSE, element2);

            resize(qrmat, qrmat, Size(800, 800));
            normalize(qrmat, qrmat, 255, 0, NORM_MINMAX);
            Mat qrmatblur1, qrmatblur2;
            GaussianBlur(qrmat, qrmatblur1, Size(5, 5), 0);
            GaussianBlur(qrmat, qrmatblur2, Size(155, 155), 0);
            Mat mask = qrmatblur1 - qrmatblur2;
            qrmat = qrmat + mask;
            threshold(qrmat, qrmat, 0, 255, THRESH_OTSU);
            GaussianBlur(qrmat, qrmat, Size(5, 5), 0);
//        normalize(qrmat,qrmat,400,0,NORM_MINMAX);
//        threshold(qrmat,qrmat,0,255,THRESH_OTSU);
//        adaptiveThreshold(qrmat, qrmat, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 5);
//        qrmat=repairQRMat(qrmat);


            ssqr << imagePath;
            ssqr << "/dst_";
            ssqr << i;
            ssqr << "_qr.jpg";

            imwrite(ssqr.str().c_str(), qrmat);
        }

        Mat scalar;
        GaussianBlur(res, res, Size(3, 3), 0);
        Scharr(res, scalar, res.depth(), 1, 0);
        Mat scalar1;
        GaussianBlur(scalar, scalar1, Size(3, 3), 0);
        Mat scalar2;
        blur(scalar, scalar2, Size(scalar.cols / 2, scalar.rows / 2));
        Mat scalarrr = scalar1 - scalar2;
        normalize(scalarrr, scalarrr, 255, 0, NORM_MINMAX);
        stringstream scalarimg;
        scalarimg << imagePath;
        scalarimg << "/scalar_";
        scalarimg << i;
        scalarimg << ".jpg";

        imwrite(scalarimg.str().c_str(), scalarrr);

        vector<vector<Point> > linecontours;
        findContours(scalarrr, linecontours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<RotatedRect> lineareas;
        Point center;
        vector<Rect> list;
        for (int j = 0; j < linecontours.size(); j++) {
            auto lincontour = linecontours[j];
            auto box = minAreaRect(lincontour);
            Rect boxRect = box.boundingRect();
            list.push_back(boxRect);
            if (box.center.x > 260 && box.center.x < 360) {
                if (box.center.y < 150 && box.center.y > 50) {
                    if (box.boundingRect().height > 20 && box.boundingRect().height < 60 &&
                        box.boundingRect().width < 40) {
                        center = box.center;
                        Point2f P[4];
                        box.points(P);
                        for (int j = 0; j <= 3; j++) {
//                            line(res, P[j], P[(j + 1) % 4], Scalar(0), 1);
                        }
                    }
                }
            }
        }
        if (center.x == 0 || center.y == 0) {
            jstring jvc = env->NewStringUTF("0");
            jstring jvt = env->NewStringUTF("0");
            jstring jpro = env->NewStringUTF(ssqr.str().c_str());
            jstring jpath = env->NewStringUTF(sska.str().c_str());
            jobject jniobj = env->NewObject(stucls, constrocMID, jvc, jvt, jpro, jpath);
            env->CallBooleanMethod(list_obj, list_add, jniobj);
            continue;
        }
//        LOGD("test %d", 2);
        Point a(center.x - 40, center.y - 8);
        Point b(center.x + 120, center.y + 8);
        Rect roi(b, a);
        Mat dst(cres, roi);


        //测试平均灰度
        Mat greydst;
        Scalar meanScalar, devScalar;
        cvtColor(dst, greydst, COLOR_BGR2GRAY);
        meanStdDev(greydst, meanScalar, devScalar);
        double meanroatio = 200.0 - meanScalar[0];
        double devroatio = 10 / devScalar[0];
        dst.convertTo(dst, -1, 1, meanroatio);

        stringstream ss;
        ss << imagePath;
        ss << "/dst_";
        ss << i;
        ss << "_pic.jpg";
        string imagename = ss.str();
//        Vec3b smean = getMinScalar(dst);

        Mat hsvmat, linemat, blurmat;
        Mat matArr[3];
        vector<float> arr;
        cvtColor(dst, hsvmat, COLOR_BGR2HSV);
        split(hsvmat, matArr);
        GaussianBlur(matArr[1], blurmat, Size(35, 35), 0);
        resize(blurmat, linemat, Size(dst.cols, 1));
        for (int i = 0; i < linemat.cols; i++) {
            float data = (float) linemat.at<uchar>(0, i);
            arr.push_back(data);
        }
        float majority = majorityElement(arr);
        vector<float> beginarr, endarr;
        for (int i = 20; i < 45; i++) {
            beginarr.push_back(arr[i] - majority);
        }
        for (int i = 90; i < 125; i++) {
            endarr.push_back(arr[i] - majority);
        }

        sort(beginarr.begin(), beginarr.end());
        sort(endarr.begin(), endarr.end());

        double vc, vt;
        if (beginarr.size() == 0) {
            vc = 0;
        } else {
            vc = (beginarr[beginarr.size() - 3] + beginarr[beginarr.size() - 2] +
                  beginarr[beginarr.size() - 1]) / 3;
        }
        if (endarr.size() == 0) {
            vt = 0;
        } else {
            vt = (endarr[endarr.size() - 3] + endarr[endarr.size() - 2] +
                  endarr[endarr.size() - 1]) / 3;

//            double sumvt = std::accumulate(std::begin(endarr), std::end(endarr), 0.0);
//            vt = sumvt / endarr.size(); //均值
//            vt = majorityElement(endarr);
        }


        cout << "vc:" << vc << " ,vt:" << vt << endl;
        stringstream ss1;
        ss1 << "vc: " << vc << " vt: " << vt;
        LOGD("ss1 :: %s", ss1.str().c_str());

        stringstream ssvc, ssvt;
        ssvc << vc;
        ssvt << vt;
        jstring jvc = env->NewStringUTF(ssvc.str().c_str());
        jstring jvt = env->NewStringUTF(ssvt.str().c_str());
        jstring jpro = env->NewStringUTF(ssqr.str().c_str());
//        jstring jpath = env->NewStringUTF(imagename.c_str());
        jstring jpath = env->NewStringUTF(sska.str().c_str());
        jobject jniobj = env->NewObject(stucls, constrocMID, jvc, jvt, jpro, jpath);
        env->CallBooleanMethod(list_obj, list_add, jniobj);
    }
    env->ReleaseIntArrayElements(data_, data, 0);
    return list_obj;
}



extern "C"
JNIEXPORT jstring JNICALL
Java_com_zodo_camerawithdetection_activity_CameraActivity_detectionPCR(JNIEnv *env,
                                                                       jobject instance,
                                                                       jintArray data_, jint width,
                                                                       jint height) {
    jint *data = env->GetIntArrayElements(data_, NULL);
    if (data == NULL) {
        return nullptr;
    }
    Mat org(height, width, CV_8UC4, (unsigned char *) data);
    cvtColor(org, org, COLOR_BGRA2BGR);
    resize(org, org, Size(1200, 1400));
    resize(org, org, Size(org.cols / 2, org.rows / 2), 0);
    GaussianBlur(org, org, Size(9, 9), 0);
    Mat hsv;
    cvtColor(org, hsv, COLOR_BGR2HSV);
    Mat mats[3];
    split(hsv, mats);
    for (int i = 0; i < hsv.rows; i++) {
        for (int j = 0; j < hsv.cols; j++) {
//            cout << (double) hsv.at<Vec3b>(i, j)[1] << endl;
            if (hsv.at<Vec3b>(i, j)[1] > 40 &&
                (hsv.at<Vec3b>(i, j)[0] > 170 || hsv.at<Vec3b>(i, j)[0] < 25) &&
                hsv.at<Vec3b>(i, j)[2] > 150) {

            } else {
                org.at<Vec3b>(i, j)[0] = 255;
                org.at<Vec3b>(i, j)[1] = 255;
                org.at<Vec3b>(i, j)[2] = 255;
            }
        }
    }
    Mat gray;
    cvtColor(org, gray, COLOR_BGR2GRAY);
    Mat scalar;
//    threshold(gray,scalar,200,255,THRESH_BINARY);
    adaptiveThreshold(gray, scalar, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 101, 10);

    Mat element3 = getStructuringElement(MORPH_RECT, Size(5, 5));
    erode(scalar, scalar, element3);
    morphologyEx(scalar, scalar, MORPH_OPEN, element3);
    dilate(scalar, scalar, element3);
    morphologyEx(scalar, scalar, MORPH_CLOSE, element3);
    vector<vector<Point> > linecontours;
    findContours(scalar, linecontours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<RotatedRect> lineareas;
    stringstream ss;
    for (int j = 0; j < linecontours.size(); j++) {
        auto lincontour = linecontours[j];
        auto box = minAreaRect(lincontour);
        if (box.center.x < 450 && box.center.x > 150 && box.center.y < 700 && box.center.y > 100 &&
            box.boundingRect().area() < 2000 && box.boundingRect().area() > 100) {
            int value = (int) hsv.at<Vec3b>(box.center)[0];
            int v = hsv.at<Vec3b>(box.center)[0];
            LOGD("value %d", v);
            ss << value;
            ss << ",";
//            cout << "box.center::" << box.center << endl;
//            cout << "box.area::" << box.boundingRect().area() << endl;
        }
    }

    env->ReleaseIntArrayElements(data_, data, 0);

    return env->NewStringUTF(ss.str().c_str());
}

float dectionMat(Mat org) {
    Mat hsv;
    cvtColor(org, hsv, COLOR_BGR2HSV);
    Mat mats[3];
    split(hsv, mats);
    int result = 0;
    float r = 0.0f;
    vector<int> vector1;
    for (int i = 0; i < hsv.rows; i++) {
        for (int j = 0; j < hsv.cols; j++) {
//            cout << (double) hsv.at<Vec3b>(i, j)[1] << endl;
            if ((hsv.at<Vec3b>(i, j)[0] > 156 || hsv.at<Vec3b>(i, j)[0] < 10) &&
                hsv.at<Vec3b>(i, j)[1] > 15) {
//                int data=hsv.at<Vec3b>(i, j)[1];
//                vector1.push_back(data);
                result += hsv.at<Vec3b>(i, j)[1];
                r = result / 7500.0f;
            } else {
                org.at<Vec3b>(i, j)[0] = 255;
                org.at<Vec3b>(i, j)[1] = 255;
                org.at<Vec3b>(i, j)[2] = 255;
            }
        }
    }
//    if(vector1.empty()){
//        return 0;
//    }
//    int data = majorityElement(vector1);
//    imshow("res", org);
//    cout<<"result:: "<<r<<endl;
    return r;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_zodo_camerawithdetection_activity_CameraActivity_detectionLuoDanMinB(JNIEnv *env,
                                                                              jobject instance,
                                                                              jintArray data_,
                                                                              jint width,
                                                                              jint height,
                                                                              jstring imagePath_) {
    jint *data = env->GetIntArrayElements(data_, NULL);
    string imagePath = env->GetStringUTFChars(imagePath_, 0);
    if (data == NULL) {
        return nullptr;
    }
    Mat org(height, width, CV_8UC4, (unsigned char *) data);
    cvtColor(org, org, COLOR_BGRA2BGR);
    resize(org, org, Size(1200, 1400));
    resize(org, org, Size(org.cols / 2, org.rows / 2), 0);
    Mat gray;
    cvtColor(org, gray, COLOR_BGR2GRAY);
    normalize(gray, gray, 255, 0, NORM_MINMAX);
    Mat grayblur1, grayblur2;
    GaussianBlur(gray, grayblur1, Size(3, 3), 0);
    GaussianBlur(gray, grayblur2, Size(255, 255), 0);
    Mat mask = grayblur2 - grayblur1;
    normalize(mask, mask, 255, 0, NORM_MINMAX);
    vector<vector<Point> > linecontours;
    vector<Rect> rects;
    Rect rect1;
    findContours(mask, linecontours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    for (int j = 0; j < linecontours.size(); j++) {
        auto lincontour = linecontours[j];
        auto box = minAreaRect(lincontour);
        if (
                box.boundingRect().height < 90 && box.boundingRect().height > 50 &&
                box.boundingRect().width < 170 && box.boundingRect().width > 130
                && box.center.y < 180
                ) {
            rect1 = box.boundingRect();
//            rects.push_back(box.boundingRect());
//            cout<<box.size<<endl;
        }
    }

    Rect rect2(rect1.x, rect1.y + 92, 160, 80);
    Rect rect3(rect1.x, rect1.y + 320, 160, 80);
    Rect rect4(rect1.x, rect1.y + 415, 160, 80);
    rects.push_back(rect4);
    rects.push_back(rect3);
    rects.push_back(rect2);
    rects.push_back(rect1);

    Mat drawmat = org.clone();
    drawContours(drawmat, linecontours, -1, Scalar(0, 0, 255));

    stringstream ssimage;
    ssimage << imagePath;
    ssimage << "/luodanmin.jpg";
    stringstream ss;
    for (int i = 0; i < rects.size(); i++) {
        Mat cropRect;
        Rect rect = rects[i];
        getRectSubPix(org, rect.size(), Point(rect.x + rect.width / 2, rect.y + rect.height / 2),
                      cropRect);

        Mat gray;
        cvtColor(cropRect, gray, COLOR_BGR2GRAY);
        Mat grayblur1, grayblur2;
        GaussianBlur(gray, grayblur1, Size(3, 3), 0);
        GaussianBlur(gray, grayblur2, Size(255, 255), 0);
        Mat mask = grayblur2 - grayblur1;
        normalize(mask, mask, 255, 0, NORM_MINMAX);
        Mat nmask(mask, Rect(10, 10, mask.cols - 20, mask.rows - 20));
        Scalar mean, dev;
        meanStdDev(nmask, mean, dev);
//        cout<<"dev: "<<dev[0]<<endl;

        float value = dectionMat(cropRect);
        LOGD("luodanmin %f, dev %f", value, dev[0]);
        ss << value << ",";
//        if(dev[0]<20){
//            ss<<"-1";
//            ss<<",";
//        }else{
//            ss<<value;
//            ss<<",";
//        }
    }

    // TODO

    env->ReleaseIntArrayElements(data_, data, 0);

    return env->NewStringUTF(ss.str().c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_zodo_camerawithdetection_activity_CameraActivity_detectionNongcan(JNIEnv *env,
                                                                           jobject instance,
                                                                           jintArray data_,
                                                                           jint width,
                                                                           jint height) {
    jint *data = env->GetIntArrayElements(data_, NULL);
    if (data == NULL) {
        return nullptr;
    }
    Mat org(height, width, CV_8UC4, (unsigned char *) data);
    resize(org, org, Size(1200, 1400));
    GaussianBlur(org, org, Size(5, 5), 0);
    cvtColor(org, org, COLOR_BGRA2BGR);
    Mat grey, momat, outmat;
    cvtColor(org, grey, COLOR_BGR2GRAY);

    adaptiveThreshold(grey, outmat, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 185, 20);
    Mat element0 = getStructuringElement(MORPH_RECT, Size(2, 2));
    morphologyEx(outmat, outmat, MORPH_CLOSE, element0);
//    dilate(outmat, outmat, element0);
//    imshow("adaptivemat", outmat);
    Mat element = getStructuringElement(MORPH_RECT, Size(15, 9));
    erode(outmat, outmat, element);
    morphologyEx(outmat, outmat, MORPH_OPEN, element);
    dilate(outmat, outmat, element);
    morphologyEx(outmat, outmat, MORPH_CLOSE, element);
//    imshow("res", outmat);
    vector<vector<Point> > contours;
    findContours(outmat, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<RotatedRect> areas;
    for (auto contour:contours) {
        auto box = minAreaRect(contour);
        Rect rect = box.boundingRect();
        if (box.center.x > 220 && box.center.x < 480 && rect.width > 150 && rect.width < 300 &&
            rect.height > 150 &&
            rect.height < 300) {
            cout << box.boundingRect().width << " , " << box.boundingRect().height << endl;
            areas.push_back(box);
        }
    }
//    string imagePath = env->GetStringUTFChars(imagePath_, 0);
    stringstream ss;
    for (int i = 0; i < areas.size(); i++) {

        Mat qrmat, greyqrmat, kamat;
        Mat cres = deskewAndCrop3(org, areas[i], qrmat, kamat);
        blur(cres, cres, Size(3, 3));
//        bitwise_not(cres, cres);
        int x = cres.cols / 2;
        int y = cres.rows / 2;
        Rect dstrect(x - 60, y - 60, 120, 120);

        Mat roi2 = cres(dstrect);
        Mat greyroi, resImg;
        cvtColor(roi2, greyroi, COLOR_BGR2GRAY);
        Rect mdstrect(20, 20, 100, 100);
        Mat dstimg = roi2(mdstrect);

        Rect maskrect(0, 0, 20, 20);
        Mat maskimg = roi2(maskrect);
        resize(maskimg, maskimg, Size(100, 100));
        Mat aux = -dstimg + maskimg;

        Mat haux;
        cvtColor(aux, haux, COLOR_BGR2HSV);
        for (int k = 0; k < haux.rows; k++) {
            for (int j = 0; j < haux.cols; j++) {
                Vec3b data = haux.at<Vec3b>(k, j);
                if (data[2] < 20 || (data[1] < 43 && data[2] > 46)) {
                    aux.at<Vec3b>(k, j)[0] = 0;
                    aux.at<Vec3b>(k, j)[1] = 0;
                    aux.at<Vec3b>(k, j)[2] = 0;
                }
            }
        }

        int total = 0;
        for (int i = 0; i < aux.rows; i++) {
            for (int j = 0; j < aux.cols; j++) {
                uchar data = aux.at<uchar>(i, j);
                total += (int) data;
            }
        }
        float resultvalue = total / (100 * 100);
        if (resultvalue < 0) {
            resultvalue = 0;
        }

        ss << resultvalue << ',';

        Mat rectMat = cres(dstrect);
//        stringstream srcstr;
//        srcstr << imagePath;
//        srcstr << "/dst_";
//        srcstr << i;
//        srcstr << "_img.jpg";
//        imwrite(srcstr.str().c_str(), rectMat);

//        double total = 0;
//        for (int i = 0; i < rectMat.rows; i++) {
//            for (int j = 0; j < rectMat.cols; j++) {
//                int b = rectMat.at<Vec3b>(i, j)[0];
//                int g = rectMat.at<Vec3b>(i, j)[1];
//                int r = rectMat.at<Vec3b>(i, j)[2];
//                if (r > (b+g)/2) {
//                    int rbg = pow(r - b, 2) + pow(r - g, 2);
//                    double data = sqrt(rbg / 2);
//                    total += data;
//                } else {
//                    total += 0;
//                }
//            }
//        }
//        double tdata = total/3600;
//        ss << tdata << ',';
    }
    env->ReleaseIntArrayElements(data_, data, 0);
    return env->NewStringUTF(ss.str().c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_zodo_camerawithdetection_activity_CameraActivity_detectionJGDNongcan(JNIEnv *env,
                                                                              jobject instance,
                                                                              jintArray data_,
                                                                              jint width,
                                                                              jint height,
                                                                              jstring imagePath_) {

    jint *data = env->GetIntArrayElements(data_, NULL);
    if (data == NULL) {
        return nullptr;
    }
    Mat src(height, width, CV_8UC4, (unsigned char *) data);
    cvtColor(src, src, COLOR_BGRA2BGR);
    Mat grey, binImg;
    cvtColor(src, grey, COLOR_BGR2GRAY);
    threshold(grey, binImg, 10, 255, THRESH_OTSU);
    vector<vector<Point>> squares;
    Rect roiRect;

    int kvalue = 15;
//    normalize(grey, grey, 255, 0, NORM_MINMAX);
    //对图像进行滤波过滤并使用边缘算法呈现图像轮廓
    blur(grey, grey, Size(3, 3));
    Canny(grey, grey, 30, 70, 3, true);
//    Laplacian(grey, grey, CV_8U, 5, 1, 0, BORDER_DEFAULT);
    blur(grey, grey, Size(3, 3));

    findContours(grey, squares, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Rect> rects;

    vector<Vec3f> circles;
    Mat srcImage;
    //获取图像中的圆形
    HoughCircles(grey, circles, HOUGH_GRADIENT, 1, 5, 80, 35, 60, 100);
    int radius = 50;

    string imagePath = env->GetStringUTFChars(imagePath_, 0);
    stringstream ssgrey;
    ssgrey << imagePath;
    ssgrey << "/grey.jpg";
    imwrite(ssgrey.str().c_str(), grey);

//    for (auto contour:squares) {
//        auto box = minAreaRect(contour);
//        Rect rect = box.boundingRect();
//        if (rect.x>300&&rect.x<400&&rect.width>110&&rect.width<150&&rect.height>110&&rect.height<150){
//            radius=60;
//            roiRect=rect;
//            rects.push_back(rect);
//        }
//    }
    //对获取到的圆形列表进行筛选
    for (int i = 0; i < circles.size(); i++) {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

        int rads = cvRound(circles[i][2]);
        if (center.x > 300 && rads > 65 && rads < 70) {
            radius = cvRound(circles[i][2]);
//            srcImage=Mat(Size(radius*2,radius*2),CV_8UC3);
            Rect rect(Point(center.x - radius, center.y - radius),
                      Point(center.x + radius, center.y + radius));
            roiRect = rect;
            rects.push_back(rect);
        }

        //绘制圆轮廓
//        int rad=cvRound(circles[i][2]);
//        srcImage=Mat(Size(rad*2,rad*2),CV_8UC3);
////        circle(srcImage, center, rad, Scalar(255),-1);
//        circle(srcImage, center, rad, Scalar(255, 50, 0), 3, 8, 0);
    }
    if (roiRect.empty()) {
        return env->NewStringUTF("none");
    }
    cout << roiRect << endl;
    Mat roi2 = src(roiRect);

//    Mat copymat=roi2;
//    copymat.copyTo(Mat(Size(radius*2,radius*2),CV_8UC3),srcImage);
//    stringstream srcimage;
//    srcimage << imagePath;
//    srcimage << "/luo.jpg";
//    imwrite(srcimage.str().c_str(), srcImage);
//    blur(roi2, roi2, Size(15, 15));

    stringstream ssimage;
    ssimage << imagePath;
    ssimage << "/luodanmin.jpg";
    imwrite(ssimage.str().c_str(), roi2);

    //取底色区域与样本区域进行色差对比
    Mat greyroi, resImg;
    cvtColor(roi2, greyroi, COLOR_BGR2GRAY);
    int rad = 100;
    Rect dstrect(20, 20, rad, rad);
    Mat dstimg = roi2(dstrect);

    Rect maskrect(0, 0, 20, 20);
    Mat maskimg = roi2(maskrect);
    resize(maskimg, maskimg, Size(rad, rad));
    Mat aux = -dstimg + maskimg;

    Mat haux;
    cvtColor(aux, haux, COLOR_BGR2HSV);
    for (int k = 0; k < haux.rows; k++) {
        for (int j = 0; j < haux.cols; j++) {
            Vec3b data = haux.at<Vec3b>(k, j);
            if (data[2] < 35 || (data[1] < 43 && data[2] > 46)) {
                aux.at<Vec3b>(k, j)[0] = 0;
                aux.at<Vec3b>(k, j)[1] = 0;
                aux.at<Vec3b>(k, j)[2] = 0;
            }
        }
    }

    int total = 0;
    for (int i = 0; i < aux.rows; i++) {
        for (int j = 0; j < aux.cols; j++) {
            uchar data = aux.at<uchar>(i, j);
            total += (int) data;
        }
    }
    float resultvalue = total / (100 * 100);
    if (resultvalue < 0) {
        resultvalue = 0;
    }

    stringstream ss;
    ss << resultvalue;
    env->ReleaseIntArrayElements(data_, data, 0);
    return env->NewStringUTF(ss.str().c_str());
}