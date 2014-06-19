<?php

/**
 *
 * @param type $account
 */
function _subscription_user_addition($account, $reset = false){
  static $accounts  = array();
  // reset cache
  if ($reset)
     $accounts  = array();

  //get from cache
  $_account = $accounts[$account->id];
  if (empty($_account)){
    $result =  db_fetch_array(db_query("select * from {ori_subscription_user} where id = %d limit %d", $account->id, 1));
    if($result){
        foreach($result as $name => $value){
          $account->{$name} = $value;
        }
    }

   //save to cache
   $accounts[$account->id] = $_account = $account;
 }


 return $_account;
}


function _do_replace_templates($text, $templates){
  foreach($templates as $key => $value){
    $text = str_replace("{{".$key."}}", $value, $text);
  }
  return $text;
}



//TODO replace TAG
/**
 * replace tags with values
 * @param type $text
 * @param type $account
 * @return type
 */
function _prepare_mail_text_for_user($text, $account){

  //get modules tokens
  $replaces =  module_invoke_all('subscription_message_tokens', $account);
  foreach($replaces as $topic=>$item){
      foreach($item as $token=>$value){
         $text = s_replace_tag($token, $value, $text);
      }
  }


  // insert images instead of wysiwyg-tokens
  if (is_callable('_wysiwyg_imageupload_filter_process'))
    $text = _wysiwyg_imageupload_filter_process($text);


  return $text;
}

function _prepare_subscription_links($user){
  $subs = Subscription::getList();
  $sub_ids = $out = array();
  $dec_uid = __encode($user->id, ENC_KEY);

  foreach ($subs as $sub){
    $sub_id = __encode($sub->id, ENC_KEY);

    $out[$sub->shortname]  = $sub->title;

    $out[$sub->shortname."|п"] = l((empty($sub->smsg)? variable_get(SUBSCRIBE_TEXT,''):$sub->smsg)." ".$sub->title,
                                    "suser/subscribe/$dec_uid/$sub_id", array('absolute' => TRUE));
    $out[$sub->shortname."|о"] = l((empty($sub->umsg)? variable_get(UNSUBSCRIBE_TEXT,''):$sub->umsg)." ".$sub->title,
                                    "suser/unsubscribe/$dec_uid/$sub_id", array('absolute' => TRUE));

    $sub_ids[] = $sub->id;
  }

  if (count($sub_ids) > 1){
    $ids_for_url = array();
    foreach($sub_ids as $sub_id){
      $ids_for_url[] = __encode($sub_id, ENC_KEY);
    }

    $ids_for_url = implode('-', $ids_for_url);
    $link = l(variable_get(SUBSCRIBE_TO_ALL, ''), "suser/subscribe/all/$dec_uid/$ids_for_url", array('absolute' => TRUE));
    $out['|пв'] = $link;
  }

  return $out;
}

function _user_fio($user_id){
  $user = User::get($user_id);
  $user_fio = "$user->lastname $user->firstname $user->middlename";
  return $user_fio;
}

function _prepare_page_mail_text_for_notification($mail_text, $sub_ids='all', $user_id){
  $mail_text = subscription_prepare_page_mail_text($mail_text);

  $user = User::get($user_id);
  $mail_text = _prepare_mail_text_for_user($mail_text, $user);
  if($sub_ids == 'all'){
     return $mail_text;
  }
  elseif (is_array($sub_ids) && count($sub_ids) > 0){
    $sids = implode(', ', $sub_ids);
    $subscription_res = db_query("select title from {subscription} where id in ($sids)");
    $subs = array();
    while($sub = db_fetch_object($subscription_res)){
      $subs[] = $sub->title;
    }
    $list = '';
    if(count($subs) > 1){
      $list = "<ul>";
      foreach($subs as $sub){
        $list .= "<li>$sub</li>";
      }
      $list .= "</ul>";
    }
    else{
      $list = "\"".$subs[0]."\"";
    }
    $mail_text = s_replace_tag('рассылки', $list, $mail_text);
  }
  else {
    $mail_text = variable_get(ALREADY_ACTIVE_OR_APPROVE, "Вы уже подключены или на подтверждении подключения к рассылке(кам)");
  }
  return $mail_text;
}

function _get_notification_subject($n_type){
  switch ($n_type){
    case 1 :
      return variable_get(SUBSCRIPTION_DEACTIVATE_SUBJECT, '');
    case 2 :
      return variable_get(SUBSCRIPTION_ACTIVATE_SUBJECT, '');
    case 3 :
      return variable_get(SUBSCRIPTION_APPROVE_SUBJECT, '');
    case 4 :
      return variable_get(SUBSCRIPTION_DEACTIVATE_ADMIN_SUBJECT, '');
    case 5 :
      return variable_get(SUBSCRIPTION_DECLINED_SUBJECT, '');
    case 6 :
      return variable_get(ACTIVATE_MAILS_PAGE_SUBJECT, '');
    case 7 :
      return variable_get(DEACTIVATE_MAILS_PAGE_SUBJECT, '');
  }
  return '';
}

function _get_notification_text($n_type){
  switch ($n_type){
    case 1 :
      return variable_get(SUBSCRIPTION_DEACTIVATE_TEXT, '');//SUBSCRIPTION_DEACTIVATE_TEXT
    case 2 :
      return variable_get(SUBSCRIPTION_ACTIVATE_TEXT, '');
    case 3 :
      return variable_get(SUBSCRIPTION_APPROVE_TEXT, '');
    case 4 :
      return variable_get(SUBSCRIPTION_DEACTIVATE_ADMIN_TEXT, '');
    case 5 :
      return variable_get(SUBSCRIPTION_DECLINED_TEXT, '');
    case 6 :
      return variable_get(ACTIVATE_MAILS_PAGE, '');
    case 7 :
      return variable_get(DEACTIVATE_MAILS_PAGE, '');
  }
  return '';
}

/*********************************NOTIFICATION END **********************************/



/********************* ENCRYPTION  START ********************************/

function __encode($string,$key) {
  $key = sha1($key);
  $strLen = strlen($string);
  $keyLen = strlen($key);
  for ($i = 0; $i < $strLen; $i++) {
    $ordStr = ord(substr($string,$i,1));
    if ($j == $keyLen) { $j = 0; }
    $ordKey = ord(substr($key,$j,1));
    $j++;
    $hash .= strrev(base_convert(dechex($ordStr + $ordKey),16,36));
  }
  return $hash;
}

function __decode($string,$key) {
  $key = sha1($key);
  $strLen = strlen($string);
  $keyLen = strlen($key);
  for ($i = 0; $i < $strLen; $i+=2) {
    $ordStr = hexdec(base_convert(strrev(substr($string,$i,2)),36,16));
    if ($j == $keyLen) { $j = 0; }
    $ordKey = ord(substr($key,$j,1));
    $j++;
    $hash .= chr($ordStr - $ordKey);
  }
  return $hash;
}








