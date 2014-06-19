<?php

class OriRegform {
    const CLASSCODE = 'ori_regform';
    const CLASSROOT = '/classes';
    public static $country;
    
    public static function init($country = 'default', $attr = array()) {
        self::$country = $country; //country geocode
        $classcountry = (self::class_file_exists($country))? $country : 'default';
        $classname = self::CLASSCODE .'_'. ucfirst($classcountry) ; //class ori_client_Ukr
        
        // auto include classfile if it not included
        if (!class_exists($classname, false)) {
            OriRegform::autoload($classcountry);
        }

        return new $classname();
    }

    public static function autoload($country) {
        // filename of including class
        if (!self::class_file_exists($country))
            throw new Exception('Class file not found! ' . $filename);
        
        self::class_file_include($country);
        
        return true;
    }

    
    public static function class_file_exists($country){
        $classfile = self::CLASSCODE .'.'. strtolower($country); //ori_client.ukr.php
        $filename = drupal_get_path('module', self::CLASSCODE) . self::CLASSROOT . '/' . $classfile . '.php';
        return file_exists($filename);
        
    }
    
    public static function class_file_include($country){
        $classfile = self::CLASSCODE .'.'. strtolower($country); //ori_client.ukr.php
        $filename = drupal_get_path('module', self::CLASSCODE) . self::CLASSROOT . '/' . $classfile . '.php';
        include_once($filename);
        
    }
}



/**
 * Common regform country class
 */

class ori_regform_Base {

  public static function getSelect_country($arg = array()){
   $out = array();  
   
   $data  = OriClient::init()->get_country();
   if($data){
   foreach($data as $r){
       $out['result'][$r['geocode']] = $r['title'];     
       $out['phonecodes'][$r['geocode']] = $r['code']; 
      }
   }
   
   return $out; 
  }
  
}