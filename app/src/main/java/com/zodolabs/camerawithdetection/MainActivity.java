package com.zodolabs.camerawithdetection;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.tbruyelle.rxpermissions2.RxPermissions;
import com.zodo.camerawithdetection.CameraDetection;
import com.zodo.camerawithdetection.bean.DetectionProject;
import com.zodo.camerawithdetection.bean.QueryResBean;
import com.zodo.camerawithdetection.callback.OnResultCallback;
import com.zodo.camerawithdetection.common.DetectionType;

import java.math.RoundingMode;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.concurrent.TimeUnit;

import io.reactivex.Observable;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.functions.Consumer;
import io.reactivex.functions.Function;
import io.reactivex.schedulers.Schedulers;

public class MainActivity extends AppCompatActivity {

    private Button btn_project,btn_project_list,btn_stop,btn_nc;
    private TextView tv_rate;

    private Disposable disposable;
    private boolean isEnd=false;

    private double total=0;
    private double success=0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        RxPermissions rxPermissions = new RxPermissions(this);
        rxPermissions.request(Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.CAMERA)
                .subscribe(new Consumer<Boolean>() {
                    @Override
                    public void accept(Boolean aBoolean) throws Exception {

                    }
                });

        btn_project=findViewById(R.id.btn_project);
        btn_project_list=findViewById(R.id.btn_project_list);
        btn_stop=findViewById(R.id.btn_stop);
        btn_nc=findViewById(R.id.btn_nc);
        tv_rate=findViewById(R.id.tv_rate);

