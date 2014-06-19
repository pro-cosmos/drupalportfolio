<?php


class Mail{
  public static function get($id){
    return DB::fetch_object("select id, subject, mtext, stype, delay from {mail} where id = $id");
  }

  public static function getUserMails($id){
    return DB::fetch_array("select id, subject, mtext, type, delay from {mail} where id in (select mid from {usermail} where uid = $id)");
  }
}

class Subscription{
  public static function get($id){
    return DB::fetch_object("select id, title, shortname, onlynew, smsg, umsg from {subscription} where id = $id");
  }

  public static function getList(){
    return DB::fetch_array("select id, title, shortname, smsg, umsg from {subscription}");
  }
}

class Template{
  public static function get($id){
    return DB::fetch_object("select id, tid, value from {template} where tid = $id");
  }

  public static function getList(){
    return DB::fetch_array("select id, tid, value from {template}");
  }
}

class Tooltip{

  public static $SelectSQL = "select id, selector, title, text, mode from {tooltip}";
  public static $CountSQL = "select count(id) from {tooltip}";
  public static $RowsCount = 20;

  public static function getList(){
    return DB::fetch_array(Tooltip::$SelectSQL);
  }

  public static function delete($id){
    DB::delete('tooltip', 'id', $id);
  }
}


class Catalog{

  public static $RowsCount = 50;

  public static function newId($catalog_type){
    $id = db_fetch_object(db_query("select max(id) + 1 as id from {catalog} where type = '$catalog_type'"))->id;
    return $id == NULL ? 0 : $id;
  }

  public static function getSelectSQL($catalog_type){
    return "select id, type, value, weight from {catalog} where type = '$catalog_type'";
  }

  public static function getCountSQL($catalog_type){
    return "select count(id) from {catalog} where type = '$catalog_type'";
  }

  public static function get($catalog_type, $id){
    return DB::fetch_object("select id, type, value, weight from {catalog} where type = '$catalog_type' and id = $id");
  }

  public static function getList($catalog_type){
    return DB::fetch_array(self::getSelectSQL($catalog_type));
  }
}