<?php


class ori_regform_Default extends ori_regform_Base {

  public static function getSelect($object, $arg = array()){
      $method = __FUNCTION__ .'_'. strtolower($object);
      //нужно вставить проверку сущесв метода     
      return self::$method($arg);

  }

  public static function getSelect_state(){
   
   return OriClient::init()->get_state(OriRegform::$country);   
  }
 
  public static function getSelect_index($arg){
   // ключ поиска 
   $search = $arg['search']; 
    
   return OriClient::init()->get_postindex(OriRegform::$country, $search);
  }

  public static function getSelect_town($arg){
   // ключ поиска 
   $search = $arg['search']; 
   // ключ поиска 
   $state = $arg['state']; 
   
   return OriClient::init()->get_town(OriRegform::$country, $search, $state);  
  }
  
  public static function getSelect_district($arg){
   // ключ поиска 
   $search = $arg['search']; 
   // ключ поиска 
   $state = $arg['state']; 
   
   return OriClient::init()->get_district(OriRegform::$country, $search, $state); 
  }

  public static function getSelect_place($arg){
   // ключ поиска 
   $search = $arg['search']; 
   // область 
   $state = $arg['state']; 
   // ключ район 
   $area = $arg['area'];
   
   
  return OriClient::init()->get_place(OriRegform::$country, $search, $state, $area); 
  }
  
  
}

