package com.zodo.camerawithdetection.callback;

import com.zodo.camerawithdetection.bean.CheckSampleBean;
import com.zodo.camerawithdetection.bean.JniBeans;
import com.zodo.camerawithdetection.bean.QueryRes;

import java.util.ArrayList;

public interface OnResultCallback {
    /**
     * 不进行数据比对直接回传
     */
//    void resultWithOutCompare(String path, ArrayList<JniBeans> jniBeans);

    /**
     * 单通道检测
     */
    void resultWithProject(String path, String checkValue,String checkResult);

    /**
     * 多通道检测
     */
    void resultWithProjectLists(String path, ArrayList<QueryRes> rowsDTOS);
}
