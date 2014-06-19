<?php

class OriServer {
   protected static $instance; // object instance
   public static $version = '1.0';

    public static function init() {
      if ( is_null(self::$instance) ) {
          self::$instance = new self;
      }
    return self::$instance;
    }


    //todo: replace this method to Country class in file pms_users.class.inc
    public static function getCountry($key, $value){
      $out = false;
   //watchdog('get country',$key.' -- '.$value);
      if($key && $value){
       $q = "select * from `{ori_country}` WHERE  `".$key."` = '%s' ";
       $out = db_fetch_array(db_query_range($q, $value,0, 1));
       }

       return $out;
    }

    public static function Auth($secretcode){
      //check partner site
      module_load_include('inc', 'ori_partner_site');
      if (!ori_partner_site_auth_check($secretcode)){
        watchdog('ori_server', 'Auth failed for @code', array('@code' => $secretcode), WATCHDOG_ERROR);
        return FALSE;
      }

      return TRUE;
   }

  /**
   *
   * @param type $arg
   * @return type
   */
  public static function unpack($r){
    return json_decode($r,1);
    }

 /**
   *
   * @param type $r
   * @return type
   */
  public static function pack($r){
     return json_encode($r);
  }
}