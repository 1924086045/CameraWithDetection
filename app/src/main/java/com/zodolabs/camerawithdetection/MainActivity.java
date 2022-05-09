package com.zodolabs.camerawithdetection;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.google.gson.Gson;
import com.zodo.camerawithdetection.CameraDetection;
import com.zodo.camerawithdetection.bean.DetectionProject;
import com.zodo.camerawithdetection.bean.QueryResBean;
import com.zodo.camerawithdetection.callback.OnResultCallback;
import com.zodo.camerawithdetection.common.DetectionType;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    private Button btn_project,btn_project_list;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btn_project=findViewById(R.id.btn_project);
        btn_project_list=findViewById(R.id.btn_project_list);

        //单通道
        btn_project.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                DetectionProject project=new DetectionProject();
                project.setId(1L);
                project.setProjectmethod("比色");
                project.setProjectname("孔雀石绿(比色)");
                project.setYuzhi(0.8);
                CameraDetection.builder(MainActivity.this)
                        .setMultiChannel(false)
                        .setDetectionProject(project)
                        .build(new OnResultCallback() {
                            @Override
                            public void resultWithProject(String path, String checkValue, String checkResult) {
                                Log.e("path",path);
                            }

                            @Override
                            public void resultWithProjectLists(String path, String queryReBeans) {

                            }
                        });
            }
        });

        //多通道
        btn_project_list.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                DetectionProject project=new DetectionProject();
                project.setId(1L);
                project.setProjectmethod("比色");
                project.setProjectname("孔雀石绿(比色)");
                project.setYuzhi(0.8);
                ArrayList<DetectionProject> projects=new ArrayList<>();
                projects.add(project);

                ArrayList<QueryResBean> listData=new ArrayList<>();
                for (int i = 0; i < 4; i++) {
                    QueryResBean queryResBean = new QueryResBean(null, "1", "", "", "",
                            project.getProjectname(), "", "", "", "", "0", "", "", "", "", "");
                    listData.add(queryResBean);
                }

                String gson=new Gson().toJson(listData);

                String projectStr=new Gson().toJson(projects);

                CameraDetection.builder(MainActivity.this)
                        .setMultiChannel(true)
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
                            }
                        });
            }
        });
    }
}