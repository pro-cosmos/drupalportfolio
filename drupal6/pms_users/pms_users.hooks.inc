<?php

/**
 * Implementation of hook_ori_user_after_save
 */
function pms_users_ori_user_after_save($oldaccount, $account, $values) {
global $user;

  $user_allowed_fields = variable_get('pms_users_partner_allowed_fields', array());
  $user_edit = array();

  if ((pms_users_admin_access() || in_array('user_name', $user_allowed_fields)) && !empty($values['user_name'])) {
    $user_edit['name'] = $values['user_name'];
  }
  if ((pms_users_admin_access() || in_array('user_password', $user_allowed_fields)) && !empty($values['user_password'])) {
    $user_edit['pass'] = $values['user_password'];
  }

  //TODO@alexbuturlakin: https://github.com/oriflameda/partneraccount/issues/219
  if (pms_users_admin_access(PMS_USERS_OPERATOR_ROLE) && isset($values['user_role'])) {
    $rid = (int) $values['user_role'];
    $level_roles = _pms_users_get_level_roles();
    if ($rid > 0 && isset($level_roles[$rid])) {
      //set level role
      $user_edit['roles'][$rid] = $level_roles[$rid];
    }
  }


  if (!empty($account->regnumber) && $oldaccount->regnumber != $account->regnumber) {
    //если меняет оператор и старый регномер не был пустым то не меняем регномер
      if (pms_users_is_operator() && !empty($oldaccount->regnumber)){
           User::save($account->id, array('regnumber' => $oldaccount->regnumber));
      }
      // если меняется регномер, то меняем регдату
      User::save($account->id, array('regdate' => time()));
  }


   if ($account->status == PMS_USERS_STATUS_CONFIRMED){
    // если статус confirmed то email подтверждаем
      User::save($account->id, array('email_confirmed' => 1));
    }



  //save drupal account
  if (!empty($user_edit)) {
    $_user = user_load($values['uid']);
    if ($_user){
      user_save($_user, $user_edit);
    }
  }
}

/**
 * Implementation of hook_subscription_message_tokens().
 * @param object $account
 */
function pms_users_subscription_message_tokens($account) {

  foreach ((array) $account as $key => $val) {
    $out['account_' . $key] = $val;
  }

  $druaccount = user_load($account->uid);
  $out['account_login'] = $druaccount->name;
  $out['account_password'] = $account->drupalpassword;
  $out['account_registration_date'] = date('d/m/Y', $account->regdate);

  $url = 'http://' . variable_get('ori_server_regformsite', '') .'/ref/'.$account->regnumber;
  $out['account_referal'] = l($url,$url);

  //get confirm back link
  $uniqkey = md5($account->id);
  $pathurl = '/ori_partner/confirm';
  //pmsregform
  if ($account->regsource == variable_get('ori_server_partnerregform', '')) {
    $pathurl = str_replace('//', '/', url($pathurl));
  }

  $out["registrlink"] = l(
      variable_get(CONFIRM_LINK, CONFIRM_LINK),
      'http://' . $account->regsource . check_plain(trim($pathurl)) . '/' . $uniqkey);

  //get confirm back link
  //pmsregform
  $out["ori_emailchangeconfirm_link"] = l(
      variable_get(CONFIRM_LINK, CONFIRM_LINK),
      'http://' . variable_get('ori_server_partnerregform', '') . url(variable_get('ori_partner_baseurl', '') . '/emailchange_confirm/') . $uniqkey
      );

  //repeat registration link
  $out['ori_repeated_registration'] = l(
      variable_get(REPEATREGISTR_LINK, 'link'),
      url(variable_get('ori_partner_baseurl', '') . '/repeat_registration/' . $uniqkey, array('absolute' => TRUE)));

  return array('User account' => $out);
}

/**
 *  Implementation of hook_block_important_messages
 * @return type
 */
function pms_users_block_important_messages() {
  $out = array();

  $query_count = "select count(ur.uid) from {users_roles} ur inner join {ori_user} u  on ur.uid = u.uid where ur.rid = %d " ;// AND u.status!=" . PMS_USERS_STATUS_DISABLED;
  $count = db_result(db_query($query_count, PMS_USERS_MODERATION_ROLE));
  if ($count > 0) {
    $out['users_moderation'] = t('Users') . ':' . l(
            t('There are @count moderation users', array('@count' => $count)), variable_get('pms_users_baseurl', ''), array('query' => 'role=' . PMS_USERS_MODERATION_ROLE .'&status=0')
    );
  }

  return $out;
}

/**
 * Implementation of hook_pms_users_save.
 * internal save
 */
function pms_users_pms_users_save_alter($fields, $id) {

 if ($fields['status'] == PMS_USERS_STATUS_CONFIRMED){
    $fields['email_confirmed'] = 1;
 }

}


/* custom hooks */
/**
 *
 * @param type $account
 */
function pms_users_ori_deleteuser($account){
  user_delete(array(),$account->uid);
}


/**
 * Imlementation of hook ori_server_userregister_alter
 * @param type $account
 * @param type $values
 */
function pms_users_ori_server_userregister_alter(&$account, $values) {
  // регситрация пльзователями с регномером
  if (!empty($values['regnumber'])) {
    $account->psid = variable_get('fake_sponsor_uid',0);
  }
}