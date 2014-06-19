<?php


class pms_userform_Default extends pms_userform_Base {

  public static function getSelect($object, $arg = array()){
      $method = __FUNCTION__ .'_'. strtolower($object);
      //нужно вставить проверку сущесв метода     
      return self::$method($arg);

  }

  public static function getSelect_state(){
  return State::init(PMS_userform::$country)->getList();
  }
 
  public static function getSelect_index($arg){
   // ключ поиска 
   $search = $arg['search']; 
   return OriGeo::init(PMS_userform::$country)->IndexAutocomplete($search);
  }

  public static function getSelect_town($arg){
   
   $search = $arg['search']; // ключ поиска 
   $state = $arg['state']; // ключ поиска 

   return Town::init(PMS_userform::$country)->autocomplete($state, $search);
  }
  
  public static function getSelect_district($arg){
   // ключ поиска 
   $search = $arg['search']; 
   // код области 
   $state = $arg['state']; 
   
   return OriGeo::init(PMS_userform::$country)->DistrictAutocomplete($state, $search);
  }

  public static function getSelect_place($arg){
   // ключ поиска 
   $search = $arg['search']; 
   // код области 
   $state = $arg['state']; 
   // ключ район 
   $area = $arg['area'];
   
   return OriGeo::init(PMS_userform::$country)->PlaceAutocomplete($state, $area, $search);   
  }
  
  
}

