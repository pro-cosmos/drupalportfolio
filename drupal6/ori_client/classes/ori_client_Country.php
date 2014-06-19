<?php


class ori_client_Regform {

  public static function getList($object, $arg = array()){
      $method = 'getSel_'.strtolower($object);
      if(method_exists(self, $method))
        return self::$method($arg);      
  }
}

function getSelect_country($arg = array()){
  return array('0'=>'Россия','1'=>'Украина'); 
}


