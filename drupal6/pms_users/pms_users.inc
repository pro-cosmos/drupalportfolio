<?php

/**
 * Get users level roles
 * @return type
 */
function _pms_users_get_level_roles() {

  $roles = user_roles();
  $res = array_intersect_key($roles, array(
    PMS_USERS_PARTNER_ROLE => 1,
    PMS_USERS_LEADER_ROLE => 1,
    PMS_USERS_DIRECTOR_ROLE => 1
      ));

  ksort($res);

  return $res;
}

/**
 * Form modification
 */
function _pms_users_edit_extra_form_alter(&$form, &$form_state, $mode = 'partner') {
  global $user;

  $account = User::get(arg(3));
  $_user = user_load($account->uid);
  $reqstar = '<span class="form-required" title="Обязательное поле">*</span>';

  $form['user_account'] = array(
    '#value' => '<p><h1 class="page-title">' . t('User account') . '</h1></p>',
    '#type' => 'markup',
    '#weight' => 200
  );

  $form['table_account_start'] = array(
    '#value' => "<table class='user-table'>",
    '#type' => 'markup',
    '#weight' => 201
  );

  $form['user_name'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Login') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Login'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => false,
    '#default_value' => $_user->name,
    '#weight' => 202
  );

  $form['user_password'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Password') . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Password'),
    '#type' => 'password',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => false,
    '#attributes' => array('style' => 'float:left', 'disabled' => 'disabled'),
    '#weight' => 203
  );

  $form['table_account_end'] = array(
    '#value' => "</table>",
    '#type' => 'markup',
    '#weight' => 204
  );


  $path = drupal_get_path('module', 'pms_users');
  $setting = array(
    'pass' => $account->drupalpassword
  );

  drupal_add_js($path . '/js/pms_users.editform.js', 'module');
  drupal_add_js(array('pms_users_editform' => $setting), 'setting');
}

/**
 * Alter user profile form depend of user role
 * @param type $form
 * @param type $form_state
 */
function _pms_users_edit_partner_form_alter(&$form, &$form_state, $mode = 'partner') {
  global $user;

  $excludes = array('form_build_id', 'form_id', 'table_start', 'table_end');
  $user_allowed_fields = variable_get('pms_users_' . $mode . '_allowed_fields', array());

  foreach (element_children($form) as $key) {
    if (!in_array($key, $excludes) && in_array($form[$key]['#type'], array('textfield', 'textarea', 'select', 'checkbox', 'radio', 'radios'))) {
      //denied fields
      if (!in_array($key, $user_allowed_fields)) {
        //put fields in fieldset
        unset($form[$key]);
      } else {
        //allowed fields
        if ($mode == 'partnerchild') {
          $form[$key]['#attributes']['disabled'] = 'disabled';
        }
      }
    }
  }

  $account = User::get(arg(3));

  if (isset($form['regnumber'])) {
    $form['regnumber']['#type'] = 'markup';
    //show empty fields too
    $form['regnumber']['#value'] = ($form['regnumber']['#value']) ? $form['regnumber']['#value'] : '&nbsp;';
  }

  //custom role altering
  if ($mode == 'partner') {
    if (isset($form['psid']['#value']))
      $form['psid']['#value'] = strip_tags($form['psid']['#value']);
  }

  //custom altering
  if ($mode == 'partnerchild') {
    //hide from partner oriflame pasw field on his childuser profile page
    if ($account->psid == $user->uid) {

      if (pms_users_og_is_member(PMS_USERS_OG_ACCOUNT_VIEWPASS))
        $form['oriflamepassword']['#type'] = 'markup';
      else
        unset($form['oriflamepassword']);

      unset($form['submit']);
    }
  }

  $form['submit']['#weight'] = 999;
}

/**
 * Alter user profile form depend of user role
 * @param type $form
 * @param type $form_state
 */
function _pms_users_filter_form_alter(&$form, &$form_state) {
  global $user;

  $user_allowed_filters = variable_get('pms_users_partner_allowed_filters', array());
  $excludes = array('table_start', 'table_end');

  foreach (element_children($form['filters']) as $key) {
    if (!in_array($key, $user_allowed_filters)) {
      if (!in_array($key, $excludes) && in_array($form['filters'][$key]['#type'], array('textfield', 'textarea', 'select', 'checkbox', 'radio', 'radios'))) {
        //put fields in fieldset
        unset($form['filters'][$key]);
      }
    }
  }
}

/**
 *
 * @global type $user
 * @param type $_account
 * @param type $editedaccount
 * @param type $values
 * @return type
 */
