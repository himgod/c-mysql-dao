#include "DAO.h"
#include<syslog.h>


/*
 *To get format time function,eg:2014-12-15 22:43:55
 *parameter:
 *parameter:
 *returned: format time string
 */
int get_db_format_time_from_tm(struct tm* timeinfo,char* format_time)
{
    if( timeinfo == NULL || format_time == NULL )
    {
      syslog(LOG_ERR,"%s bad input parameters timeinfo %p and format_time %p\n",__func__,timeinfo,format_time);  
      return FAILURE;
    }
    
    snprintf(format_time,SIMPLE_STR_LEN-1,"%4d-%02d-%02d %02d:%02d:%02d",1900+timeinfo->tm_year,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    return SUCCESS;

}
/*
 *从结构体struct tm中转换为格式化的时间字符串
 *param:timeinfo,输入参数。
 *param:format_tiem_str,输出参数，格式如:"%Y-%m-%d %H:%M:%S"
 *return:返回值0为成功，-1为失败
 */
int get_db_format_time_str_from_tm(struct tm* timeinfo,char*format_time_str)
{
    if( timeinfo == NULL || format_time_str == NULL )
    {
      syslog(LOG_ERR,"%s bad input parameters timeinfo %p and format_time %p\n",__func__,timeinfo,format_time_str);  
      return FAILURE;
    }
    strftime(format_time_str,SIMPLE_STR_LEN,FORMAT_TIME_STR,timeinfo);
    return SUCCESS;
}

/*
 *it's  from format_time_str to struct tm or time_t
 *input parameter:format_time_str
 *output parameter:struct tm* timeinfo
 *return: 0 means success ,-1 means failure
 */
int get_db_tm_from_format_time(char* format_time_str,struct tm* timeinfo)
{
    if( timeinfo == NULL || format_time_str == NULL )
    {
       syslog(LOG_ERR,"%s bad input parameters timeinfo %p and format_time_str %p\n",__func__,timeinfo,format_time_str);
       return FAILURE;
    }

    if(strptime(format_time_str,FORMAT_TIME_STR,timeinfo))
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *just to connect to dabase
 *param:mysql, a handle of mysql
 *param:host,means mysqlserver's hostname.default localhost
 *param:user,mysql's username
 *param:passwd
 *param:db_name,means database's name you will access.
 *return: a global mysql descriptor
 *
 */
MYSQL* db_connect_to_database(MYSQL* mysql,const char* host,const char* user,
                            const char* passwd,const char* db_name)
{
    MYSQL* mysql_ptr = NULL;
    if( !db_name )
    {
        syslog(LOG_ERR,"DB NAME  error\n");
        return NULL;
    }
    // initilize the mysql 
    mysql_ptr = mysql_init(mysql);
    if(mysql_ptr == NULL)
    {
        syslog(LOG_ERR,"Initlize MySQL failed\n");
        return NULL;
    }

    if(!mysql_real_connect(mysql_ptr,host,user,passwd,db_name,0,NULL,0))
    {
        syslog(LOG_ERR,"Cannot connect to database\n");
        mysql_error(mysql_ptr);
        return NULL;
    }
    return mysql_ptr;
    
}
MYSQL* db_get_database_connection(MYSQL* mysql,const char* host,const char* user,
                            const char* passwd,const char* db_name)
{
    return db_connect_to_database(mysql,host,user,passwd,db_name);
}

/*
 * close mysql
 */
int db_disconnect_to_database(MYSQL* mysql)
{
   if(mysql)
   {
       mysql_close(mysql);
       return SUCCESS;
   }
   syslog(LOG_ERR,"Disconnected mysql database. Mysql maybe null\n");
   return FAILURE;
}

int db_put_database_connection(MYSQL* mysql)
{
   return db_disconnect_to_database(mysql);
}

/*a common interface for mysql's operation
 * param:mysql is a handle for mysql
 * param:query, a SQL sentence ,for select ,update ,insert and so on.
 * param: call_back , a function to handle with data
 * result: return result set
 */
int db_do_query(MYSQL* mysql,const char* query,void(*call_back)(void*,MYSQL_ROW,int,int),void* result)
{
    int affected_rows = 0;
    if( mysql_query(mysql,query) != 0)
    {
       syslog(LOG_ERR,"Database query failed:%s [%s]\n",mysql_error(mysql),query);
       return FAILURE;
    }

    if( mysql_field_count(mysql) > 0 )
    {
       MYSQL_RES* res = NULL; // to store result set
       MYSQL_ROW row;

       int num_fields = 0;//the number of the filed
       int num_rows = 0;// the number of the row
       if( !(res = mysql_store_result(mysql)))
       {
           syslog(LOG_ERR,"mysql_store_result() happened error:%s",mysql_error(mysql));
           return FAILURE;
       }

       num_fields = mysql_num_fields(res);
       num_rows = mysql_num_rows(res);

       while(( row = mysql_fetch_row(res) ) != NULL)
       {
           if(call_back)
           {
                call_back(result,row,num_fields,num_rows);
           }
       }
       mysql_free_result(res);

       return num_rows;
       
    }
    else
    {
        affected_rows = mysql_affected_rows(mysql);
        syslog(LOG_DEBUG," Operation %s affected rows %d\n.",query,affected_rows);
        return affected_rows;
    }
    //return 0;
}


/*a common interface for mysql's operation
 * param:mysql is a handle for mysql
 * param:query, a SQL sentence ,for select ,update ,insert and so on.
 * param: call_back , a function to handle with data
 */
int db_do_query_2(MYSQL* mysql,const char* query,int(*call_back)(void*,MYSQL_RES*,int,int),void* result)
{
    int affected_rows = 0;
    if( mysql_query(mysql,query) != 0)
    {
       syslog(LOG_ERR,"Database query failed:%s [%s]\n",mysql_error(mysql),query);
       return FAILURE;
    }

    if( mysql_field_count(mysql) > 0 )
    {
       MYSQL_RES* res = NULL; // to store result set
      // MYSQL_ROW row;

       int num_fields = 0;//the number of the filed
       int num_rows = 0;// the number of the row
       if( !(res = mysql_store_result(mysql)))
       {
           syslog(LOG_ERR,"mysql_store_result() happened error:%s",mysql_error(mysql));
           return FAILURE;
       }

       num_fields = mysql_num_fields(res);
       num_rows = mysql_num_rows(res);

       //while(( row = mysql_fetch_row(res) ) != NULL)
       {
           if(call_back)
           {
                call_back(result,res,num_fields,num_rows);
           }
       }
       mysql_free_result(res);

       return num_rows;
       
    }
    else
    {
        affected_rows = mysql_affected_rows(mysql);
        syslog(LOG_DEBUG," Operation %s affected rows %d\n.",query,affected_rows);
        return affected_rows;
    }
    //return 0;
}

/*
 * 打印对应数据库表的结果集信息
 * param:arg,对应的返回类型，是函数的输出变量
 * param:row,对应查询表的的每一行记录集
 * param:num_fields,对应查询表的字段数
 * param:num_rows,查询结果的数据库记录行数
 * return:返回值，无.
 */
void call_back_print_query_info(void* result,MYSQL_ROW row,int num_fields,int num_rows)
{

    int i;
    for(i = 0; i < num_fields;i++)
    {
        printf("filed[%d]: %s",i,row[i]);
        
    }
    printf("\n");
    return;
}

/*
 * 处理数据库表ap_index_info的回调函数
 * param:arg,对应的返回类型，是函数的输出变量
 * param:resultSet,对应查询表的结果集
 * param:num_fields,对应查询表的字段数
 * param:num_rows,查询结果的数据库记录行数
 * return:返回值，0为成功，-1为失败
 */
int call_back_get_ap_index_info(void* arg,MYSQL_RES* resultSet,int num_fields,int num_rows)
{
    printf("*********************************%s In*************************\n",__func__);
    query_ap_info* result = (query_ap_info*)arg;
   // ap_info** ap = (ap_info**)malloc(num_rows*sizeof(ap_info*));
   /* if( ap == NULL )
    {
        printf("struct ap_info** malloced memory failed\n");
        return FAILURE;
    }
    */

   // int cur_row = 0;
    MYSQL_ROW row;
    if( result == NULL )
    {
        printf("%s function %d line bad input parameter.\n",__func__,__LINE__);
        syslog(LOG_ERR,"%s function %d bad input parameter.\n",__func__,__LINE__);
        return FAILURE;
    }
    printf("num_fields:%d,num_rows:%d\n",num_fields,num_rows);
    if(result && (result->apInfo == NULL) && num_fields > 0 && num_rows > 0)
    {
        
        printf("%s In\n",__func__);
        result->apInfo = (ap_info*)malloc(num_rows*sizeof(ap_info));
        if( NULL == result->apInfo )
        {
            syslog(LOG_ERR,"%s function %d memory malloc failed.\n",__func__,__LINE__);
            return FAILURE;
        }

        memset(result->apInfo,0,num_rows*sizeof(ap_info));
        result->num_columns = num_fields;
        result->num_rows = num_rows;
        result->cur_row = 0;
    }

    if(result->apInfo != NULL && result->cur_row < result->num_rows )
    {
        while(( row = mysql_fetch_row(resultSet) ) != NULL)
        {
            //i = atoi(row[1]);
            printf("%d\n",result->cur_row);
            if(row[1])
            {
                result->apInfo[result->cur_row].signal_intensity = atoi(row[1]);
            }
            else
            {
                result->apInfo[result->cur_row].signal_intensity = 0;
            }
            printf("Field[1]:%d\t",result->apInfo[result->cur_row].signal_intensity);
            if(row[2])
            {
                result->apInfo[result->cur_row].down_up_BW = atoi(row[2]);
            }
            else
            {
                result->apInfo[result->cur_row].down_up_BW = 0;
            }
            printf("Field[2]:%d\t",result->apInfo[result->cur_row].down_up_BW);
            if(row[3])
            {
                result->apInfo[result->cur_row].time_delay = atoi(row[3]);
            }
            else
            {
                result->apInfo[result->cur_row].time_delay = 0;
            }
            printf("Field[3]:%d\t",result->apInfo[result->cur_row].time_delay);
            printf("Field[4]:%s\n",row[4]);
            if(row[4])
            {
                strcpy(result->apInfo[result->cur_row].collect_timestamp,row[4]);
            }
            else
            {
                strcpy(result->apInfo[result->cur_row].collect_timestamp,"");
            }
            result->cur_row++;
        }
        return SUCCESS;
    }
   // result = (void*)ap;
   // free(ap);
    return FAILURE;
}

/*
 * 处理数据库表afi_detailinfo_db的回调函数
 * param:arg,对应的返回类型，是函数的输出变量
 * param:resultSet,对应查询表的结果集
 * param:num_fields,对应查询表的字段数
 * param:num_rows,查询结果的数据库记录行数
 * return:返回值，0为成功，-1为失败
 */
int call_back_get_afi_info(void* arg,MYSQL_RES* resultSet,int num_fields,int num_rows)
{
    printf("*****************************%s In*****************************\n",__func__);
    query_afi_info* result = (query_afi_info*)arg;
   // ap_info** ap = (ap_info**)malloc(num_rows*sizeof(ap_info*));
   /* if( ap == NULL )
    {
        printf("struct ap_info** malloced memory failed\n");
        return FAILURE;
    }
    */

   // int cur_row = 0;
    MYSQL_ROW row;
    if( result == NULL )
    {
        printf("%s function %d line bad input parameter.\n",__func__,__LINE__);
        syslog(LOG_ERR,"%s function %d bad input parameter.\n",__func__,__LINE__);
        return FAILURE;
    }
    printf("num_fields:%d,num_rows:%d\n",num_fields,num_rows);
    if(result && (result->afiInfo == NULL) && num_fields > 0 && num_rows > 0)
    {
        
       // printf("%s In\n",__func__);
        result->afiInfo = (afi_info*)malloc(num_rows*sizeof(afi_info));
        if( NULL == result->afiInfo )
        {
            syslog(LOG_ERR,"%s function %d memory malloc failed.\n",__func__,__LINE__);
            return FAILURE;
        }

        memset(result->afiInfo,0,num_rows*sizeof(afi_info));
        result->num_columns = num_fields;
        result->num_rows = num_rows;
        result->cur_row = 0;
    }

    if(result->afiInfo != NULL && result->cur_row < result->num_rows )
    {
        while(( row = mysql_fetch_row(resultSet) ) != NULL)
        {
            //i = atoi(row[1]);
            printf("%d\n",result->cur_row);
            if(row[1])
            {
                strcpy(result->afiInfo[result->cur_row].mac_address,row[1]);
            }
            else
            {
                strcpy(result->afiInfo[result->cur_row].afi_version,"");
            }
            strcpy(result->afiInfo[result->cur_row].afi_name,row[2]);
            if(row[3])
            {
                strcpy(result->afiInfo[result->cur_row].afi_type,row[3]);
            }
            else
            {
                strcpy(result->afiInfo[result->cur_row].afi_version,"");
            }
            if(row[4])
            {
                strcpy(result->afiInfo[result->cur_row].afi_version,row[4]);
            }
            else
            {
                strcpy(result->afiInfo[result->cur_row].afi_version,"");
            }
            if(row[5])
            {
                strcpy(result->afiInfo[result->cur_row].afi_ip_address,row[5]);
            }
            else
            {    
                strcpy(result->afiInfo[result->cur_row].afi_ip_address,"");
            }
            if(row[6])
            {
                result->afiInfo[result->cur_row].running_time = atof(row[6]);
            }
            else
            {
                result->afiInfo[result->cur_row].running_time = 0.0;
            }
            if(row[7])
            {
                result->afiInfo[result->cur_row].uplink_packet_num = atol(row[7]);
            }
            else
            {
                result->afiInfo[result->cur_row].uplink_packet_num = 0L;
            }
            if(row[8])
            {
                result->afiInfo[result->cur_row].downlink_packet_num = atol(row[8]);
            }
            else
            {
                result->afiInfo[result->cur_row].downlink_packet_num = 0L;
            }
            if(row[9])
            {
                result->afiInfo[result->cur_row].uplink_byte_num = atol(row[9]);
            }
            else
            {
                result->afiInfo[result->cur_row].uplink_byte_num = 0L;
            }
            if(row[10])
            {
                result->afiInfo[result->cur_row].downlink_byte_num = atol(row[10]);
            }
            else
            {
                result->afiInfo[result->cur_row].downlink_byte_num = 0L;
            }
            if(row[11])
            {
                strcpy(result->afiInfo[result->cur_row].collect_timestamp,row[11]);
            }
            else
            {
                strcpy(result->afiInfo[result->cur_row].collect_timestamp,"");
            }
            printf("Field[1]:%s\t",result->afiInfo[result->cur_row].mac_address);
            printf("Field[2]:%s\t",result->afiInfo[result->cur_row].afi_name);
            printf("Field[3]:%s\t",result->afiInfo[result->cur_row].afi_type);
            printf("Field[4]:%s\t",result->afiInfo[result->cur_row].afi_version);
            printf("Field[5]:%s\n",result->afiInfo[result->cur_row].afi_ip_address);
            printf("Field[6]:%f\t",result->afiInfo[result->cur_row].running_time);
            printf("Field[7]:%ld\t",result->afiInfo[result->cur_row].uplink_packet_num);
            printf("Field[8]:%ld\t",result->afiInfo[result->cur_row].downlink_packet_num);
            printf("Field[9]:%ld\t",result->afiInfo[result->cur_row].uplink_byte_num);
            printf("Field[10]:%ld\n",result->afiInfo[result->cur_row].downlink_byte_num);
            printf("Field[11]:%s\n",result->afiInfo[result->cur_row].collect_timestamp);
            result->cur_row++;
        }
        return SUCCESS;
    }
   // result = (void*)ap;
   // free(ap);
    return FAILURE;
}
/*
 * 处理数据库表sys_health_score_info的回调函数
 * param:arg,对应的返回类型，是函数的输出变量
 * param:resultSet,对应查询表的结果集
 * param:num_fields,对应查询表的字段数
 * param:num_rows,查询结果的数据库记录行数
 * return:返回值，0为成功，-1为失败
 */
int call_back_get_sys_score_info(void* arg,MYSQL_RES* resultSet,int num_fields,int num_rows)
{
    printf("*****************************%s In*****************************\n",__func__);
    query_sys_info* result = (query_sys_info*)arg;
   // ap_info** ap = (ap_info**)malloc(num_rows*sizeof(ap_info*));
   /* if( ap == NULL )
    {
        printf("struct ap_info** malloced memory failed\n");
        return FAILURE;
    }
    */

   // int cur_row = 0;
    MYSQL_ROW row;
    if( result == NULL )
    {
        printf("%s function %d line bad input parameter.\n",__func__,__LINE__);
        syslog(LOG_ERR,"%s function %d bad input parameter.\n",__func__,__LINE__);
        return FAILURE;
    }
    printf("num_fields:%d,num_rows:%d\n",num_fields,num_rows);
    if(result && (result->sysInfo == NULL) && num_fields > 0 && num_rows > 0)
    {
        
        printf("%s In\n",__func__);
        result->sysInfo = (sys_score*)malloc(num_rows*sizeof(sys_score));
        if( NULL == result->sysInfo )
        {
            syslog(LOG_ERR,"%s function %d memory malloc failed.\n",__func__,__LINE__);
            return FAILURE;
        }

        memset(result->sysInfo,0,num_rows*sizeof(sys_score));
        result->num_columns = num_fields;
        result->num_rows = num_rows;
        result->cur_row = 0;
    }

    if(result->sysInfo != NULL && result->cur_row < result->num_rows )
    {
        while(( row = mysql_fetch_row(resultSet) ) != NULL)
        {
            //i = atoi(row[1]);
            printf("%d\n",result->cur_row);
            if(row[1])
            {
                result->sysInfo[result->cur_row].health_score = atoi(row[1]);
            }
            else
            {
                result->sysInfo[result->cur_row].health_score = 0;
            }
            if(row[2])
            {
                result->sysInfo[result->cur_row].signal_interference = atoi(row[2]);
            }
            else
            {
                result->sysInfo[result->cur_row].signal_interference = 0;
            }
            if(row[3])
            {
                result->sysInfo[result->cur_row].downlink_byte_num = atol(row[3]);
            }
            else
            {
                result->sysInfo[result->cur_row].downlink_byte_num = 0L;
            }
            if(row[4])
            {
               result->sysInfo[result->cur_row].uplink_byte_num = atol(row[4]);
            }
            else
            {
                result->sysInfo[result->cur_row].uplink_byte_num = 0L;
            }
            if(row[5])
            {
                result->sysInfo[result->cur_row].cover_area = atoi(row[5]);
            }
            else
            {
                result->sysInfo[result->cur_row].cover_area = 0;
            }
            if(row[6])
            {
                result->sysInfo[result->cur_row].signal_intensity = atoi(row[6]);
            }
            else
            {
                result->sysInfo[result->cur_row].signal_intensity = 0;
            }
            if(row[7])
            {
                result->sysInfo[result->cur_row].user_access_rate = atoi(row[7]);
            }
            else
            {
                result->sysInfo[result->cur_row].user_access_rate = 0;
            }
            if(row[8])
            {
                result->sysInfo[result->cur_row].online_user_num = atoi(row[8]);
            }
            else
            {
                result->sysInfo[result->cur_row].online_user_num = 0;;
            }
            if(row[9])
            {
                strcpy(result->sysInfo[result->cur_row].collect_timestamp,row[9]);
            }
            else
            {
                strcpy(result->sysInfo[result->cur_row].collect_timestamp,"");
            }
            printf("Field[1]:%d\t",result->sysInfo[result->cur_row].health_score);
            printf("Field[2]:%d\t",result->sysInfo[result->cur_row].signal_interference);
            printf("Field[3]:%ld\t",result->sysInfo[result->cur_row].downlink_byte_num);
            printf("Field[4]:%ld\n",result->sysInfo[result->cur_row].uplink_byte_num);
            printf("Field[5]:%d\t",result->sysInfo[result->cur_row].cover_area);
            printf("Field[6]:%d\t",result->sysInfo[result->cur_row].signal_intensity);
            printf("Field[7]:%d\t",result->sysInfo[result->cur_row].user_access_rate);
            printf("Field[8]:%d\n",result->sysInfo[result->cur_row].online_user_num);
            printf("Field[9]:%s\n",result->sysInfo[result->cur_row].collect_timestamp);
            result->cur_row++;
        }
        return SUCCESS;
    }
   // result = (void*)ap;
   // free(ap);
    return FAILURE;
}

/*
 * 处理数据库表throughout_index_info的回调函数
 * param:arg,对应的返回类型，是函数的输出变量
 * param:resultSet,对应查询表的结果集
 * param:num_fields,对应查询表的字段数
 * param:num_rows,查询结果的数据库记录行数
 * return:返回值，0为成功，-1为失败
 */
int call_back_get_throughout_info(void* arg,MYSQL_RES* resultSet,int num_fields,int num_rows)
{
    printf("****************************%s In**************************\n",__func__);
    query_throughout_info* result = (query_throughout_info*)arg;
   // ap_info** ap = (ap_info**)malloc(num_rows*sizeof(ap_info*));
   /* if( ap == NULL )
    {
        printf("struct ap_info** malloced memory failed\n");
        return FAILURE;
    }
    */

   // int cur_row = 0;
    MYSQL_ROW row;
    if( result == NULL )
    {
        printf("%s function %d line bad input parameter.\n",__func__,__LINE__);
        syslog(LOG_ERR,"%s function %d bad input parameter.\n",__func__,__LINE__);
        return FAILURE;
    }
    printf("num_fields:%d,num_rows:%d\n",num_fields,num_rows);
    if(result && (result->throughoutInfo == NULL) && num_fields > 0 && num_rows > 0)
    {
        
       // printf("%s In\n",__func__);
        result->throughoutInfo = (throughout_info*)malloc(num_rows*sizeof(throughout_info));
        if( NULL == result->throughoutInfo )
        {
            syslog(LOG_ERR,"%s function %d memory malloc failed.\n",__func__,__LINE__);
            return FAILURE;
        }

        memset(result->throughoutInfo,0,num_rows*sizeof(throughout_info));
        result->num_columns = num_fields;
        result->num_rows = num_rows;
        result->cur_row = 0;
    }

    if(result->throughoutInfo != NULL && result->cur_row < result->num_rows )
    {
        while(( row = mysql_fetch_row(resultSet) ) != NULL)
        {
            //i = atoi(row[1]);
            printf("%d\n",result->cur_row);
            if(row[1])
            {
                result->throughoutInfo[result->cur_row].uplink_packet_num = atol(row[1]);
            }
            else
            {
                result->throughoutInfo[result->cur_row].uplink_packet_num = 0L;
            }
            if(row[2])
            {
                result->throughoutInfo[result->cur_row].downlink_packet_num = atol(row[2]);
            }
            else
            {
                result->throughoutInfo[result->cur_row].downlink_packet_num = 0L;
            }
            if(row[3])
            {
                result->throughoutInfo[result->cur_row].uplink_byte_num = atol(row[3]);
            }
            else
            {
                result->throughoutInfo[result->cur_row].uplink_byte_num = 0L;
            }
            if(row[4])
            {
                result->throughoutInfo[result->cur_row].downlink_byte_num = atol(row[4]);
            }
            else
            {
                result->throughoutInfo[result->cur_row].downlink_byte_num = 0L;
            }
            if(row[5])
            {
                strcpy(result->throughoutInfo[result->cur_row].collect_timestamp,row[5]);
            }
            else
            {
                strcpy(result->throughoutInfo[result->cur_row].collect_timestamp,"");
            }
            printf("Field[1]:%ld\t",result->throughoutInfo[result->cur_row].uplink_packet_num);
            printf("Field[2]:%ld\t",result->throughoutInfo[result->cur_row].downlink_packet_num);
            printf("Field[3]:%ld\t",result->throughoutInfo[result->cur_row].uplink_byte_num);
            printf("Field[4]:%ld\n",result->throughoutInfo[result->cur_row].downlink_byte_num);
            printf("Filed[5]:%s\n",result->throughoutInfo[result->cur_row].collect_timestamp);
            result->cur_row++;
        }
        return SUCCESS;
    }
   // result = (void*)ap;
   // free(ap);
    return FAILURE;
}

/*
 * 处理数据库表user_detailinfo_db的回调函数
 * param:arg,对应的返回类型，是函数的输出变量
 * param:resultSet,对应查询表的结果集
 * param:num_fields,对应查询表的字段数
 * param:num_rows,查询结果的数据库记录行数
 * return:返回值，0为成功，-1为失败
 */
int call_back_get_user_detail_info(void* arg,MYSQL_RES* resultSet,int num_fields,int num_rows)
{
    printf("****************************%s In**************************\n",__func__);
    query_user_detailinfo* result = (query_user_detailinfo*)arg;
   // ap_info** ap = (ap_info**)malloc(num_rows*sizeof(ap_info*));
   /* if( ap == NULL )
    {
        printf("struct ap_info** malloced memory failed\n");
        return FAILURE;
    }
    */

   // int cur_row = 0;
    MYSQL_ROW row;
    if( result == NULL )
    {
        printf("%s function %d line bad input parameter.\n",__func__,__LINE__);
        syslog(LOG_ERR,"%s function %d bad input parameter.\n",__func__,__LINE__);
        return FAILURE;
    }
    printf("num_fields:%d,num_rows:%d\n",num_fields,num_rows);
    if(result && (result->userInfo == NULL) && num_fields > 0 && num_rows > 0)
    {
        
       // printf("%s In\n",__func__);
        result->userInfo = (user_detailinfo*)malloc(num_rows*sizeof(user_detailinfo));
        if( NULL == result->userInfo )
        {
            syslog(LOG_ERR,"%s function %d memory malloc failed.\n",__func__,__LINE__);
            return FAILURE;
        }

        memset(result->userInfo,0,num_rows*sizeof(user_detailinfo));
        result->num_columns = num_fields;
        result->num_rows = num_rows;
        result->cur_row = 0;
    }

    if(result->userInfo != NULL && result->cur_row < result->num_rows )
    {
        while(( row = mysql_fetch_row(resultSet) ) != NULL)
        {
            //i = atoi(row[1]);
            printf("%d\n",result->cur_row);
            if(row[1])
            {
                strcpy(result->userInfo[result->cur_row].mac_address,row[1]);
            }
            else
            {
                strcpy(result->userInfo[result->cur_row].mac_address,"");
            }
            if(row[2])
            {
                strcpy(result->userInfo[result->cur_row].mac_alias,row[2]);
            }
            else
            {
                strcpy(result->userInfo[result->cur_row].mac_alias,"");
            }
            if(row[3])
            {
                strcpy(result->userInfo[result->cur_row].hostname,row[3]);
            }
            else
            {
                strcpy(result->userInfo[result->cur_row].hostname,"");
            }
            if(row[4])
            {
                strcpy(result->userInfo[result->cur_row].ip_address,row[4]);
            } 
            else
            {
                strcpy(result->userInfo[result->cur_row].ip_address,"");
            }
            if(row[5])
            {
                strcpy(result->userInfo[result->cur_row].access_point,row[5]);
            }
            else
            {
                strcpy(result->userInfo[result->cur_row].access_point,"");
            }
            if(row[6])
            {
                result->userInfo[result->cur_row].running_time = atof(row[6]);
            }
            else
            {
                result->userInfo[result->cur_row].running_time = 0.0;
            }
            if(row[7])
            {
                result->userInfo[result->cur_row].uplink_packet_num = atol(row[7]);
            }
            else
            {
                result->userInfo[result->cur_row].uplink_packet_num = 0L;
            }
            if(row[8])
            {
                result->userInfo[result->cur_row].downlink_packet_num = atol(row[8]);
            }
            else
            {
                result->userInfo[result->cur_row].downlink_packet_num = 0L;
            }
            if(row[9])
            {
                result->userInfo[result->cur_row].uplink_byte_num = atol(row[9]);
            }
            else
            {
                result->userInfo[result->cur_row].uplink_byte_num = 0L;
            }
            if(row[10])
            {
                result->userInfo[result->cur_row].downlink_byte_num = atol(row[10]);
            }
            else
            { 
                result->userInfo[result->cur_row].downlink_byte_num = 0L;
            }
            if(row[11])
            {
                strcpy(result->userInfo[result->cur_row].collect_timestamp,row[11]);
            }
            else
            {
                strcpy(result->userInfo[result->cur_row].collect_timestamp,"");
            }

            printf("Field[1]:%s\t",result->userInfo[result->cur_row].mac_address);
            printf("Field[2]:%s\t",result->userInfo[result->cur_row].mac_alias);
            printf("Field[3]:%s\t",result->userInfo[result->cur_row].hostname);
            printf("Field[4]:%s\t",result->userInfo[result->cur_row].ip_address);
            printf("Field[5]:%s\n",result->userInfo[result->cur_row].access_point);
            printf("Field[6]:%f\t",result->userInfo[result->cur_row].running_time);
            printf("Field[7]:%ld\t",result->userInfo[result->cur_row].uplink_packet_num);
            printf("Field[8]:%ld\t",result->userInfo[result->cur_row].downlink_packet_num);
            printf("Field[9]:%ld\t",result->userInfo[result->cur_row].uplink_byte_num);
            printf("Field[10]:%ld\n",result->userInfo[result->cur_row].downlink_byte_num);
            printf("Field[11]:%s\n",result->userInfo[result->cur_row].collect_timestamp);
            result->cur_row++;
        }

        printf("****************************%s Out**************************\n",__func__);
        return SUCCESS;
    }
   // result = (void*)ap;
   // free(ap);
    return FAILURE;
}
/*
 * 处理数据库表user_index_info的回调函数
 * param:arg,对应的返回类型，是函数的输出变量
 * param:resultSet,对应查询表的结果集
 * param:num_fields,对应查询表的字段数
 * param:num_rows,查询结果的数据库记录行数
 * return:返回值，0为成功，-1为失败
 */
int call_back_get_user_index_info(void* arg,MYSQL_RES* resultSet,int num_fields,int num_rows)
{
    printf("*********************************%s In*************************\n",__func__);
    query_user_info* result = (query_user_info*)arg;
   // ap_info** ap = (ap_info**)malloc(num_rows*sizeof(ap_info*));
   /* if( ap == NULL )
    {
        printf("struct ap_info** malloced memory failed\n");
        return FAILURE;
    }
    */

   // int cur_row = 0;
    MYSQL_ROW row;
    if( result == NULL )
    {
        printf("%s function %d line bad input parameter.\n",__func__,__LINE__);
        syslog(LOG_ERR,"%s function %d bad input parameter.\n",__func__,__LINE__);
        return FAILURE;
    }
    printf("num_fields:%d,num_rows:%d\n",num_fields,num_rows);
    if(result && (result->userInfo == NULL) && num_fields > 0 && num_rows > 0)
    {
        
        //printf("%s In\n",__func__);
        result->userInfo = (user_info*)malloc(num_rows*sizeof(user_info));
        if( NULL == result->userInfo )
        {
            syslog(LOG_ERR,"%s function %d memory malloc failed.\n",__func__,__LINE__);
            return FAILURE;
        }

        memset(result->userInfo,0,num_rows*sizeof(user_info));
        result->num_columns = num_fields;
        result->num_rows = num_rows;
        result->cur_row = 0;
    }

    if(result->userInfo != NULL && result->cur_row < result->num_rows )
    {
        while(( row = mysql_fetch_row(resultSet) ) != NULL)
        {
            //i = atoi(row[1]);
            printf("%d\n",result->cur_row);
            if(row[1])
            {
                result->userInfo[result->cur_row].signal_intensity = atoi(row[1]);
            }
            else
            {
                result->userInfo[result->cur_row].signal_intensity = 0;
            }
            if(row[2])
            {
                result->userInfo[result->cur_row].consultation_rate = atoi(row[2]);
            }
            else
            {
                result->userInfo[result->cur_row].consultation_rate = 0;
            }
            if(row[3])
            {
                result->userInfo[result->cur_row].access_rate = atoi(row[3]);
            }
            else
            {
                result->userInfo[result->cur_row].access_rate = 0;
            }
            if(row[4])
            {
                result->userInfo[result->cur_row].WL_retransmission_rate = atoi(row[4]);
            }
            else
            {
                result->userInfo[result->cur_row].WL_retransmission_rate = 0;
            }
            if(row[5])
            {
                strcpy(result->userInfo[result->cur_row].collect_timestamp,row[5]);
            }
            else
            {
                strcpy(result->userInfo[result->cur_row].collect_timestamp,"");
            }
            printf("Field[1]:%d\t",result->userInfo[result->cur_row].signal_intensity);
            printf("Field[2]:%d\t",result->userInfo[result->cur_row].consultation_rate);
            printf("Field[3]:%d\t",result->userInfo[result->cur_row].access_rate);
            printf("Field[4]:%d\n",result->userInfo[result->cur_row].WL_retransmission_rate);
            printf("Field[5]:%s\n",result->userInfo[result->cur_row].collect_timestamp);
            result->cur_row++;
        }
        return SUCCESS;
    }
   // result = (void*)ap;
   // free(ap);
    return FAILURE;
}


/*
 * 处理数据库表wifi_index_info的回调函数
 *param:arg,输出参数，用来存储对应类型的结果。
 *param:resultSet:输入参数,相关表的查询的结果集
 *param:num_fields,对应数据库表格的字段数目
 *param:num_rows,查询结果集包含的记录行数
 *param:return,返回值int，0为成功，-1失败
 */
int call_back_get_wifi_index_info(void* arg,MYSQL_RES* resultSet,int num_fields,int num_rows)
{
    printf("*********************************%s In*************************\n",__func__);
    query_wifi_info* result = (query_wifi_info*)arg;
   // ap_info** ap = (ap_info**)malloc(num_rows*sizeof(ap_info*));
   /* if( ap == NULL )
    {
        printf("struct ap_info** malloced memory failed\n");
        return FAILURE;
    }
    */

   // int cur_row = 0;
    MYSQL_ROW row;//each row of table's record
    if( result == NULL )
    {
        printf("%s function %d line bad input parameter.\n",__func__,__LINE__);
        syslog(LOG_ERR,"%s function %d bad input parameter.\n",__func__,__LINE__);
        return FAILURE;
    }
    printf("num_fields:%d,num_rows:%d\n",num_fields,num_rows);
    if(result && (result->wifiInfo == NULL) && num_fields > 0 && num_rows > 0)
    {
        
        printf("%s In\n",__func__);
        result->wifiInfo = (wifi_info*)malloc(num_rows*sizeof(wifi_info));
        if( NULL == result->wifiInfo )
        {
            syslog(LOG_ERR,"%s function %d memory malloc failed.\n",__func__,__LINE__);
            return FAILURE;
        }

        memset(result->wifiInfo,0,num_rows*sizeof(wifi_info));
        result->num_columns = num_fields;
        result->num_rows = num_rows;
        result->cur_row = 0;
    }

    if(result->wifiInfo != NULL && result->cur_row < result->num_rows )
    {
        while(( row = mysql_fetch_row(resultSet) ) != NULL)
        {
            //i = atoi(row[1]);
            printf("%d\n",result->cur_row);
            if(row[1])
            {
                result->wifiInfo[result->cur_row].signal_noise_rate = atoi(row[1]);
            }
            else
            {
                result->wifiInfo[result->cur_row].signal_noise_rate = 0;
            }
            if(row[2])
            {
                result->wifiInfo[result->cur_row].channel_utilization = atoi(row[2]);
            }
            else
            {
                result->wifiInfo[result->cur_row].channel_utilization = 0;
            }
            if(row[3])
            {
                result->wifiInfo[result->cur_row].signal_interference = atoi(row[3]);
            }
            else
            {
                result->wifiInfo[result->cur_row].signal_interference = 0;
            }
            if(row[4])
            {
                strcpy(result->wifiInfo[result->cur_row].collect_timestamp,row[4]); 
            }
            else
            {
                strcpy(result->wifiInfo[result->cur_row].collect_timestamp,""); 
            }
            printf("Field[1]:%d\t",result->wifiInfo[result->cur_row].signal_noise_rate);
            printf("Field[2]:%d\t",result->wifiInfo[result->cur_row].channel_utilization);
            printf("Field[3]:%d\t",result->wifiInfo[result->cur_row].signal_interference);
            printf("Field[4]:%s\n",result->wifiInfo[result->cur_row].collect_timestamp);
            result->cur_row++;
        }
        return SUCCESS;
    }
   // result = (void*)ap;
   // free(ap);
    return FAILURE;
}


/*
 * 处理数据库表wireless_index_info的回调函数
 * param:arg,对应的返回类型，是函数的输出变量
 * param:resultSet,对应查询表的结果集
 * param:num_fields,对应查询表的字段数
 * param:num_rows,查询结果的数据库记录行数
 * return:返回值，0为成功，-1为失败
 */
int call_back_get_wireless_index_info(void* arg,MYSQL_RES* resultSet,int num_fields,int num_rows)
{
    printf("*********************************%s In*************************\n",__func__);
    query_wireless_info* result = (query_wireless_info*)arg;
   // ap_info** ap = (ap_info**)malloc(num_rows*sizeof(ap_info*));
   /* if( ap == NULL )
    {
        printf("struct ap_info** malloced memory failed\n");
        return FAILURE;
    }
    */

   // int cur_row = 0;
    MYSQL_ROW row;//each row's record of table
    if( result == NULL )
    {
        printf("%s function %d line bad input parameter.\n",__func__,__LINE__);
        syslog(LOG_ERR,"%s function %d bad input parameter.\n",__func__,__LINE__);
        return FAILURE;
    }
    printf("num_fields:%d,num_rows:%d\n",num_fields,num_rows);
    if(result && (result->wirelessInfo == NULL) && num_fields > 0 && num_rows > 0)
    {
        
        printf("%s In\n",__func__);
        result->wirelessInfo = (wireless_info*)malloc(num_rows*sizeof(wireless_info));
        if( NULL == result->wirelessInfo )
        {
            syslog(LOG_ERR,"%s function %d memory malloc failed.\n",__func__,__LINE__);
            return FAILURE;
        }

        memset(result->wirelessInfo,0,num_rows*sizeof(wireless_info));
        result->num_columns = num_fields;
        result->num_rows = num_rows;
        result->cur_row = 0;
    }

    if(result->wirelessInfo != NULL && result->cur_row < result->num_rows )
    {
        while(( row = mysql_fetch_row(resultSet) ) != NULL)
        {
            //i = atoi(row[1]);
            printf("%d\n",result->cur_row);
            if(row[1])
            {
                strcpy(result->wirelessInfo[result->cur_row].essid,row[1]);
            }
            else
            {
                strcpy(result->wirelessInfo[result->cur_row].essid,"");
            }
            if(row[2])
            {
                strcpy(result->wirelessInfo[result->cur_row].AFi_n,row[2]);
            }
            else
            {
                strcpy(result->wirelessInfo[result->cur_row].AFi_n,"");
            }
            if(row[3])
            {
                result->wirelessInfo[result->cur_row].online_user_count = atoi(row[3]);
            }
            else
            {
                result->wirelessInfo[result->cur_row].online_user_count = 0;
            }
            if(row[4])
            {
                result->wirelessInfo[result->cur_row].uplink_packet_num = atol(row[4]);
            }
            else
            {
                result->wirelessInfo[result->cur_row].uplink_packet_num = 0L;
            }
            if(row[5])
            {
                result->wirelessInfo[result->cur_row].downlink_packet_num = atol(row[5]);
            }
            else
            {
                result->wirelessInfo[result->cur_row].downlink_packet_num = 0L;
            }
            if(row[6])
            {
                result->wirelessInfo[result->cur_row].uplink_byte_num = atol(row[6]);
            }
            else
            {
                result->wirelessInfo[result->cur_row].uplink_byte_num = 0L;
            }
            if(row[7])
            {
                result->wirelessInfo[result->cur_row].downlink_byte_num = atol(row[7]);
            }
            else
            {
                result->wirelessInfo[result->cur_row].downlink_byte_num = 0L;
            }
            if(row[8])
            {
                strcpy(result->wirelessInfo[result->cur_row].collect_timestamp,row[8]);
            }
            else
            {
                strcpy(result->wirelessInfo[result->cur_row].collect_timestamp,"");
            }
            printf("Field[1]:%s\t",result->wirelessInfo[result->cur_row].essid);      
            printf("Field[2]:%s\t",result->wirelessInfo[result->cur_row].AFi_n);
            printf("Field[3]:%d\n",result->wirelessInfo[result->cur_row].online_user_count);
            printf("Field[4]:%ld\t",result->wirelessInfo[result->cur_row].uplink_packet_num);
            printf("Field[5]:%ld\t",result->wirelessInfo[result->cur_row].downlink_packet_num);
            printf("Field[6]:%ld\t",result->wirelessInfo[result->cur_row].uplink_byte_num);
            printf("Field[7]:%ld\t",result->wirelessInfo[result->cur_row].downlink_byte_num);
            printf("Field[8]:%s\n",result->wirelessInfo[result->cur_row].collect_timestamp);
            result->cur_row++;
        }
        return SUCCESS;
    }
   // result = (void*)ap;
   // free(ap);
    return FAILURE;
}


/*
 * 查询数据库表ap_index_info的接口，即用户调用的入口。
 *param:query_info,类型为结构体query_ap_info，输出对应的结果集 
 *return:FAILURE -1,0 means SUCCESS
 */
int db_get_ap_index_info(query_ap_info* query_info)
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_AP_INDEX_INFO_TABLE);

    ret = db_do_query_2(&mysql,queryStmt,call_back_get_ap_index_info,query_info);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }

    return FAILURE;
}

