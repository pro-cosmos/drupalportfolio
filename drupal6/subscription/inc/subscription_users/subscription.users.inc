<?php
require_once(drupal_get_path('module', 'subscription') . "/inc/subscription_mail/subscription.mail.inc");


/**
 *
 * @param type $id
 */
function subsctiption_user_add_subscriptions_to_user($id){
  $result = db_query("select id from {subscription}");
  while ( $sub = db_fetch_object($result)){
 //add all available subs with non-active status
      db_query("insert into {substatus} (uid, sid, stid, atime) values($id, $sub->id, 1, 0)");
  }
}

/*
 * Get all messages with type event-register. Add them to user queue.
 * stype это регистрация = 3 это тип событие
 * $eventid=1 - регистрация
 */
function subscription_user_send_event_mails($uid, $eventid, $force = true){
    $account = _subscription_user_addition(User::get($uid, true), true);

    if($account->mail_status && $eventid > 0){
    //get list messages by event id
    $result = db_query("select m.id, m.delay, m.stype from mail m
              inner join mailevent me
              on me.mid = m.id
              where m.stype = %d and me.eid = %d", SUBSCRIPTION_MAIL_TYPE_EVENT, $eventid);

    while($mail = db_fetch_object($result)){
    $mail_country = get_filter_value('user country', $mail->id);

    if ($mail_country != -1 and $mail_country != $account->cid){
      continue;
    }

    $send_time = subscription_today() + subscription_convert_delay($mail->delay);
    $record = new stdClass();
    $record->uid = $account->id;
    $record->mid = $mail->id;
    $record->senddate = $send_time;
    $record->status = 0;
    //добавляем письма в очередь для отправки 1 пользователю
    drupal_write_record('usermail', $record);
  }

  //force send user emails
  if($force)
    subscription_user_send_ready_mails($account);
  }
}











function str_replace_once($str_pattern, $str_replacement, $string){
  if (strpos($string, $str_pattern) !== false){
      $occurrence = strpos($string, $str_pattern);
      return substr_replace($string, $str_replacement, strpos($string, $str_pattern), strlen($str_pattern));
  }
  return $string;
}

function get_state_town_links(){
  $sql = "select s.id as state, t.id as town from {state} s inner join {town} t on t.sid = s.id";
  foreach(DB::fetch_array($sql) as $obj){
    $result[$obj->state][] = $obj->town;
  }
  return drupal_to_js($result);
}

function get_country_state_links(){
  $sql = "select c.id as country, s.id as state from {country} c inner join {state} s on s.cid = c.id";
  foreach(DB::fetch_array($sql) as $obj){
    $result[$obj->country][] = $obj->state;
  }
  return drupal_to_js($result);
}

function get_country_code_values(){
  $sql = "select id, code from {country}";
  foreach(DB::fetch_array($sql) as $obj){
    $result[$obj->id][] = $obj->code;
  }
  return drupal_to_js($result);
}

function subscription_users_list() {

  $output .= drupal_get_form('subscription_user_filter_form');
  $output .= drupal_get_form('subscription_user_operations_form');

  return $output;
}


function subscription_users_list_table(){
   $STATUS = array('','Зарег.','Подтв.');
  $ROLE   = array('','Партнёр');

  drupal_add_js(drupal_get_path('module', 'subscription').'/inc/subscription_users/subscription.user.list.js');

   $selall_checkbox = array(
          '#type' => 'checkbox',
          '#id'  => 'selAllUsers',
          '#name'  => 'selAll',
          '#default_value' => ''
    );

  $headers = array(
    array('data' => drupal_render($selall_checkbox)),
    array('data' => t('ФИО'), 'field' => 'lastname'),
    array('data' => 'Дата', 'field' => 'regdate'),
    array('data' => t('Статус'), 'field' => 'status'),
    array('data' => t('Роль'), 'field' => 'role'),
    array('data' => t('Email'), 'field' => 'email'),
    array('data' => ''),
    array('data' => ''),
    array('data' => t('Рассылка'), 'field' => 'mail_status'),
    array('data' => t('Опции'), 'colspan' => 3, 'style' => 'text-align:center')
  );

  $sql = "select * from {ori_user}";
  $query_count = "select count(id) from {ori_user}";
  $where_query = _generate_users_where_query();
  if ($where_query != ''){
    $sql .= $where_query;
    $query_count .= $where_query;
  }
  $sql .= tablesort_sql($headers);
  $limit = 20;
  $result = pager_query($sql, $limit, 0, $query_count);
  $data = array();
  while($account = db_fetch_object($result)){

    $num_checkbox = array(
          '#type' => 'checkbox',
          '#id'  => 'id'.$account->id,
          '#name'  => 'id['.$account->id.']',
          '#attributes' => array('class'=>'idCheckbox'),
          '#default_value' => 0
    );

    $data[] = array(
      drupal_render($num_checkbox),
      l($account->lastname.' '.$account->firstname.' '.$account->middlename, "admin/subscription/users/$account->id/edit"),
      date('m/d/Y H:i', $account->regdate),
      $STATUS[$account->status],
      $ROLE[$account->role],
      $account->email,
      l('Письма', "admin/subscription/users/$account->id/maillist"),
      l('Рассылки', "admin/subscription/users/$account->id/subscriptions"),
      ($account->mail_status? "Вкл./<a href='/' class='disable-mails' userid='$account->id'>[Откл.]</a>":"<a href='/' class='enable-mails' userid='$account->id'>[Вкл.]</a>/Откл."),
      l(t('[Удалить]'), "admin/subscription/users/$account->id/delete", array('attributes' => array('onclick' => 'return confirm(\''.t('Вы действительно хотите удалить этого пользователя?').'\');'))),
    );
  }

  $activation_data = _generate_subscription_requests_table();
  $count = $activation_data['count'];
  $table = $activation_data['table'];
  if ($count > 0){
  $out =
        "<fieldset class='collapsible collapsed' id='activate-fieldset'>
          <legend class='collapse-processed'><a href='#' id='hide-user-requests'>Запросы на подключение к рассылке: $count</a></legend>
          <div class='fieldset-wrapper' id='activate-table' style='display: block;'>
            $table
          </div>
        </fieldset>";
  }
  $out .= theme('table', $headers, $data);
  $out .= theme('pager', NULL, $limit, 0);


  return $out;
}
/**
 *
 * @return type
 */
function subscription_user_operations_form(){
  $form['filters'] = array(
    '#title' => 'Групповые операции',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  //todo сделать чтобы можно было добавлять через хуки сюда операции
  $options = array(-1=>t('Any'),
      'users_delete'=>'Удалить пользователей',
      'user_deactivatemail' =>'Отключить от писем');

    $form['filters']['group_operations'] = array(
    '#title' =>'C отмеченными пользователями',
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => -1,
  );
  $form['filters']['submit'] = array(
  '#type' => 'submit',
  '#value' => 'Применить',
  );


  $form['table']['#value'] = subscription_users_list_table();



  return $form;
  }


function subscription_user_operations_form_submit($form, &$form_state){

    if($form['#post']['group_operations']){
        module_invoke_all('group_operations', $form['#post']['group_operations'], $form['#post']['id']);
    }
  }



function subscription_user_filter_form(){
  $form['filters'] = array(
    '#title' => 'Фильтры',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['filters']['table_start'] = array(
    '#type' => 'markup',
    '#value' => '<table>'
  );
  $form['filters']['lastname'] = array(
    '#prefix' => "<tr><td class='row-label'>Фамилия:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 20,
  );
  $form['filters']['firstname'] = array(
    '#prefix' => "<tr><td class='row-label'>Имя:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 20,
  );
  $form['filters']['middlename'] = array(
    '#prefix' => "<tr><td class='row-label'>Отчество:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 20,
  );
  $form['filters']['email'] = array(
    '#prefix' => "<tr><td class='row-label'>Email:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 40,
  );
  $countries = Country::select_list();
  $countries[-1] = 'Все';
  $form['filters']['country'] = array(
    '#prefix' => "<tr><td class='row-label'>Страна:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'select',
    '#options' => $countries,
    '#default_value' => -1,
  );
  $towns = Town::select_list();
  $towns[-1] = 'Все';
  $form['filters']['town'] = array(
    '#prefix' => "<tr><td class='row-label'>Город:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'select',
    '#options' => $towns,
    '#default_value' => -1,
  );
  $purposes = load_catalog('purpose');
  $purposes[-1] = 'Все';
  $form['filters']['purpose'] = array(
    '#prefix' => "<tr><td class='row-label'>Цель регистрации:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'select',
    '#options' => $purposes,
    '#default_value' => -1,
  );
  $form['filters']['regdate_from'] = array(
    '#prefix' => "<tr><td class='row-label'>Дата регистрации от:</td><td class='row-value'>",
    '#suffix' => '</td>',
    '#type' => 'date',
    '#process' => array('subscription_expand_regdate'),
    '#default_value' => array(
      'month' => -1,
      'day' => -1,
      'year' => -1,
    ),
    '#element_validate' => array()
  );
  $form['filters']['regdate_to'] = array(
    '#prefix' => "<td class='row-label'>до:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'date',
    '#process' => array('subscription_expand_regdate'),
    '#default_value' => array(
      'month' => -1,
      'day' => -1,
      'year' => -1,
    ),
    '#element_validate' => array()
  );
  $form['filters']['psid'] = array(
    '#prefix' => "<tr><td class='row-label'>Номер спонсора:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 5,
    '#size' => 5,
  );

  $form['filters']['table_end'] = array(
    '#type' => 'markup',
    '#value' => '</table>'
  );


  $form['filters']['filter'] = array(
    '#type' => 'submit',
    '#value' => 'Отфильтровать',
  );
  $form['filters']['reset'] = array(
    '#type' => 'submit',
    '#value' => 'Сбросить',
  );
  $form['#validate'] = array();

  $filters = $_SESSION['user_filters'];
  $filters = is_array($filters) ? $filters : array();
  if ($filters['email'] != NULL){
    $form['filters']['email']['#default_value'] = $filters['email'];
  }
  if ($filters['firstname'] != NULL){
    $form['filters']['firstname']['#default_value'] = $filters['firstname'];
  }
  if ($filters['lastname'] != NULL){
    $form['filters']['lastname']['#default_value'] = $filters['lastname'];
  }
  if ($filters['middlename'] != NULL){
    $form['filters']['middlename']['#default_value'] = $filters['middlename'];
  }
  if ($filters['town'] != NULL){
    $form['filters']['town']['#default_value'] = $filters['town'];
  }
  if ($filters['country'] != NULL){
    $form['filters']['country']['#default_value'] = $filters['country'];
  }
  if ($filters['purpose'] != NULL){
    $form['filters']['purpose']['#default_value'] = $filters['purpose'];
  }
  if ($filters['regdate_from'] != NULL){
    $form['filters']['regdate_from']['#default_value'] = $filters['regdate_from'];
  }
  if ($filters['regdate_to'] != NULL){
    $form['filters']['regdate_to']['#default_value'] = $filters['regdate_to'];
  }
  if ($filters['psid'] != NULL){
    $form['filters']['psid']['#default_value'] = $filters['psid'];
  }

  drupal_add_css(drupal_get_path('module', 'subscription').'/inc/subscription_mail/subscription.mail.send.css');

  return $form;
}

function subscription_user_filter_form_submit($form, &$form_state){
  $filters = $_SESSION['user_filters'];
  $filters = is_array($filters) ? $filters : array();
  $values = $form_state['values'];
  if ($values['op'] == 'Отфильтровать'){
    if (isset($values['email'])){
      $filters['email'] = $values['email'];
    }
    else{
      unset($filters['email']);
    }
    if (isset($values['firstname'])){
      $filters['firstname'] = $values['firstname'];
    }
    else{
      unset($filters['firstname']);
    }
    if (isset($values['lastname'])){
      $filters['lastname'] = $values['lastname'];
    }
    else{
      unset($filters['lastname']);
    }
    if (isset($values['middlename'])){
      $filters['middlename'] = $values['middlename'];
    }
    else{
      unset($filters['middlename']);
    }
    if (isset($values['country'])){
      if ($values['country'] == -1){
        unset($filters['country']);
      }
      else {
        $filters['country'] = $values['country'];
      }
    }
    if (isset($values['town'])){
      if ($values['town'] == -1){
        unset($filters['town']);
      }
      else {
        $filters['town'] = $values['town'];
      }
    }
    if (isset($values['purpose'])){
      if ($values['purpose'] == -1){
        unset($filters['purpose']);
      }
      else {
        $filters['purpose'] = $values['purpose'];
      }
    }
    if (isset($values['regdate_from'])){
      $filters['regdate_from'] = $values['regdate_from'];
    }
    if (isset($values['regdate_to'])){
      $filters['regdate_to'] = $values['regdate_to'];
    }
    if (isset($values['psid'])){
      $filters['psid'] = $values['psid'];
    }
  }
  if ($values['op'] == 'Сбросить'){
    $filters = array();
  }
  $_SESSION['user_filters'] = $filters;
}

function _generate_users_where_query(){
  $filters = $_SESSION['user_filters'];
  $filters = is_array($filters) ? $filters : array();
  $where_filters = array();
  $filter = $filters['email'];
  if ($filter != NULL){
    $where_filters[] = " email like \"%$filter%\" ";
  }
  $filter = $filters['firstname'];
  if ($filter != NULL){
    $where_filters[] = " firstname like \"%$filter%\" ";
  }
  $filter = $filters['lastname'];
  if ($filter != NULL){
    $where_filters[] = " lastname like \"%$filter%\" ";
  }
  $filter = $filters['middlename'];
  if ($filter != NULL){
    $where_filters[] = " middlename like \"%$filter%\" ";
  }
  $filter = $filters['country'];
  if ($filter != NULL){
    $where_filters[] = " cid = $filter ";
  }
  $filter = $filters['town'];
  if ($filter != NULL){
    $where_filters[] = " tid = $filter ";
  }
  $filter = $filters['purpose'];
  if ($filter != NULL){
    $where_filters[] = " purid = $filter ";
  }
  $filter = $filters['regdate_from'];
  if ($filter != NULL and $filter['month'] != -1 and $filter['day'] != -1 and $filter['year'] != -1 ){
    $filter = mktime(0, 0, 0, $filter['month'],
      $filter['day'],
      $filter['year']);
    $where_filters[] = " regdate >= $filter ";
  }
  $filter = $filters['regdate_to'];
  if ($filter != NULL and $filter['month'] != -1 and $filter['day'] != -1 and $filter['year'] != -1){
    $filter = mktime(24, 60, 60, $filter['month'],
      $filter['day'],
      $filter['year']);
    $where_filters[] = " regdate <= $filter ";
  }
  $filter = $filters['psid'];
  if ($filter != NULL){
    $where_filters[] = " psid = $filter ";
  }
  $output = '';
  if (count($where_filters) > 0){
    $output .= ' where (1 = 1) ';
  }
  foreach($where_filters as $filter){
    $output .= " and $filter ";
  }
  return $output;
}

function _generate_subscription_requests_table(){
  $headers = array(
    array('data' => t('ФИО')),
    array('data' => t('Email')),
    array('data' => t('Рассылка'), 'style' => 'text-align:center'),
    array('data' => t('Опции'), 'style' => 'text-align:center')
  );

  $sql = "select u.id as user, u.firstname, u.lastname, u.middlename, u.email, s.title, ss.sid from {substatus} ss join {ori_user} u on ss.uid = u.id join {subscription} s on s.id = ss.sid where ss.stid = 3";
  $query_count = "select count(u.id) from {substatus} ss join {ori_user} u on ss.uid = u.id join {subscription} s on s.id = ss.sid where ss.stid = 3";
  $sql .= tablesort_sql($headers);
  $limit = 10;
  $result = pager_query($sql, $limit, 0, $query_count);
  $data = array();
  while($resobject = db_fetch_object($result)){
    $row = array();
    $row[] = "$resobject->lastname $resobject->firstname $resobject->middlename";
    $row[] = $resobject->email;
    $row[] = $resobject->title;
    $row[] = l(t('[Активировать]'), "admin/subscription/user/$resobject->user/activate/$resobject->sid", array('attributes' => array('class' => 'activate-user-sub')));
    $row[] = l(t('[Отказать]'), "admin/subscription/user/$resobject->user/deactivate/$resobject->sid", array('attributes' => array('class' => 'activate-user-sub')));
    $data[] = $row;
  }

  $table = theme('table', $headers, $data);
  $table .= theme('pager', NULL, $limit, 0);

  $count = db_result(db_query($query_count));
  $count = $count == FALSE ? 0 : $count;
  return array('count' => $count, 'table' => $table);
}

function subscription_user_mails_list($account){

  drupal_set_title($account->lastname.' '.$account->firstname.' '.$account->middlename.' (User messages)');

  $user_id = $account->id;

  $headers = array(
    array('data' => t('#'), 'field' => 'subject'),
    array('data' => t('Письмо'), 'field' => 'subject'),
    array('data' => t('Дата'), 'field' => 'senddate', 'sort' => 'desc'),
    array('data' => t('Статус'), 'field' => 'status'),
    array('data' => '' ),
    array('data' => '', 'style' => 'text-align:center')
  );

  $user_mail_status  = $account->mail_status;


  $sql = "select m.id, m.subject, um.senddate, um.status,um.id as umid from {usermail} um join {mail} m on m.id = um.mid where um.uid = ".(int)$user_id." order by um.senddate DESC";
  $query_count = "select count(m.id) from {usermail} um join {mail} m on m.id = um.mid where um.uid = ".(int)$user_id;
  $sql .= tablesort_sql($headers);
  $limit = 30;
  $result = pager_query($sql, $limit, 0, $query_count);
  $data = array();
  $destination = drupal_get_destination();

  $i=1;
  while($umail = db_fetch_object($result)){

    $trclass = '';
    switch ($umail->status) {
      case 0:
        $status = "Ожидает отправки: <br />" . _date_diff(subscription_today(), $umail->senddate);
        if ($user_mail_status == 0) {
          $status = "Приостановлено";
        }
        $trclass = 'tryellow';
        break;
      case 1:
        $status = "Успешно отправлено";
        $trclass = 'trgreen';
        break;
      case 2:
        $status = "Ошибка отправки";
        $trclass = 'trred';
        break;
    }

    $tddata = array(
      $i,
      $umail->subject,
      date('d.m.Y H:i:s', $umail->senddate),
      $status,
      '<nobr>['.l(t('Resend'), variable_get('subscription_baseurl','')."/mail/resend/$umail->id/$user_id", array('query' => $destination)).']</nobr>',
      '<nobr>['.l(t('Delete'), variable_get('subscription_baseurl','')."/mail/queue/$umail->umid/delete", array('query' => $destination)).']</nobr>',
      );


      $data[] = array('data' =>  $tddata,'class' => $trclass);
      $i++;
  }

  $output = theme('table', $headers, $data);
  $output .= theme('pager', NULL, $limit, 0);
  $output .= "<div>".l('Вернуться к списку', variable_get('subscription_userlist_page',''))."</div>";

  return $output;
}

//Date diff for <Left> ...
function _date_diff($start, $end)
{
  $sdate = $start;
  $edate = $end;

  $time = $edate - $sdate;
  if($time>=0 && $time<=59) {
    // Seconds
    $timeshift = $time.' с. ';
  } elseif($time>=60 && $time<=3599) {
    // Minutes + Seconds
    $pmin = ($edate - $sdate) / 60;
    $premin = explode('.', $pmin);

    $presec = $pmin-$premin[0];
    $sec = $presec*60;

    $timeshift = $premin[0].' м. '.round($sec,0).' с. ';

  } elseif($time>=3600 && $time<=86399) {
    // Hours + Minutes
    $phour = ($edate - $sdate) / 3600;
    $prehour = explode('.',$phour);

    $premin = $phour-$prehour[0];
    $min = explode('.',$premin*60);

    $presec = '0.'.$min[1];
    $sec = $presec*60;

    $timeshift = $prehour[0].' ч. '.$min[0].' м. ';

  } elseif($time>=86400) {
    // Days + Hours + Minutes
    $pday = ($edate - $sdate) / 86400;
    $preday = explode('.',$pday);

    $phour = $pday-$preday[0];
    $prehour = explode('.',$phour*24);

    $premin = ($phour*24)-$prehour[0];
    $min = explode('.',$premin*60);

    $presec = '0.'.$min[1];
    $sec = $presec*60;

    $timeshift = $preday[0].' д. '.$prehour[0].' ч. ';
  }
  return $timeshift;
}

function subscription_user_subscriptions($account){

  drupal_set_title($account->lastname.' '.$account->firstname.' '.$account->middlename.' (User subscriptions)');

  subscription_check_new_subscriptions_for_user($account);
  $headers = array(
    array('data' => t('Рассылка'), 'field' => 'title'),
    array('data' => t('Состояние'), 'field' => 'status', 'sort' => 'desc'),
    array('data' => t('Опции'), 'colspan' => 1, 'style' => 'text-align:center')
  );

  $sql = "select s.id as subid, s.title, st.id as stid, st.value as status from {subscription} s inner join {catalog} st on st.type = 'status' and st.id = (select ss.stid from {substatus} ss where ss.sid = s.id and ss.uid = ".$account->id.")";
  $query_count = "select count(s.id) from {subscription} s inner join {catalog} st on st.type='status' and st.id = (select ss.stid from {substatus} ss where ss.sid = s.id and ss.uid = ".$account->id.")";
  $sql .= tablesort_sql($headers);
  $limit = 30;
  $result = pager_query($sql, $limit, 0, $query_count);
  $data = array();
  $destination = drupal_get_destination();

  while($resobject = db_fetch_object($result)){
    $row = array();
    $row[] = $resobject->title;
    $row[] = $resobject->status;
    if ($resobject->stid != 2){
      $row[] = l(t('[Активировать]'), "admin/subscription/user/".$account->id."/changesubstatus/$resobject->subid/2", array('query'=>$destination));
    }
    else {
      $row[] = l(t('[Деактивировать]'), "admin/subscription/user/".$account->id."/changesubstatus/$resobject->subid/1", array('query'=>$destination));
    }
    $data[] = $row;
  }

  $output = theme('table', $headers, $data);
  $output .= theme('pager', NULL, $limit, 0);
  $output .= "<div>".l('Вернуться к списку', variable_get('subscription_userlist_page',''))."</div>";

  return $output;
}

function subscription_users_delete($id){
  if (is_numeric($id) && $id > 0)
  {
  User::deleteUser($id);
  drupal_goto(variable_get('subscription_userlist_page',''));
  }
}

function subscription_user_subscription_changestatus($account, $sub_id, $status_id, $go_to_subs = TRUE){
   $user_id = $account->id;

    db_query("update {substatus} set stid = $status_id where uid = $user_id and sid = $sub_id");
  if ($status_id == 1){ //deactivate
    _remove_subscription_mails_from_usermails($user_id, $sub_id);
    subscription_send_notification_email($go_to_subs ? 4 : 5, $user_id, array($sub_id)); //если отключается со страницы пользователя то - Отключена админом : Отказано в подключении
  }
  if ($status_id == 2){ //activate
    _activate_subscription($account->id, $sub_id);
  }
  subscription_user_send_ready_mails($account);

  if ($go_to_subs)
    drupal_goto(drupal_get_destination());

}

function _activate_subscription($user_id, $sub_id){
  $today = subscription_today();
  db_query("update {substatus} set stid = 2, atime = $today where uid = $user_id and sid = $sub_id");
  $is_subscription_only_new = db_fetch_object(db_query("select onlynew from {subscription} where id = $sub_id"))->onlynew;
  if ($is_subscription_only_new == 0){
    $sql_mails = "select m.id, m.delay from {mail} m where m.id in (select ms.mid from {mailsubscription} ms where ms.sid = $sub_id)";
    $mails = db_query($sql_mails);
    while($mail = db_fetch_object($mails)){
      $user_country = db_fetch_object(db_query("select cid from {ori_user} where id = $user_id"))->cid;
      $country_filter = get_filter_value('user country', $mail->id);
      if ($country_filter != -1 and $user_country != $country_filter){
        continue;
      }
      $senddate = $today + subscription_convert_delay($mail->delay);
      db_query("insert into {usermail} (mid, uid, status, senddate) values ($mail->id, $user_id, 0, $senddate)");
    }
  }
  subscription_send_notification_email(2, $user_id, array($sub_id));
}

function _deactivate_subscription($uid, $sub_id){
    $account = _subscription_user_addition(User::get($uid));
  subscription_user_subscription_changestatus($account, $sub_id, 1, FALSE);
}

function subscription_user_activate_subscription($uid, $sub_id){

  $account = _subscription_user_addition(User::get($uid));

  _activate_subscription($account->id, $sub_id);

  subscription_user_send_ready_mails($account);

  drupal_goto(drupal_get_destination());
}

function subscription_user_deactivate_subscription($user_id, $sub_id){
  _deactivate_subscription($user_id, $sub_id);

   drupal_goto(drupal_get_destination());
}

function subscription_check_new_subscriptions_for_user($account){
  $user_id = $account->id;

  $subscriptions_result = db_query("select id from {subscription}");
  $subscriptions = array();
  while ($sub = db_fetch_object($subscriptions_result)){
    $subscriptions[] = $sub->id;
  }

  $user_subs_result = db_query("select sid as id from {substatus} where uid = $user_id");
  $user_subs = array();
  while($user_sub = db_fetch_object($user_subs_result)){
    $user_subs[] = $user_sub->id;
  }

  foreach($subscriptions as $sub_id){
    if (!in_array($sub_id, $user_subs)){
      db_query("insert into {substatus} (uid, sid, stid, atime) values ($user_id, $sub_id, 1, 0)");
    }
  }
}

function subscription_user_subscribe($uid, $sub_id){
  $uid = __decode($uid, ENC_KEY);

  $account = User::get($uid);
  if (!$account){
    return "Нет такого пользователя";
  }

  $sub_id = __decode($sub_id, ENC_KEY);
  $status = db_fetch_object(db_query("select stid from {substatus} where uid = %d and sid = %d", $uid, $sub_id));

  if ($status){
    if ($status->stid == 1){
      db_query("update {substatus} set stid = 3 where uid = %d and sid = %d", $uid, $sub_id);
    }
    else {
      return variable_get(ALREADY_ACTIVE_OR_APPROVE, "Вы уже подключены или на подтверждении подключения к рассылке(кам)");
    }
  }
  else{
    if (db_fetch_object(db_query("select id from {subscription} where id = %d", $sub_id))){
      db_query("insert into {substatus} (uid, sid, stid, atime) values (%d, %d, 3, 0)", $uid, $sub_id);
    }
    else{
      return "Такой рассылки не существует";
    }
  }
  subscription_send_notification_email(3, $uid, array($sub_id));
  $output = _prepare_page_mail_text_for_notification(variable_get(SUBSCRIPTION_APPROVE_TEXT, ''), array($sub_id),$uid);
  return $output;
}



function subscription_user_subscribe_all($user_id, $sub_ids){
  $user_id = __decode($user_id, ENC_KEY);

    if (!User::get($user_id))
  {
    return "Нет такого пользователя";
  }

  $ids = explode('-', $sub_ids);
  $ids_a = array();
  foreach ($ids as $id){
    $ids_a[] = __decode($id, ENC_KEY);
  }
  $ids = implode(',', $ids_a);
  $subs = array();
  $sql = "select s.id, ss.stid from {subscription} s left join {substatus} ss on s.id = ss.sid and ss.uid = $user_id where s.id in ($ids)";
  $substatuses = db_query($sql);
  while($substatus = db_fetch_object($substatuses)){
    if ($substatus->stid == 1){
      db_query("update {substatus} set stid = 3 where uid = $user_id and sid = $substatus->id");
      $subs[] = $substatus->id;
    }
    else if ($substatus->stid == NULL){
      $sub_id = $substatus->id;
      if (db_fetch_object(db_query("select id from {subscription} where id = $sub_id"))){
        db_query("insert into {substatus} (uid, sid, stid, atime) values ($user_id, $sub_id, 3, 0)");
      }
      $subs[] = $substatus->id;
    }
  }
  subscription_send_notification_email(3, $user_id, $subs);
  $output = _prepare_page_mail_text_for_notification(variable_get(SUBSCRIPTION_APPROVE_TEXT, ''), $subs, $user_id);
  return $output;
}


function subscription_user_unsubscribe($enc_id, $enc_subid){
  $id = __decode($enc_id, ENC_KEY);

  if (!User::get($id))
  {
    return "Нет такого пользователя";
  }

  $sub_id = __decode($enc_subid, ENC_KEY);
  $exists_subs = DB::fetch_result("select count(uid) as value from {substatus} where uid = $id and sid = $sub_id") != 0;
  if ($exists_subs){
    db_query("update {substatus} set stid = 1 where uid = %d and sid = %d", $id,$sub_id);
  }
  else{
    db_query("insert into {substatus} (uid, sid, stid, atime) values (%d, %d, 1, 0)", $id, $sub_id);
  }
  _remove_subscription_mails_from_usermails($id, $sub_id);
  subscription_send_notification_email(1, $id, array($sub_id));
  $output = _prepare_page_mail_text_for_notification(variable_get(SUBSCRIPTION_DEACTIVATE_TEXT, ''), array($sub_id), $id);

  return $output;
}

function _remove_subscription_mails_from_usermails($user_id, $sub_id){
    db_query("delete from {usermail} where status = 0 and mid in (select sm.mid from {mailsubscription} sm where sm.sid = %d) and uid = %d", $sub_id,$user_id);
}




/*********************************** MAIL STATUS START ********************************/

function subscription_user_deactivate_mails_user($enc_id){
  $user_id = __decode($enc_id, ENC_KEY);
  //send notification email
  subscription_send_notification_email(7, $user_id, 'all');

  subscription_user_set_status($user_id, 0);
  return s_prepare_page_text_about_mail_status(DEACTIVATE_MAILS_PAGE, $user_id);
}


function subscription_user_activate_mails_user($enc_id){
  $id = __decode($enc_id, ENC_KEY);
  //send notification email
  subscription_send_notification_email(6, $id, 'all');

  subscription_user_set_status($id, 1);
  return s_prepare_page_text_about_mail_status(ACTIVATE_MAILS_PAGE, $id);
}

function subscription_user_activate_mails($user_id, $new_page = TRUE){
  $today = subscription_today();
  $status_change_date = db_result(db_query("select mail_status_date from {ori_subscription_user} where id = %d", $user_id));
  $delay = $today - $status_change_date;
  db_query("update {ori_subscription_user} set mail_status = 1, mail_status_date = %d where id = %d", $today, $user_id);
  db_query("update {usermail} set senddate = senddate + %d where status = 0", $delay);
  $deactivated = "Нет" . " <a href='/' class='disable-mails' userid='$user_id'>[Отключить]</a>";
  if ($new_page)
    print drupal_to_js(array( 'status' => 'OK', 'link' => $deactivated ));
}

/*********************************** MAIL STATUS END ********************************/

function subscription_usermail_delete($mid, $status=0) {

  if (user_access('administer mail and subscription') && (int)$mid >0 && in_array($status, array(0,2))){
    db_query("delete from {usermail} where status=%d and mid=%d", (int)$status, (int)$mid);
    $deleted = db_affected_rows();
    $m = Mail::get($mid);
    watchdog('subscription', 'Deleted %del mails from mail queue "!subject"', array('%del'=>(int)$deleted, '!subject'=>$m->subject), WATCHDOG_INFO);
  }

  //go to prev page
  drupal_goto(drupal_get_destination());
}