function _pms_users_edituser_role_submit($account, $editedaccount, $values) {
  global $user;

  $_account = $account;
  //Get allowed fields array
  if (!pms_users_admin_access()) {
    $mode = ($user->uid == $_account->uid) ? 'partner' : 'partnerchild';
    //save  allowed by role fields only
    $user_allowed_fields = variable_get('pms_users_' . $mode . '_allowed_fields', array());
    //deny partner change his novice data
    if ($mode == 'partnerchild') {
      $user_allowed_fields = array();
    }
  } else {
    //custom edits
    if ((int) $values['psid_edit'] > 0) {
      if (false != $sp = User::get(array('regnumber' => (int) $values['psid_edit']))) {
        if ($_account->uid != $sp->uid) {
          //set new sponsor to account
          $editedaccount->psid = $sp->uid;
        }
        else
          drupal_set_message(t('The user cant be sponsor of yourself'));
      }
      else
        drupal_set_message(t('There is no sponsor with regnumber "@regnumber"', array('@regnumber' => (int) $values['psid_edit'])));
    }

    //for admin allow all fields
    $user_allowed_fields = array_keys((array) $editedaccount);
  }

  // set new values to allowed fields
  if (!empty($user_allowed_fields)) {
    //change allowed fields
    foreach ($user_allowed_fields as $key) {
      //if field has new value
      if (isset($editedaccount->{$key})) {
        //set new value
        $_account->{$key} = $editedaccount->{$key};
      }
    }
  }

  return $_account;
}

/**
 * Add some default users
 */
function _pms_users_after_install($reset=false) {

  //todo run function not insert query
  module_load_include('inc', 'ori_server', 'inc/ori_server.user_register');

  if ($reset) {
    $q = db_query("select id from {ori_user}");
    while ($row = db_fetch_array($q)) {
      User::deleteUser($row['id']);
    }

    $q = db_query("select uid from {users} where uid > 1");
    while ($row = db_fetch_array($q)) {
      user_delete(array(), $row['uid']);
    }


    // Reset cached schema
    drupal_get_schema(NULL, TRUE);
    db_query("ALTER TABLE `ori_user` AUTO_INCREMENT = 1");
    db_query("ALTER TABLE `users` AUTO_INCREMENT = 3");

    db_query("TRUNCATE TABLE `usermail`");

    //ori_robot_user
    db_query("TRUNCATE TABLE `ori_robot_user`");
    db_query("ALTER TABLE `ori_robot_user` AUTO_INCREMENT = 1");
    //ori_partner_site
    db_query("delete from `ori_partner_site` where partner_site_id > 6");
    db_query("ALTER TABLE `ori_partner_site` AUTO_INCREMENT = 6");
    //ori_partner_maillist
    db_query("TRUNCATE TABLE `ori_partner_maillist`");
    db_query("ALTER TABLE `ori_partner_maillist` AUTO_INCREMENT = 1");
    //ori_og_groups2users
    db_query("TRUNCATE TABLE `ori_og_groups2users`");
    db_query("TRUNCATE TABLE `ori_balance_operation`");

    db_query("TRUNCATE TABLE `ori_subscription_user`");
    db_query("TRUNCATE TABLE `ori_training_statistic`");
    db_query("TRUNCATE TABLE `ori_training_statistic_view`");

    db_query("delete from `ori_balance_user` where uid > 4");
    db_query("TRUNCATE TABLE `ori_instruction_viewer`");
  }


  //add roles
  variable_set('pms_users_roles', user_roles(true));

  //add operators
  $insert = new stdClass();
  $insert->name = 'operator1';
  $insert->drupalpassword = 'operator1';
  $insert->email = 'operator1@test.ru';
  _ori_server_userregister_create_druaccount($insert, 'operator');

  //add smolinsky account
  variable_set('smolinsky_regnumber', 5652921);
  $smolinsky_regnumber = variable_get('smolinsky_regnumber', 0);

  $values = array();
  $values['firstname'] = "Сергей";
  $values['middlename'] = "Олегович";
  $values['lastname'] = "Смолинский";
  $values['sex'] = 1;
  $values['number'] = 108119;
  $values['series'] = 2400;
  $values['email'] = 'Lyubov-sm2@yandex.ru';
  $values['birthday'] = '14/11/1975';
  $values['regdate'] = $random_reg_date;
  $values['regnumber'] = $smolinsky_regnumber;
  $values['psid'] = 0;
  $values['country'] = 'ru';
  $values['street'] = 'ул. Семеновская';
  $values['index'] = 153009;
  $values['town'] = 'Иваново';
  $values['oriflamepassword'] = rand(100000, 999999);
  $values['house'] = 1;
  $values['purposeType'] = 2;
  $values['mprefix'] = 903;
  $values['mnumber'] = 8887878;

  //create account
  $smolinsky = _ori_server_userregister_create_account($values, 'import');
  variable_set('pms_users_admin_uid', $smolinsky->uid);

  //add role admin
  $smolinsky_user = user_load($smolinsky->uid);
  $roles = array(3 => 'admin', 5 => 'partner'); //user_roles(true); // load the site's roles

  $update['roles'] = $smolinsky_user->roles;
  $adminrolekey = array_search('admin', $roles);
  $update['roles'][$adminrolekey] = $roles[$adminrolekey]; // add admin role
  $partnerrolekey = array_search('partner', $roles);
  $update['roles'][$partnerrolekey] = $roles[$partnerrolekey]; // add partner role
  user_save($smolinsky_user, $update);

  //add og role - autoregsponsor to smolinsy
  module_load_include('module', 'pms_users_og');
  pms_users_og_member_add(PMS_USERS_OG_AUTOREGSPONSOR, $smolinsky->id);
  //set status confirmed
  User::save($smolinsky->id, array('status' => PMS_USERS_STATUS_CONFIRMED));

  // create test data if test-mode is enabled
  if ($PMS_USERS_TEST_MODE == TRUE) {
    $random_count = 5; //rand(5, 7);

    for ($i = 2; $i <= $random_count; $i++) {
      $sponsor_id = 0;
      $email = 'sboevkm@gmail.com' . $i;
      if ($i == 3) {
        $sponsor_id = 5; //$i % 2 + 1;
      }
      if ($i == 4) {
        $sponsor_id = 6; //$i % 2 + 1;
      }


      $random_reg_date = time() - rand(108000, 308000);
      $random_birthday = time() - rand(998000, 10800000);

      $values = array();
      $values['firstname'] = "Name$i";
      $values['middlename'] = "Middle$i";
      $values['lastname'] = "tmpuser";
      $values['sex'] = 1;
      $values['number'] = rand(1000, 9999);
      $values['series'] = rand(100000, 999999);
      $values['email'] = $email;
      $values['birthday'] = date('d/m/Y', $random_birthday);
      $values['regdate'] = $random_reg_date;
      $values['psid'] = $sponsor_id;
      $values['country'] = 'ru';
      $values['street'] = 'Test Street' . $i;
      $values['index'] = '426000';
      $values['state_hidden'] = 74;
      $values['oriflamepassword'] = rand(100000, 999999);
      $values['house'] = rand(1, 100);

      //create account
      _ori_server_userregister_create_account($values, 'import');
    }

    db_query("update {ori_user} set regnumber = id +100");
  }


  _pms_users_after_install_create_fake_temporary_sponsor();

  variable_set('pms_users_after_install', TRUE);
}

