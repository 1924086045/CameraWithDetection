package com.zodo.camerawithdetection.activity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.king.zxing.util.CodeUtils;
import com.xuexiang.xqrcode.XQRCode;
import com.zodo.camerawithdetection.R;
import com.zodo.camerawithdetection.bean.DetectionProject;
import com.zodo.camerawithdetection.bean.JniBeans;
import com.zodo.camerawithdetection.bean.QueryResBean;
import com.zodo.camerawithdetection.callback.CameraCallBack;
import com.zodo.camerawithdetection.common.CommonString;
import com.zodo.camerawithdetection.common.DetectionType;
import com.zodo.camerawithdetection.fragment.Camera2BasicFragment;
import com.zodo.camerawithdetection.utils.CommonUtils;
import com.zodo.camerawithdetection.utils.ImageUtils;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import cn.bertsir.zbar.utils.QRUtils;

public class CameraActivity extends AppCompatActivity implements CameraCallBack {

    static {
        System.loadLibrary("native-lib");
    }

    public native ArrayList<JniBeans> detection(int[] data, int width, int height, String imagePath,boolean isJGD,int minThresheold,int maxThresheold);

    public native String detectionNongcan(int[] data, int width, int height);

    public native String detectionJGDNongcan(int[] data, int width, int height, String imagePath);

    public native String detectionPCR(int[] data, int width, int height);

    public native String detectionLuoDanMinB(int[] data, int width, int height, String imagepath);

    private int cameraId;//摄像头id

    private int detectionType;//检测方法

    private boolean isMultiChannel = false;//是否为多通道

    private DetectionProject mProject;//单通道检测项目

    private ArrayList<DetectionProject> detectionProjects;//检测项目列表

    private ArrayList<QueryResBean> listData;//多通道列表

