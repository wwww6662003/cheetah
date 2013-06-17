<?php
session_start();
define('IN_GALAVY',true);//入口标识
define('GALAVY_DEBUG',true);//调试模式true打开,false关闭
define('GALAVY_TIMEZONE','Asia/ShangHai');//时区
header("Content-type:text/html;charset=UTF-8");//编码
define('ROOT',dirname(dirname(dirname(dirname(__FILE__)))));//根目录
define('APP_ROOT',dirname(dirname(dirname(__FILE__))));//应用根目录
define('APP_NAME','cheetah');//应用名字
define('APP_PATH',APP_ROOT.(APP_NAME?"/".APP_NAME:""));//应用目录
define('DAO_PATH',ROOT.'/lib/dao');//dao类库目录
define('VIEW_PATH',APP_PATH.'/template');//视图目录
define("LOG_PATH",APP_PATH."/log");//日志目录
include(APP_PATH.'/config/config.php');//应用配置文件
$app=new CheetahApplicationCore();
$conn=mysql_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);//database conn
if($conn){
$app->initConn($conn);
}else{
echo "link fail.";
}
$app->runWeb();
//print_r(parse_url('http://cheetah/test.php?c=index&a=index#tag'));
/*$request_uri=$_SERVER['REQUEST_URI'];
echo $path_info;
$router=new CheetahRouterCore();
$router->parseUrl();//$request_uri*/

//$app=new CheetahApplicationCore();
//$app->runWeb();
//var_dump($app->getAction());
//var_dump($app->getParam());

//$index=new TestController();
//$index->index();
//$index2=new IndexController();
//$index2->index();
//var_dump($router->toArray);



die();
/* D:\GTK\bin;E:\Program Files\SlickEditV14.0.1\win\;D:\Program Files\Microsoft Visual Studio\Common\Tools\WinNT;
D:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin;D:\Program Files\Microsoft Visual Studio\Common\Tools;
D:\Program Files\Microsoft Visual Studio\VC98\bin;D:\cplusplus\mingw\bin;D:\cplusplus\mingw\libexec\gcc\mingw32\3.4.5;
D:\cplusplus\mysql\include;D:\cplusplus\mysql\lib\opt;E:\phpcompiler\win32build\bin;D:\Program Files\phpStudy\PHP5;
D:\wwwphp\test\mvc\ZendFramework\bin;E:\Python25;E:\Python25\Scripts;E:\Python25\Lib\site-packages\django\bin;
D:\perl_python\python;E:\apachecompiler;E:\lex\bin;D:\CodeBlocks10\MinGW\bin;
D:\cplusplus\gtk+\bin;%JAVA_HOME%\bin;%JAVA_HOME%\jre\bin;F:\cygwin\bin;
F:\cygwin\usr\sbin;F:\cygwin\home\Administrator\hadoop-0.20.2\bin  */

if (!extension_loaded('cheetah')) {
   echo "loaded";
}
phpinfo();
?>