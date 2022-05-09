package com.zodo.camerawithdetection.bean;

import java.io.Serializable;

/**
 * Created by zzf on 17-7-25.
 */

public class QueryResBean implements Serializable {
    Long id;
    String num;
    String detail;
    String until;
    String giveUntil;
    String project;
    String value;
    String result;
    String date;
    String person;
    String upload;
    String ypfl;
    String ypmc;
    String imgpath;
    String weight;
    String phone;

    public QueryResBean(Long id, String num, String detail, String until, String giveUntil, String project, String value, String result, String date, String person,
                        String upload, String ypfl, String ypmc, String imgpath, String weight, String phone) {
        this.id = id;
        this.num = num;
        this.detail = detail;
        this.until = until;
        this.giveUntil = giveUntil;
        this.project = project;
        this.value = value;
        this.result = result;
        this.date = date;
        this.person = person;
        this.upload = upload;
        this.ypfl = ypfl;
        this.ypmc = ypmc;
        this.imgpath = imgpath;
        this.weight = weight;
        this.phone = phone;
    }

    public QueryResBean() {
    }

    public Long getId() {
        return this.id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getNum() {
        return this.num;
    }

    public void setNum(String num) {
        this.num = num;
    }

    public String getDetail() {
        return this.detail;
    }

    public void setDetail(String detail) {
        this.detail = detail;
    }

    public String getUntil() {
        return this.until;
    }

    public void setUntil(String until) {
        this.until = until;
    }

    public String getGiveUntil() {
        return this.giveUntil;
    }

    public void setGiveUntil(String giveUntil) {
        this.giveUntil = giveUntil;
    }

    public String getProject() {
        return this.project;
    }

    public void setProject(String project) {
        this.project = project;
    }

    public String getValue() {
        return this.value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public String getResult() {
        return this.result;
    }

    public void setResult(String result) {
        this.result = result;
    }

    public String getDate() {
        return this.date;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public String getPerson() {
        return this.person;
    }

    public void setPerson(String person) {
        this.person = person;
    }

    public String getUpload() {
        return this.upload;
    }

    public void setUpload(String upload) {
        this.upload = upload;
    }

    public String getYpfl() {
        return this.ypfl;
    }

    public void setYpfl(String ypfl) {
        this.ypfl = ypfl;
    }

    public String getYpmc() {
        return this.ypmc;
    }

    public void setYpmc(String ypmc) {
        this.ypmc = ypmc;
    }

    public String getImgpath() {
        return this.imgpath;
    }

    public void setWeight(String weight) {
        this.weight = weight;
    }

    public void setImgpath(String imgpath) {
        this.imgpath = imgpath;
    }

    public String getWeight() {
        return this.weight;
    }

    public String getPhone() {
        return this.phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

}
