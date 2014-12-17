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
  `MAC_address` varchar(40) DEFAULT '',
  `AFi_name` varchar(200) DEFAULT '',
  `AFi_type` varchar(40) DEFAULT '',
  `AFi_version` varchar(40) DEFAULT '',
  `ip_address` varchar(16) DEFAULT '0.0.0.0',
  `running_time` double(11,0) DEFAULT '0.0',
  `uplink_packet_num` bigint(11) DEFAULT '0',
  `downlink_packet_numi` bigint(11) DEFAULT '0',
  `uplink_byte_num` bigint(11) DEFAULT '0',
  `downlink_byte_num` bigint(11) DEFAULT '0',
  `collect_timestamp` varchar(40) DEFAULT '',
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
  `signal_intensity` int(11) DEFAULT '0',
  `down_up_BW` int(11) DEFAULT '0' COMMENT '上下行带宽',
  `time_delay` int(11) DEFAULT '0',
  `collect_timestamp` varchar(40) DEFAULT '',
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
  `signal_interference` int(11) DEFAULT '0',
  `downlink_byte_num` bigint(11) DEFAULT '0',
  `uplink_byte_num` bigint(11) DEFAULT '0',
  `cover_area` int(11) DEFAULT '0' COMMENT '覆盖面积，单位平方米',
  `signal_intensity` int(11) DEFAULT '0',
  `user_access_rate` int(11) DEFAULT '0',
  `online_user_num` int(11) DEFAULT '0',
  `collect_timestamp` varchar(40) DEFAULT '',
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
  `uplink_packet_num` bigint(11) DEFAULT '0',
  `downlink_packet_num` bigint(11) DEFAULT '0',
  `uplink_byte_num` bigint(11) DEFAULT '0',
  `downlink_byte_num` bigint(11) DEFAULT '0',
  `collect_timestamp` varchar(40) DEFAULT '',
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
  `MAC_address` varchar(32) DEFAULT '',
  `MAC_alias` varchar(200) DEFAULT '',
  `hostname` varchar(200) DEFAULT '',
  `ip_address` varchar(16) DEFAULT '0.0.0.0',
  `device_type` varchar(32) DEFAULT NULL,
  `is_online` varchar(8) DEFAULT NULL,
  `access_point` varchar(200) DEFAULT '' COMMENT '注意区分漫游接入点',
  `running_time` double(11,0) DEFAULT '0.0',
  `uplink_packet_num` bigint(11) DEFAULT '0',
  `downlink_packet_num` bigint(11) DEFAULT '0',
  `uplink_byte_num` bigint(11) DEFAULT '0',
  `downlink_byte_num` bigint(11) DEFAULT '0',
  `collect_timestamp` varchar(40) DEFAULT '',
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
  `signal_intensity` int(11) DEFAULT '0',
  `consultation_rate` int(11) DEFAULT '0' COMMENT '协商速率',
  `access_rate` int(11) DEFAULT '0' COMMENT '接入速率',
  `WL_retransmission_rate` int(11) DEFAULT '0',
  `collect_timestamp` varchar(40) DEFAULT '',
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
  `signal_noise_rate` int(11) DEFAULT '0',
  `channel_utilization` int(11) DEFAULT '0',
  `signal_interference` int(11) DEFAULT '0',
  `collect_timestamp` varchar(40) DEFAULT '',
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
  `essid` varchar(200) DEFAULT '',
  `AFi_n` varchar(200) DEFAULT '',
  `online_user_count` int(11) DEFAULT '0',
  `uplink_packet_num` bigint(11) DEFAULT '0',
  `downlink_packet_num` bigint(11) DEFAULT '0',
  `uplink_byte_num` bigint(11) DEFAULT '0',
  `downlink_byte_num` bigint(11) DEFAULT '0',
  `collect_timestamp` varchar(40) DEFAULT '',
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of wireless_index_info
-- ----------------------------
