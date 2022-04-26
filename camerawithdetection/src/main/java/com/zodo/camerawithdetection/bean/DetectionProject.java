package com.zodo.camerawithdetection.bean;

import java.io.Serializable;

public class DetectionProject implements Serializable {
    Long id;
    String projectname;
    String projectmethod;
    double yuzhi;

    public DetectionProject(Long id, String projectname, String projectmethod,
                            double yuzhi) {
        this.id = id;
        this.projectname = projectname;
        this.projectmethod = projectmethod;
        this.yuzhi = yuzhi;
    }

    public DetectionProject() {
    }

    @Override
    public String toString() {
        return "DetectionProject{" +
                "id=" + id +
                ", projectname='" + projectname + '\'' +
                ", projectmethod='" + projectmethod + '\'' +
                ", yuzhi=" + yuzhi +
                '}';
    }

    public Long getId() {
        return this.id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getProjectname() {
        return this.projectname;
    }

    public void setProjectname(String projectname) {
        this.projectname = projectname;
    }

    public String getProjectmethod() {
        return this.projectmethod;
    }

    public void setProjectmethod(String projectmethod) {
        this.projectmethod = projectmethod;
    }

    public double getYuzhi() {
        return this.yuzhi;
    }

    public void setYuzhi(double yuzhi) {
        this.yuzhi = yuzhi;
    }
}
