<?php


/**
 *
 * @param type $account
 */
function subscription_ori_deleteuser($account){
  db_query("delete from {usermail} where uid=%d ",$account->id);
  db_query("delete from {substatus} where uid=%d ",$account->id);
}


/**
 * Implementation of hook_subscription_message_tokens
 */
function subscription_ori_user_pre_save($oldaccount, $values){
  $errors = array();

  $values['email'] = check_plain($values['email']);

  $valid = valid_email_address(trim($values['email']));
  if (empty($values['email']) || !$valid) {
    drupal_set_message(t('Please enter valid email!'), 'error');
   return false;
  }

  $existemail = User::get(array('email' => $values['email'], 'id'=>array('!=',$oldaccount->id)), true);
  if ($existemail) {
    drupal_set_message(t('Email exists already!').' '. $values['email'], 'error');
    return false;
  }

}


/**
 * Implementation of hook_subscription_message_tokens
 */
function subscription_ori_user_after_save($oldaccount, $account, $values){

  $values['email'] = check_plain($values['email']);
  if ($values['email'] != $oldaccount->email) {

    $edit = array('status' => PMS_USERS_STATUS_NEW, 'email_confirmed'=>0);
    $is_saved = User::save($oldaccount->id, $edit);
    if ($is_saved){
       subscription_user_send_event_mails($oldaccount->id, variable_get('subscription_emailchangedconfirm_event', 7));
       drupal_set_message('Email confirmation sent to '. $values['email']);
       }
      else
       drupal_set_message('Cant apply changes','error');
  }

  //оключаем письма отключенным пользователям
  if ($account->status == PMS_USERS_STATUS_DISABLED){
     subscription_user_set_status($account->id, 0);
  }

}

/**
 * Implementation of hook_subscription_message_tokens
 */
function subscription_subscription_message_tokens($account) {
    $dec_uid = __encode($account->id, ENC_KEY);

    $out['Subscription tokens'] = array(
        "имя" => $account->firstname,
        "фамилия" => $account->lastname,
        "отчество" => $account->middlename,
        "фио" => $account->lastname . ' ' . $account->firstname . ' ' . $account->middlename,
        "письма|о" => l(variable_get(DEACTIVATE_MAILS, ''), "suser/deactivate_mails/".$dec_uid, array('absolute' => TRUE)),
        "письма|п" => l(variable_get(ACTIVATE_MAILS, ''), "suser/activate_mails/".$dec_uid, array('absolute' => TRUE))
    );



  //get tokens
  $prefix = variable_get('constant_subscription_prefix','');
  $settings = variable_get($prefix.'_settings_list','');
  if (!empty($settings)){
   foreach ($settings as $name=>$attr){
     $out['Subscription templates'][str_replace($prefix.'_', '', $name)] = variable_get($name,'');
     }
  }

 //get subscription tokens
 $tokens = _prepare_subscription_links($account);
 foreach($tokens as $k=>$v){
   $out['Subscriptions'][$k] = $v;
 }



return  $out;
}

/**
 * Implementation of hook_checkregnumber_submit
 */
function subscription_checkregnumber_submit($account) {
//submit mail with login and password by 4 event
    subscription_user_send_event_mails($account['id'], variable_get('subscription_sendpassword_event',4));
}



/**
 * Implementation of hook_block_important_messages
 */
function subscription_block_important_messages() {

    $query_count = "select count(u.id) from {substatus} ss join {ori_user} u on ss.uid = u.id join {subscription} s on s.id = ss.sid where ss.stid = 3";
    $count = db_result(db_query($query_count));
    if ($count > 0) {
        $out['subscription_request'] = t('Subscription') . ':' . l(t('There are @count subscribe request', array('@count' => $count)), variable_get('subscription_baseurl', '') . '/subscription_request', array('query' => 'filter_subsstatus[]=3') );
    }

    $dir = file_directory_path()."/mails";
    if(!is_dir($dir)){
      $out['subscription_mail_dir'] = t('Subscription mail') . ': Cant create directory '. $dir;
    }
    
    if (pms_users_admin_access() || user_access('administer mail and subscription')) {
        $count = db_result(db_query("select count(*) as cnt from {usermail} where status in (%d, %d)", SUBSCRIPTION_QUEUE_ERROR_STATUS, SUBSCRIPTION_QUEUE_WAIT_STATUS));
        if ($count > 0) {

          $sql = "SELECT  um.mid, m.subject, um.status, count(um.id) as cnt
                      FROM `usermail` um inner join mail m on um.mid=m.id
                      where um.status in (%d, %d)
                      group by um.mid, um.status";
          $result = db_query($sql, SUBSCRIPTION_QUEUE_ERROR_STATUS, SUBSCRIPTION_QUEUE_WAIT_STATUS);
          if ($result) {
            $txt = '';
            $Statuses = array('Ждут отправки','','Ошибка отправки');
            $i=1;
            while ($r = db_fetch_object($result)) {
              $statuslnk = l($Statuses[(int)$r->status],
                             "admin/oriflame/pms_users/groupaction/mailqueue",
                             array('query'=>'subscription_usermail_mid='.(int)$r->mid. '&subscription_usermail_status='.(int)$r->status.'&status=0',
                                   'attributes'=>array('title'=>'Посмотреть пользователей с нетправленным письмом «'.$r->subject.'»',
                                                       'class'=>(int)$r->status == SUBSCRIPTION_QUEUE_ERROR_STATUS?'red-text':'')));

              $dellnk = l(t('Delete all'), "admin/subscription/mailqueue/" . $r->mid . "/delete/".(int)$r->status, array('query' => drupal_get_destination()));
              $txt.= '<tr>'.
                     '<td style="width:80">'.$i.'</td>'.
                     '<td>' .$r->subject. '</td>'.
                     '<td>' . $r->cnt . 'шт.</td>'.
                     '<td>'.$statuslnk.'</td><td> [' . $dellnk . ']</td></tr>';


             $i++;
            }
            $out['subscription_cron_notsended'] = t('Subscription') . ': не отправленные письма:<br />' .
                '<table>'.$txt.'</table>';
          }
        }
  }

return $out;
}

