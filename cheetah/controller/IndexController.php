<?php
class IndexController extends CheetahControllerCore{
	public function index(){
		echo "<br/>IndexController's index method!";
	}
	public function test(){
		//CheetahControllerCore::getParam();
		//echo "<br/>".CheetahControllerCore::getGet("id");
		//echo "<br/>".CheetahControllerCore::getPost("name22");
		//echo "<br/>".CheetahControllerCore::getRequest("name22");
		//CheetahControllerCore::assign("test","test_valuefffff");
		//CheetahControllerCore::assign("test2","test_value2");
		//CheetahControllerCore::assign("test3","test_value3");
		//CheetahControllerCore::display("index");
		echo "<br/>TestController's test method!3333";
		//echo CheetahViewCore::getVar("test");
		//CheetahControllerCore::assignArr("test22",array("id"=>"test_valuefffff"));
		//CheetahControllerCore::display("index");
		$conn=mysql_connect("localhost:3306","root","root");
		var_dump($conn);
		$model=new CheetahModelCore();
		$model->selectDb("cheetah",$conn);
		//mysql_select_db("cheetah",$conn);
		
	}
}
?>