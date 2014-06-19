<?php

define('ORI_BASE_DEFAULT_COUNTRY', 'ru');

class User {

  const fields = "*"; //," id, firstname, lastname, middlename, birthday, email, mail_status, mail_status_date ";
  const fields_extra = " cid, tid, stid ";
  const usertable = '{ori_user}';

  static protected $accounts = array();

  private static function select() {
    return "select " . self::fields . " from " . self::usertable;
  }

  private static function select_full() {
    return "select " . self::fields . ", " . self::fiels_extra . " from " . self::usertable;
  }

  public static function getList() {
    return DB::fetch_array(self::select());
  }

  public static function getListFull() {
    return DB::fetch_array(self::select_full());
  }

  public static function get($arg, $reset = false) {
    $where = array();
    if (empty($arg))
      return false;
    $key = json_encode($arg);

    //clear cache
    if ($reset)
      unset(self::$accounts[$key]);

    // if get by id
    if (is_numeric($arg))
      $arg = array("id" => (int) $arg);

    //error
    if (!is_array($arg))
      watchdog('User::get', json_encode($arg));

    // construct where
    foreach ($arg as $field => $value) {
      $sign = '=';
      if (is_array($value)) {
        $sign = $value[0];
        $value = $value[1];
      }

      $where[] = "(`" . $field . "` " . $sign . " '" . $value . "' )";
    }

    //make query
    $sql = self::select() . " where " . implode(' AND ', $where);

    //get cache
    if (isset(self::$accounts[$key])) {
      return self::$accounts[$key];
    }

    //do query
    $result = empty($where) ? false : DB::fetch_object($sql);

    //save cache
    if (!empty($result))
      self::$accounts[$key] = $result;

    return $result;
  }

  public static function getFull($id) {
    return DB::fetch_object(self::select_full() . " where id = $id");
  }

  public static function deleteUser($id) {
    $account = User::get($id);
    if ($id) {
      //delete ori account
      db_query("delete from " . self::usertable . " where id = %d", $id);
      //delete drupal account
      user_delete(null, $account->uid);
      //let module do something
      module_invoke_all('ori_deleteuser', $account);
    }
  }

  public static function deleteUserAll($uids) {
    if (!empty($uids) && is_array($uids)) {
      foreach ($uids as $id) {
        self::deleteUser($id);
      }
    }
  }

  /**
   * Returns a list of email addresses for specified ori_user ids
   * @param array $partner_ids
   * @return array of strings
   */
  public static function getEmailsByIDs($partner_ids) {
    $list = array();
    $ids_string = implode(', ', $partner_ids);

    $query = 'SELECT email FROM ' . self::usertable . ' WHERE id IN (%s)';
    $result = db_query($query, $ids_string);
    while ($row = db_fetch_array($result)) {
      $list[] = $row['email'];
    }

    return $list;
  }

  public static function save($id, $fields = array()) {

    if ($id > 0 && !empty($fields)) {

       //let modules change something
      drupal_alter('pms_users_save', $fields, $id);

      foreach ($fields as $field => $value) {
        //todo check acces to user fields
        $set_fields[$field] = "`" . $field . "` = '" . $value . "'";
      }
      return db_query("UPDATE {ori_user} SET " . implode(", ", $set_fields) . " WHERE id = %d ", $id);
    }

    return false;
  }

  public static function enable($account) {
    $account = is_numeric($account) ? self::get($account) : $account;

    if (is_object($account)) {

      self::save($account->id, array('status' => 1));
      db_query("UPDATE {users} SET status=1 WHERE uid = %d", $account->uid);

      //let modules do something
      module_invoke_all('pms_user_enable', $account);
    }
  }

  public static function disable($account) {
    $account = is_numeric($account) ? self::get($account) : $account;

    if (is_object($account)) {
      self::save($account->id, array('status' => 4));
      db_query("UPDATE {users} SET status = 0 WHERE uid = %d", $account->uid);

      //let modules do something
      module_invoke_all('pms_user_disable', $account);
    }
  }

  public static function getExtra($account){
    return $account->extra? json_decode('{'.$account->extra.'}') : new stdClass();
  }

  public static function setExtra($id, $extra=false){
     $v = ($extra)? json_encode($extra):'';
     self::save($id, array('extra' => $v));
  }
}

