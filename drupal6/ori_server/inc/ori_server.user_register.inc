<?php

/**
 *
 * @param type $values
 */
function _ori_server_userregister_get_method(&$values) {

  //если не работают куки, то прверяем post переменную referal_partner_uid_hidden
  $values['referal_partner_uid'] = $values['referal_partner_uid_hidden']?$values['referal_partner_uid_hidden']:$values['referal_partner_uid'];

  //если не работают куки, то проверяем post переменную referer_hidden
  $values['site_referer'] = $values['referer_hidden']?$values['referer_hidden']:$values['site_referer'];
  if ($values['site_referer']){
    $partnersite = _ori_server_get_partnersite($values['site_referer']);
  }

    //если не найден партнерский сайт то регим то
  //проверяем еще $values['http_host'] а не сразу регим как с регформы под смолинского
  if (!$partnersite) {
    //ищем парнерский сайт по имени домена на котором была форма регистрации
    $partnersite = _ori_server_get_partnersite($values['http_host']);
    //если все таки не нашли парнера с которого регятся то регим как с регформы общей
    if (!$partnersite)
      $partnersite = _ori_server_get_partnersite(variable_get('ori_server_regformsite', ''));
  }

if (ORI_SERVER_TESTMODE)
        watchdog('partnersite', json_encode($partnersite));


  //если не по рефсылке регим
  if(!$values['referal_partner_uid']){
    //let autoreg module change partnersite information
    drupal_alter('ori_server_userregister_partnersite', $partnersite);
    watchdog('partnersite alter', 'partner uid: "@uid"', array('@uid' => $partnersite['uid']), WATCHDOG_INFO);
  }
  watchdog('ori_server', 'registr from "@site"', array('@site' => $partnersite?$partnersite['domain_name']:''), WATCHDOG_INFO);


  $method = 'partnersite';
  if ($partnersite['partner_site_id'] == variable_get('ori_partner_site_regform_id', 0))
    $method = 'regform';
  if ($partnersite['partner_site_id'] == variable_get('ori_partner_site_partnerregform_id', 0) && $values['form_id']=='pms_regform_regform')
    $method = 'partnerregform';


  $values['partner_site_id'] = $partnersite['partner_site_id'];
  $values['partner_site_name'] = $partnersite['domain_name'];
  $values['psid'] = $partnersite['uid'];
  $values['external_site_return_url'] = $partnersite['external_site_return_url'];

  //custom changes
  if ($values['regnumber']) {
    $admin_uid = variable_get('pms_users_admin_uid', 1);
    $values['psid'] = variable_get('fake_sponsor_uid', $admin_uid);
  }

  if ($values['referal_partner_uid']){
       //$values['partner_site_id'] = $partnersite['partner_site_id'];
       //$values['partner_site_name'] = $partnersite['domain_name'];
       $values['psid'] = $values['referal_partner_uid'];
  }

  return $method;
}

/**
 *
 * @param type $values
 * @return type
 */
function _ori_server_userregister_create_account($values, $method) {
  //do common operations
  $account = _ori_server_userregister_account_prepare($values, $method);

  //create drupal account
  if (ORI_SERVER_TESTMODE)
    watchdog('try create drupal account', json_encode($account));
  $druaccount = _ori_server_userregister_create_druaccount($account, $method, $values);
  if ($druaccount) {
    $account->uid = $druaccount->uid;

    //save user record
    if (ORI_SERVER_TESTMODE)
      watchdog('try create oriacc', json_encode($account));


    module_load_include('module', 'pms_users');
    $is_insert = pms_users_create_user($account);
    if (!$is_insert) {
      watchdog('ori_server', 'Can`t create ori account!', '', WATCHDOG_ERROR);
      return false;
    }

    //allow module do anything after user registration
    module_invoke_all('ori_server_userregister', $method, $account);
  } else {
    watchdog('ori_server', 'Can`t create drupal account!', '', WATCHDOG_ERROR);
    return false;
  }


  return $account;
}

/**
 *
 * @param type $values
 * @return stdClass
 */
