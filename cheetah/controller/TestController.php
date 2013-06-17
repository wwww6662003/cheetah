<?php
class TestController extends CheetahControllerCore{
	public function index(){
		echo "<br/>TestController's index method!222";
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
		
		CheetahControllerCore::assignArr("test22",array("id"=>"test_valuefffff"));
		CheetahControllerCore::display("index");
		
	}
}
?>