/*
 * 查询数据库表afi_detailinfo_db的接口，即用户调用的入口。
 *param:query_info,类型为结构体query_afi_info，输出对应的结果集 
 *return:FAILURE -1,0 means SUCCESS
 */
int db_get_afi_info(query_afi_info* query_info)
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_AFI_DETAILINFO_DB_TABLE);
    ret = db_do_query_2(&mysql,queryStmt,call_back_get_afi_info,query_info);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 * 查询数据库表sys_health_score_info的接口，即用户调用的入口。
 *param:query_info,类型为结构体query_sys_info，输出对应的结果集 
 *return:FAILURE -1,0 means SUCCESS
 */
int db_get_sys_score_info(query_sys_info* query_info)
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_SYS_HEALTH_SCORE_TABLE);
    ret = db_do_query_2(&mysql,queryStmt,call_back_get_sys_score_info,query_info);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 * 查询数据库表throughout_index_info的接口，即用户调用的入口。
 *param:query_info,类型为结构体query_throughout_info，输出对应的结果集 
 *return:FAILURE -1,0 means SUCCESS
 */
int db_get_throughout_info(query_throughout_info* query_info)
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_THROUGHOUT_INDEX_INFO_TABLE);
    ret = db_do_query_2(&mysql,queryStmt,call_back_get_throughout_info,query_info);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 * 查询数据库表user_detailinfo_db的接口，即用户调用的入口。
 *param:query_info,类型为结构体query_user_detailinfo，输出对应的结果集 
 *return:FAILURE -1,0 means SUCCESS
 */