function _ori_server_userregister_account_prepare($values, $method) {
if (ORI_SERVER_TESTMODE)
    watchdog('prepare values', json_encode($values));

  //common register operation
  $insert = new stdClass();
  $insert->firstname = mb_strtoupper(mb_substr($values['firstname'], 0, 1)) . mb_substr($values['firstname'], 1);
  $insert->lastname = mb_strtoupper(mb_substr($values['lastname'], 0, 1)) . mb_substr($values['lastname'], 1);
  $insert->middlename = mb_strtoupper(mb_substr($values['middlename'], 0, 1)) . mb_substr($values['middlename'], 1);
  //create pass
  $insert->drupalpassword = _ori_server_userregister_create_password();
  //создаем метку unix в часовом поясе UTC а не текущем для пользователя
  $insert->birthday = strtotime(str_replace('/', '-', $values['birthday']) . " UTC");
  $insert->passportseries = trim($values['series']);
  $insert->passportnumber = trim($values['number']);
  $insert->sex = $values['sex'];

  $insert->oriflamepassword = $values['oriflamepassword'];
  $insert->sponsor = $values['sponsor'];

  //common
  $insert->street = $values['street'];
  $insert->house = $values['house'];
  $insert->houseblock = $values['houseblock'];
  $insert->flat = $values['flat'];
  //$ccode = $values['tccode'];
  $code = $values['tcode'];
  $number = $values['tnumber'];

  $insert->tcode = $code;
  $insert->tnumber = $number;
  // $ccode = $values['mccode'];
  $prefix = $values['mprefix'];
  $number = $values['mnumber'];

  $insert->mprefix = $prefix;
  $insert->mnumber = $number;
  //$country_code = DB::fetch_result("select code from {countrycode} where cid = $user_record->cid");
  $insert->email = trim($values['email']);
  $insert->purid = $values['purposeType'];
  $insert->pother = $values['pother'];
  $insert->profession = $values['profession'];
  $insert->regdate = $values['regdate'] ? $values['regdate'] : time(); // todo: delete after testing
  $insert->psid = $values['psid'];

  //ставим статус = 1  - клиент зареган и ему отправлено письмо
  $insert->status = 1;


  //prepare to alter
  $values['method'] = $method;
  $values['ctype'] = $values['communityType'];
  $values['state'] = $values['state_hidden'];
  //$values['place'] = $values['place_hidden'];
  //$values['district'] = $values['district_hidden'];
  //alter account before save
  drupal_alter('ori_server_userregister', $insert, $values);

  //если регаистрация по рефсылке то прописываем рефсылку вместо сайта источника регистрации
  $referal_partner_uid = (int)$values['referal_partner_uid']>0?(int)$values['referal_partner_uid']:(int)$values['referal_partner_uid_hidden'];
  if ($referal_partner_uid>0){
    $p = User::get(array('uid'=>$referal_partner_uid));
    $insert->regsource = variable_get('ori_server_regformsite', '').'/ref/'.$p->regnumber;
  }


  if (ORI_SERVER_TESTMODE) {
    watchdog('altered insert', json_encode($insert));
  }

  return $insert;
}

/**
 *
 * @param type $insert
 */
function _ori_server_userregister_create_druaccount($insert, $method='', $values=array()) {

  //check user with the same login
  $login = 'Partner-' . _ori_server_userregister_create_password();
  $existuid = db_result(db_query("select uid from {users} where `name` = '%s' ", $login));
  if ($existuid)
    $login = 'Partner-' . $insert->passportnumber;
watchdog('druacc values', json_encode($values));
  //set roles
  $roles = array(PMS_USERS_PARTNER_ROLE => 1);

  if ($method == 'partnerregform') {
    $roles = array(PMS_USERS_PARTNER_ROLE => 1);
    if (!empty($values['regnumber'])) {
      $roles = array(PMS_USERS_MODERATION_ROLE => 1);
    }
  }

  if ($method == 'operator') {
    $roles = array(PMS_USERS_OPERATOR_ROLE => 1);
    $login = $insert->name;
  }

  return user_save('', array(
        //Основное
        'name' => $login, //логин
        'pass' => $insert->drupalpassword, //plain text
        'mail' => $insert->email,
        //Служебное
        'status' => '1', //1 - активный
        'created' => time(), //время регистрации
        'access' => time(), //время последнего доступа. Unixtime
        'init' => $insert->email,
        'picture' => 'http://tmpuser.gif', //ссылка на аватар
        //	'profile_nick' => '',//поля из модуля profile
        'roles' => $roles,
          )
  );
}

/**
 *
 * @param type $method
 * @param type $account
 * @return string
 */
function _ori_server_userregister_response($account, $method, $values) {
  if (ORI_SERVER_TESTMODE)
    watchdog('get response', $method . '  ' . json_encode($account));

  $out = '';
  $redirect_host = '';
  $page = 'ori_partner/register_ok/';
  $hash = _ori_server_user_hash($account);

  switch ($method) {
    case 'partnersite':
      //redirect to site referer
      $redirect_host = $account->regsource . '/';
      if (isset($values['external_site_return_url'])){
         $a = explode('.',$account->regsource);
         unset($a[0]);
         $redirect_host = implode('.',$a).'/';
         $page = $values['external_site_return_url'];
         $hash = '';
         }
      break;

    case 'partnerregform':
      //redirect to partner regform complete registration page
      $redirect_host = variable_get('ori_server_partnerregform', '');
      $page = url($page);
      break;

    case 'regform':
      //redirect to regform complete registration page
      $redirect_host = variable_get('ori_server_regformsite', '') . '/';
      break;
    default:
      watchdog('ori_server', t('Unknhown register method'));
  }


  if (ORI_SERVER_TESTMODE)
    watchdog('redirect_host', $redirect_host);

  if ($redirect_host)
    $out['register_page'] = 'http://' . $redirect_host . $page . $hash;

  return $out;
}

/**
 *
 * @param type $domen
 * @return type
 */
function _ori_server_get_partnersite($domen) {
  if ($domen) {
    $q = "SELECT * FROM {ori_partner_site} WHERE domain_name = '%s' AND UNIX_TIMESTAMP() <= `date_paid_till` ";
    return db_fetch_array(db_query($q, $domen));
  }
}

/**
 * @param $id
 * @return array|bool|null
 */
function _ori_server_get_partnersite_by_id($id = 1) {
  return db_fetch_array(db_query("SELECT * FROM {ori_partner_site} WHERE partner_site_id = %d AND UNIX_TIMESTAMP() <= `date_paid_till`", $id));
}

/**
 *
 * @param type $insert
 */
function _ori_server_userregister_create_password($passwordLength=8, $insert = array()) {
  //get random letter and num password
  return substr(base64_encode(md5(crypt(microtime()))), rand(1, 45 - $passwordLength), $passwordLength);
}

