package com.zodo.camerawithdetection.bean;

import java.io.Serializable;
import java.util.ArrayList;

public class CheckSampleBean implements Serializable {

    private int code;
    private String msg;
    private ArrayList<RowsDTO> rows;
    private String state;
    private String total;

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getMsg() {
        return msg;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }

    public ArrayList<RowsDTO> getRows() {
        return rows;
    }

    public void setRows(ArrayList<RowsDTO> rows) {
        this.rows = rows;
    }

    public String getState() {
        return state;
    }

    public void setState(String state) {
        this.state = state;
    }

    public String getTotal() {
        return total;
    }

    public void setTotal(String total) {
        this.total = total;
    }

    public static class RowsDTO implements Serializable {
        private String checkKey;
        private String checkKeyId;
        private String checkType;
        private String checkTypeStr;
        private String checkWay;
        private String checkWayId;
        private String checkerName;
        private String location;
        private String manufacturerName;
        private String sampleName;
        private String sampleNo;
        private String samplingPersons;
        private String taskFrom;
        private String taskFromStr;
        private String project;
        private String weight;
        private String phone;
        private String giveUntil;
        private String person;
        private String upload;
        private String value;
        private String result;
        private String date;
        private String imgpath;
        private String num;
        private String projectmethod;
        private String area;

        public RowsDTO() {
        }

        public RowsDTO(String checkKey, String checkKeyId, String checkType, String checkTypeStr, String checkWay, String checkWayId, String checkerName,
                       String location, String manufacturerName, String sampleName, String sampleNo, String samplingPersons, String taskFrom, String taskFromStr,
                       String project, String weight, String phone, String giveUntil, String person, String upload, String value, String result, String date, String imgpath,
                       String num,String projectmethod) {
            this.checkKey = checkKey;
            this.checkKeyId = checkKeyId;
            this.checkType = checkType;
            this.checkTypeStr = checkTypeStr;
            this.checkWay = checkWay;
            this.checkWayId = checkWayId;
            this.checkerName = checkerName;
            this.location = location;
            this.manufacturerName = manufacturerName;
            this.sampleName = sampleName;
            this.sampleNo = sampleNo;
            this.samplingPersons = samplingPersons;
            this.taskFrom = taskFrom;
            this.taskFromStr = taskFromStr;
            this.project = project;
            this.weight = weight;
            this.phone = phone;
            this.giveUntil = giveUntil;
            this.person = person;
            this.upload = upload;
            this.value = value;
            this.result = result;
            this.date = date;
            this.imgpath = imgpath;
            this.num=num;
            this.projectmethod=projectmethod;
        }

        public String getArea() {
            return area;
        }

        public void setArea(String area) {
            this.area = area;
        }

        public String getProjectmethod() {
            return projectmethod;
        }

        public void setProjectmethod(String projectmethod) {
            this.projectmethod = projectmethod;
        }

        public String getNum() {
            return num;
        }

        public void setNum(String num) {
            this.num = num;
        }

        public String getImgpath() {
            return imgpath;
        }

        public void setImgpath(String imgpath) {
            this.imgpath = imgpath;
        }

        public String getValue() {
            return value;
        }

        public void setValue(String value) {
            this.value = value;
        }

        public String getDate() {
            return date;
        }

        public void setDate(String date) {
            this.date = date;
        }

        public String getResult() {
            return result;
        }

        public void setResult(String result) {
            this.result = result;
        }

        public String getGiveUntil() {
            return giveUntil;
        }

        public void setGiveUntil(String giveUntil) {
            this.giveUntil = giveUntil;
        }

        public String getPerson() {
            return person;
        }

        public void setPerson(String person) {
            this.person = person;
        }

        public String getUpload() {
            return upload;
        }

        public void setUpload(String upload) {
            this.upload = upload;
        }

        public String getPhone() {
            return phone;
        }

        public void setPhone(String phone) {
            this.phone = phone;
        }

        public String getWeight() {
            return weight;
        }

        public void setWeight(String weight) {
            this.weight = weight;
        }

        public String getProject() {
            return project;
        }

        public void setProject(String project) {
            this.project = project;
        }

        public String getCheckKey() {
            return checkKey;
        }

        public void setCheckKey(String checkKey) {
            this.checkKey = checkKey;
        }

        public String getCheckKeyId() {
            return checkKeyId;
        }

        public void setCheckKeyId(String checkKeyId) {
            this.checkKeyId = checkKeyId;
        }

        public String getCheckType() {
            return checkType;
        }

        public void setCheckType(String checkType) {
            this.checkType = checkType;
        }

        public String getCheckTypeStr() {
            return checkTypeStr;
        }

        public void setCheckTypeStr(String checkTypeStr) {
            this.checkTypeStr = checkTypeStr;
        }

        public String getCheckWay() {
            return checkWay;
        }

        public void setCheckWay(String checkWay) {
            this.checkWay = checkWay;
        }

        public String getCheckWayId() {
            return checkWayId;
        }

        public void setCheckWayId(String checkWayId) {
            this.checkWayId = checkWayId;
        }

        public String getCheckerName() {
            return checkerName;
        }

        public void setCheckerName(String checkerName) {
            this.checkerName = checkerName;
        }

        public String getLocation() {
            return location;
        }

        public void setLocation(String location) {
            this.location = location;
        }

        public String getManufacturerName() {
            return manufacturerName;
        }

        public void setManufacturerName(String manufacturerName) {
            this.manufacturerName = manufacturerName;
        }

        public String getSampleName() {
            return sampleName;
        }

        public void setSampleName(String sampleName) {
            this.sampleName = sampleName;
        }

        public String getSampleNo() {
            return sampleNo;
        }

        public void setSampleNo(String sampleNo) {
            this.sampleNo = sampleNo;
        }

        public String getSamplingPersons() {
            return samplingPersons;
        }

        public void setSamplingPersons(String samplingPersons) {
            this.samplingPersons = samplingPersons;
        }

        public String getTaskFrom() {
            return taskFrom;
        }

        public void setTaskFrom(String taskFrom) {
            this.taskFrom = taskFrom;
        }

        public String getTaskFromStr() {
            return taskFromStr;
        }

        public void setTaskFromStr(String taskFromStr) {
            this.taskFromStr = taskFromStr;
        }

        public String toStringnovalue() {
            return "\n采样编号：" + sampleNo + '\n' +
                    "检测项目：" + project + '\n' +
                    "样品名称：" + sampleName + '\n' +
                    "被检单位：" + giveUntil + '\n' +
                    "检测人员：" + person + '\n' +
                    "检测结果：" + result + '\n' +
                    "检测日期：" + date + '\n';
        }
        //合格证序列
        public String toQualifiedString() {
            return "│                            │" +'\n'+
                    "│产品名称:"+ sampleName + '\n' +
                    "│数量（重量）:" + weight + '\n' +
                    "│产地:" + giveUntil + '\n' +
                    "│生产者盖章或签名:" +"           │"+ '\n' +
                    "│联系方式:" + phone + '\n' +
                    "│开具日期:" + date +"│"+ '\n';
        }
    }
}