function _pms_users_after_install_create_fake_temporary_sponsor() {
  //add fake sponsor account
  watchdog('fake install','run');
  module_load_include('inc', 'ori_server', 'inc/ori_server.user_register');

  //ниже идут две удивительные строчки кода особенно 2я
  variable_set('fake_sponsor_regnumber', 900000009);
  $reg_number = variable_get('fake_sponsor_regnumber', 0);

  $values = array();
  $values['firstname'] = "Временнный";
  $values['middlename'] = "Спонсор";
  $values['lastname'] = "Иванович";
  $values['sex'] = 1;
  $values['number'] = 555555;
  $values['series'] = 55;
  $values['email'] = 'fake-sponsor@pravda-zdorovo.ru';
  $values['birthday'] = '14/11/1975';
  $values['regdate'] = time() - rand(108000, 308000);
  ;
  $values['regnumber'] = $reg_number;
  $values['psid'] = 0;
  $values['country'] = 'ru';
  $values['street'] = 'ул. Семеновская';
  $values['index'] = 153009;
  $values['town'] = 'Иваново';
  $values['oriflamepassword'] = rand(100000, 999999);
  $values['house'] = 1;
  $values['purposeType'] = 2;
  $values['mprefix'] = 903;
  $values['mnumber'] = 8887878;

  //create account
  $fake = _ori_server_userregister_create_account($values, 'import');

  //add role admin
  $fake_user = user_load($fake->uid);
  $roles = array(3 => 'admin', 5 => 'partner'); //user_roles(true); // load the site's roles

  $update['roles'] = $fake_user->roles;
  $partnerrolekey = array_search('partner', $roles);
  $update['roles'][$partnerrolekey] = $roles[$partnerrolekey]; // add partner role
  user_save($fake_user, $update);

//  add og role - autoregsponsor to smolinsy
  module_load_include('module', 'pms_users_og');
  pms_users_og_member_add(PMS_USERS_OG_AUTOREGSPONSOR, $fake->id);
//  set status confirmed
  User::save($fake->id, array('status' => PMS_USERS_STATUS_CONFIRMED));

  $partner_site = _ori_server_get_partnersite_by_id();

  if (0 != $fake->id) {
    variable_set('fake_sponsor_uid', $fake_user->uid);
  }
}

/**
 * функция возвращает массив id текущих отфильтрованных пользователей
 * @param type $mode
 * @param type $addfilters
 * @return type
 */
function _pms_users_get_selected_users_query($mode ='', $addfilters = array()) {

  $sql = $uids = array();
  module_load_include('inc', 'pms_users', 'pms_users.pages');
 //запрет не админу выделять всех пользователей только тех кто им доступен по привилегиям
  $mode = ($mode=='all' && pms_users_admin_access())? $mode : 'filtered';

  switch ($mode) {
    case 'all':
      $sql = "select uid from {ori_user} ";
      break;

    case 'filtered':
      $sql_filtered = _pms_users_list_query_where($addfilters);
      $sql_filtered = $sql_filtered['sql'];
      //remove no need fields
      $tmp = explode(' from ', strtolower($sql_filtered), 2);
      $sql = "select u.id from " . $tmp[1];
      break;
  }

  if ($sql) {
    $rez = db_query($sql);
    while ($r = db_fetch_array($rez)) {
      $uids[] = $r['id'];
    }
  }

  return $uids;
}