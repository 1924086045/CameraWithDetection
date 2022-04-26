package com.zodo.camerawithdetection.bean;

import java.io.Serializable;

public class JniBeans implements Serializable {
    String vc;
    String vt;
    String pro;
    String path;
    String majority;

    public JniBeans(String vc, String vt, String pro, String path) {
        this.vc = vc;
        this.vt = vt;
        this.pro = pro;
        this.path = path;
    }

    public JniBeans(String vc, String vt, String pro, String path, String majority) {
        this.vc = vc;
        this.vt = vt;
        this.pro = pro;
        this.path = path;
        this.majority = majority;
    }

    public String getMajority() {
        return majority;
    }

    public void setMajority(String majority) {
        this.majority = majority;
    }

    public String getVc() {
        return vc;
    }

    public void setVc(String vc) {
        this.vc = vc;
    }

    public String getVt() {
        return vt;
    }

    public void setVt(String vt) {
        this.vt = vt;
    }

    public String getPro() {
        return pro;
    }

    public void setPro(String pro) {
        this.pro = pro;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    @Override
    public String toString() {
        float fvc= Float.valueOf(vc);
        float fvt = Float.valueOf(vt);
        float ratio = fvt/fvc;
        return "检测值{" +
                "vc='" + vc + '\'' +"\n"+
                "vt='" + vt + '\'' +"\n"
                +"比值:"+ratio+
                '}';
    }
}