        btn_stop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isEnd=true;
            }
        });

        //农残单通道
        btn_nc.setOnClickListener(new View.OnClickListener() {
            @SuppressLint("CheckResult")
            @Override
            public void onClick(View v) {
                total=0;
                success=0;
                isEnd=false;
                startSingleNCChannel();
            }
        });

        //单通道
        btn_project.setOnClickListener(new View.OnClickListener() {
            @SuppressLint("CheckResult")
            @Override
            public void onClick(View v) {
                total=0;
                success=0;
                isEnd=false;
                startSingleChannel();
            }
        });

        //多通道
        btn_project_list.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startMutilChannel();
            }
        });
    }

    private void startMutilChannel(){
        total++;
        DetectionProject project=new DetectionProject();
        project.setId(57L);
        project.setProjectmethod("比色");
        project.setProjectname("孔雀石绿(比色)");//盐酸克伦特罗(消线)    孔雀石绿(比色)
        project.setYuzhi(0.8);

        DetectionProject project1=new DetectionProject();
        project1.setId(6L);
        project1.setProjectmethod("比色");
        project1.setProjectname("孔雀石绿(比色)");//盐酸克伦特罗(消线)    孔雀石绿(比色)
        project1.setYuzhi(0.8);
        ArrayList<DetectionProject> projects=new ArrayList<>();
        projects.add(project);
        projects.add(project1);

        ArrayList<QueryResBean> listData=new ArrayList<>();
        for (int i = 0; i < 2; i++) {
            QueryResBean queryResBean = new QueryResBean(null, (i+1)+"", "", "", "",
                    project.getProjectname(), "", "", "", "", "0", "", "", "", "", "");
            listData.add(queryResBean);
        }

        String gson=new Gson().toJson(listData);

        String projectStr=new Gson().toJson(projects);

        CameraDetection.builder(MainActivity.this)
                .setMultiChannel(true)
                .setMinThresheold(35)
                .setMaxThresheold(85)
                .setDetectionType(DetectionType.JIAO_JIN_TI)
                .setDetectionProjectList(projectStr)
                .setMultiList(gson)
                .build(new OnResultCallback() {
                    @Override
                    public void resultWithProject(String path, String checkValue, String checkResult) {

                    }

                    @Override
                    public void resultWithProjectLists(String path, String queryReBeans) {
                        Log.e("path",path);
                        Log.e("queryReBeans：",queryReBeans);
                        if (!TextUtils.isEmpty(queryReBeans)) {
                            ArrayList<QueryResBean> queryResBeans = new Gson().fromJson(queryReBeans, new TypeToken<ArrayList<QueryResBean>>() {
                            }.getType());
                            if (queryResBeans != null &&queryResBeans.size()==2&&
                                    queryResBeans.get(0).getResult().equals("阴性")&&
                                    queryResBeans.get(1).getResult().equals("疑似阳性")) {
                                success++;
                            }
                        }

                        DecimalFormat decimalFormat = (DecimalFormat) NumberFormat.getInstance();
                        decimalFormat.setMaximumFractionDigits(1);
                        decimalFormat.setRoundingMode(RoundingMode.HALF_UP);
                        double rate=success/total*100;
                        tv_rate.setText("识别次数：" + (int) success + "/" + (int) total + "    " + "识别成功率：" + decimalFormat.format(rate) + "%");
                        if (!isEnd&&total<200){
                            new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
                                @Override
                                public void run() {
                                    if (!isEnd) {
                                        startMutilChannel();
                                    }
                                }
                            },2500);
                        }
                    }
                });
    }

    private void startSingleChannel(){
        total++;
        DetectionProject project=new DetectionProject();
        project.setId(1L);
        project.setProjectmethod("比色");
        project.setProjectname("孔雀石绿(比色)");////盐酸克伦特罗(消线)    孔雀石绿(比色)
        project.setYuzhi(0.8);
//        project.setId(1L);
//        project.setProjectmethod("消线");
//        project.setProjectname("盐酸克伦特罗(消线)");////盐酸克伦特罗(消线)    孔雀石绿(比色)
//        project.setYuzhi(35);
        String gsonProject=new Gson().toJson(project);
        CameraDetection.builder(MainActivity.this)
                .setMultiChannel(false)
                .setDetectionProject(gsonProject)
                .build(new OnResultCallback() {
                    @SuppressLint("SetTextI18n")
                    @Override
                    public void resultWithProject(String path, String checkValue, String checkResult) {
                        Log.e("checkValue：",checkValue);
                        Log.e("checkResult：",checkResult);
                        if (!checkResult.equals("阴性")){
                            success++;
                        }else {
                            isEnd=true;
                        }

//                        if (!TextUtils.isEmpty(checkValue)&&!checkValue.equals("0")&&!checkValue.equals("0.0")
//                                &&Double.parseDouble(checkValue.substring(0,checkValue.indexOf("(")))>0.6){
//                            success++;
//                        }else {
////                            isEnd=true;
//                        }

                        DecimalFormat decimalFormat = (DecimalFormat) NumberFormat.getInstance();
                        decimalFormat.setMaximumFractionDigits(1);
                        decimalFormat.setRoundingMode(RoundingMode.HALF_UP);
                        double rate=success/total*100;
                        tv_rate.setText("识别次数：" + (int) success + "/" + (int) total + "    " + "识别成功率：" + decimalFormat.format(rate) + "%");
                        if (!isEnd&&total<200){
                            new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
                                @Override
                                public void run() {
                                    if (!isEnd) {
                                        startSingleChannel();
                                    }
                                }
                            },2500);
                        }
                    }

                    @Override
                    public void resultWithProjectLists(String path, String queryReBeans) {

                    }
                });
    }

    private void startSingleNCChannel(){
        total++;
        DetectionProject project=new DetectionProject();
        project.setId(24L);
        project.setProjectmethod("消线");
        project.setProjectname("农药残留");////盐酸克伦特罗(消线)    孔雀石绿(比色)
        project.setYuzhi(5);
//        project.setId(1L);
//        project.setProjectmethod("消线");
//        project.setProjectname("盐酸克伦特罗(消线)");////盐酸克伦特罗(消线)    孔雀石绿(比色)
//        project.setYuzhi(35);
        String gsonProject=new Gson().toJson(project);
        CameraDetection.builder(MainActivity.this)
                .setMultiChannel(false)
                .setMinThresheold(25)
                .setMaxThresheold(55)
                .setIsJianGuanYi(true)
                .setDetectionProject(gsonProject)
                .build(new OnResultCallback() {
                    @SuppressLint("SetTextI18n")
                    @Override
                    public void resultWithProject(String path, String checkValue, String checkResult) {
                        Log.e("checkValue：",checkValue);
                        Log.e("checkResult：",checkResult);
                        if (checkResult.equals("不合格")){
                            success++;
                        }else {
//                            isEnd=true;
                        }

//                        if (!TextUtils.isEmpty(checkValue)&&!checkValue.equals("0")&&!checkValue.equals("0.0")
//                                &&Double.parseDouble(checkValue.substring(0,checkValue.indexOf("(")))>0.6){
//                            success++;
//                        }else {
////                            isEnd=true;
//                        }

                        DecimalFormat decimalFormat = (DecimalFormat) NumberFormat.getInstance();
                        decimalFormat.setMaximumFractionDigits(1);
                        decimalFormat.setRoundingMode(RoundingMode.HALF_UP);
                        double rate=success/total*100;
                        tv_rate.setText("识别次数：" + (int) success + "/" + (int) total + "    " + "识别成功率：" + decimalFormat.format(rate) + "%");
                        if (!isEnd&&total<200){
                            new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
                                @Override
                                public void run() {
                                    if (!isEnd) {
                                        startSingleNCChannel();
                                    }
                                }
                            },2500);
                        }
                    }

                    @Override
                    public void resultWithProjectLists(String path, String queryReBeans) {

                    }
                });
    }
}