/**
 * Implementation of hook_ori_server_userregister
 * @param type $method
 * @param type $account
 */
function subscription_ori_server_userregister($method, $account) {

    switch ($method) {
        case 'import':
            $mail_status=0;
            //add activate sending mails
            if(!empty($account->email)){
               if(valid_email_address(trim($account->email))){
                  $mail_status = 1;
                }
            }
             subscription_user_set_status($account->id, $mail_status);
            break;

        default: //case 'regform': case 'partnersite': case 'partnerregform':
            //add activate sending mails
            subscription_user_set_status($account->id, 1);
            //send all messages by register event
            subscription_user_send_event_mails($account->id, variable_get('subscription_register_event', 1));
            //подписка на рассылки
            subsctiption_user_add_subscriptions_to_user($account->id);

            //отсылка письма рег данных админу
            subscription_send_registration2admin($account);
            break;
    }
}

/**
 * Implementation of hook_pms_user_enable
 */
function subscription_pms_user_enable($account) {
    //on email sending
    subscription_user_set_status($account->id, 1);
}

/**
 * Implementation of hook_pms_user_disable
 */
function subscription_pms_user_disable($account) {
    //off email sending
    subscription_user_set_status($account->id, 0);
}

/**
 * Implementation of hook_pms_users_list_query_alter().
 */
function subscription_pms_users_list_query_alter($fields, $join, $where) {
    $q['fields'][] = 'su.mail_status';
    $q['join'][] = ' LEFT JOIN {ori_subscription_user} su on u.id=su.id';


    $filters = $_SESSION['pms_users_filters'];

  if (pms_users_admin_access()) {
    if ($filters['subscription_usermail_mid'] || is_numeric($filters['subscription_usermail_status']) && $filters['subscription_usermail_status']>=0) {
      $q['join'][] = ' INNER JOIN {usermail} um on u.id=um.uid';

      $f = $filters['subscription_usermail_mid'];
      if (!empty($f)) {
        $q['where'][] = " um.mid = " . (int) $f;
      }

      $f = $filters['subscription_usermail_status'];
      if (is_numeric($f) && $f>=0) {
        $q['where'][] = "((um.status =0 OR um.status =2 ) AND um.status = " . (int) $f.' )';
      }
    }
  }

    return $q;
}

 /**
 * Implementation of hook_ori_server_user_confirm
 */
function subscription_ori_server_user_confirm($accountrow) {
    $id = $accountrow['id'];
    //изменение статуса на - email подтвержден
    User::save($id, array('status'=>PMS_USERS_STATUS_CONFIRMED));

    //вкл отправки писем
    //subscription_user_set_status($id, 1);

    //send all messages by confirm event
    $event = empty($accountrow['regnumber']) ? variable_get('subscription_confirm_event', '') : variable_get('subscription_confirmpartner_event', '');
    if ($event > 0)
        subscription_user_send_event_mails($id, $event);
}


/**
 * After send user mail hook
 * @param type $record
 */
function subscription_subscription_user_mail_send($record){
   // 21 - код письма отправки инструкции
   if ($record->mid == 21){
     User::save($record->uid, array('instruction_send'=>time(),'status'=>PMS_USERS_STATUS_INSTRUCTIONS_SENT));
     }

}


function subscription_pms_users_result_alter($res, $ids, $uids){
  $res = db_query("select uid,id from usermail where status = 2 and uid in (".implode(',',$ids).") group by uid");
  while ($r = db_fetch_object($res)) {
   $out[$r->uid] = 1;
  }
  return array('subscription'=>$out);
}