/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50151
Source Host           : localhost:3306
Source Database       : softdb

Target Server Type    : MYSQL
Target Server Version : 50151
File Encoding         : 65001

Date: 2014-12-10 10:21:31
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `afi_detailinfo_db`
-- ----------------------------
DROP TABLE IF EXISTS `afi_detailinfo_db`;
CREATE TABLE `afi_detailinfo_db` (
  `_id` int(11) NOT NULL AUTO_INCREMENT,
  `MAC_address` varchar(40) DEFAULT NULL,
  `AFi_name` varchar(200) DEFAULT NULL,
  `AFi_type` varchar(40) DEFAULT NULL,
  `AFi_version` varchar(40) DEFAULT NULL,
  `ip_address` varchar(16) DEFAULT NULL,
  `running_time` double(11,0) DEFAULT NULL,
  `uplink_packet_num` bigint(11) DEFAULT NULL,
  `downlink_packet_numi` bigint(11) DEFAULT NULL,
  `uplink_byte_num` bigint(11) DEFAULT NULL,
  `downlink_byte_num` bigint(11) DEFAULT NULL,
  `collect_timestamp` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of afi_detailinfo_db
-- ----------------------------

-- ----------------------------
-- Table structure for `ap_index_info`
-- ----------------------------
DROP TABLE IF EXISTS `ap_index_info`;
CREATE TABLE `ap_index_info` (
  `_id` int(11) NOT NULL AUTO_INCREMENT,
  `signal_intensity` int(11) DEFAULT NULL,
  `down_up_BW` int(11) DEFAULT NULL COMMENT '上下行带宽',
  `time_delay` int(11) DEFAULT NULL,
  `collect_timestamp` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of ap_index_info
-- ----------------------------

-- ----------------------------
-- Table structure for `sys_health_score`
-- ----------------------------
DROP TABLE IF EXISTS `sys_health_score`;
CREATE TABLE `sys_health_score` (
  `_id` int(11) NOT NULL AUTO_INCREMENT,
  `health_score` int(11) DEFAULT '0',
  `signal_interference` int(11) DEFAULT NULL,
  `downlink_byte_num` bigint(11) DEFAULT NULL,
  `uplink_byte_num` bigint(11) DEFAULT NULL,
  `cover_area` int(11) DEFAULT NULL COMMENT '覆盖面积，单位平方米',
  `signal_intensity` int(11) DEFAULT NULL,
  `user_access_rate` int(11) DEFAULT NULL,
  `online_user_num` int(11) DEFAULT NULL,
  `collect_timestamp` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of sys_health_score
-- ----------------------------

-- ----------------------------
-- Table structure for `throughout_index_info`
-- ----------------------------
DROP TABLE IF EXISTS `throughout_index_info`;
CREATE TABLE `throughout_index_info` (
  `_id` int(11) NOT NULL AUTO_INCREMENT,
  `uplink_packet_num` bigint(11) DEFAULT NULL,
  `downlink_packet_num` bigint(11) DEFAULT NULL,
  `uplink_byte_num` bigint(11) DEFAULT NULL,
  `downlink_byte_num` bigint(11) DEFAULT NULL,
  `collect_timestamp` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of throughout_index_info
-- ----------------------------

-- ----------------------------
-- Table structure for `user_detailinfo_db`
-- ----------------------------
DROP TABLE IF EXISTS `user_detailinfo_db`;
CREATE TABLE `user_detailinfo_db` (
  `_id` int(11) NOT NULL AUTO_INCREMENT,
  `MAC_address` varchar(32) DEFAULT NULL,
  `MAC_alias` varchar(200) DEFAULT NULL,
  `hostname` varchar(200) DEFAULT NULL,
  `ip_address` varchar(16) DEFAULT NULL,
  `access_point` varchar(200) DEFAULT NULL COMMENT '注意区分漫游接入点',
  `running_time` double(11,0) DEFAULT NULL,
  `uplink_packet_num` bigint(11) DEFAULT NULL,
  `downlink_packet_num` bigint(11) DEFAULT NULL,
  `uplink_byte_num` bigint(11) DEFAULT NULL,
  `downlink_byte_num` bigint(11) DEFAULT NULL,
  `collect_timestamp` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of user_detailinfo_db
-- ----------------------------

-- ----------------------------
-- Table structure for `user_index_info`
-- ----------------------------
DROP TABLE IF EXISTS `user_index_info`;
CREATE TABLE `user_index_info` (
  `_id` int(11) NOT NULL AUTO_INCREMENT,
  `signal_intensity` int(11) DEFAULT NULL,
  `consultation_rate` int(11) DEFAULT NULL COMMENT '协商速率',
  `access_rate` int(11) DEFAULT NULL COMMENT '接入速率',
  `WL_retransmission_rate` int(11) DEFAULT NULL,
  `collect_timestamp` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of user_index_info
-- ----------------------------

-- ----------------------------
-- Table structure for `wifi_index_info`
-- ----------------------------
DROP TABLE IF EXISTS `wifi_index_info`;
CREATE TABLE `wifi_index_info` (
  `_id` int(11) NOT NULL AUTO_INCREMENT,
  `signal_noise_rate` int(11) DEFAULT NULL,
  `channel_utilization` int(11) DEFAULT NULL,
  `signal_interference` int(11) DEFAULT NULL,
  `collect_timestamp` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of wifi_index_info
-- ----------------------------

-- ----------------------------
-- Table structure for `wireless_index_info`
-- ----------------------------
DROP TABLE IF EXISTS `wireless_index_info`;
CREATE TABLE `wireless_index_info` (
  `_id` int(11) NOT NULL AUTO_INCREMENT,
  `essid` varchar(200) DEFAULT NULL,
  `AFi_n` varchar(200) DEFAULT NULL,
  `online_user_count` int(11) DEFAULT NULL,
  `uplink_packet_num` bigint(11) DEFAULT NULL,
  `downlink_packet_num` bigint(11) DEFAULT NULL,
  `uplink_byte_num` bigint(11) DEFAULT NULL,
  `downlink_byte_num` bigint(11) DEFAULT NULL,
  `collect_timestamp` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of wireless_index_info
-- ----------------------------