int db_get_user_detail_info(query_user_detailinfo* query_info)
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_USER_DETAILINFO_DB_TABLE);
    ret = db_do_query_2(&mysql,queryStmt,call_back_get_user_detail_info,query_info);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 * 查询数据库表user_index_info的接口，即用户调用的入口。
 *param:query_info,类型为结构体query_user_info，输出对应的结果集 
 *return:FAILURE -1,0 means SUCCESS
 */
int db_get_user_index_info(query_user_info* query_info)
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_USER_INDEX_INFO_TABLE);
    ret = db_do_query_2(&mysql,queryStmt,call_back_get_user_index_info,query_info);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}


/*
 * 查询数据库表wifi_index_info的接口，即用户调用的入口。
 *param:query_info,类型为结构体query_wifi_info，输出对应的结果集 
 *return:FAILURE -1,0 means SUCCESS
 */
int db_get_wifi_index_info(query_wifi_info* query_info)
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_WIFI_INDEX_INFO_TABLE);
    ret = db_do_query_2(&mysql,queryStmt,call_back_get_wifi_index_info,query_info);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 * 查询数据库表wireless_index_info的接口，即用户调用的入口。
 *param:query_info,类型为结构体query_wireless_info，输出对应的结果集 
 *return:FAILURE -1,0 means SUCCESS
 */
