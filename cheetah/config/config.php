<?php
/**
 * 应用配置文件
 */
/*
=== 数据库配置(DB) ===
*/
//数据库类型
define("DB_DIALECT","mysql");
//数据库服务器地址
define("DB_HOST","localhost:3306");
//数据库用户名
define("DB_USER","root");
//数据库密码
define("DB_PASS","root");
//数据库名称
define("DB_NAME","cheetah");
//通用数据库错误
define("DB_ERR","Database error!");
//无法连接数据库
define("DB_ERR_HOST","Unable to connect the host!");
//数据库不存在
define("DB_ERR_SCHEMA","Schema unavailable!");
//数据查询错误
define("DB_ERR_QUERY","SQL query error!");
//用户密码的密钥
define("PASSWORD_MD5_KEY","uccs)%!(");
?>