class Country {

  public static function get($id, $fldid = 'id') {
    return DB::fetch_object("select * from {ori_country} where " . $fldid . " = '" . $id . "' ");
  }

  public static function getList() {
    return DB::fetch_array("select * from {ori_country} where active=1");
  }

  public static function select_list($fldid = 'id', $fldvalue = 'title') {
    return DB::list_to_select(self::getList(), $fldid, $fldvalue);
  }

}



class State {

  static public $country = ORI_BASE_DEFAULT_COUNTRY;
  static protected $instance = NULL;

  protected function __clone() {

  }

  protected function __construct() {

  }

  public static function init($geocode) {
    //check country code
    if (Country::get($geocode,'geocode')){
       self::$country = $geocode;
    }
    else
     return false;

    if (self::$instance == NULL) {
      $class = __CLASS__;
      self::$instance = new $class();
    }
    return self::$instance;
  }

  public static function get($id) {
    return DB::fetch_object("select * from {ori_" . self::$country . "state} where id = $id");
  }

  public static function getList() {
    return DB::fetch_array("select * from {ori_" . self::$country . "_state}");
  }

  public static function listByCountry($cid) {
    return DB::fetch_array("select * from {ori_" . self::$country . "_state} where cid = $cid");
  }

  public static function select_list() {
    return DB::list_to_select(self::getList());
  }

  public static function getByCity($city_id){
    return DB::fetch_object("
      SELECT DISTINCT(s.`id`)  as id, s.`title`
      FROM ori_" . self::$country . "_postindex AS pi
      LEFT JOIN ori_ru_state s ON pi.`state_id` = s.`id`
      WHERE pi.`city_id` = " . $city_id
    );
  }
}

class Town {

  static public $country = ORI_BASE_DEFAULT_COUNTRY;
  static protected $instance = NULL;

  protected function __clone() {

  }

  protected function __construct() {

  }

  public static function init($country) {
    self::$country = $country; //country geocode

    if (self::$instance == NULL) {
      $class = __CLASS__;
      self::$instance = new $class();
    }
    return self::$instance;
  }

  public static function get($id) {
    return DB::fetch_result("select title from {ori_" . self::$country . "_city} where id = %d", $id);
  }

  public static function getList($cid = '') {
    return DB::fetch_array("select title from {ori_" . self::$country . "_city} group by title");
  }

  public static function listByState($sid) {
    return DB::fetch_array("select * from {ori_" . self::$country . "_city} where sid = $sid");
  }

  public static function select_list() {
    return DB::list_to_select(self::getList());
  }

  public static function autocomplete($state_id = -1, $search = '') {
    $where = '';
    if ($state_id > 0) {
      $where = "ix.state_id = %d AND ";
    }

    $query = "SELECT ix.city_id as `id`, c.title as `label` FROM `{ori_" . self::$country . "_postindex}` ix
        INNER JOIN `{ori_" . self::$country . "_city}` c ON  ix.city_id = c.id
        WHERE " . $where . "  c.title LIKE '%s%%'
        GROUP BY c.id";
    if ($state_id > 0)
      return DB::fetch_array($query, $state_id, $search);
    else
      return DB::fetch_array($query, $search);
  }

}

class OriGeo {

  static public $country = ORI_BASE_DEFAULT_COUNTRY;
  static protected $instance = NULL;

  protected function __clone() {

  }

  protected function __construct() {

  }

  public static function init($country) {
    self::initCountry($country);


    if (self::$instance == NULL) {
      $class = __CLASS__;
      self::$instance = new $class();
    }
    return self::$instance;
  }

  protected function initCountry($country) {
    //get country list
    $country_list = variable_get('pmsusers_country_list', array());
    if (in_array($country, $country_list)) {
      self::$country = $country; //country geocode
    } else {
      self::$country = ORI_BASE_DEFAULT_COUNTRY;
      //watchdog('OriGeo class', 'empty country code '.$country ,NULL,'error');
    }
  }