int db_get_wireless_index_info(query_wireless_info* query_info)
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_WIRELESS_INDEX_INFO_TABLE);
    ret = db_do_query_2(&mysql,queryStmt,call_back_get_wireless_index_info,query_info);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 * query every table'info ,eg. select * from tablename;
 *
 */
/*int db_get_table_info()
{
    MYSQL mysql;
    char* prepStmt = "select * from %s;";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_AP_INDEX_INFO_TABLE);

    ret = db_do_query(&mysql,queryStmt,call_back_print_query_info,NULL);
    
    db_put_database_connection(&mysql);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}
*/
/*
 *数据库表ap_index_info中增加一条记录
 *param:ap,类型为结构体ap_info,表示输入表对应的结构体类型数据
 *return:0 means success,-1 means failure
 */
int db_add_ap_index_info(ap_info* ap)
{
    MYSQL mysql;
    char* prepStmt = "insert into %s values(NULL,%d,%d,%d,'%s');";
    char* queryStmt = (char*)malloc(2048);
    int ret = 0;
    
    if( NULL == ap || NULL == queryStmt )
    {
        if(queryStmt)
        {
            free(queryStmt);
        }
        return FAILURE;
    }
    // mysql_init(mysql);
    //连接数据库
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    //填充数据库插入语句
    sprintf(queryStmt,prepStmt,SOFTAC_AP_INDEX_INFO_TABLE,ap->signal_intensity,ap->down_up_BW,ap->time_delay,ap->collect_timestamp);
    //执行操作
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    free(queryStmt);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *数据库表afi_detailinfo_db中增加一条记录
 *param:afi,类型为结构体afi_info,表示输入表对应的结构体类型数据
 *return:0 means success,-1 means failure
 */
int db_add_afi_info(afi_info* afi)
{
    MYSQL mysql;
    char* prepStmt = "insert into %s values(NULL,'%s','%s','%s','%s','%s',%f,%ld,%ld,%ld,%ld,'%s');";
    char* queryStmt = (char*)malloc(2048);
    int ret = 0;
    
    if( NULL == afi || NULL == queryStmt )
    {
        if(queryStmt)
        {
            free(queryStmt);
        }
        return FAILURE;
    }
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_AFI_DETAILINFO_DB_TABLE,afi->mac_address,afi->afi_name,afi->afi_type,afi->afi_version,afi->afi_ip_address,afi->running_time,afi->uplink_packet_num,afi->downlink_packet_num,afi->uplink_byte_num,afi->downlink_byte_num,afi->collect_timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    free(queryStmt);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *数据库表sys_health_score_info中增加一条记录
 *param:sys,类型为结构体sys_score,表示输入表对应的结构体类型数据
 *return:0 means success,-1 means failure
 */
int db_add_sys_score_index_info(sys_score* sys)
{
    MYSQL mysql;
    char* prepStmt = "insert into %s values(NULL,%d,%d,%ld,%ld,%d,%d,%d,%d,'%s');";
    
    char* queryStmt = (char*)malloc(2048);
    int ret = 0;
    
    if( NULL == sys || NULL == queryStmt )
    {
        if(queryStmt)
        {
            free(queryStmt);
        }
        return FAILURE;
    }
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_SYS_HEALTH_SCORE_TABLE,sys->health_score,sys->signal_interference,sys->downlink_byte_num,sys->uplink_byte_num,sys->cover_area,sys->signal_intensity,sys->user_access_rate,sys->online_user_num,sys->collect_timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    free(queryStmt);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *数据库表throughout_index_info中增加一条记录
 *param:throughout,类型为结构体throughout_info,表示输入表对应的结构体类型数据
 *return:0 means success,-1 means failure
 */
int db_add_throughout_index_info(throughout_info* throughout)
{
    MYSQL mysql;
    char* prepStmt = "insert into %s values(NULL,%ld,%ld,%ld,%ld,'%s');";
    
    char* queryStmt = (char*)malloc(2048);
    int ret = 0;
    
    if( NULL == throughout || NULL == queryStmt )
    {
        if(queryStmt)
        {
            free(queryStmt);
        }
        return FAILURE;
    }
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_THROUGHOUT_INDEX_INFO_TABLE,throughout->uplink_packet_num,throughout->downlink_packet_num,throughout->uplink_byte_num,throughout->downlink_byte_num,throughout->collect_timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    free(queryStmt);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *数据库表user_detailinfo_db中增加一条记录
 *param:userinfo,类型为结构体user_detailinfo,表示输入表对应的结构体类型数据
 *return:0 means success,-1 means failure
 */
int db_add_user_detail_info(user_detailinfo* userinfo)
{
    MYSQL mysql;
    char* prepStmt = "insert into %s values(NULL,'%s','%s','%s','%s','%s',%f,%ld,%ld,%ld,%ld,'%s');";
    
    char* queryStmt = (char*)malloc(2048);
    int ret = 0;
    
    if( NULL == userinfo || NULL == queryStmt )
    {
        if(queryStmt)
        {
            free(queryStmt);
        }
        return FAILURE;
    }
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_USER_DETAILINFO_DB_TABLE,userinfo->mac_address,userinfo->mac_alias,userinfo->hostname,userinfo->ip_address,userinfo->access_point,userinfo->running_time,userinfo->uplink_packet_num,userinfo->downlink_packet_num,userinfo->uplink_byte_num,userinfo->downlink_byte_num,userinfo->collect_timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    free(queryStmt);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *数据库表user_index_info中增加一条记录
 *param:userinfo,类型为结构体user_info,表示输入表对应的结构体类型数据
 *return:0 means success,-1 means failure
 */
int db_add_user_index_info(user_info* userinfo)
{
    MYSQL mysql;
    char* prepStmt = "insert into %s values(NULL,%d,%d,%d,%d,'%s');";
    
    char* queryStmt = (char*)malloc(2048);
    int ret = 0;
    
    if( NULL == userinfo || NULL == queryStmt )
    {
        if(queryStmt)
        {
            free(queryStmt);
        }
        return FAILURE;
    }
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_USER_INDEX_INFO_TABLE,userinfo->signal_intensity,userinfo->consultation_rate,userinfo->access_rate,userinfo->WL_retransmission_rate,userinfo->collect_timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    free(queryStmt);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *数据库表wifi_index_info中增加一条记录
 *param:wifi,类型为结构体wifi_info,表示输入表对应的结构体类型数据
 *return:0 means success,-1 means failure
 */
int db_add_wifi_index_info(wifi_info* wifi)
{
    MYSQL mysql;
    char* prepStmt = "insert into %s values(NULL,%d,%d,%d,'%s');";
    
    char* queryStmt = (char*)malloc(2048);
    int ret = 0;
    
    if( NULL == wifi || NULL == queryStmt )
    {
        if(queryStmt)
        {
            free(queryStmt);
        }
        return FAILURE;
    }
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_WIFI_INDEX_INFO_TABLE,wifi->signal_noise_rate,wifi->channel_utilization,wifi->signal_interference,wifi->collect_timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    free(queryStmt);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *数据库表wireless_index_info中增加一条记录
 *param:wireless,类型为结构体wireless_info,表示输入表对应的结构体类型数据
 *return:0 means success,-1 means failure
 */
int db_add_wireless_index_info(wireless_info* wireless)
{
    MYSQL mysql;
    char* prepStmt = "insert into %s values(NULL,'%s','%s',%d,%ld,%ld,%ld,%ld,'%s');";
    
    char* queryStmt = (char*)malloc(2048);
    int ret = 0;
    
    if( NULL == wireless || NULL == queryStmt )
    {
        if(queryStmt)
        {
            free(queryStmt);
        }
        return FAILURE;
    }
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_WIRELESS_INDEX_INFO_TABLE,wireless->essid,wireless->AFi_n,wireless->online_user_count,wireless->uplink_packet_num,wireless->downlink_packet_num,wireless->uplink_byte_num,wireless->downlink_byte_num,wireless->collect_timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    free(queryStmt);
    if(ret > 0)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *删除表格ap_index_info中的时间点timestamp之前的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_ap_index_info_before_time(char* timestamp)
{
    
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp < '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_AP_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *删除表格ap_index_info中的时间点timestamp之后的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_ap_index_info_after_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp > '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_AP_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *删除表格sys_health_score_info中的时间点timestamp之前的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_sys_score_info_before_time(char* timestamp)
{
    
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp < '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_SYS_HEALTH_SCORE_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *删除表格sys_health_score_info中的时间点timestamp之后的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_sys_score_info_after_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp > '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_SYS_HEALTH_SCORE_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *删除表格throughout_index_info中的时间点timestamp之前的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_throughout_index_info_before_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp < '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_THROUGHOUT_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *删除表格throughout_index_info中的时间点timestamp之后的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_throughout_index_info_after_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp > '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_THROUGHOUT_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *删除表格user_index_info中的时间点timestamp之前的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_user_index_info_before_time(char* timestamp)
{
    
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp < '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_USER_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *删除表格user_index_info中的时间点timestamp之后的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_user_index_info_after_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp > '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_USER_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *删除表格wifi_index_info中的时间点timestamp之前的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_wifi_index_info_before_time(char* timestamp)
{
    
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp < '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_WIFI_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *删除表格wifi_index_info中的时间点timestamp之后的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_wifi_index_info_after_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp > '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_WIFI_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *删除表格wireless_index_info中的时间点timestamp之前的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_wireless_index_info_before_time(char* timestamp)
{
    
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp < '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_WIRELESS_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *删除表格wireless_index_info中的时间点timestamp之后的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_wireless_index_info_after_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp > '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_WIRELESS_INDEX_INFO_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *删除表格afi_detailinfo中的时间点timestamp之前的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_afi_info_before_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp < '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_AFI_DETAILINFO_DB_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *删除表格afi_detailinfo中的时间点timestamp之后的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_afi_info_after_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp > '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_AFI_DETAILINFO_DB_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *删除表格user_detailinfo中的时间点timestamp之前的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_user_detail_info_before_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp < '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_USER_DETAILINFO_DB_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 *删除表格user_detailinfo中的时间点timestamp之后的记录
 *param:timestamp格式化的时间字符串，输入参数，也是过滤条件
 *return:0 means success,-1 means failure
 */
int db_delete_user_detail_info_after_time(char* timestamp)
{
    MYSQL mysql;
    char* prepStmt = "delete from %s where collect_timestamp > '%s';";
    char queryStmt[256] = {0};
    int ret = 0;
    
    // mysql_init(mysql);
    db_get_database_connection(&mysql,SOFTAC_DATABASE_HOST,SOFTAC_DATABASE_USER,SOFTAC_DATABASE_PASSWD,SOFTAC_DATABASE_NAME);
    sprintf(queryStmt,prepStmt,SOFTAC_USER_DETAILINFO_DB_TABLE,timestamp);
    ret = db_do_query_2(&mysql,queryStmt,NULL,NULL);
    
    db_put_database_connection(&mysql);
    if( ret > 0 )
    {
        return SUCCESS;
    }
    return FAILURE;
}


#ifdef TEST_MAIN
#if TEST_MAIN 
int main()
{
    //MYSQL* mysql = NULL;
    //ap_info** info = NULL;
    /*********************************************/
    /************* Just test Format Time *********/
    time_t timep;
    struct tm* p;
    time(&timep);
    p = localtime(&timep);
    char format_time[20];
   // get_db_format_time_from_tm(p,format_time);
    get_db_format_time_str_from_tm(p,format_time);
    printf("Format Time is %s\n",format_time);
    struct tm tmpTime;
    get_db_tm_from_format_time("2014-12-24 12:00:00",&tmpTime);
    printf("from format_time_str to tm_month is %d\n",tmpTime.tm_mon);
    /*********************************************/
    /************ap_index_info********************/
    ap_info ap;
    ap.signal_intensity = 200;
    ap.down_up_BW = 1024;
    ap.time_delay = 69;
    strcpy(ap.collect_timestamp,format_time);
    query_ap_info queryApInfo;
    memset(&queryApInfo,0,sizeof(queryApInfo));
    db_add_ap_index_info(&ap);
    db_delete_ap_index_info_before_time(format_time);
    db_get_ap_index_info(&queryApInfo);

   // printf("rows:%d\n",queryApInfo.num_rows);
    /*********************************************/
    /************afi_info*************************/
    afi_info afi;
    strcpy(afi.mac_address,"1e:bc:ed:55:9a:10");
    strcpy(afi.afi_name,"mina_afi");
    strcpy(afi.afi_type,"afi");
    strcpy(afi.afi_version,"1.0.0.1");
    strcpy(afi.afi_ip_address,"127.0.0.1");
    afi.running_time = 1.29;
    afi.uplink_packet_num = 212345;
    afi.downlink_packet_num = 22345;
    afi.uplink_byte_num = 0;
    afi.downlink_byte_num = 0;
    strcpy(afi.collect_timestamp,format_time);
    query_afi_info queryAfiInfo;
    memset(&queryAfiInfo,0,sizeof(queryAfiInfo));

    db_add_afi_info(&afi); 
    db_delete_afi_info_before_time(format_time);
    db_get_afi_info(&queryAfiInfo);
    printf("rows:%d\n",queryAfiInfo.num_rows);
    /********************************************/
    /***********sys_health_score*****************/
    sys_score sys;
    sys.health_score = 99;
    sys.signal_interference = -23;
    sys.downlink_byte_num = 23435617;
    sys.uplink_byte_num = 123456789;
    sys.cover_area = 150;
    sys.signal_intensity = -89;
    sys.user_access_rate = 100;
    sys.online_user_num = 1204;
    strcpy(sys.collect_timestamp,format_time);
    query_sys_info querySysInfo;
    memset(&querySysInfo,0,sizeof(querySysInfo));
    db_add_sys_score_index_info(&sys); 
    db_delete_sys_score_info_before_time(format_time);
    db_get_sys_score_info(&querySysInfo);
    printf("rows:%d\n",querySysInfo.num_rows);
    /*******************************************/
    /***********throughout_index_info***********/
    throughout_info throughout;
    throughout.uplink_packet_num = 567;
    throughout.downlink_packet_num = 567;
    throughout.uplink_byte_num = 234;
    throughout.downlink_byte_num = 21;
    strcpy(throughout.collect_timestamp,format_time);
    query_throughout_info queryThroughoutInfo;
    memset(&queryThroughoutInfo,0,sizeof(queryThroughoutInfo));

    db_add_throughout_index_info(&throughout);
    db_delete_throughout_index_info_before_time(format_time);
    db_get_throughout_info(&queryThroughoutInfo);
    printf("rows:%d\n",queryThroughoutInfo.num_rows);
    /*******************************************/
   
    /***********user_detailinfo_db**************/
    user_detailinfo user;
    strcpy(user.mac_address,"11:3e:fe:4e:12:45");
    strcpy(user.mac_alias,"himgod");
    strcpy(user.hostname,"ubuntu");
    strcpy(user.ip_address,"12.0.0.1");
    strcpy(user.access_point,"mina");
    user.running_time = 1024;
    user.uplink_packet_num = 1267;
    user.downlink_packet_num = 127;
    user.uplink_byte_num = 1234;
    user.downlink_byte_num = 2345;
    strcpy(user.collect_timestamp,format_time);

    query_user_detailinfo queryUserDetailInfo;
    memset(&queryUserDetailInfo,0,sizeof(queryUserDetailInfo));

    db_add_user_detail_info(&user);
    db_delete_user_detail_info_before_time(format_time);
    db_get_user_detail_info(&queryUserDetailInfo);
    printf("rows:%d\n",queryUserDetailInfo.num_rows);
    /*******************************************/
    /***********user_index_info*****************/
    user_info userinfo;
    userinfo.signal_intensity = -98;
    userinfo.consultation_rate = 1024;
    userinfo.access_rate = 101;
    userinfo.WL_retransmission_rate = 88;
    strcpy(userinfo.collect_timestamp,format_time);
    query_user_info queryUserInfo;
    memset(&queryUserInfo,0,sizeof(queryUserInfo));

    db_add_user_index_info(&userinfo);
    db_delete_user_index_info_before_time(format_time);
    db_get_user_index_info(&queryUserInfo);
    printf("rows:%d\n",queryUserInfo.num_rows);
    /*******************************************/
    /***********wireless_index_info*************/
    wifi_info wifiinfo;
    wifiinfo.signal_noise_rate = 121;
    wifiinfo.channel_utilization = 100;
    wifiinfo.signal_interference = -990;
    strcpy(wifiinfo.collect_timestamp,format_time);
    query_wifi_info queryWifiInfo;
    memset(&queryWifiInfo,0,sizeof(queryWifiInfo));
    
    db_add_wifi_index_info(&wifiinfo);
    db_delete_wifi_index_info_before_time(format_time);
    db_get_wifi_index_info(&queryWifiInfo);
    printf("rows:%d\n",queryWifiInfo.num_rows);
    /*******************************************/
    /***********wifi_index_info*****************/
    wireless_info wire;
    strcpy(wire.essid,"test");
    strcpy(wire.AFi_n,"1011");
    wire.online_user_count = 1024;
    wire.uplink_packet_num = 1234567;
    wire.downlink_packet_num = 1234567;
    wire.uplink_byte_num = 987651234;
    wire.downlink_byte_num = 0;
    strcpy(wire.collect_timestamp,format_time);

    query_wireless_info queryWirelessInfo;
    memset(&queryWirelessInfo,0,sizeof(queryWirelessInfo));
    db_add_wireless_index_info(&wire);
    db_delete_wireless_index_info_before_time(format_time);
    db_get_wireless_index_info(&queryWirelessInfo);
    printf("rows:%d\n",queryWirelessInfo.num_rows);
    
    return 0;
}

#endif
#endif

