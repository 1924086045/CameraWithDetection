package com.zodo.camerawithdetection;

import android.app.Activity;
import android.content.Intent;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.zodo.camerawithdetection.activity.CameraActivity;
import com.zodo.camerawithdetection.bean.DetectionProject;
import com.zodo.camerawithdetection.bean.QueryResBean;
import com.zodo.camerawithdetection.callback.OnResultCallback;
import com.zodo.camerawithdetection.common.CommonString;
import com.zodo.camerawithdetection.utils.OnResultRequest;

import java.io.Serializable;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

/**
 * 拍照检测
 */
public class CameraDetection {

    private Gson gson;

    private WeakReference<Activity> activity;

    private int cameraID;//摄像头id

    private int detectionType;//检测方法

    private boolean isMultiChannel = false;//是否为多通道

    private DetectionProject detectionProject;//单通道检测项目

    private ArrayList<DetectionProject> detectionProjects;//检测项目列表

    private ArrayList<QueryResBean> listData;//多通道列表

    public static Builder builder(Activity activity) {
        return new Builder(activity);
    }

    public static class Builder {

        private CameraDetection detection = new CameraDetection();

        private Builder(Activity activity) {
            detection.gson=new Gson();
            detection.activity = new WeakReference<>(activity);
        }

        /**
         * 设置摄像头
         */
        public Builder setCameraID(int cameraId) {
            detection.cameraID = cameraId;
            return this;
        }

        /**
         * 设置检测类型
         */
        public Builder setDetectionType(int type) {
            detection.detectionType = type;
            return this;
        }

        /**
         * 是否为多通道
         */
        public Builder setMultiChannel(boolean multiChannel) {
            detection.isMultiChannel = multiChannel;
            return this;
        }

        /**
         * 单通道检测项目
         */
        public Builder setDetectionProject(String project) {
            detection.detectionProject = detection.gson.fromJson(project,DetectionProject.class);
            return this;
        }

        /**
         * 检测项目列表
         */
        public Builder setDetectionProjectList(String projects) {
            ArrayList<DetectionProject> detectionProjects=detection.gson.fromJson(projects,new TypeToken<ArrayList<DetectionProject>>(){}.getType());
            detection.detectionProjects = new ArrayList<>(detectionProjects);
            return this;
        }

        /**
         * 多通道列表
         */
        public Builder setMultiList(String list) {
            ArrayList<QueryResBean> queryResBeans=detection.gson.fromJson(list,new TypeToken<ArrayList<QueryResBean>>(){}.getType());
            detection.listData = new ArrayList<>(queryResBeans);
            return this;
        }

        public CameraDetection build(OnResultCallback resultCallback) {
            Intent intent = new Intent(detection.activity.get(), CameraActivity.class);
            intent.putExtra(CommonString.CAMERA_ID, detection.cameraID);
            intent.putExtra(CommonString.DETECTION_TYPE, detection.detectionType);
            intent.putExtra(CommonString.IS_MULTI_CHANNEL, detection.isMultiChannel);
            intent.putExtra(CommonString.DETECTION_PROJECT, (Serializable) detection.detectionProject);
            intent.putExtra(CommonString.DETECTION_PROJECT_LISTS, (Serializable) detection.detectionProjects);
            intent.putExtra(CommonString.MULTI_CHANNEL_LISTS, (Serializable) detection.listData);

            OnResultRequest resultRequest = new OnResultRequest(detection.activity.get());
            resultRequest.startForResult(intent, new OnResultRequest.Callback() {
                @Override
                public void onActivityResult(int resultCode, Intent data) {
                    if (resultCode == Activity.RESULT_OK) {
                        String path = data.getExtras().getString(CommonString.CAMERA_PHOTO, "");
                        //进行数据比对后直接返回检测结果
                        ArrayList<QueryResBean> queryReBeans = (ArrayList<QueryResBean>) data.getExtras().getSerializable(CommonString.RESULT_LISTS);
                        if (detection.isMultiChannel) {
                            String multiList=new Gson().toJson(queryReBeans);
                            resultCallback.resultWithProjectLists(path, multiList);
                        } else {
                            if (queryReBeans != null && !queryReBeans.isEmpty()) {
                                QueryResBean rowsDTO = queryReBeans.get(0);
                                resultCallback.resultWithProject(path, rowsDTO.getValue(), rowsDTO.getResult());
                            } else {
                                resultCallback.resultWithProject(path, "", "");
                            }
                        }
                    }
                }
            });

            return detection;
        }
    }
}
