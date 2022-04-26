package com.zodo.camerawithdetection.utils;

import android.os.Environment;

import java.io.File;

public class CommonUtils {
    public static String getImgpath() {
        return Environment.getExternalStorageDirectory() + File.separator + "zodolabs" + File.separator + "img";
    }
}