  public static function IndexAutocomplete($search) {
    $query = "SELECT ix.*, ix.`pindex` as `id`, ix.`pindex` as `label`, c.title as city FROM `{ori_" . self::$country . "_postindex}` ix
                INNER JOIN `{ori_" . self::$country . "_city}` c ON  ix.city_id = c.id
                WHERE ix.`pindex` LIKE '%s%%' ";
    return DB::fetch_array($query, $search);
  }

  public static function getIndex($search) {
    static $cache = array();

    $query = "SELECT ix.*,s.title as state,c.title as city FROM `{ori_" . self::$country . "_postindex}` ix
                INNER JOIN `{ori_" . self::$country . "_state}` s ON  ix.state_id = s.id
                INNER JOIN `{ori_" . self::$country . "_city}` c ON  ix.city_id = c.id
                WHERE ix.`pindex` = '%s%'";

    //cache get
    $key = md5($query . $search);
    if (isset($cache[$key]))
      return $cache[$key];

    $res = DB::fetch_array($query, $search);
    $res = $res ? $res[0] : false;

    //cache set
    if ($res)
      $cache[$key] = $res;

    return $res;
  }

  public static function DistrictAutocomplete($state_id, $search) {
    $query = "SELECT area as `id`, area as `label` FROM `{ori_" . self::$country . "_postindex}` WHERE state_id = %d and `area` LIKE '%s%%' group by area";
    return DB::fetch_array($query, $state_id, $search);
  }

  public static function PlaceAutocomplete($state_id, $area, $search) {
    $query = "SELECT ix.city_id as `id`, c.title as `label` FROM `{ori_" . self::$country . "_postindex}` ix
        INNER JOIN `{ori_" . self::$country . "_city}` c ON  ix.city_id = c.id
        WHERE ix.state_id = %d AND area = '%s' AND c.title LIKE '%s%%'
        GROUP BY c.id";
    return DB::fetch_array($query, $state_id, $area, $search);
  }

}

class DB {

  public static function fetch_object($sql) {
    return db_fetch_object(@db_query($sql));
  }

  public static function fetch_result($sql) {
    $args = func_get_args();
    $q = (count($args) > 1) ? call_user_func_array('db_query', $args) : db_query($sql);
    return db_result($q);
  }

  public static function fetch_array($sql) {
    $args = func_get_args();
    $q = (count($args) > 1) ? call_user_func_array('db_query', $args) : db_query($sql);

    $result = array();
    while ($r = db_fetch_object($q)) {
      $result[] = $r;
    }

    return $result;
  }

  public static function delete($table, $field, $value) {
    db_query('delete from {' . $table . '} where ' . $field . " = '$value'");
  }

  public static function list_to_select($options, $id_field = 'id', $title_field = 'title') {
    foreach ($options as $obj) {
      $result[$obj->{$id_field}] = $obj->{$title_field};
    }
    return $result;
  }

}

class PMS_userform {

  const CLASSCODE = 'pms_userform';
  const CLASSROOT = '/classes';

  public static $country;

  public static function init($country = 'default', $attr = array()) {
    self::$country = $country; //country geocode
    $classcountry = (self::class_file_exists($country)) ? $country : 'default';
    $classname = self::CLASSCODE . '_' . ucfirst($classcountry); //class pms_userform_Ukr
    // auto include classfile if it not included
    if (!class_exists($classname, false)) {
      PMS_userform::autoload($classcountry);
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

  public static function class_file_exists($country) {
    $classfile = self::CLASSCODE . '.' . strtolower($country); //ori_client.ukr.php
    $filename = drupal_get_path('module', self::CLASSCODE) . self::CLASSROOT . '/' . $classfile . '.php';
    return file_exists($filename);
  }

  public static function class_file_include($country) {
    $classfile = self::CLASSCODE . '.' . strtolower($country); //ori_client.ukr.php
    $filename = drupal_get_path('module', self::CLASSCODE) . self::CLASSROOT . '/' . $classfile . '.php';
    include_once($filename);
  }

}

/**
 * Common regform country class
 */
class pms_userform_Base {

  public static function getSelect_country($arg = array()) {
    $out = array();

    $data = Country::getList();
    if ($data) {
      foreach ($data as $r) {
        $out['result'][$r['geocode']] = $r['title'];
        $out['phonecodes'][$r['geocode']] = $r['code'];
      }
    }

    return $out;
  }

}