    private Date date;
    private SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.CHINA);

    private String imagepath="";//图片路径

    private boolean isJianGuanYi=false;//是否为监管仪

    private int minThresheold=35;  //图像检索最小阈值
    private int maxThresheold=85;  //图像检索最大阈值

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        cameraId = getIntent().getIntExtra(CommonString.CAMERA_ID, 0);
        detectionType = getIntent().getExtras().getInt(CommonString.DETECTION_TYPE, DetectionType.JIAO_JIN_TI);
        isMultiChannel = getIntent().getBooleanExtra(CommonString.IS_MULTI_CHANNEL, false);
        mProject = (DetectionProject) getIntent().getExtras().getSerializable(CommonString.DETECTION_PROJECT);
        detectionProjects = (ArrayList<DetectionProject>) getIntent().getExtras().getSerializable(CommonString.DETECTION_PROJECT_LISTS);
        listData = (ArrayList<QueryResBean>) getIntent().getExtras().getSerializable(CommonString.MULTI_CHANNEL_LISTS);
        minThresheold=getIntent().getIntExtra("minThresheold",35);
        maxThresheold=getIntent().getIntExtra("maxThresheold",85);

        String dirpath = String.valueOf(new Date().getTime());
        imagepath = CommonUtils.getImgpath() + File.separator + dirpath;
        File file = new File(imagepath);
        if (!file.exists()) {
            file.mkdirs();
        }

        if (!isMultiChannel && listData == null) {
            //如果为单通道默认生成一个实体
            listData = new ArrayList<>();
            QueryResBean queryResBean = new QueryResBean(null, "1", "", "", "",
                    mProject.getProjectname(), "", "", "", "", "0", "", "", "", "", "");
            listData.add(queryResBean);
        }
        date = new Date();

        if (null == savedInstanceState) {
            getSupportFragmentManager().beginTransaction()
                    .replace(R.id.container, Camera2BasicFragment.newInstance(this, cameraId))
                    .commit();
        }
    }

    /**
     * 回传摄像头拍摄的照片
     */
    private void getImagePath(byte[] bytes) {
        String path = "";
        Bitmap picBitmap = null;
        if (bytes.length >= 1000) {
            picBitmap = ImageUtils.getBitmap(bytes, 0);
            path = imagepath + "/gg.jpg";
            ImageUtils.save(picBitmap, path, Bitmap.CompressFormat.JPEG);
        }
        detectionImageWithType(picBitmap, imagepath, path);
    }

    /**
     * 根据检测类型调用检测方法
     */
    private void detectionImageWithType(Bitmap picBitmap, String imagepath, String path) {
        switch (detectionType) {
            case DetectionType.NONG_YAO_CAN_LIU:
                //农残
                getDetectionNCResult(picBitmap, path);
                break;
            case DetectionType.PCR:
                //PCR
                getDetectionPCRResult(picBitmap, path);
                break;
            case DetectionType.LUO_DAN_MING_B:
                //罗丹明B
                getDetectionLDMBResult(picBitmap, imagepath, path);
                break;
            default:
                //默认为胶金体
                if (isJianGuanYi){
                    if (isMultiChannel){

                    }else {
                        if (mProject.getId()==24||mProject.getId()==51){
                            //监管端农药残留检测
                            getDetectionJGDNcResult(picBitmap,imagepath, path);
                        }else {
                            getDetectionResult(picBitmap, imagepath, path);
                        }
                    }
                }else {
                    getDetectionResult(picBitmap, imagepath, path);
                }
                break;

        }
    }


    @Override
    public void takePhoto(byte[] bytes) {
        getImagePath(bytes);
    }

    /**
     * 胶金体检测
     */
    @SuppressLint("SetTextI18n")
    private void getDetectionResult(Bitmap picBitmap, String imagepath, String path) {
        if (picBitmap == null || TextUtils.isEmpty(imagepath)) {
            setErrorListResult(path);
        } else {
            int width = picBitmap.getWidth();
            int height = picBitmap.getHeight();
            Log.d("MainActivity", "width: " + width + " ,height: " + height);
            int[] pixs = new int[width * height];
            picBitmap.getPixels(pixs, 0, width, 0, 0, width, height);
            ArrayList<JniBeans> list = null;
            try {
                list = detection(pixs, width, height, imagepath,true,minThresheold,maxThresheold);
                if (list != null && list.size() > 0) {
                    Collections.reverse(list);
                    boolean isAllRight = true;
                    for (int i = 0; i < list.size(); i++) {
                        JniBeans bean = list.get(i);
                        QueryResBean resData = listData.get(i);
                        resData.setImgpath(bean.getPath());
                        String proRes="";
                        if (!isJianGuanYi) {
                            Uri imgUri = new Uri.Builder().path(bean.getPro()).build();
                            proRes = CodeUtils.parseCode(imgUri.getPath());
                            if (TextUtils.isEmpty(proRes)) {
                                proRes = XQRCode.analyzeQRCode(imgUri.getPath());
                            }
                            if (TextUtils.isEmpty(proRes)) {
                                try {
                                    proRes = QRUtils.getInstance().decodeQRcode(imgUri.getPath());
                                } catch (Exception e) {
                                    proRes = "";
                                }
                            }
                        }
                        try {
                            Log.d("detectioncardactivity", "::  " + proRes);
                            DetectionProject detectionProject = null;
                            detectionProject = getDetectionProject(proRes, resData.getProject());

                            resData.setProject(detectionProject.getProjectname());
//                                resData.setProjectmethod(detectionProject.getProjectmethod());
                            if ("消线".equals(detectionProject.getProjectmethod())) {
                                if ("0".equals(bean.getVc())) {
                                    resData.setValue("0");
                                    resData.setResult("无效卡");
                                } else {
                                    float fvt = Float.valueOf(bean.getVt());
                                    resData.setValue(fvt + "");
                                    resData.setResult(fvt > detectionProject.getYuzhi() ? getString(R.string.hege) : getString(R.string.buhege));
                                }
                            } else {
                                if ("0".equals(bean.getVc())) {
                                    resData.setValue("0");
                                    resData.setResult("无效卡");
                                } else {
                                    float fvc = Float.valueOf(bean.getVc());
                                    float fvt = Float.valueOf(bean.getVt());
                                    float bizhi = fvt / fvc;
                                    float ratio = Math.round(bizhi * 1000f) / 1000f;
                                    Log.d("detection", "ration: " + ratio);
                                    resData.setValue(String.valueOf(ratio) + "(" + fvt + "/" + fvc + ")");
                                    resData.setResult(ratio > detectionProject.getYuzhi() ? getString(R.string.hege) : getString(R.string.buhege));
                                }
                            }
                        } catch (NumberFormatException e) {
                            e.printStackTrace();
                            setErrorListResult(path);
                        }
                        resData.setDate(format.format(date));
                    }

                    setIntentValueResult(path, listData);
                } else {
                    setErrorListResult(path);
                }
            } catch (Exception e) {
                e.printStackTrace();
                setErrorListResult(path);
            }
        }
    }

    /**
     * 农残检测
     */
    private void getDetectionNCResult(Bitmap picBitmap, String path) {
        try {
            if (picBitmap == null || TextUtils.isEmpty(path)) {
                setErrorListResult(path);
            } else {
                int width = picBitmap.getWidth();
                int height = picBitmap.getHeight();
                Log.d("MainActivity", "width: " + width + " ,height: " + height);
                int[] pixs = new int[width * height];
                picBitmap.getPixels(pixs, 0, width, 0, 0, width, height);
                String data = detectionNongcan(pixs, width, height);
                String[] dataArr = data.split(",");
                List<String> list = Arrays.asList(dataArr);
                List<String> stringList = new ArrayList<>(list);
                Collections.reverse(stringList);
                String proRes = "24";
                for (int i = 0; i < stringList.size(); i++) {
                    QueryResBean resData = listData.get(i);
                    DetectionProject detectionProject = getDetectionProject(proRes, resData.getProject());
                    double yuzhi = detectionProject.getYuzhi();
                    if ("".equals(resData.getYpmc())) {
                        continue;
                    }
                    resData.setValue(stringList.get(i));
                    resData.setDate(format.format(date));
                    double value = Double.valueOf(stringList.get(i));
                    if (value > yuzhi) {
                        resData.setResult("合格");
                    } else {
                        resData.setResult("不合格");
                    }
                    resData.setProject("农药残留");
                }

                setIntentValueResult(path, listData);
            }
        }catch (Exception e){
            e.printStackTrace();
            setErrorListResult(path);
        }
    }

    /**
     * PCR检测
     */
    private void getDetectionPCRResult(Bitmap picBitmap, String path) {
        try {
            if (picBitmap == null || TextUtils.isEmpty(path)) {
                setErrorListResult(path);
            } else {
                int width = picBitmap.getWidth();
                int height = picBitmap.getHeight();
                int[] pixs = new int[width * height];
                picBitmap.getPixels(pixs, 0, width, 0, 0, width, height);
                String data = detectionPCR(pixs, width, height);
                String[] dataArr = data.split(",");
                List<String> list = Arrays.asList(dataArr);
                List<String> stringList = new ArrayList<>(list);
//        stringList.remove(stringList.size()-1);
                Collections.reverse(stringList);
                for (int i = 0; i < stringList.size(); i++) {
                    QueryResBean resData = listData.get(i);
                    resData.setValue(stringList.get(i));
                    int value = Integer.valueOf(stringList.get(i));
                    if (value < 34 && value > 10) {
                        resData.setResult("合格");
                    } else {
                        resData.setResult("不合格");
                    }
                    resData.setProject(resData.getProject());
                    resData.setDate(format.format(new Date()));
                }
                setIntentValueResult(path, listData);
            }
        }catch (Exception e){
            e.printStackTrace();
            setErrorListResult(path);
        }
    }

    /**
     * 罗丹明B检测
     */
    private void getDetectionLDMBResult(Bitmap picBitmap, String imagepath, String path) {
        try {
            if (picBitmap == null || TextUtils.isEmpty(path)) {
                setErrorListResult(path);
            } else {
                int width = picBitmap.getWidth();
                int height = picBitmap.getHeight();
                int[] pixs = new int[width * height];
                picBitmap.getPixels(pixs, 0, width, 0, 0, width, height);
                String data = detectionLuoDanMinB(pixs, width, height, imagepath);
                String[] dataArr = data.split(",");
                List<String> list = Arrays.asList(dataArr);
                List<String> stringList = new ArrayList<>(list);
                Collections.reverse(stringList);
                if (stringList.size() > 4) {
                    Toast.makeText(this, "请检查试剂是否安放正确!", Toast.LENGTH_SHORT).show();
                    return;
                }
                for (int i = 0; i < stringList.size(); i++) {
                    QueryResBean resData = listData.get(i);
                    if (resData.getYpmc().equals("")) {
                        continue;
                    }
                    resData.setValue(stringList.get(i));
                    float value = Float.valueOf(stringList.get(i));
                    if (value < 5) {
                        resData.setResult("合格");
                        resData.setProject(resData.getProject());
                    } else {
                        resData.setResult("不合格");
                        resData.setProject(resData.getProject());
                    }
                    resData.setDate(format.format(new Date()));
                }
                setIntentValueResult(path, listData);
            }
        }catch (Exception e){
            e.printStackTrace();
            setErrorListResult(path);
        }
    }

    /**
     * 监管端-农药残留检测
     */
    private void getDetectionJGDNcResult(Bitmap picBitmap,String imagepath, String path){
        try {
            if (picBitmap == null || TextUtils.isEmpty(path)) {
                setErrorListResult(path);
            } else {
                int width = picBitmap.getWidth();
                int height = picBitmap.getHeight();
                Log.d("MainActivity", "width: " + width + " ,height: " + height);
                int[] pixs = new int[width * height];
                picBitmap.getPixels(pixs, 0, width, 0, 0, width, height);
                String result = detectionJGDNongcan(pixs, width, height,imagepath);
                if ("none".equals(result)) {
                    setErrorListResult(path);
                    return;
                }
                double vt = Double.parseDouble(result);
                listData.get(0).setDate(format.format(new Date()));
                listData.get(0).setValue(String.format("%.2f", vt));
                if (mProject.getYuzhi() > vt) {
                    listData.get(0).setResult("不合格");
                } else {
                    listData.get(0).setResult("合格");
                }
                setIntentValueResult(path, listData);
            }
        }catch (Exception e){
            e.printStackTrace();
            setErrorListResult(path);
        }
    }

    /**
     * 回传原始数据
     */
    private void setIntentJniResult(String path, ArrayList<JniBeans> jniBeans) {
        Intent intent = new Intent();
        intent.putExtra(CommonString.CAMERA_PHOTO, path);
        intent.putExtra(CommonString.JNIBEAN_LIST, jniBeans);
        setResult(RESULT_OK, intent);
        finish();
    }

    /**
     * 回传检测结果数据
     */
    private void setIntentValueResult(String path, ArrayList<QueryResBean> rowsDTOS) {
        Intent intent = new Intent();
        intent.putExtra(CommonString.CAMERA_PHOTO, path);
        intent.putExtra(CommonString.RESULT_LISTS, rowsDTOS);
        setResult(RESULT_OK, intent);
        finish();
    }

    private void setErrorListResult(String path) {
        setIntentValueResult(path, new ArrayList<>());
    }

    /**
     * 匹配检测项目
     */
    private DetectionProject getDetectionProject(String projectId, String projectName) {
        DetectionProject project = null;
        if (isMultiChannel) {
            for (DetectionProject detectionProject : detectionProjects) {
                if (TextUtils.isEmpty(projectId)) {
                    if (projectName.equals(detectionProject.getProjectname() + "")) {
                        project = detectionProject;
                        break;
                    }
                } else {
                    if (projectId.equals(detectionProject.getId() + "")) {
                        project = detectionProject;
                        break;
                    }
                }
            }
        } else {
            project = mProject;
        }

        return project;
    }

}
